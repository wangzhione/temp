// 聊天室协议定义, 包含报文类型、结构和编解码接口
#pragma once

#include "platform.h"

// 聊天报文类型枚举, 标识当前 UDP 文本协议中的业务类型
typedef enum chat_packet_type {
    // 非法类型, 仅用于初始化或校验失败
    CHAT_PACKET_INVALID = 0,
    // 客户端加入聊天室
    CHAT_PACKET_JOIN = 1,
    // 客户端主动离开聊天室
    CHAT_PACKET_LEAVE = 2,
    // 普通聊天消息
    CHAT_PACKET_CHAT = 3,
    // 服务端系统通知
    CHAT_PACKET_SYSTEM = 4,
    // 客户端心跳探测
    CHAT_PACKET_PING = 5,
    // 服务端心跳响应
    CHAT_PACKET_PONG = 6,
    // 协议错误或业务错误响应
    CHAT_PACKET_ERROR = 7,
} chat_packet_type_t;

// 聊天协议报文, 发送端与接收端共享的统一内存表示
typedef struct chat_packet {
    // 协议版本, 便于后续做兼容升级
    uint32_t version;
    // 报文类型, 见 chat_packet_type_t
    uint32_t type;
    // 发送者名字, system/error 场景通常由服务端填充
    char sender[NAME_BYTES_MAX + 1];
    // 正文载荷, join/leave 可为空
    char payload[TEXT_BYTES_MAX + 1];
} chat_packet_t;

enum {
    // 当前文本协议版本, 编解码双方必须一致
    CHAT_PROTOCOL_VERSION = 1,
};

extern void chat_packet_init(chat_packet_t * packet);
// 编码聊天报文; 成功返回编码字节数, 失败返回 -EINVAL、-EPROTO 或 -EMSGSIZE
extern int chat_packet_encode(const chat_packet_t * packet, char * buf, size_t buf_len);
// 解码聊天报文; 成功返回 0, 失败返回 -EINVAL、-EPROTO 或 -EMSGSIZE
extern int chat_packet_decode(chat_packet_t * packet, const char * buf, size_t buf_len);
extern const char * chat_packet_type_name(uint32_t type);

