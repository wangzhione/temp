// 聊天室客户端对外接口与启动配置定义
#pragma once

#include "chat_protocol.h"
#include "socketx.h"
#include "threadx.h"

// 聊天室客户端启动配置
typedef struct chat_client_config {
    // 服务端主机名或 IPv4 地址
    const char * server_host;
    // 服务端端口
    uint16_t server_port;
    // 当前客户端用户名
    const char * name;
    // 本地绑定端口, 0 表示交给系统分配
    uint16_t local_port;
} chat_client_config_t;

// 启动聊天室客户端; 成功返回 0, 失败返回负的错误码
extern int chat_client_run(const chat_client_config_t * cfg);
