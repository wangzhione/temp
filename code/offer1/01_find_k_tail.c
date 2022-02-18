#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <stdlib.h>

struct list {
    int node;
    struct list * next;
};

static void list_print(struct list * list, int k) {
    printf("find k = %d list->node : ", k);
    if (list) {
        printf("存在 = %d\n", list->node);
    } else {
        printf("不存在\n");
    }
}

struct list * find_k_tail(struct list * head, int k) {
    if (k <= 0 || head == NULL) {
        // 边界 case, 函数参数异常
        return NULL;
    }

    struct list * tail = head;
    while (k > 0 && tail) {
        tail = tail->next;
        k--;
    }

    if (k != 0) {
        // 边界异常 case, k 过大
        return NULL;
    }

    while (tail) {
        tail = tail->next;
        head = head->next;
    }

    return head;
}

//
// build : gcc -g -O3 -Wall -Wextra -Werror -o 01_find_k_tail 01_find_k_tail.c
//
int main(void) {
    int i;
    struct list head[10];

    for (i = 0; i < 9; i++) {
        head[i].node = i+1;
        head[i].next = &head[i+1];
    }
    head[i].node = i+1;
    head[i].next = NULL;

    for (i = 0; i < 9; i++) {
        printf("%d -> ", head[i].node);
    }
    printf("%d\n", head[i].node);

    struct list * value;
    int k;

    k = -1;
    value = find_k_tail(head, k);
    list_print(value, k);
    assert(value == NULL);

    k = 0;
    value = find_k_tail(head, k);
    list_print(value, k);
    assert(value == NULL);

    k = 1;
    value = find_k_tail(head, k);
    list_print(value, k);
    assert(value != NULL && value->node == 10);

    k = 2;
    value = find_k_tail(head, k);
    list_print(value, k);
    assert(value != NULL && value->node == 9);

    k = 3;
    value = find_k_tail(head, k);
    list_print(value, k);
    assert(value != NULL && value->node == 8);

    k = 9;
    value = find_k_tail(head, k);
    list_print(value, k);
    assert(value != NULL && value->node == 2);

    k = 10;
    value = find_k_tail(head, k);
    list_print(value, k);
    assert(value != NULL && value->node == 1);

    k = 11;
    value = find_k_tail(head, k);
    list_print(value, k);
    assert(value == NULL);

    exit(EXIT_SUCCESS);
}