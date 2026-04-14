#include "chat_client.h"
#include "utf8.h"
#include <stdatomic.h>

#if defined(_WIN32)
static volatile LONG chat__client_stop_flag = 0;
static HANDLE chat__client_main_thread = nullptr;

static BOOL WINAPI
chat__client_console_handler(DWORD ctrl_type) {
    switch (ctrl_type) {
    case CTRL_C_EVENT:
    case CTRL_BREAK_EVENT:
    case CTRL_CLOSE_EVENT:
    case CTRL_SHUTDOWN_EVENT:
        InterlockedExchange(&chat__client_stop_flag, 1);
        if (chat__client_main_thread != nullptr) {
            (void)CancelSynchronousIo(chat__client_main_thread);
        }
        return TRUE;
    default:
        return FALSE;
    }
}

static int
chat__client_install_stop_handler(void) {
    InterlockedExchange(&chat__client_stop_flag, 0);
    chat__client_main_thread = nullptr;
    if (!DuplicateHandle(
            GetCurrentProcess(),
            GetCurrentThread(),
            GetCurrentProcess(),
            &chat__client_main_thread,
            0,
            FALSE,
            DUPLICATE_SAME_ACCESS
        )) {
        return -EIO;
    }
    if (!SetConsoleCtrlHandler(chat__client_console_handler, TRUE)) {
        CloseHandle(chat__client_main_thread);
        chat__client_main_thread = nullptr;
        return -EIO;
    }
    return 0;
}

static void
chat__client_uninstall_stop_handler(void) {
    (void)SetConsoleCtrlHandler(chat__client_console_handler, FALSE);
    if (chat__client_main_thread != nullptr) {
        CloseHandle(chat__client_main_thread);
        chat__client_main_thread = nullptr;
    }
}

static bool
chat__client_stop_requested(void) {
    return InterlockedCompareExchange((volatile LONG *)&chat__client_stop_flag, 0, 0) != 0;
}
#else
static volatile sig_atomic_t chat__client_stop_flag = 0;
static struct sigaction chat__client_prev_sigint = {};
static struct sigaction chat__client_prev_sigterm = {};
static bool chat__client_stop_handler_installed = false;

static void
chat__client_signal_handler(int signo) {
    (void)signo;
    chat__client_stop_flag = 1;
}

static int
chat__client_install_stop_handler(void) {
    struct sigaction sa = {};

    sa.sa_handler = chat__client_signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    chat__client_stop_flag = 0;

    if (sigaction(SIGINT, &sa, &chat__client_prev_sigint) != 0) {
        return -errno;
    }
    if (sigaction(SIGTERM, &sa, &chat__client_prev_sigterm) != 0) {
        int err = errno;

        (void)sigaction(SIGINT, &chat__client_prev_sigint, nullptr);
        return -err;
    }

    chat__client_stop_handler_installed = true;
    return 0;
}

static void
chat__client_uninstall_stop_handler(void) {
    if (!chat__client_stop_handler_installed) {
        return;
    }

    (void)sigaction(SIGINT, &chat__client_prev_sigint, nullptr);
    (void)sigaction(SIGTERM, &chat__client_prev_sigterm, nullptr);
    chat__client_stop_handler_installed = false;
}

static bool
chat__client_stop_requested(void) {
    return chat__client_stop_flag != 0;
}
#endif

typedef struct chat_client_runtime {
    // 本地 UDP socket
    socket_t fd;
    // 目标服务端地址, 客户端仅接受该地址返回的数据
    endpoint_t server;
    // stdin 主线程和接收线程共享发送路径时的保护锁
    mutex_t send_mutex;
    // 线程退出标记
    atomic_bool running;
    // 当前用户名, 允许 UTF-8
    char name[NAME_BYTES_MAX + 1];
} chat_client_runtime_t;

enum {
    // 客户端接收线程轮询等待间隔, 兼顾退出响应速度与空闲 CPU 占用
    CHAT_CLIENT_RECV_WAIT_MS = 500,
};

static int
chat__client_validate_text(const char * text, size_t bytes_max, size_t chars_max) {
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

static void
udp__trim_eol(char * text) {
    size_t text_len;

    if (text == nullptr) {
        return;
    }

    text_len = strlen(text);
    while (text_len > 0 && (text[text_len - 1] == '\n' || text[text_len - 1] == '\r')) {
        text[text_len - 1] = '\0';
        --text_len;
    }
}

static void
udp__drain_stdin_line(void) {
    int ch;

    do {
        ch = getchar();
    } while (ch != '\n' && ch != '\r' && ch != EOF);
}

static int
chat__client_send_packet(chat_client_runtime_t * rt, uint32_t type, const char * payload) {
    chat_packet_t packet;
    char wire[PACKET_MAX];
    int wire_len;
    int rc;

    chat_packet_init(&packet);
    packet.type = type;
    (void)snprintf(packet.sender, sizeof(packet.sender), "%s", rt->name);
    (void)snprintf(packet.payload, sizeof(packet.payload), "%s", payload != nullptr ? payload : "");

    wire_len = chat_packet_encode(&packet, wire, sizeof(wire));
    if (wire_len < 0) {
        return wire_len;
    }

    // 当前只有主线程发送, 这里仍保留锁, 方便后续扩展心跳线程/重发线程
    mutex_lock(&rt->send_mutex);
    rc = socket_sendto(rt->fd, wire, (size_t)wire_len, &rt->server);
    mutex_unlock(&rt->send_mutex);
    return rc;
}

static void
chat__client_print_packet(const chat_packet_t * packet) {
    if (packet == nullptr) {
        return;
    }

    switch (packet->type) {
    case CHAT_PACKET_CHAT:
        fprintf(stdout, "[chat][%s] %s\n", packet->sender, packet->payload);
        fflush(stdout);
        break;
    case CHAT_PACKET_SYSTEM:
        fprintf(stdout, "[system] %s\n", packet->payload);
        fflush(stdout);
        break;
    case CHAT_PACKET_ERROR:
        fprintf(stderr, "[error] %s\n", packet->payload);
        fflush(stderr);
        break;
    case CHAT_PACKET_PONG:
        fprintf(stdout, "[pong] %s\n", packet->payload);
        fflush(stdout);
        break;
    default:
        fprintf(stdout, "[%s][%s] %s\n",
            chat_packet_type_name(packet->type),
            packet->sender,
            packet->payload
        );
        fflush(stdout);
        break;
    }
}

static void *
chat__client_recv_loop(void * arg) {
    chat_client_runtime_t * rt = (chat_client_runtime_t *)arg;
    char wire[RECV_MAX + 1];

    // 独立接收线程负责打印服务端广播, 主线程只处理 stdin
    while (atomic_load(&rt->running)) {
        endpoint_t from;
        int wait_rc = socket_wait_readable(rt->fd, CHAT_CLIENT_RECV_WAIT_MS);
        int nread;

        if (wait_rc < 0) {
            if (!atomic_load(&rt->running)) {
                break;
            }
            {
                char errbuf[128];
                fprintf(stderr, "[client][error] wait failed: %s\n", socket_error_string(wait_rc, errbuf, sizeof(errbuf)));
                fflush(stderr);
            }
            continue;
        }
        if (wait_rc == 0) {
            continue;
        }

        nread = socket_recvfrom(rt->fd, wire, RECV_MAX, &from);
        if (nread < 0) {
            int err = -nread;
            if (!atomic_load(&rt->running)) {
                break;
            }
            {
                char errbuf[128];
                fprintf(stderr, "[client][error] recv failed: %s\n", socket_error_string(err, errbuf, sizeof(errbuf)));
                fflush(stderr);
            }
            continue;
        }

        // UDP 天然可被任意源地址发包, 这里主动丢弃非目标服务端数据
        if (!endpoint_equal(&from, &rt->server)) {
            fprintf(stderr, "[client][warn] drop packet from unexpected peer\n");
            fflush(stderr);
            continue;
        }

        wire[nread] = '\0';
        {
            chat_packet_t packet;
            int rc = chat_packet_decode(&packet, wire, (size_t)nread);
            if (rc < 0) {
                fprintf(stderr, "[client][warn] drop invalid packet\n");
                fflush(stderr);
                continue;
            }
            chat__client_print_packet(&packet);
        }
    }
    return nullptr;
}

int
chat_client_run(const chat_client_config_t * cfg) {
    chat_client_runtime_t rt = {};
    thread_t recv_thread = {};
    char input[TEXT_BYTES_MAX + 64];
    int rc;
    bool socket_started = false;
    bool mutex_inited = false;
    bool recv_thread_started = false;
    bool stop_handler_installed = false;
    bool joined = false;
    bool leave_sent = false;

    if (cfg == nullptr || cfg->server_host == nullptr || cfg->name == nullptr || cfg->server_port == 0) {
        return -EINVAL;
    }
    rc = chat__client_validate_text(cfg->name, NAME_BYTES_MAX, NAME_CHAR_MAX);
    if (rc < 0) {
        return rc;
    }

    platform_init_console();
    rt.fd = INVALID_SOCKET;
    atomic_init(&rt.running, true);
    (void)snprintf(rt.name, sizeof(rt.name), "%s", cfg->name);

    rc = chat__client_install_stop_handler();
    if (rc < 0) {
        fprintf(stderr, "[client][error] install stop handler failed\n");
        fflush(stderr);
        return rc;
    }
    stop_handler_installed = true;

    rc = socket_startup();
    if (rc < 0) {
        goto cleanup;
    }
    socket_started = true;

    rc = socket_open(&rt.fd);
    if (rc < 0) {
        goto cleanup;
    }

    if (cfg->local_port != 0) {
        rc = socket_bind_ipv4(rt.fd, "0.0.0.0", cfg->local_port);
        if (rc < 0) {
            char errbuf[128];
            fprintf(stderr, "[client][error] bind failed: %s\n", socket_error_string(rc, errbuf, sizeof(errbuf)));
            fflush(stderr);
            goto cleanup;
        }
    }

    rc = socket_resolve_ipv4(cfg->server_host, cfg->server_port, &rt.server);
    if (rc < 0) {
        char errbuf[128];
        fprintf(stderr, "[client][error] resolve failed: %s\n", socket_error_string(rc, errbuf, sizeof(errbuf)));
        fflush(stderr);
        goto cleanup;
    }

    rc = mutex_init(&rt.send_mutex);
    if (rc < 0) {
        goto cleanup;
    }
    mutex_inited = true;

    rc = thread_start(&recv_thread, chat__client_recv_loop, &rt);
    if (rc < 0) {
        goto cleanup;
    }
    recv_thread_started = true;

    rc = chat__client_send_packet(&rt, CHAT_PACKET_JOIN, "");
    if (rc < 0) {
        char errbuf[128];
        fprintf(stderr, "[client][error] join failed: %s\n", socket_error_string(rc, errbuf, sizeof(errbuf)));
        fflush(stderr);
        goto cleanup;
    }
    joined = true;

    fprintf(stdout, "[client][info] commands: /quit /ping, other text will be sent as chat message\n");
    fflush(stdout);
    while (atomic_load(&rt.running) && !chat__client_stop_requested() && fgets(input, sizeof(input), stdin) != nullptr) {
        size_t input_len = strlen(input);

        if (input_len == sizeof(input) - 1 && input[input_len - 1] != '\n') {
            fprintf(stderr, "[client][warn] input too long, remaining characters dropped\n");
            fflush(stderr);
            udp__drain_stdin_line();
        }

        udp__trim_eol(input);
        if (input[0] == '\0') {
            continue;
        }

        if (strcmp(input, "/quit") == 0) {
            (void)chat__client_send_packet(&rt, CHAT_PACKET_LEAVE, "");
            leave_sent = true;
            break;
        }
        if (strcmp(input, "/ping") == 0) {
            (void)chat__client_send_packet(&rt, CHAT_PACKET_PING, "ping");
            continue;
        }

        rc = chat__client_validate_text(input, TEXT_BYTES_MAX, TEXT_CHAR_MAX);
        if (rc < 0) {
            fprintf(stderr, "[client][error] invalid message, require valid UTF-8 and max 512 Unicode chars\n");
            fflush(stderr);
            continue;
        }

        rc = chat__client_send_packet(&rt, CHAT_PACKET_CHAT, input);
        if (rc < 0) {
            char errbuf[128];
            fprintf(stderr, "[client][error] send failed: %s\n", socket_error_string(rc, errbuf, sizeof(errbuf)));
            fflush(stderr);
            break;
        }
    }

    if (chat__client_stop_requested()) {
        fprintf(stdout, "[client][info] shutdown requested\n");
        fflush(stdout);
    }

cleanup:
    if (joined && !leave_sent) {
        (void)chat__client_send_packet(&rt, CHAT_PACKET_LEAVE, "");
    }
    atomic_store(&rt.running, false);
    if (recv_thread_started) {
        thread_join(recv_thread);
    }
    if (mutex_inited) {
        mutex_destroy(&rt.send_mutex);
    }
    if (rt.fd != INVALID_SOCKET) {
        socket_close(rt.fd);
        rt.fd = INVALID_SOCKET;
    }
    if (socket_started) {
        socket_cleanup();
    }
    if (stop_handler_installed) {
        chat__client_uninstall_stop_handler();
    }
    return rc;
}


