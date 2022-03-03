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

   在数组中的两个数字如果前面一个数字大于后面的数字, 则这两个数字组成一个逆序对.
   输入一个数组, 求出这个数组中的逆序对的总数.
 */

static void array_printf(int a[], int len) {
    printf("array: ");
    for (int i = 0; i < len; i++) {
        printf("%4d", a[i]);
    }
    printf("\n");
}

static void merge(int a[], int left, int middle, int right, int * restrict tmp) {
    int i = left;
    int j = middle;
    int k = i;
    while (i < middle && j < right) {
        if (a[i] <= a[j])
            tmp[k++] = a[i++];
        else
            tmp[k++] = a[j++];
    }

    if (i < middle) {
        memcpy(tmp + k, a + i, (middle - i) * sizeof(int));
    } else if (j < right) {
        memcpy(tmp + k, a + j, (right - j) * sizeof(int));
    }

    memcpy(a + left, tmp + left, (right - left) * sizeof(int));
}

void sort_merge_partial(int a[], int left, int right, int * restrict tmp) {
    int middle = (right - left) / 2 + left;
    if (left < middle - 1) {
        sort_merge_partial(a, left, middle, tmp);
    }
    if (middle < right - 1) {
        sort_merge_partial(a, middle, right, tmp);
    }
    merge(a, left, middle, right, tmp);
}

void sort_merge(int a[], int len) {
    if (a == NULL || len <= 1) {
        return;
    }

    int * tmp = malloc(len * sizeof(int));
    sort_merge_partial(a, 0, len, tmp);
    free(tmp);
}

void sort_merge_non_recursive(int a[], int len) {
    if (a == NULL || len <= 1) {
        return;
    }

    int * tmp = malloc(len * sizeof(int));

    // 非递归, step 划分区域 1 -> 2 -> 4 -> ...

    // 为什么不需要 step <= len 呢?
    // step 分割量, 分割为 0, step, 2*step, 3*step, 4*step, 5*step, ... n*step, len
    // 因为 step = len / 2 时候. 如果 2*step < len 将还会存在 2*step 循环
    for (int step = 1; step < len; step *= 2) {
        // 为什么不需要 i + step <= len ?
        // 这个更好回答, 当 i = 0 时候, 此时如果 0 + step == len 代表, 
        // 在上一轮 step/2 时候已经调整好了, 不需要单方面调整了.
        for (int i = 0; i + step < len; i += 2*step) {
            int left = i, middle = i + step, right = middle + step;
            // 边界处理
            if (right > len) {
                right = len;
            }

            merge(a, left, middle, right, tmp);
        }
    }

    free(tmp);
}

static int merge_inverse_count(int a[], int left, int middle, int right, int * restrict tmp) {
    int i = middle - 1;
    int j = right - 1;
    int k = right - 1;

    int count = 0;
    while (i >= left && j >= middle) {
        if (a[i] > a[j]) {
            tmp[k--] = a[i--];
            count += j - middle + 1;
        } else {
            tmp[k--] = a[j--];
        }
    }

    if (i >= left) {
        do {
            tmp[k--] = a[i--];
        } while (i >= left);
    } else if (j >= middle) {
        do {
            tmp[k--] = a[j--];
        } while (j >= middle);
    }

    memcpy(a + left, tmp + left, (right - left) * sizeof(int));

    return count;
}

static int merge_inverse_partial(int a[], int left, int right, int * restrict tmp) {
    int middle = (right - left) / 2 + left;

    int left_count = 0;
    if (left < middle - 1) {
        left_count = merge_inverse_partial(a, left, middle, tmp);
    }
    int right_count = 0;
    if (middle < right - 1) {
        right_count = merge_inverse_partial(a, middle, right, tmp);
    }

    int count = merge_inverse_count(a, left, middle, right, tmp);
    return left_count + right_count + count;
}

int merge_inverse(int a[], int len) {
    if (a == NULL || len <= 1) {
        return 0;
    }

    int * tmp = malloc(len * sizeof(int));
    int count = merge_inverse_partial(a, 0, len, tmp);
    free(tmp);

    return count;
}

// build:
// gcc -g -O3 -Wall -Wextra -Werror -o 54_inverse 54_inverse.c
//
int main(void) {
    int a[] = { 7, 5, 6, 4 };
    int alen = sizeof a / sizeof *a;

    int b[] = { 1, 2, 3, 2, 2, 2, 5, 4, 2, 9, -1, 13, 4, -13, 7 };
    int blen = sizeof b / sizeof *b;

    array_printf(a, alen);
    array_printf(b, blen);

    sort_merge(a, alen);
    sort_merge_non_recursive(b, blen);

    array_printf(a, alen);
    array_printf(b, blen);

    int count = merge_inverse(a, alen);
    printf("count = %d\n", count);

    int c[] = { 7, 5, 6, 4 };
    int clen = sizeof c / sizeof *c;

    array_printf(c, clen);
    count = merge_inverse(c, clen);
    array_printf(c, clen);
    printf("count = %d\n", count);

    exit(EXIT_SUCCESS);
}
