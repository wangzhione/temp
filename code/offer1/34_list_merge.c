#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

/*
 描述: 
 合并有序链表
 */

struct list {
    int value;
    struct list * next;
};

struct list * list_order_merge(struct list * a, struct list * b) {
    if (a == NULL || b == NULL) {
        return a == NULL ? b : a;
    }

    struct list * c;
    // 设置第一个结点
    if (a->value <= b->value) {
        c = a;
        a = a->next;
    } else {
        c = b;
        b = b->next;
    }

    // 保存待返回结点
    struct list * head = c;

    while (a != NULL && b != NULL) {
        if (a->value <= b->value) {
            c->next = a;
            c = a;
            a = a->next;
        } else {
            c->next = b;
            c = b;
            b = b->next;
        }
    }

    if (a != NULL) {
        c->next = a;
    } else if (b != NULL) {
        c->next = b;
    }

    return head;
}

void list_print(struct list * head) {
    printf("list : ");
    while (head) {
        printf(" %d ", head->value);
        head = head->next;
    }
    printf("\n");
}

// build:
// gcc -g -O3 -Wall -Wextra -Werror -o 34_list_merge 34_list_merge.c
//
int main(void) {
    struct list node1[10], node2[10];
    int i;

    node1[0].value = 0;
    node2[0].value = 0;
    for (i = 1; i < 10; i++) {
        node1[i].value = i;
        node2[i].value = i;
        node1[i-1].next = node1 + i;
        node2[i-1].next = node2 + i;
    }
    node1[i-1].next = NULL;
    node2[i-1].next = NULL;

    list_print(node1);

    list_print(node2);

    struct list * c = list_order_merge(node1, node2);

    list_print(c);

    exit(EXIT_SUCCESS);
}
