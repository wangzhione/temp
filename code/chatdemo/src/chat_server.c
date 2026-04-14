#include "chat_server.h"
#include "utf8.h"

enum {
    // 客户端超过该时间未发任何合法报文, 视为超时离线
    CHAT_IDLE_TIMEOUT_MS = 120000,
    // 服务端 recv 超时, 用于周期性检查空闲客户端
    CHAT_SERVER_RECV_TIMEOUT_MS = 1000,
};

#if defined(_WIN32)
static volatile LONG chat__server_stop_flag = 0;

static BOOL WINAPI
chat__server_console_handler(DWORD ctrl_type) {
    switch (ctrl_type) {
    case CTRL_C_EVENT:
    case CTRL_BREAK_EVENT:
    case CTRL_CLOSE_EVENT:
    case CTRL_SHUTDOWN_EVENT:
        InterlockedExchange(&chat__server_stop_flag, 1);
        return TRUE;
    default:
        return FALSE;
    }
}

static int
chat__server_install_stop_handler(void) {
    InterlockedExchange(&chat__server_stop_flag, 0);
    return SetConsoleCtrlHandler(chat__server_console_handler, TRUE) ? 0 : -EIO;
}

static void
chat__server_uninstall_stop_handler(void) {
    (void)SetConsoleCtrlHandler(chat__server_console_handler, FALSE);
}

static bool
chat__server_stop_requested(void) {
    return InterlockedCompareExchange((volatile LONG *)&chat__server_stop_flag, 0, 0) != 0;
}
#else
static volatile sig_atomic_t chat__server_stop_flag = 0;
static struct sigaction chat__server_prev_sigint = {};
static struct sigaction chat__server_prev_sigterm = {};
static bool chat__server_stop_handler_installed = false;

static void
chat__server_signal_handler(int signo) {
    (void)signo;
    chat__server_stop_flag = 1;
}

static int
chat__server_install_stop_handler(void) {
    struct sigaction sa = {};

    sa.sa_handler = chat__server_signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    chat__server_stop_flag = 0;

    if (sigaction(SIGINT, &sa, &chat__server_prev_sigint) != 0) {
        return -errno;
    }
    if (sigaction(SIGTERM, &sa, &chat__server_prev_sigterm) != 0) {
        int err = errno;

        (void)sigaction(SIGINT, &chat__server_prev_sigint, nullptr);
        return -err;
    }

    chat__server_stop_handler_installed = true;
    return 0;
}

static void
chat__server_uninstall_stop_handler(void) {
    if (!chat__server_stop_handler_installed) {
        return;
    }

    (void)sigaction(SIGINT, &chat__server_prev_sigint, nullptr);
    (void)sigaction(SIGTERM, &chat__server_prev_sigterm, nullptr);
    chat__server_stop_handler_installed = false;
}

static bool
chat__server_stop_requested(void) {
    return chat__server_stop_flag != 0;
}
#endif

typedef struct chat_client_slot {
    // 槽位是否被占用
    bool active;
    // 聊天室内唯一用户名
    char name[NAME_BYTES_MAX + 1];
    // 客户端来源地址, UDP 会话依赖这个地址识别
    endpoint_t endpoint;
    // 最近一次收到该客户端合法报文的时间
    uint64_t last_seen_ms;
} chat_client_slot_t;

typedef struct udp_chat_server {
    // 服务端 UDP socket
    socket_t fd;
    // 固定容量客户端槽位表, 避免运行期碎片化管理
    chat_client_slot_t * clients;
    size_t max_clients;
} chat_server_t;

static int
chat__server_validate_text(const char * text, size_t bytes_max, size_t chars_max) {
    size_t bytes;
    size_t chars;
    int rc;

    if (text == nullptr) {
        return -EINVAL;
    }

    bytes = strlen(text);
    if (bytes == 0 || bytes > bytes_max) {
        return -EINVAL;
    }

    rc = utf8_validate_and_count(text, bytes, &chars);
    if (rc < 0) {
        return rc;
    }
    if (chars == 0 || chars > chars_max) {
        return -EINVAL;
    }
    return 0;
}

static uint64_t
chat__clock_ms(void) {
#if defined(_WIN32)
    return (uint64_t)GetTickCount64();
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000ULL + (uint64_t)(ts.tv_nsec / 1000000ULL);
#endif
}

static void
chat__server_log(const char * level, const char * message) {
    if (level == nullptr || message == nullptr) {
        return;
    }

    fprintf(stdout, "[server][%s] %s\n", level, message);
    fflush(stdout);
}

static void
chat__server_log_addr(const char * level, const char * prefix, const endpoint_t * ep) {
    char addr[64];

    if (endpoint_to_string(ep, addr, sizeof(addr)) < 0) {
        (void)snprintf(addr, sizeof(addr), "unknown");
    }
    fprintf(stdout, "[server][%s] %s%s\n", level, prefix != nullptr ? prefix : "", addr);
    fflush(stdout);
}

static int
chat__server_send_packet(chat_server_t * server, const chat_packet_t * packet, const endpoint_t * target) {
    char wire[PACKET_MAX];
    int rc;
    int wire_len;

    wire_len = chat_packet_encode(packet, wire, sizeof(wire));
    if (wire_len < 0) {
        return wire_len;
    }

    rc = socket_sendto(server->fd, wire, (size_t)wire_len, target);
    if (rc < 0) {
        return rc;
    }
    return 0;
}

static int
chat__server_send_system(chat_server_t * server, const endpoint_t * target, const char * text, uint32_t type) {
    chat_packet_t packet;

    chat_packet_init(&packet);
    packet.type = type;
    (void)snprintf(packet.sender, sizeof(packet.sender), "server");
    (void)snprintf(packet.payload, sizeof(packet.payload), "%s", text != nullptr ? text : "");
    return chat__server_send_packet(server, &packet, target);
}

static ptrdiff_t
chat__server_find_by_endpoint(chat_server_t * server, const endpoint_t * ep) {
    size_t idx;

    for (idx = 0; idx < server->max_clients; ++idx) {
        if (!server->clients[idx].active) {
            continue;
        }
        if (endpoint_equal(&server->clients[idx].endpoint, ep)) {
            return (ptrdiff_t)idx;
        }
    }
    return -1;
}

static ptrdiff_t
chat__server_find_by_name(chat_server_t * server, const char * name) {
    size_t idx;

    for (idx = 0; idx < server->max_clients; ++idx) {
        if (!server->clients[idx].active) {
            continue;
        }
        if (strncmp(server->clients[idx].name, name, sizeof(server->clients[idx].name)) == 0) {
            return (ptrdiff_t)idx;
        }
    }
    return -1;
}

static ptrdiff_t
chat__server_alloc_slot(chat_server_t * server) {
    size_t idx;

    for (idx = 0; idx < server->max_clients; ++idx) {
        if (!server->clients[idx].active) {
            return (ptrdiff_t)idx;
        }
    }
    return -1;
}

static void
chat__server_broadcast(chat_server_t * server, const chat_packet_t * packet, const endpoint_t * exclude) {
    size_t idx;

    // UDP 广播本质是循环单播; exclude 用于 join 时不把入场通知回发给自己
    for (idx = 0; idx < server->max_clients; ++idx) {
        if (!server->clients[idx].active) {
            continue;
        }
        if (exclude != nullptr && endpoint_equal(&server->clients[idx].endpoint, exclude)) {
            continue;
        }
        (void)chat__server_send_packet(server, packet, &server->clients[idx].endpoint);
    }
}

static void
chat__server_broadcast_shutdown(chat_server_t * server) {
    chat_packet_t broadcast;

    chat_packet_init(&broadcast);
    broadcast.type = CHAT_PACKET_SYSTEM;
    (void)snprintf(broadcast.sender, sizeof(broadcast.sender), "server");
    (void)snprintf(broadcast.payload, sizeof(broadcast.payload), "server shutting down");
    chat__server_broadcast(server, &broadcast, nullptr);
}

static int
chat__server_validate_name(const char * name) {
    return chat__server_validate_text(name, NAME_BYTES_MAX, NAME_CHAR_MAX);
}

static void
chat__server_prune_idle(chat_server_t * server) {
    uint64_t now = chat__clock_ms();
    size_t idx;

    // UDP 无连接, 服务端只能靠业务层心跳/消息时间来回收“断开”的客户端
    for (idx = 0; idx < server->max_clients; ++idx) {
        chat_packet_t broadcast;

        if (!server->clients[idx].active) {
            continue;
        }
        if (now - server->clients[idx].last_seen_ms < CHAT_IDLE_TIMEOUT_MS) {
            continue;
        }

        chat_packet_init(&broadcast);
        broadcast.type = CHAT_PACKET_SYSTEM;
        (void)snprintf(broadcast.sender, sizeof(broadcast.sender), "server");
        (void)snprintf(broadcast.payload, sizeof(broadcast.payload), "%s timed out", server->clients[idx].name);
        server->clients[idx].active = false;
        chat__server_broadcast(server, &broadcast, nullptr);
    }
}

static void
chat__server_handle_join(chat_server_t * server, const chat_packet_t * packet, const endpoint_t * ep) {
    ptrdiff_t endpoint_idx;
    ptrdiff_t name_idx;
    ptrdiff_t free_idx;
    chat_packet_t broadcast;
    char text[TEXT_BYTES_MAX + 1];
    char addr[64];

    if (chat__server_validate_name(packet->sender) < 0) {
        (void)chat__server_send_system(server, ep, "invalid user name, require valid UTF-8 and max 31 Unicode chars", CHAT_PACKET_ERROR);
        return;
    }

    // 先看是否同一地址重复 join, 避免一个 endpoint 冒充多个用户名
    endpoint_idx = chat__server_find_by_endpoint(server, ep);
    if (endpoint_idx >= 0) {
        chat_client_slot_t * slot = &server->clients[endpoint_idx];
        if (strncmp(slot->name, packet->sender, sizeof(slot->name)) != 0) {
            (void)chat__server_send_system(server, ep, "endpoint already joined with another user name", CHAT_PACKET_ERROR);
            return;
        }
        slot->last_seen_ms = chat__clock_ms();
        (void)chat__server_send_system(server, ep, "join refreshed", CHAT_PACKET_SYSTEM);
        return;
    }

    // 再校验用户名唯一性, 防止不同地址抢同名
    name_idx = chat__server_find_by_name(server, packet->sender);
    if (name_idx >= 0) {
        (void)chat__server_send_system(server, ep, "user name already exists", CHAT_PACKET_ERROR);
        return;
    }

    free_idx = chat__server_alloc_slot(server);
    if (free_idx < 0) {
        (void)chat__server_send_system(server, ep, "server is full", CHAT_PACKET_ERROR);
        return;
    }

    server->clients[free_idx].active = true;
    server->clients[free_idx].last_seen_ms = chat__clock_ms();
    server->clients[free_idx].endpoint = *ep;
    (void)snprintf(server->clients[free_idx].name, sizeof(server->clients[free_idx].name), "%s", packet->sender);

    if (endpoint_to_string(ep, addr, sizeof(addr)) < 0) {
        (void)snprintf(addr, sizeof(addr), "unknown");
    }
    (void)snprintf(text, sizeof(text), "%s joined from %s", packet->sender, addr);
    chat__server_log("info", text);

    (void)chat__server_send_system(server, ep, "join ok", CHAT_PACKET_SYSTEM);

    chat_packet_init(&broadcast);
    broadcast.type = CHAT_PACKET_SYSTEM;
    (void)snprintf(broadcast.sender, sizeof(broadcast.sender), "server");
    (void)snprintf(broadcast.payload, sizeof(broadcast.payload), "%s entered the room", packet->sender);
    chat__server_broadcast(server, &broadcast, ep);
}

static void
chat__server_handle_leave(chat_server_t * server, const endpoint_t * ep) {
    ptrdiff_t idx;
    chat_packet_t broadcast;
    char name[NAME_BYTES_MAX + 1];

    idx = chat__server_find_by_endpoint(server, ep);
    if (idx < 0) {
        (void)chat__server_send_system(server, ep, "leave ignored, client not joined", CHAT_PACKET_ERROR);
        return;
    }

    (void)snprintf(name, sizeof(name), "%s", server->clients[idx].name);
    server->clients[idx].active = false;

    chat_packet_init(&broadcast);
    broadcast.type = CHAT_PACKET_SYSTEM;
    (void)snprintf(broadcast.sender, sizeof(broadcast.sender), "server");
    (void)snprintf(broadcast.payload, sizeof(broadcast.payload), "%s left the room", name);
    chat__server_broadcast(server, &broadcast, nullptr);
}

static void
chat__server_handle_chat(chat_server_t * server, const chat_packet_t * packet, const endpoint_t * ep) {
    ptrdiff_t idx;
    chat_packet_t relay;

    idx = chat__server_find_by_endpoint(server, ep);
    if (idx < 0) {
        (void)chat__server_send_system(server, ep, "please join first", CHAT_PACKET_ERROR);
        return;
    }
    // sender 必须和服务端登记名一致, 防止已加入客户端伪造他人身份
    if (strncmp(server->clients[idx].name, packet->sender, sizeof(server->clients[idx].name)) != 0) {
        (void)chat__server_send_system(server, ep, "sender mismatch", CHAT_PACKET_ERROR);
        return;
    }
    if (packet->payload[0] == '\0') {
        (void)chat__server_send_system(server, ep, "empty message ignored", CHAT_PACKET_ERROR);
        return;
    }
    if (chat__server_validate_text(packet->payload, TEXT_BYTES_MAX, TEXT_CHAR_MAX) < 0) {
        (void)chat__server_send_system(server, ep, "invalid message, require valid UTF-8 and max 512 Unicode chars", CHAT_PACKET_ERROR);
        return;
    }

    server->clients[idx].last_seen_ms = chat__clock_ms();

    chat_packet_init(&relay);
    relay.type = CHAT_PACKET_CHAT;
    (void)snprintf(relay.sender, sizeof(relay.sender), "%s", packet->sender);
    (void)snprintf(relay.payload, sizeof(relay.payload), "%s", packet->payload);
    chat__server_broadcast(server, &relay, nullptr);
}

static void
chat__server_handle_ping(chat_server_t * server, const chat_packet_t * packet, const endpoint_t * ep) {
    ptrdiff_t idx;
    chat_packet_t pong;

    idx = chat__server_find_by_endpoint(server, ep);
    if (idx >= 0) {
        server->clients[idx].last_seen_ms = chat__clock_ms();
    }

    chat_packet_init(&pong);
    pong.type = CHAT_PACKET_PONG;
    (void)snprintf(pong.sender, sizeof(pong.sender), "server");
    (void)snprintf(pong.payload, sizeof(pong.payload), "%s", packet->payload);
    (void)chat__server_send_packet(server, &pong, ep);
}

int
chat_server_run(const chat_server_config_t * cfg) {
    chat_server_t server = {};
    char wire[RECV_MAX + 1];
    int rc;
    bool socket_started = false;
    bool stop_handler_installed = false;

    if (cfg == nullptr || cfg->bind_port == 0) {
        return -EINVAL;
    }

    platform_init_console();
    rc = chat__server_install_stop_handler();
    if (rc < 0) {
        fprintf(stderr, "[server][error] install stop handler failed\n");
        fflush(stderr);
        return rc;
    }
    stop_handler_installed = true;

    server.fd = INVALID_SOCKET;
    server.max_clients = cfg->max_clients == 0 ? CLIENT_MAX : cfg->max_clients;
    if (server.max_clients > CLIENT_MAX) {
        server.max_clients = CLIENT_MAX;
    }

    server.clients = (chat_client_slot_t *)calloc(server.max_clients, sizeof(*server.clients));
    if (server.clients == nullptr) {
        rc = -ENOMEM;
        goto cleanup;
    }

    rc = socket_startup();
    if (rc < 0) {
        goto cleanup;
    }
    socket_started = true;

    rc = socket_open(&server.fd);
    if (rc < 0) {
        goto cleanup;
    }

    (void)socket_set_reuseaddr(server.fd, true);

    rc = socket_bind_ipv4(server.fd, cfg->bind_ip, cfg->bind_port);
    if (rc < 0) {
        char errbuf[128];
        fprintf(stderr, "[server][error] bind failed: %s\n", socket_error_string(rc, errbuf, sizeof(errbuf)));
        fflush(stderr);
        goto cleanup;
    }

    fprintf(stdout, "[server][info] listen on %s:%u max_clients=%u\n",
        (cfg->bind_ip != nullptr && cfg->bind_ip[0] != '\0') ? cfg->bind_ip : "0.0.0.0",
        (unsigned)cfg->bind_port,
        (unsigned)server.max_clients
    );
    fflush(stdout);

    rc = 0;
    for (;;) {
        endpoint_t from;
        chat_packet_t packet;
        int wait_rc;
        int nread;

        if (chat__server_stop_requested()) {
            chat__server_log("info", "shutdown requested");
            break;
        }

        wait_rc = socket_wait_readable(server.fd, CHAT_SERVER_RECV_TIMEOUT_MS);
        if (wait_rc < 0) {
            if (chat__server_stop_requested()) {
                chat__server_log("info", "shutdown requested");
                break;
            }
            char errbuf[128];
            fprintf(stderr, "[server][error] wait failed: %s\n", socket_error_string(wait_rc, errbuf, sizeof(errbuf)));
            fflush(stderr);
            continue;
        }
        if (wait_rc == 0) {
            chat__server_prune_idle(&server);
            continue;
        }

        nread = socket_recvfrom(server.fd, wire, RECV_MAX, &from);
        if (nread < 0) {
            int err = -nread;
            if (chat__server_stop_requested()) {
                chat__server_log("info", "shutdown requested");
                break;
            }
            {
                char errbuf[128];
                fprintf(stderr, "[server][error] recvfrom failed: %s\n", socket_error_string(err, errbuf, sizeof(errbuf)));
                fflush(stderr);
            }
            continue;
        }

        wire[nread] = '\0';
        rc = chat_packet_decode(&packet, wire, (size_t)nread);
        if (rc < 0) {
            chat__server_log_addr("warn", "drop invalid packet from ", &from);
            (void)chat__server_send_system(&server, &from, "invalid packet", CHAT_PACKET_ERROR);
            continue;
        }

        switch (packet.type) {
        case CHAT_PACKET_JOIN:
            chat__server_handle_join(&server, &packet, &from);
            break;
        case CHAT_PACKET_LEAVE:
            chat__server_handle_leave(&server, &from);
            break;
        case CHAT_PACKET_CHAT:
            chat__server_handle_chat(&server, &packet, &from);
            break;
        case CHAT_PACKET_PING:
            chat__server_handle_ping(&server, &packet, &from);
            break;
        default:
            (void)chat__server_send_system(&server, &from, "unsupported packet type", CHAT_PACKET_ERROR);
            break;
        }
    }

    if (server.fd != INVALID_SOCKET) {
        chat__server_broadcast_shutdown(&server);
    }
    chat__server_log("info", "server stopped");

cleanup:
    if (server.fd != INVALID_SOCKET) {
        socket_close(server.fd);
        server.fd = INVALID_SOCKET;
    }
    if (socket_started) {
        socket_cleanup();
    }
    free(server.clients);
    if (stop_handler_installed) {
        chat__server_uninstall_stop_handler();
    }
    return rc;
}


