// UDP socket 抽象层, 统一地址解析、收发、超时和错误处理接口
#pragma once

#include "platform.h"

// 通用 UDP 端点, 封装对端地址与地址长度
typedef struct endpoint {
    // 通用 socket 地址存储, 当前实现实际使用 IPv4
    struct sockaddr_storage addr;
    // 地址结构实际有效长度
    socklen_t addr_len;
} endpoint_t;

// 初始化 socket 子系统; 成功返回 0, 失败返回负的底层错误码
extern int socket_startup(void);
extern void socket_cleanup(void);
extern int socket_last_error(void);
// 错误码转可读字符串; 兼容传入正错误码或负错误码
extern const char * socket_error_string(int err, char * buf, size_t buf_len);

// 打开一个 UDP socket; 成功返回 0, 失败返回负的 errno/socket 错误码
extern int socket_open(socket_t * fd);
extern void socket_close(socket_t fd);
// 绑定 IPv4 地址; 成功返回 0, 非法 IP 返回 -EINVAL, bind 失败返回负的底层错误码
extern int socket_bind_ipv4(socket_t fd, const char * ip, uint16_t port);
extern int socket_set_reuseaddr(socket_t fd, bool enable);
// 设置接收超时; 成功返回 0, timeout_ms 非法返回 -EINVAL, setsockopt 失败返回负的底层错误码
extern int socket_set_recv_timeout(socket_t fd, int timeout_ms);
// 等待 socket 可读; 返回 1 表示可读, 返回 0 表示超时, 失败返回负的错误码
extern int socket_wait_readable(socket_t fd, int timeout_ms);
// 解析 IPv4 端点; 成功返回 0, 参数非法返回 -EINVAL, 无可用地址返回 -EADDRNOTAVAIL
extern int socket_resolve_ipv4(const char * host, uint16_t port, endpoint_t * ep);
// 发送 UDP 报文; 成功返回 0, 参数非法返回 -EINVAL, 报文过大返回 -EMSGSIZE
extern int socket_sendto(socket_t fd, const void * buf, size_t buf_len, const endpoint_t * ep);
// 接收 UDP 报文; 成功返回读取字节数, 失败返回负的错误码, 参数非法返回 -EINVAL
extern int socket_recvfrom(socket_t fd, void * buf, size_t buf_len, endpoint_t * ep);
extern bool endpoint_equal(const endpoint_t * lhs, const endpoint_t * rhs);
// 端点格式化为 "ip:port"; 成功返回 0, 失败返回负的错误码
extern int endpoint_to_string(const endpoint_t * ep, char * buf, size_t buf_len);


