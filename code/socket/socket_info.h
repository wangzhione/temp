#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "alloc.h"

#define SOCKET_INFO_UNKNOWN  0
#define SOCKET_INFO_LISTEN   1
#define SOCKET_INFO_TCP      2
#define SOCKET_INFO_UDP      3
#define SOCKET_INFO_BIND     4
#define SOCKET_INFO_CLOSEING 5

struct socket_info {
    int id;
    int type;
    uintptr_t opaque;
    int read;
    int write;
    uint64_t rtime;
    uint64_t wtime;
    int wbuffer;
    bool reading;
    bool writing;
    char name[128];
    struct socket_info * next;
};

inline struct socket_info * socket_info_create(struct socket_info * last) {
    struct socket_info * info = calloc(1, sizeof(struct socket_info));
    info->next = last;
    return info;
}

inline void socket_info_delete(struct socket_info * info) {
    while (info) {
        struct socket_info * temp = info;
        info = info->next;
        free(temp);
    }
}
