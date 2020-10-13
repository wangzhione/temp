#pragma once

#include <stdlib.h>
#include <assert.h>

struct graph_queue {
    int     head;       // 头结点
    int     tail;       // 尾结点
    int      cap;       // 队列容量
    int   data[];       // 队列实体
};

inline struct graph_queue * graph_queue_create(int num) {
    struct graph_queue * q = malloc(sizeof(struct graph_queue) + num * sizeof(int));
    assert(q && num > 0);
    q->head = 0;
    q->tail = -1;
    q->cap = num;
    return q;
}

inline void graph_queue_delete(struct graph_queue * q) {
    free(q);
}

inline void graph_queue_push(struct graph_queue * q, int v) {
    assert(q && q->tail < q->cap);
    q->data[++q->tail] = v;
}

// 队列为 empty 时候, 返回 -1
inline int graph_queue_pop(struct graph_queue * q) {
    if (q->tail == -1) {
        return -1;
    }

    int v = q->data[q->head++];
    if (q->head > q->tail) {
        q->head = 0;
        q->tail = -1;
    }
    return v;
}
