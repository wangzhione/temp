#include <stdio.h>
#include <stdlib.h>

#include "socket.h"

extern int socket_addr(char ip[INET6_ADDRSTRLEN], uint16_t port, sockaddr_t a);

int main(void) {

    sockaddr_t addr;

    int ret = socket_addr("::0.0.0.0", 9527, addr);
    if (ret < 0) {
        EXIT("socket_addr error");
    }

    char ip[INET6_ADDRSTRLEN] = { 0 };
    char * res = socket_ntop(addr, ip);
    printf("ip = [%s], res = [%s], port = %d\n", ip, res, ntohs(addr->sin6_port));

    ret = socket_addr("", 9527, addr);
    if (ret < 0) {
        EXIT("socket_addr error");
    }

    memset(ip, 0, sizeof ip);
    res = socket_ntop(addr, ip);
    printf("ip = [%s], res = [%s], port = %d\n", ip, res, ntohs(addr->sin6_port));




    exit(EXIT_FAILURE);
}
