// 聊天室服务端对外接口与启动配置定义
#pragma once

#include "chat_protocol.h"
#include "socketx.h"

// 聊天室服务端启动配置
typedef struct chat_server_config {
    // 服务端监听 IP, 传空或 0.0.0.0 表示绑定所有网卡
    const char * bind_ip;
    // 服务端监听端口
    uint16_t bind_port;
    // 最大在线客户端数, 0 表示使用默认值
    size_t max_clients;
} chat_server_config_t;

// 启动聊天室服务端; 成功返回 0, 失败返回负的错误码
extern int chat_server_run(const chat_server_config_t * cfg);
