#include <stdio.h>
#include <errno.h>
#include <wchar.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* describe : 剑指 Offer II 026. 重排链表

   给定一个单链表 L 的头结点 head ，单链表 L 表示为：

     L0 → L1 → … → Ln-1 → Ln 
    请将其重新排列后变为：

    L0 → Ln → L1 → Ln-1 → L2 → Ln-2 → …

    不能只是单纯的改变结点内部的值，而是需要实际的进行结点交换。
 */

struct ListNode {
    int val;
    struct ListNode * next;
};

void reorderList0(struct ListNode * head) {
    // L0 -> L1 -> ... -> Ln-1 -> Ln
    // L0 -> Ln -> L1 -> Ln-1 -> L2 -> Ln-2 -> ...
    struct ListNode * L0 = head;
    while (L0 != NULL && L0->next != NULL && L0->next->next != NULL) {
        struct ListNode * Ln_1 = L0;
        while (Ln_1->next->next != NULL) {
            Ln_1 = Ln_1->next;
        }

        Ln_1->next->next = L0->next;
        L0->next = Ln_1->next;
        Ln_1->next = NULL;

        L0 = L0->next->next;
    }
}

void reorderList(struct ListNode * head) {
    int n = 0;
    
    struct ListNode * node = head;
    while (node) {
        node = node->next;
        n++;
    }

    if (n <= 2) {
        return;
    }

    int i = 0;
    struct ListNode ** nodes = malloc(sizeof(struct ListNode *) * n);
    
    for (node = head; node; node = node->next) {
        nodes[i++] = node;
    }

    // L0 -> L1 -> ... -> Ln-1 -> Ln
    // L0 -> Ln -> L1 -> Ln-1 -> L2 -> Ln-2 -> ...
    struct ListNode * L0 = head;
    for (i = n-1; i >= 2 && L0->next != NULL && L0->next->next != NULL; i--) {
        struct ListNode * Ln_1 = nodes[i-1];
        struct ListNode * Ln = nodes[i];

        Ln->next = L0->next;
        L0->next = Ln;
        Ln_1->next = NULL;

        L0 = L0->next->next;
    }

    free(nodes);
}

// build:
// gcc -g -O3 -Wall -Wextra -Werror -o 014_026_list_reorder 014_026_list_reorder.c
//
int main(void) {

    exit(EXIT_SUCCESS);
}
