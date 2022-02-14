#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>

//
// pipe - 移植 linux 函数, 通过 WinSock 实现
// pipefd   : 索引 0 表示 recv fd, 1 是 send fd
// return   : 0 is success -1 is error returned
//
int pipe(int pipefd[2]) {
    struct sockaddr_in6 name;
    socklen_t len = sizeof(struct sockaddr_in6);

    int s = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    if (s == -1) {
        return -1;
    }
        
    // 默认走 :: in6addr_any 默认地址
    memset(&name, 0, sizeof(struct sockaddr_in6));
    name.sin6_family = AF_INET6;
    // 绑定默认网卡, 多平台上更容易 connect success
    name.sin6_addr = in6addr_loopback;

    if (bind(s, (struct sockaddr *)&name, len)) 
        goto err_close;

    if (listen(s, 1))
        goto err_close;

    // 得到绑定端口和本地地址
    if (getsockname(s, (struct sockaddr *)&name, &len))
        goto err_close;

    // 开始构建互相通信的 socket
    pipefd[0]= socket(name.sin6_family, SOCK_STREAM, IPPROTO_TCP);
    if (pipefd[0] == -1) 
        goto err_close;

    if (connect(pipefd[0], (struct sockaddr *)&name, len))
        goto err_pipe;

    // 通过 accept 通信避免一些意外
    pipefd[1] = accept(s, (struct sockaddr *)&name, &len);
    if (pipefd[1] == -1) 
        goto err_pipe;

    close(s);
    return 0;
err_pipe:
    close(pipefd[0]);
err_close:
    close(s);
    return -1;
}

// socket_set_nonblock - 设置套接字是非阻塞
inline static int socket_set_nonblock(int s) {
    int mode = fcntl(s, F_GETFL);
    if (mode == -1) {
        return -1;
    }
    return fcntl(s, F_SETFL, mode | O_NONBLOCK);
}

/*
  build:
  gcc -g -O3 -Wall -Wextra -Werror -o recv_zero recv_zero.c
 */
int main(void) {
    int n;
    int len = 0;
    char buf[256];

    // read fd 测试没有问题
    n = read(STDIN_FILENO, buf, len);
    if (n < 0) {
        perror("main read");
        exit(EXIT_FAILURE);
    }

    printf("read len = %d, n = %d, errno = %d, strerror(errno) = %s\n", len, n, errno, strerror(errno));

    // recv socket fd 测试看看
    int fd[2];
    if (pipe(fd)) {
        perror("main piep");
        exit(EXIT_FAILURE);
    }

    // int reuse = 1;
    // if (setsockopt(fd[0], SOL_SOCKET, SO_REUSEADDR, (void *)&reuse, sizeof(reuse))) {
    //     perror("main setsockopt:");
    //     exit(EXIT_FAILURE);
    // }

    // block socket fd recv 0 len is block

    // nonblocking socket fd recv 0 len is nonblock
    // recv len = 0, n = -1, errno = 11, strerror(errno) = Resource temporarily unavailable
    // errno = 11 = EAGAIN
    // #define	EAGAIN		11	/* Try again */
    socket_set_nonblock(fd[0]);
    n = recv(fd[0], buf, len, 0);
    if (n < 0) {
        perror("main recv");
    }

    printf("recv len = %d, n = %d, errno = %d, strerror(errno) = %s\n", len, n, errno, strerror(errno));

    close(fd[1]);
    close(fd[0]);
    exit(EXIT_SUCCESS);
}
