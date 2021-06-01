#pragma once

#include <sys/types.h>

#include <time.h>
#include <fcntl.h>
#include <signal.h>

#include "struct.h"

#if defined __linux__

#include <netdb.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include <sys/un.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/resource.h>

// This is used instead of -1, since the. by WinSock
// On now linux EAGAIN and EWOULDBLOCK may be the same value 
// connect 链接中, linux 是 EINPROGRESS，winds 是 WSAEWOULDBLOCK
//
typedef int             socket_t;

#define INVALID_SOCKET  (~0)
#define SOCKET_ERROR    (-1)

// socket_init - 初始化 socket 库初始化方法
inline void socket_init(void) {
    // 防止管道破裂, 忽略 SIGPIPE 信号
    signal(SIGPIPE, SIG_IGN);
    // 防止会话关闭, 忽略 SIGHUP  信号
    signal(SIGHUP , SIG_IGN);
}

inline int socket_close(socket_t s) {
    return close(s);
}

// socket_set_block - 设置套接字是阻塞
inline static int socket_set_block(socket_t s) {
    int mode = fcntl(s, F_GETFL);
    if (mode == SOCKET_ERROR) {
        return SOCKET_ERROR;
    }
    return fcntl(s, F_SETFL, mode & ~O_NONBLOCK);
}

// socket_set_nonblock - 设置套接字是非阻塞
inline int socket_set_nonblock(socket_t s) {
    int mode = fcntl(s, F_GETFL);
    if (mode == SOCKET_ERROR) {
        return SOCKET_ERROR;
    }
    return fcntl(s, F_SETFL, mode | O_NONBLOCK);
}

#endif

union sockaddr_all {
	struct sockaddr s;
	struct sockaddr_in v4;
	struct sockaddr_in6 v6;
};

// sockaddr_t 为 socket 默认通用地址结构
typedef struct {
    union sockaddr_all u;
    socklen_t len;
} sockaddr_t[1];

// socket_recv - 读取数据
inline int socket_recv(socket_t s, void * buf, int sz) {
    return sz > 0 ? (int)recv(s, buf, sz, 0) : 0;
}

// socket_send - 写入数据
inline int socket_send(socket_t s, const void * buf, int sz) {
    return (int)send(s, buf, sz, 0);
}

// socket_dgram - 创建 UDP socket
inline socket_t socket_dgram(void) {
    return socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
}

// socket_stream - 创建 TCP socket
inline socket_t socket_stream(void) {
    return socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
}

inline int socket_set_enable(socket_t s, int optname) {
    int ov = 1;
    return setsockopt(s, SOL_SOCKET, optname, (void *)&ov, sizeof ov);
}

// socket_set_reuse - 开启端口和地址复用
inline int socket_set_reuse(socket_t s) {
    return socket_set_enable(s, SO_REUSEPORT);
}

// socket_set_keepalive - 开启心跳包检测, 默认 5 次/2h
inline int socket_set_keepalive(socket_t s) {
    return socket_set_enable(s, SO_KEEPALIVE);
}

inline int socket_set_time(socket_t s, int ms, int optname) {
    struct timeval ov = { 0,0 };
    if (ms > 0) {
        ov.tv_sec = ms / 1000;
        ov.tv_usec = (ms % 1000) * 1000;
    }
    return setsockopt(s, SOL_SOCKET, optname, (void *)&ov, sizeof ov);
}

// socket_set_rcvtimeo - 设置接收数据毫秒超时时间
inline int socket_set_rcvtimeo(socket_t s, int ms) {
    return socket_set_time(s, ms, SO_RCVTIMEO);
}

// socket_set_sndtimeo - 设置发送数据毫秒超时时间
inline int socket_set_sndtimeo(socket_t s, int ms) {
    return socket_set_time(s, ms, SO_SNDTIMEO);
}

// socket_get_error - 获取 socket error 值, 0 正确, 其它都是 error
inline int socket_get_error(socket_t s) {
    int err, no = errno;
    socklen_t len = sizeof(err);
    return getsockopt(s, SOL_SOCKET, SO_ERROR, (void *)&err, &len) ? no : err;
}

// socket_ntop - 点分十进制转 ip 串
extern char * socket_ntop(sockaddr_t a, char ip[INET6_ADDRSTRLEN]);

// socket_recvn - socket 接受 sz 个字节
extern int socket_recvn(socket_t s, void * buf, int sz);

// socket_sendn - socket 发送 sz 个字节
extern int socket_sendn(socket_t s, const void * buf, int sz);

// socket_bind - 返回绑定好端口的 socket fd, family return PF_INET PF_INET6
extern socket_t socket_bind(const char * ip, uint16_t port, uint8_t protocol, int * family);

// socket_listen - 返回监听好的 socket fd
extern socket_t socket_listen(const char * ip, uint16_t port, int backlog);

// socket_connect - 返回链接后的阻塞套接字
extern socket_t socket_connect(char ip[INET6_ADDRSTRLEN], uint16_t port);

// socket_connectms - 尝试在固定时间内返回链接后的非阻塞套接字
extern socket_t socket_connectms(char ip[INET6_ADDRSTRLEN], uint16_t port, int ms);
