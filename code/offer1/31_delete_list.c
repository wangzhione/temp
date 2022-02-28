#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct ListNode {
    int m_nValue;
    struct ListNode * m_pNext;
};

typedef struct ListNode ListNode;

// void DeleteNode(ListNode ** pListHead, ListNode * pToBeDeleted) {
//     // step 1: 异常 case
//     if (pListHead == NULL || pToBeDeleted == NULL || *pListHead == NULL) {
//         return;
//     }

//     // step 2: pToBeDeleted 是头结点
//     if (*pListHead == pToBeDeleted) {
//         *pListHead = pToBeDeleted->m_pNext;
//         return;
//     }

//     // step 3: pToBeDeleted 是尾结点
//     if (pToBeDeleted->m_pNext == NULL) {
//         ListNode * head = *pListHead;
//         while (head->m_pNext != pToBeDeleted && head->m_pNext != NULL) {
//             head = head->m_pNext;
//         }
//         assert(head->m_pNext == pToBeDeleted);
//         head->m_pNext = pToBeDeleted->m_pNext;
//         return;
//     }

//     // step 4: pToBeDeleted 是中间结点
//     ListNode * next = pToBeDeleted->m_pNext;
//     pToBeDeleted->m_nValue = next->m_nValue;
//     pToBeDeleted->m_pNext = next->m_pNext;

//     // 存在内存泄露风险, 这种代码纯是文字游戏, 完全不符合工程版本要求
// }

ListNode * DeleteNode(ListNode ** pListHead, ListNode * pToBeDelete) {
    // step 1 : 健壮性 case
    if (pListHead == NULL || *pListHead == NULL || pToBeDelete == NULL) {
        return pToBeDelete;
    }

    // step 2 : 头结点情况
    ListNode * head = *pListHead;
    if (head == pToBeDelete) {
        *pListHead = pToBeDelete->m_pNext;
        pToBeDelete->m_pNext = NULL;
        return pToBeDelete;
    }

    // step 3 : 尾结点情况
    if (pToBeDelete->m_pNext == NULL) {
        while (head->m_pNext != pToBeDelete && head->m_pNext != NULL) {
            head = head->m_pNext;
        }
        assert(head->m_pNext == pToBeDelete);
        head->m_pNext = NULL;
        return pToBeDelete;
    }

    // step 4 : 中间结点情况
    ListNode * next = pToBeDelete->m_pNext;
    int value = pToBeDelete->m_nValue;
    pToBeDelete->m_nValue = next->m_nValue;
    next->m_nValue = value;
    pToBeDelete->m_pNext = next->m_pNext;
    // 工程完备
    next->m_pNext = NULL;
    return next;
}

// build:
// gcc -g -O3 -Wall -Wextra -Werror -o 31_delete_list 31_delete_list.c
//
int main(void) {

    exit(EXIT_SUCCESS);
}
