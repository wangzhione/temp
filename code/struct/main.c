#include "heap.h"

struct node {
    int value;
};

static inline int node_cmp(const struct node * l, const struct node * r) {
    return l->value - r->value;
}

static void heap_print(heap_t h) {
    struct heap {
        void ** data;
        int len;
    } * obj = (struct heap *)h;

    // 数据打印
    for (int i = 0; i < obj->len; ++i) {
        struct node * node = obj->data[i];
        printf("%d ", node->value);
    }
    putchar('\n');
}

int main(void) {
    heap_t h = heap_create(node_cmp);
    struct node a[] = { { 53 }, { 17 }, { 78 }, { 9 }, { 45 }, { 65 }, { 87 }, { 23} };
    for (int i = 0; i < (int)(sizeof a / sizeof *a); ++i)
        heap_push(h, a + i);

    heap_print(h);

    // 数据打印
    struct node * node;
    while ((node = heap_pop(h))) {
        printf("%d ", node->value);
    }
    putchar('\n');

    // 重新插入数据
    for (int i = 0; i < (int)(sizeof a / sizeof *a); ++i)
        heap_push(h, a + i);

    // 删除操作 - 下沉
    heap_remove(h, 0);
    heap_print(h);

    // 插入操作
    heap_push(h, &(struct node){ 17 });
    heap_print(h);

    // 删除操作 - 上浮
    heap_remove(h, 2);
    heap_print(h);

    heap_delete(h, NULL);

    exit(EXIT_SUCCESS);
}
