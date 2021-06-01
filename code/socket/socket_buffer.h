#pragma once

#include "alloc.h"

#define SOCKET_BUFFER_MEMORY     0
#define SOCKET_BUFFER_OBJECT     1
#define SOCKET_BUFFER_RAWPOINTER 2

struct socket_sendbuffer {
    int id;
    int type;
    const void * buffer;
    int sz;
};

#define USEROBJECT (-1)

inline const void * socket_sendbuffer_clone(struct socket_sendbuffer * buf, int * sz) {
    switch (buf->type) {
    case SOCKET_BUFFER_MEMORY:
        *sz = buf->sz;
        return buf->buffer;
    case SOCKET_BUFFER_OBJECT:
        *sz = USEROBJECT;
        return buf->buffer;
    case SOCKET_BUFFER_RAWPOINTER:
        // It's a raw pointer, we need make a copy
        *sz = buf->sz;
        void * temp = malloc(*sz);
        memcpy(temp, buf->buffer, *sz);
        return temp;
    }
    // never get here
    *sz = 0;
    return NULL;
}
