#include <stdio.h>
#include <stdlib.h>

#include "socket.h"
#include "socket_buffer.h"

int main(void) {
    int status;

    // 尝试 bind 服务
    socket_t server_fd = socket_listen(NULL, 9527, 8);
    if (server_fd == INVALID_SOCKET) {
        EXIT("socket_listen is failed");
    }

    // 输出 socket 相关地址信息
    sockaddr_t sa;
    sa->len = sizeof(sa);
    status = getsockname(server_fd, &sa->u.s, &sa->len);
    if (status < 0) {
        EXIT("getsockname is failed");
    }

    // 打印地址信息
    char ip[INET6_ADDRSTRLEN];
    uint8_t uport = sa->len == sizeof(sa->u.v4) ? sa->u.v4.sin_port : sa->u.v6.sin6_port;
    printf("ip = %s, port = %hu\n", socket_ntop(sa, ip), ntohs(uport));

    socket_close(server_fd);

    exit(EXIT_FAILURE);
}
