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

/* describe : 206. 反转链表

   给你单链表的头结点 head, 请你反转链表, 并返回反转后的链表
 */

struct ListNode {
    int val;
    struct ListNode * next;
};

struct ListNode * reverseList(struct ListNode * head) {
    struct ListNode * prev = NULL;
    while (head != NULL) {
        struct ListNode * next = head->next;
        head->next = prev;
        prev = head;
        head = next;
    }
    return prev;
}

// build:
// gcc -g -O3 -Wall -Wextra -Werror -o 003_206_list_reverse 003_206_list_reverse.c
//
int main(void) {

    exit(EXIT_SUCCESS);
}
