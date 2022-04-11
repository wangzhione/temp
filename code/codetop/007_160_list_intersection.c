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

/* describe : 160. 相交链表

   给你两个单链表的头结点 headA 和 headB, 请你找出并返回两个单链表相交的起始结点.
   如果两个链表不存在相交结点, 返回 null.
 */

struct ListNode {
    int val;
    struct ListNode * next;
};

static int ListNodeLen(struct ListNode * head) {
    int size = 0;
    while (head != NULL) {
        head = head->next;
        size++;
    }
    return size;
}

struct ListNode * getIntersectionNode(struct ListNode * headA, struct ListNode * headB) {
    if (headA == NULL || headB == NULL) {
        return NULL;
    }

    int sizeA = ListNodeLen(headA);
    int sizeB = ListNodeLen(headB);
    // 从尾到头 指针长度打平
    while (sizeA < sizeB) {
        headB = headB->next;
        sizeB--;
    }
    while (sizeB < sizeA) {
        headA = headA->next;
        sizeA--;
    }

    while (headA != headB && headA != NULL && headB != NULL) {
        headA = headA->next;
        headB = headB->next;
    }
    return headA == headB ? headA : NULL;
}

// build:
// gcc -g -O3 -Wall -Wextra -Werror -o 007_160_list_intersection 007_160_list_intersection.c
//
int main(void) {

    exit(EXIT_SUCCESS);
}
