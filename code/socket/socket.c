#include "socket.h"

inline char * 
socket_ntop(sockaddr_t a, char ip[INET6_ADDRSTRLEN]) {
    if (a->len == sizeof (a->u.v4))
        return (char *)inet_ntop(a->u.v4.sin_family, &a->u.v4.sin_addr, ip, INET_ADDRSTRLEN);
    return (char *)inet_ntop(a->u.v6.sin6_family, &a->u.v6.sin6_addr, ip, INET6_ADDRSTRLEN);
}

// socket_recvn - socket 接受 sz 个字节
int 
socket_recvn(socket_t s, void * buf, int sz) {
    int r, n = sz;
    while (n > 0 && (r = recv(s, buf, n, 0)) != 0 ) {
        if (r == SOCKET_ERROR) {
            if (errno == EINTR)
                continue;
            return SOCKET_ERROR;
        }
        n -= r;
        buf = (char *)buf + r;
    }
    return sz - n;
}

// socket_sendn - socket 发送 sz 个字节
int 
socket_sendn(socket_t s, const void * buf, int sz) {
    int r, n = sz;
    while (n > 0 && (r = send(s, buf, n, 0)) != 0) {
        if (r == SOCKET_ERROR) {
            if (errno == EINTR)
                continue;
            return SOCKET_ERROR;
        }
        n -= r;
        buf = (char *)buf + r;
    }
    return sz - n;
}

socket_t 
socket_binds(const char * ip, uint16_t port, uint8_t protocol, int * family) {
    // 构建 getaddrinfo 请求参数, ipv6 兼容 ipv4
    if (!ip || !*ip) ip = NULL;
    char ports[sizeof "65535"]; sprintf(ports, "%hu", port);
    struct addrinfo * rsp, req = {
        .ai_flags    = AI_PASSIVE,
        .ai_family   = AF_UNSPEC,
        .ai_socktype = protocol == IPPROTO_TCP ? SOCK_STREAM : SOCK_DGRAM,
        .ai_protocol = protocol,
    };
    if (getaddrinfo(ip, ports, &req, &rsp))
        return INVALID_SOCKET;

    socket_t fd = socket(rsp->ai_family, rsp->ai_socktype, 0);
    if (fd == INVALID_SOCKET)
        goto err_free;
    if (socket_set_reuse(fd))
        goto err_close;
    if (bind(fd, rsp->ai_addr, (int)rsp->ai_addrlen))
        goto err_close;

    // Success return ip family
    if (family) *family = rsp->ai_family;
    freeaddrinfo(rsp);
    return fd;

err_close:
    socket_close(fd);
err_free:
    freeaddrinfo(rsp);
    return INVALID_SOCKET;
}

socket_t 
socket_listen(const char * ip, uint16_t port, int backlog) {
    socket_t fd = socket_binds(ip, port, IPPROTO_TCP, NULL);
    if (INVALID_SOCKET != fd && listen(fd, backlog)) {
        socket_close(fd);
        return INVALID_SOCKET;
    }
    return fd;
}

// socket_addr - 通过 ip, port 构造 ipv4 结构
int socket_addr(char ip[INET6_ADDRSTRLEN], uint16_t port, sockaddr_t a) {
    
    // 尝试转 v4 和 v6 地址
    if (ip != NULL) {
        if (*ip == 0)
            ip = NULL;
        else {
            union {
                struct in_addr s4;
                struct in6_addr s6;
            } s;
            if (inet_pton(AF_INET, ip, &s) != -1) {
                memset(a, 0, sizeof(sockaddr_t));
                a->u.v4.sin_family = AF_INET;
                a->u.v4.sin_addr = s.s4;
                a->u.v4.sin_port = htons(port);
                a->len = sizeof(a->u.v4);
                return 0;
            }
            if (inet_pton(AF_INET6, ip, &s) != -1) {
                memset(a, 0, sizeof(sockaddr_t));
                a->u.v6.sin6_family = AF_INET6;
                a->u.v6.sin6_addr = s.s6;
                a->u.v6.sin6_port = htons(port);
                a->len = sizeof(a->u.v6);
                return 0;
            }
        }
    } 

    struct addrinfo * rsp, req = {
        .ai_family   = AF_UNSPEC,
        .ai_socktype = SOCK_STREAM,
    };

    char * prots = NULL;
    char temp[sizeof "65535"]; 
    if (port > 0) {
        sprintf(temp, "%hu", port);
        prots = temp;
    } 
    if (getaddrinfo(ip, prots, &req, &rsp)) {
        return -1;
    }

    // 只尝试默认第一个
    memset(a, 0, sizeof(sockaddr_t));
    memcpy(&a->u, rsp->ai_addr, rsp->ai_addrlen);
    a->len = rsp->ai_addrlen;
    
    freeaddrinfo(rsp);

    return 0;
}


socket_t 
socket_connect(char ip[INET6_ADDRSTRLEN], uint16_t port) {
    sockaddr_t a;
    // 先解析 sockaddr 地址
    if (socket_addr(ip, port, a) < 0)
        return INVALID_SOCKET;

    // 获取 tcp socket 尝试 parse connect
    socket_t s = socket_stream();
    if (s != INVALID_SOCKET) {
        if (connect(s, &a->u.s, a->len) >= 0)
            return s;

        socket_close(s);
    }

    RETURN(INVALID_SOCKET, "ip = %s, port = %hu", ip, port);
}

// socket_connectm - connect 带超时的链接, 返回非阻塞 socket
static int socket_connectm(socket_t s, const sockaddr_t a, int ms) {
    int n, r;
    struct timeval timeout;
    fd_set rset, wset, eset;

    // 还是阻塞的connect
    if (ms < 0) return connect(s, &a->u.s, a->len);

    // 非阻塞登录, 先设置非阻塞模式
    r = socket_set_nonblock(s);
    if (r < 0) return r;

    // 尝试连接, connect 返回 -1 并且 errno == EINPROGRESS 表示正在建立链接
    r = connect(s, &a->u.s, a->len);
    // connect 链接中, linux 是 EINPROGRESS，winds 是 WSAEWOULDBLOCK
    if (r >= 0 || errno != EINPROGRESS) return r;

    // 超时 timeout, 直接返回结果 -1 错误
    if (ms == 0) return -1;

    FD_ZERO(&rset); FD_SET(s, &rset);
    FD_ZERO(&wset); FD_SET(s, &wset);
    FD_ZERO(&eset); FD_SET(s, &eset);
    timeout.tv_sec = ms / 1000;
    timeout.tv_usec = (ms % 1000) * 1000;
    n = select((int)s + 1, &rset, &wset, &eset, &timeout);
    // 超时返回错误, 防止客户端继续三次握手
    if (n <= 0) return -1;

    // 当连接成功时候,描述符会变成可写
    if (n == 1 && FD_ISSET(s, &wset)) return 0;

    // 当连接建立遇到错误时候, 描述符变为即可读又可写
    if (FD_ISSET(s, &eset) || n == 2) {
        // 只要最后没有 error 那就链接成功
        if (!socket_get_error(s))
            r = 0;
    }

    return r;
}

socket_t 
socket_connectms(char ip[INET6_ADDRSTRLEN], uint16_t port, int ms) {
    sockaddr_t a;
    // 先解析 sockaddr 地址
    if (socket_addr(ip, port, a) < 0)
        return INVALID_SOCKET;

    // 获取 tcp socket 尝试 parse connect
    socket_t s = socket_stream();
    if (s != INVALID_SOCKET) {
        if (socket_connectm(s, a, ms) >= 0) {
            return s;
        }

        socket_close(s);
    }

    RETURN(INVALID_SOCKET, "ip = %s, port = %hu, ms = %d", ip, port, ms);
}
