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

 数组中将 奇数放在前面, 偶数放在后面
 */

void array_odd_even(int a[], int len) {
    if (a == NULL || len <= 1) {
        return;
    }

    // 奇数在前, 偶数在后
    int low = 0, high = len - 1;
    int anchor = a[low];
    do {
        // a[high] & 1 == 0;
        while (low < high && a[high] % 2 == 0)
            high--;
        if (low >= high)
            break;
        a[low++] = a[high];

        while (low < high && a[low] % 2 == 1)
            low++;
        if (low >= high)
            break;
        a[high--] = a[low];
    } while (low < high);
    a[low] = anchor;
}


static void sort_quick_partial(int a[], int left, int right) {
    int low = left, high = right;
    int anchor = a[low];
    do {
        while (low < high && anchor <= a[high])
            high--;
        if (low >= high)
            break;
        a[low++] = a[high];

        while (low < high && anchor >= a[low])
            low++;
        if (low >= high)
            break;
        a[high--] = a[low];
    } while (low < high);
    a[low] = anchor;

    if (left < low-1) {
        sort_quick_partial(a, left, low-1);
    }
    if (high+1 < right) {
        return sort_quick_partial(a, high+1, right);
    }
}

void sort_quick(int a[], int len) {
    if (a == NULL || len <= 1) {
        return;
    }

    return sort_quick_partial(a, 0, len-1);
}

void array_print(int a[], int len) {
    printf("array : ");
    for (int i = 0; i < len; i++)
        printf(" %d ", a[i]);
    printf("\n");
}

// build:
// gcc -g -O3 -Wall -Wextra -Werror -o 32_odd_even 32_odd_even.c
//
int main(void) {
    int a[] = { 9, 1, 23, 2, 4, 0, 7, 13, 16, 10 };
    int len = sizeof a / sizeof *a;

    array_print(a, len);

    sort_quick(a, len);

    array_print(a, len);

    array_odd_even(a, len);

    array_print(a, len);

    exit(EXIT_SUCCESS);
}
