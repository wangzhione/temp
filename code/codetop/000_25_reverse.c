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

/* describe : K 个一组翻转链表

   给你一个链表, 每 k 个节点一组进行翻转, 请你返回翻转后的链表.
   k 是一个正整数, 他的值小于或等于链表长度.
   如果结点总数不是 k 的整数倍, 那么请将最后剩余的节点保持原有顺序.

   进阶:
   - 你可以设计一个只使用常数额外空间的算法来解决此问题吗?
   - 你不能只是单纯的改变结点内部的值, 而是需要实际进行结点交换
 */

// Definition for singly-linked list.
struct ListNode {
    int val;
    struct ListNode * next;
};

struct ListNode * ListNodeNew(int val) {
    struct ListNode * node = malloc(sizeof(struct ListNode));
    node->val = val;
    node->next = NULL;
    return node;
}

struct ListNode * ListNodeCreateByArray(int array[], int n) {
    assert(array != NULL && n >= 1);

    struct ListNode * nodes[n];
    nodes[0] = ListNodeNew(array[0]);
    for (int i = 1; i < n; i++) {
        nodes[i] = ListNodeNew(array[i]);
        nodes[i-1]->next = nodes[i];
    }
    return nodes[0];
}

void ListNodeDelete(struct ListNode * head) {
    while (head != NULL) {
        struct ListNode * next = head->next;
        free(head);
        head = next;
    }
}

static void ListNodeDebug(struct ListNode * head) {
    while (head != NULL) {
        struct ListNode * next = head->next;
        printf(" %d ", head->val);
        head = next;
    }
    printf("\n");
}

// 链表翻转 left -> xxx -> right (right->next = tail) 变成 right -> xxx -> left -> NULL
static void ListNodeReverse(struct ListNode * left, struct ListNode * right) {
    struct ListNode * prev = NULL; 
    struct ListNode * tail = right->next;
    while (left != tail) {
        struct ListNode * next = left->next;
        left->next = prev;
        prev = left;
        left = next;
    }
}

struct ListNode * reverseKGroup(struct ListNode * head, int k) {
    if (head == NULL || head->next == NULL || k <= 1) {
        return head;
    }

    // 切分 0, 1, ... n -> [0, 1, k-1] [k, k+1, 2k-1], ... n
    int num = 0;
    struct ListNode * left = head;
    struct ListNode * right = left;

    while (++num < k && right->next != NULL) {
        right = right->next;
    }
    // 如果未到 k 个直接返回
    if (num < k) {
        return head;
    }
    // 首次旋转 & 重新设置头结点
    head = right;
    struct ListNode * tail = right->next;
    ListNodeReverse(left, right);

    // 开始循环下一轮
    while (tail != NULL) {
        struct ListNode * left1 = tail;
        struct ListNode * right1 = left1;
        num = 0;

        while (++num < k && right1->next != NULL) {
            right1 = right1->next;
        }

        // 如果未到 k 个循环结束
        if (num < k) {
            left->next = left1;
            break;
        }
        // 继续开始旋转
        tail = right1->next;
        ListNodeReverse(left1, right1);
        left->next = right1;
        left = left1;
    }

    return head;
}

static void reverseKGroupDebug(int head[], int n, int k) {
    struct ListNode * list = ListNodeCreateByArray(head, n);
    struct ListNode * reverse = reverseKGroup(list, k);
    ListNodeDebug(reverse);
    ListNodeDelete(reverse);
}

// build:
// gcc -g -O3 -Wall -Wextra -Werror -o 000_25_reverse 000_25_reverse.c
//
int main(void) {

    int head1[] = { 1, 2, 3, 4, 5 };
    int k1 = 2;
    reverseKGroupDebug(head1, sizeof(head1)/sizeof(int), k1);

    int head2[] = { 1, 2, 3, 4, 5 };
    int k2 = 3;
    reverseKGroupDebug(head2, sizeof(head2)/sizeof(int), k2);

    int head3[] = { 1, 2, 3, 4, 5 };
    int k3 = 1;
    reverseKGroupDebug(head3, sizeof(head3)/sizeof(int), k3);

    int head4[] = { 1 };
    int k4 = 1;
    reverseKGroupDebug(head4, sizeof(head4)/sizeof(int), k4);

    int head5[] = { 1, 2 };
    int k5 = 2;
    reverseKGroupDebug(head5, sizeof(head5)/sizeof(int), k5);

    exit(EXIT_SUCCESS);
}
