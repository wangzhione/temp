#include "stack.h"

/*
 用两个栈模拟一个队列
*/

struct queue {
    struct stack sa;
    struct stack sb;
};

void queue_init(struct queue * q) {
    stack_init(&q->sa);
    stack_init(&q->sb);
}

void queue_free(struct queue * q) {
    stack_free(&q->sa);
    stack_free(&q->sb);
}

void queue_push(struct queue * q, void * m) {
    stack_push(&q->sa, m);
}

void * queue_pop(struct queue * q) {
    if (!stack_empty(&q->sb)) {
        return stack_pop(&q->sb);
    }

    // sb is empty, sa -> sb
    if (stack_empty(&q->sa)) {
        // 假定 NULL 标识业务上面的没有
        return NULL;
    }
    do {
        stack_push(&q->sb, stack_pop(&q->sa));
    } while (!stack_empty(&q->sa));
    return stack_pop(&q->sb);
}

// build:
// gcc -g -O3 -Wall -Werror -Wextra -o 26_stack 26_stack.c
//
int main(void) {
    int i;
    int a[] = { 1, 2, 3, 4, 5, 6 };
    int len = sizeof(a) / sizeof(int);

    struct queue q; queue_init(&q);

    for (i = 0; i < len; i++) {
        queue_push(&q, a+i);
    }

    for (i = 0; i < len; i++) {
        int * ptr = queue_pop(&q);
        printf(" %d ", *ptr);
    }
    printf("\n");

    queue_free(&q);
    exit(EXIT_SUCCESS);
}