#pragma once

#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

//
// 获取数组长度
//
#define LEN(a)  (sizeof(a) / sizeof(*a))

// graph 一次性 queue
struct graph_queue {
    int head;       // 头结点
    int tail;       // 尾结点
    int cap;        // 队列容量
    int  * data;    // 队列实体
    bool * visited; // 顶点访问情况记录数组
};

inline struct graph_queue * graph_queue_create(int num) {
    struct graph_queue * q = malloc(sizeof(struct graph_queue));
    q->head = 0;
    q->tail = -1;
    q->cap = num;
    q->data = malloc(sizeof(int) * num);
    q->visited = calloc(num, sizeof(bool));
    assert(q && num > 0 && q->data && q->visited);
    return q;
}

inline void graph_queue_delete(struct graph_queue * q) {
    free(q->visited);
    free(q->data);
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

inline void graph_queue_push_visited(struct graph_queue * q, int v) {
    if (!q->visited[v]) {
        // 顶点标识, 并入队
        q->visited[v] = true;
        graph_queue_push(q, v);
    }
}