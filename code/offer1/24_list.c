#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "stack.h"

struct list {
    int value;
    struct list * next;
};

struct list * list_add_tail(struct list * head, int value) {
    struct list * node = malloc(sizeof(struct list));
    node->value = value;
    node->next = NULL;

    if (head == NULL) {
        return node;
    }

    struct list * tail = head;
    while (tail->next != NULL) {
        tail = tail->next;
    }
    tail->next = node;

    return head;
}

struct list * list_remove(struct list * head, int value) {
    if (head == NULL) {
        return NULL;
    }

    struct list * prev = NULL;
    struct list * node = head;
    do {
        if (node->value == value) {
            // 找到了结点
            if (prev == NULL) {
                head = head->next;
            } else {
                prev->next = head->next;
            }
            free(node);
            break;
        }
        node = node->next;
    } while (node != NULL);

    return head;
}

void list_tail_print(struct list * head) {
    if (head != NULL) {
        list_tail_print(head->next);
        printf(" %d ", head->value);
    }
}

int main(void) {

    exit(EXIT_SUCCESS);
}