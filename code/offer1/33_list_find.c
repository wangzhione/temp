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
 1. find k
 2. reverse re 相反; 反对; + vers 转 + e -> 反转的, 颠倒的
 */

struct ListNode {
    int m_nValue;
    struct ListNode * m_pNext;
};

typedef struct ListNode ListNode;

ListNode * FindKthToTail(ListNode * pListHead, int k) {
    if (pListHead == NULL || k <= 0) {
        return 0;
    }

    ListNode * tail = pListHead;
    do {
        k--;
        tail = tail->m_pNext;
    } while (k > 0 && tail);
    // 异常情况处理, k != 0 标识, 结点数 < k
    if (k != 0) {
        return NULL;
    }

    while (tail) {
        tail = tail->m_pNext;
        pListHead = pListHead->m_pNext;
    }
    return pListHead;
}

ListNode * resvere(ListNode * head) {
    if (head == NULL || head->m_pNext == NULL) {
        return head;
    }

    ListNode * prev = NULL;
    do {
        ListNode * next = head->m_pNext;

        head->m_pNext = prev;
        prev = head;

        head = next;
    } while (head);

    return prev;
}

ListNode * ListNodeResevre(ListNode * head) {
    ListNode * prev = NULL;
    while (head) {
        ListNode * next = head->m_pNext;
        head->m_pNext = prev;
        prev = head;
        head = next;
    }
    return prev;
}

void print(ListNode * head) {
    printf("list : ");
    while (head) {
        printf(" %d ", head->m_nValue);
        head = head->m_pNext;
    }
    printf("\n");
}

// build:
// gcc -g -O3 -Wall -Wextra -Werror -o 33_list_find 33_list_find.c
//
int main(void) {
    ListNode node[10];
    int i;

    node[0].m_nValue = 0;
    for (i = 1; i < 10; i++) {
        node[i].m_nValue = i;
        node[i-1].m_pNext = node + i;
    }
    node[i-1].m_pNext = NULL;

    print(node);

    ListNode * prev = resvere(node);
    print(prev);

    ListNodeResevre(prev);

    print(node);

    exit(EXIT_SUCCESS);
}
