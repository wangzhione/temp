// 平台公共头, 统一基础类型、系统头和通用常量定义
#pragma once

#if !defined(_WIN32)
// 打开 POSIX 接口声明, 让 getaddrinfo、clock_gettime 等接口在标准模式下可见
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#endif

#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <process.h>
#include <windows.h>
#include <shellapi.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#if defined(_WIN32)
// 跨平台 socket 句柄别名, Windows 下对应 SOCKET
typedef SOCKET socket_t;
#else
// 跨平台 socket 句柄别名, POSIX 下对应文件描述符
typedef int socket_t;
#ifndef INVALID_SOCKET
#define INVALID_SOCKET (-1)
#endif
#endif

enum {
    // 用户名最大 Unicode 字符数, 不含结尾 '\0'
    NAME_CHAR_MAX = 31,
    // 用户名 UTF-8 最大字节数, 按 4 字节 UTF-8 字符预留
    NAME_BYTES_MAX = NAME_CHAR_MAX * 4,
    // 单条消息最大 Unicode 字符数, 不含结尾 '\0'
    TEXT_CHAR_MAX = 512,
    // 单条消息 UTF-8 最大字节数, 按 4 字节 UTF-8 字符预留
    TEXT_BYTES_MAX = TEXT_CHAR_MAX * 4,
    // 单次接收缓冲上限, 需要覆盖 UTF-8 + 转义后的协议报文
    RECV_MAX = 8192,
    // 服务端允许同时在线的最大客户端数
    CLIENT_MAX = 128,
    // 协议编码后的最大报文长度保护值, 需要覆盖转义放大
    PACKET_MAX = 4608,
};

extern void platform_init_console(void);

#if defined(_WIN32)
// 获取 Windows 命令行参数并统一转换为 UTF-8; 成功返回 0, 失败返回负的错误码
extern int platform_get_utf8_argv(int * argc_out, char *** argv_out);
// 释放 platform_get_utf8_argv 分配的 UTF-8 参数数组
extern void platform_free_utf8_argv(int argc, char ** argv);
#endif


