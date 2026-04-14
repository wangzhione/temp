#include "socketx.h"

static bool
socket__error_is_intr(int err) {
#if defined(_WIN32)
    (void)err;
    return false;
#else
    return err == EINTR;
#endif
}

#if !defined(_WIN32)
static uint64_t
socket__monotonic_ms(void) {
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000ULL + (uint64_t)(ts.tv_nsec / 1000000ULL);
}
#endif

void
platform_init_console(void) {
#if defined(_WIN32)
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
}

static int
socket__setsockopt_bool(socket_t fd, int level, int name, bool enable) {
    int value = enable ? 1 : 0;
    return setsockopt(fd, level, name, (const char *)&value, (socklen_t)sizeof(value));
}

int
socket_startup(void) {
#if defined(_WIN32)
    WSADATA wsa;
    {
        int rc = WSAStartup(MAKEWORD(2, 2), &wsa);
        return rc == 0 ? 0 : -rc;
    }
#else
    signal(SIGPIPE, SIG_IGN);
    return 0;
#endif
}

void
socket_cleanup(void) {
#if defined(_WIN32)
    WSACleanup();
#endif
}

int
socket_last_error(void) {
#if defined(_WIN32)
    return WSAGetLastError();
#else
    return errno;
#endif
}

const char *
socket_error_string(int err, char * buf, size_t buf_len) {
    int err_code;

    if (buf == nullptr || buf_len == 0) {
        return "";
    }

    err_code = err < 0 ? -err : err;

#if defined(_WIN32)
    DWORD flags = FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
    DWORD len = FormatMessageA(
        flags,
        nullptr,
        (DWORD)err_code,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        buf,
        (DWORD)buf_len,
        nullptr
    );
    if (len == 0) {
        (void)snprintf(buf, buf_len, "winsock error %d", err_code);
    }
#else
    const char * msg = strerror(err_code);
    if (msg == nullptr) {
        msg = "unknown error";
    }
    (void)snprintf(buf, buf_len, "%s", msg);
#endif

    buf[buf_len - 1] = '\0';
    return buf;
}

int
socket_open(socket_t * fd) {
    if (fd == nullptr) {
        return -EINVAL;
    }

    *fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (*fd == INVALID_SOCKET) {
        return -socket_last_error();
    }
    return 0;
}

void
socket_close(socket_t fd) {
    if (fd == INVALID_SOCKET) {
        return;
    }

#if defined(_WIN32)
    closesocket(fd);
#else
    close(fd);
#endif
}

int
socket_bind_ipv4(socket_t fd, const char * ip, uint16_t port) {
    struct sockaddr_in addr = {};

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (ip == nullptr || ip[0] == '\0' || strcmp(ip, "0.0.0.0") == 0) {
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
    } else if (inet_pton(AF_INET, ip, &addr.sin_addr) != 1) {
        return -EINVAL;
    }

    if (bind(fd, (const struct sockaddr *)&addr, (socklen_t)sizeof(addr)) != 0) {
        return -socket_last_error();
    }
    return 0;
}

int
socket_set_reuseaddr(socket_t fd, bool enable) {
    if (socket__setsockopt_bool(fd, SOL_SOCKET, SO_REUSEADDR, enable) != 0) {
        return -socket_last_error();
    }
    return 0;
}

int
socket_set_recv_timeout(socket_t fd, int timeout_ms) {
    if (timeout_ms < 0) {
        return -EINVAL;
    }

#if defined(_WIN32)
    DWORD value = (DWORD)timeout_ms;
    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&value, (socklen_t)sizeof(value)) != 0) {
        return -socket_last_error();
    }
#else
    struct timeval tv;
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;
    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, (socklen_t)sizeof(tv)) != 0) {
        return -socket_last_error();
    }
#endif
    return 0;
}

int
socket_wait_readable(socket_t fd, int timeout_ms) {
    fd_set readfds;
    int rc;
#if !defined(_WIN32)
    uint64_t deadline_ms;
#endif

    if (fd == INVALID_SOCKET || timeout_ms < 0) {
        return -EINVAL;
    }

#if !defined(_WIN32)
    deadline_ms = socket__monotonic_ms() + (uint64_t)timeout_ms;
#endif

    for (;;) {
        struct timeval tv;

        FD_ZERO(&readfds);
        FD_SET(fd, &readfds);

#if defined(_WIN32)
        tv.tv_sec = timeout_ms / 1000;
        tv.tv_usec = (timeout_ms % 1000) * 1000;
        rc = select(0, &readfds, nullptr, nullptr, &tv);
#else
        uint64_t now_ms = socket__monotonic_ms();
        int remain_ms;

        if (now_ms >= deadline_ms) {
            return 0;
        }
        remain_ms = (int)(deadline_ms - now_ms);
        tv.tv_sec = remain_ms / 1000;
        tv.tv_usec = (remain_ms % 1000) * 1000;
        rc = select(fd + 1, &readfds, nullptr, nullptr, &tv);
#endif
        if (rc < 0) {
            int err = socket_last_error();
            if (socket__error_is_intr(err)) {
                continue;
            }
            return -err;
        }
        if (rc == 0) {
            return 0;
        }
        return FD_ISSET(fd, &readfds) ? 1 : 0;
    }
}

int
socket_resolve_ipv4(const char * host, uint16_t port, endpoint_t * ep) {
    struct addrinfo hints = {};
    struct addrinfo * result = nullptr;
    struct addrinfo * iter = nullptr;
    char service[16];
    int rc;

    if (host == nullptr || host[0] == '\0' || ep == nullptr) {
        return -EINVAL;
    }

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    (void)snprintf(service, sizeof(service), "%u", (unsigned)port);
    rc = getaddrinfo(host, service, &hints, &result);
    if (rc != 0) {
#if defined(_WIN32)
        return -rc;
#else
        return -EINVAL;
#endif
    }

    for (iter = result; iter != nullptr; iter = iter->ai_next) {
        if (iter->ai_addrlen > sizeof(ep->addr)) {
            continue;
        }
        memset(ep, 0, sizeof(*ep));
        memcpy(&ep->addr, iter->ai_addr, iter->ai_addrlen);
        ep->addr_len = (socklen_t)iter->ai_addrlen;
        freeaddrinfo(result);
        return 0;
    }

    freeaddrinfo(result);
    return -EADDRNOTAVAIL;
}

int
socket_sendto(socket_t fd, const void * buf, size_t buf_len, const endpoint_t * ep) {
    int sent;

    if (buf == nullptr || buf_len == 0 || buf_len > INT_MAX || ep == nullptr || ep->addr_len == 0) {
        return -EINVAL;
    }

    for (;;) {
        sent = sendto(fd, (const char *)buf, (int)buf_len, 0, (const struct sockaddr *)&ep->addr, ep->addr_len);
        if (sent >= 0) {
            break;
        }
        {
            int err = socket_last_error();
            if (socket__error_is_intr(err)) {
                continue;
            }
            return -err;
        }
    }
    if ((size_t)sent != buf_len) {
        return -EMSGSIZE;
    }
    return 0;
}

int
socket_recvfrom(socket_t fd, void * buf, size_t buf_len, endpoint_t * ep) {
    int nread;

    if (buf == nullptr || buf_len == 0 || buf_len > INT_MAX) {
        return -EINVAL;
    }

    for (;;) {
        if (ep != nullptr) {
            socklen_t addr_len = (socklen_t)sizeof(ep->addr);

            memset(ep, 0, sizeof(*ep));
            nread = recvfrom(fd, (char *)buf, (int)buf_len, 0, (struct sockaddr *)&ep->addr, &addr_len);
            if (nread >= 0) {
                ep->addr_len = addr_len;
            }
        } else {
            nread = recvfrom(fd, (char *)buf, (int)buf_len, 0, nullptr, nullptr);
        }

        if (nread >= 0) {
            return nread;
        }
        {
            int err = socket_last_error();
            if (socket__error_is_intr(err)) {
                continue;
            }
            return -err;
        }
    }
}

bool
endpoint_equal(const endpoint_t * lhs, const endpoint_t * rhs) {
    const struct sockaddr_in * la;
    const struct sockaddr_in * ra;

    if (lhs == nullptr || rhs == nullptr) {
        return false;
    }
    if (lhs->addr_len != rhs->addr_len) {
        return false;
    }
    if (lhs->addr.ss_family != rhs->addr.ss_family) {
        return false;
    }
    if (lhs->addr.ss_family != AF_INET) {
        return false;
    }

    la = (const struct sockaddr_in *)&lhs->addr;
    ra = (const struct sockaddr_in *)&rhs->addr;
    return la->sin_port == ra->sin_port && la->sin_addr.s_addr == ra->sin_addr.s_addr;
}

int
endpoint_to_string(const endpoint_t * ep, char * buf, size_t buf_len) {
    const struct sockaddr_in * addr;
    char ip[INET_ADDRSTRLEN];

    if (ep == nullptr || buf == nullptr || buf_len == 0 || ep->addr.ss_family != AF_INET) {
        return -EINVAL;
    }

    addr = (const struct sockaddr_in *)&ep->addr;
    if (inet_ntop(AF_INET, &addr->sin_addr, ip, sizeof(ip)) == nullptr) {
        return -socket_last_error();
    }

    if (snprintf(buf, buf_len, "%s:%u", ip, (unsigned)ntohs(addr->sin_port)) < 0) {
        return -EINVAL;
    }
    buf[buf_len - 1] = '\0';
    return 0;
}


