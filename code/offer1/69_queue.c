#include "q.h"

/* describe : 

   queue 功能单元测试
 */

struct object {
    int value;
};

// build:
// gcc -g -O3 -Wall -Wextra -Werror -o 69_queue 69_queue.c q.c
//
int main(void) {
    int i;
    struct object object[1000];

    for (i = 0; i < (int)(sizeof object / sizeof *object); i++) {
        object[i].value = i;
    }

    q_t q; q_init(q);

    i = 0; q_push(q, object+i);
    printf("i = %2d, q_len = %2d, q->tail=%2d, q->head=%2d, q->cap=%2d\n", i, q_len(q), q->tail, q->head, q->cap);
    i = 1; q_push(q, object+i);
    printf("i = %2d, q_len = %2d, q->tail=%2d, q->head=%2d, q->cap=%2d\n", i, q_len(q), q->tail, q->head, q->cap);
    q_pop(q);
    printf("i = %2d, q_len = %2d, q->tail=%2d, q->head=%2d, q->cap=%2d\n", i, q_len(q), q->tail, q->head, q->cap);

    // 开始填充
    for (i = 0; i < 40; i++) {
        q_push(q, object+i);
        printf("i = %2d, q_len = %2d, q->tail=%2d, q->head=%2d, q->cap=%2d\n", i, q_len(q), q->tail, q->head, q->cap);
    }

    // 弹出
    for (i = 0; i < 44; i++) {
        q_pop(q);
        printf("i = %2d, q_len = %2d, q->tail=%2d, q->head=%2d, q->cap=%2d\n", i, q_len(q), q->tail, q->head, q->cap);
    }

    q_free(q);

    exit(EXIT_SUCCESS);
}
