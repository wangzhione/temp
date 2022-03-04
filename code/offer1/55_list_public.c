#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* describe : 

   两个链表第一个公共结点
 */

struct list {
    int value;
    struct list * next;
};

inline struct list * list_new(int value) {
    struct list * node = malloc(sizeof(struct list));
    node->value = value;
    node->next = NULL;
    return node;
}

inline void list_free(struct list * node) {
    free(node);
}

int list_len(struct list * head) {
    int len = 0;
    while (head != NULL) {
        len++;
        head = head->next;
    }
    return len;
}

struct list * list_public(struct list * head1, struct list * head2) {
    int head1_len = list_len(head1);
    int head2_len = list_len(head2);

    if (head1_len == 0 || head2_len == 0) {
        // 没有公共结点
        return NULL;
    }

    if (head1_len > head2_len) {
        do {
            head1 = head1->next;
        } while (--head1_len > head2_len);
    } else if (head1_len < head2_len) {
        do {
            head2 = head2->next;
        } while (--head2_len > head1_len);
    }

    // 这时候 head1_len == head2_len , head1 和 head2 持平
    // 其中 head1 != NULL && head1 != head2
    // 和 head1 != NULL && head2 != NULL && head1 != head2 等价, 前置条件是 head1_len == head2_len
    while (head1 != NULL && head1 != head2) {
        head1 = head1->next;
        head2 = head2->next;
    }
    return head1;
}

// build:
// gcc -g -O3 -Wall -Wextra -Werror -o 55_list_public 55_list_public.c
//
int main(void) {
    int a1[] = { 1, 2, 3, 4, 5 };
    int a1_len = sizeof a1 / sizeof *a1;
    int a2[] = { 9, 8, 9, 10, 11 };
    int a2_len = sizeof a2 / sizeof a2_len;
    int i;
    
    struct list * head1 = list_new(a1[0]);
    struct list * curr1 = head1;
    for (i = 1; i < a1_len; i++) {
        struct list * node = list_new(a1[i]);
        curr1->next = node;
        curr1 = node;
    }

    struct list * head2 = list_new(a2[0]);
    struct list * curr2 = head2;
    for (i = 1; i < a2_len; i++) {
        struct list * node = list_new(a2[i]);
        curr2->next = node;
        curr2 = node;
    }

    struct list * public_node = list_public(head1, head2);
    if (public_node == NULL) {
        printf("没有公共结点\n");
    } else {
        printf("公共结点值为:%d\n", public_node->value);
    }

    // 用于释放
    struct list * curr2_temp = curr2->next;

    if (curr2_temp != NULL) {
        printf("打标点值:%d\n", curr2_temp->value);
    } else {
        printf("打标点不存在\n");
    }

    curr2->next = curr1;

    public_node = list_public(head1, head2);
    if (public_node == NULL) {
        printf("没有公共结点\n");
    } else {
        printf("公共结点值为:%d\n", public_node->value);
    }

    // 释放操作, 留给系统, 或者后面人

    exit(EXIT_SUCCESS);
}
