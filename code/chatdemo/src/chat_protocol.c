#include "chat_protocol.h"

static int
chat__append_char(char * buf, size_t buf_len, size_t * off, char ch) {
    if (*off + 1 >= buf_len) {
        return -EMSGSIZE;
    }
    buf[*off] = ch;
    ++(*off);
    buf[*off] = '\0';
    return 0;
}

static int
chat__append_text_escaped(char * buf, size_t buf_len, size_t * off, const char * text) {
    size_t idx;
    size_t text_len;

    if (text == nullptr) {
        return 0;
    }

    text_len = strlen(text);
    for (idx = 0; idx < text_len; ++idx) {
        char ch = text[idx];

        switch (ch) {
        case '\\':
            if (chat__append_char(buf, buf_len, off, '\\') < 0 || chat__append_char(buf, buf_len, off, '\\') < 0) {
                return -EMSGSIZE;
            }
            break;
        case '|':
            if (chat__append_char(buf, buf_len, off, '\\') < 0 || chat__append_char(buf, buf_len, off, '|') < 0) {
                return -EMSGSIZE;
            }
            break;
        case '\n':
            if (chat__append_char(buf, buf_len, off, '\\') < 0 || chat__append_char(buf, buf_len, off, 'n') < 0) {
                return -EMSGSIZE;
            }
            break;
        case '\r':
            if (chat__append_char(buf, buf_len, off, '\\') < 0 || chat__append_char(buf, buf_len, off, 'r') < 0) {
                return -EMSGSIZE;
            }
            break;
        default:
            if (chat__append_char(buf, buf_len, off, ch) < 0) {
                return -EMSGSIZE;
            }
            break;
        }
    }
    return 0;
}

static const char *
chat__find_pipe_unescaped(const char * buf, size_t buf_len, size_t start) {
    size_t idx;
    bool escaped = false;

    for (idx = start; idx < buf_len; ++idx) {
        char ch = buf[idx];

        if (escaped) {
            escaped = false;
            continue;
        }
        if (ch == '\\') {
            escaped = true;
            continue;
        }
        if (ch == '|') {
            return buf + idx;
        }
    }
    return nullptr;
}

static int
chat__unescape_text(char * dst, size_t dst_len, const char * src, size_t src_len) {
    size_t si;
    size_t di = 0;

    if (dst == nullptr || dst_len == 0 || src == nullptr) {
        return -EINVAL;
    }

    for (si = 0; si < src_len; ++si) {
        char ch = src[si];

        if (di + 1 >= dst_len) {
            return -EMSGSIZE;
        }
        if (ch != '\\') {
            dst[di++] = ch;
            continue;
        }
        if (si + 1 >= src_len) {
            return -EPROTO;
        }

        ++si;
        switch (src[si]) {
        case '\\':
            dst[di++] = '\\';
            break;
        case '|':
            dst[di++] = '|';
            break;
        case 'n':
            dst[di++] = '\n';
            break;
        case 'r':
            dst[di++] = '\r';
            break;
        default:
            return -EPROTO;
        }
    }

    dst[di] = '\0';
    return 0;
}

static const char *
chat__find_pipe(const char * buf, size_t buf_len, size_t start) {
    return chat__find_pipe_unescaped(buf, buf_len, start);
}

void
chat_packet_init(chat_packet_t * packet) {
    if (packet == nullptr) {
        return;
    }

    *packet = (chat_packet_t){};
    packet->version = CHAT_PROTOCOL_VERSION;
}

const char *
chat_packet_type_name(uint32_t type) {
    switch (type) {
    case CHAT_PACKET_JOIN:
        return "join";
    case CHAT_PACKET_LEAVE:
        return "leave";
    case CHAT_PACKET_CHAT:
        return "chat";
    case CHAT_PACKET_SYSTEM:
        return "system";
    case CHAT_PACKET_PING:
        return "ping";
    case CHAT_PACKET_PONG:
        return "pong";
    case CHAT_PACKET_ERROR:
        return "error";
    default:
        return "invalid";
    }
}

int
chat_packet_encode(const chat_packet_t * packet, char * buf, size_t buf_len) {
    int written;
    size_t off = 0;

    if (packet == nullptr || buf == nullptr || buf_len == 0) {
        return -EINVAL;
    }
    if (packet->version != CHAT_PROTOCOL_VERSION) {
        return -EPROTO;
    }

    written = snprintf(buf, buf_len, "UDPCHAT|%u|%u|", (unsigned)packet->version, (unsigned)packet->type);
    if (written < 0 || (size_t)written >= buf_len) {
        return -EMSGSIZE;
    }
    off = (size_t)written;

    if (chat__append_text_escaped(buf, buf_len, &off, packet->sender) < 0) {
        return -EMSGSIZE;
    }
    if (chat__append_char(buf, buf_len, &off, '|') < 0) {
        return -EMSGSIZE;
    }
    if (chat__append_text_escaped(buf, buf_len, &off, packet->payload) < 0) {
        return -EMSGSIZE;
    }
    return (int)off;
}

int
chat_packet_decode(chat_packet_t * packet, const char * buf, size_t buf_len) {
    const char * p0;
    const char * p1;
    const char * p2;
    const char * p3;
    unsigned long version;
    unsigned long type;
    char field[32];
    size_t n;

    if (packet == nullptr || buf == nullptr || buf_len == 0 || buf_len > RECV_MAX) {
        return -EINVAL;
    }

    // 这里按给定长度扫描分隔符, 不依赖输入一定以 '\0' 结尾
    p0 = chat__find_pipe(buf, buf_len, 0);
    if (p0 == nullptr || (size_t)(p0 - buf) != 7 || memcmp(buf, "UDPCHAT", 7) != 0) {
        return -EPROTO;
    }

    p1 = chat__find_pipe(buf, buf_len, (size_t)(p0 + 1 - buf));
    p2 = p1 != nullptr ? chat__find_pipe(buf, buf_len, (size_t)(p1 + 1 - buf)) : nullptr;
    p3 = p2 != nullptr ? chat__find_pipe(buf, buf_len, (size_t)(p2 + 1 - buf)) : nullptr;
    if (p1 == nullptr || p2 == nullptr || p3 == nullptr) {
        return -EPROTO;
    }

    chat_packet_init(packet);

    n = (size_t)(p1 - (p0 + 1));
    if (n == 0 || n >= sizeof(field)) {
        return -EPROTO;
    }
    memcpy(field, p0 + 1, n);
    field[n] = '\0';
    version = strtoul(field, nullptr, 10);
    if (version != CHAT_PROTOCOL_VERSION) {
        return -EPROTO;
    }

    n = (size_t)(p2 - (p1 + 1));
    if (n == 0 || n >= sizeof(field)) {
        return -EPROTO;
    }
    memcpy(field, p1 + 1, n);
    field[n] = '\0';
    type = strtoul(field, nullptr, 10);
    if (type == CHAT_PACKET_INVALID || type > CHAT_PACKET_ERROR) {
        return -EPROTO;
    }

    n = (size_t)(p3 - (p2 + 1));
    if (n > NAME_BYTES_MAX * 2) {
        return -EMSGSIZE;
    }
    if (chat__unescape_text(packet->sender, sizeof(packet->sender), p2 + 1, n) < 0) {
        return -EPROTO;
    }

    // payload 允许为空, 所以这里直接吃掉最后一个分隔符之后的全部内容
    n = buf_len - (size_t)(p3 + 1 - buf);
    if (n > TEXT_BYTES_MAX * 2) {
        return -EMSGSIZE;
    }
    if (chat__unescape_text(packet->payload, sizeof(packet->payload), p3 + 1, n) < 0) {
        return -EPROTO;
    }

    packet->version = (uint32_t)version;
    packet->type = (uint32_t)type;
    return 0;
}


