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

   统计一个数字在排序数组中出现的次数. 例如输入排序数组 { 1, 2, 3, 3, 3, 3, 4, 5 } 和数字 3,
   由于 3 在这个数组中出现了 4 次, 因此输出 4.
 */

void array_printf(int a[], int len) {
    printf("array[%d]: ", len);
    for (int i = 0; i < len; i++) {
        printf("%4d", a[i]);
    }
    printf("\n");
}

int array_half_find(int a[], int len, int v) {
    if (a == NULL || len <= 0) {
        return -1;
    }

    int low = 0;
    int high = len;
    do {
        int mid = (high - low) / 2 + low;
        if (a[mid] == v) {
            return mid;
        }

        if (a[mid] > v) {
            // 假定 数组是升序
            low = mid+1;
        } else {
            high = mid;
        }

    } while (low < high);

    return -1;
}

int array_half_find_count(int a[], int len, int v) {
    int index = array_half_find(a, len, v);
    if (index == -1) {
        // not found
        return 0;
    }

    int count = 1;
    // 左右两边找
    for (int i = index-1; i >= 0 && a[i] == v; i--)
        count++;
    for(int i = index+1; i < len && a[i] == v; i++)
        count++;
    return count;
}

int array_first_half_find(int a[], int len, int v) {
    int low = 0;
    int high = len;

    while (low < high) {
        int mid = (high - low) / 2 + low;
        int anchor = a[mid];
        if (anchor == v) {
            // 看看是否是最左边的 v, 是的直接返回
            if (mid == low || a[mid-1] != v) {
                return mid;
            }

            // 否则继续找左边
            high = mid;
            continue;
        }

        if (anchor < v) {
            low = mid+1;
        } else {
            high = mid;
        }
    }

    return -1;
}

int array_last_half_find(int a[], int len, int v) {
    int low = 0;
    int high = len;

    while (low < high) {
        int mid = (high - low) / 2 + low;
        int anchor = a[mid];

        printf("low=%d, high=%d, mid=%d, anchor=%d\n", low, high, mid, anchor);
        if (anchor == v) {
            // 看看是否是最右边的 v, 是的直接返回
            if (mid+1 == high || a[mid+1] != v) {
                return mid;
            }

            // 否则继续找右边
            low = mid+1;
            continue;
        }

        if (anchor < v) {
            low = mid+1;
        } else {
            high = mid;
        }
    }

    return -1;
}

int array_half_find_count_optimize(int a[], int len, int v) {
    if (a == NULL || len <= 0) {
        return 0;
    }

    int first = array_first_half_find(a, len, v);
    printf("first = %d\n", first);
    if (first == -1) {
        return 0;
    }

    int last = array_last_half_find(a, len, v);
    printf("last = %d\n", last);

    return last-first+1;
}

// build:
// gcc -g -O3 -Wall -Wextra -Werror -o 60_array_find_half 60_array_find_half.c
// gcc -g -O3 -Wall -Wextra -Werror -Wno-array-bounds -o 60_array_find_half 60_array_find_half.c
//
int main(void) {
    int a[] = { 1, 2, 3, 3, 3, 3, 4, 5 };
    int alen = sizeof a / sizeof *a;

    array_printf(a, alen);

    int v;
    int index;
    int count;
    
    v = 3;
    index = array_half_find(a, alen, v);
    count = array_half_find_count(a, alen, v);
    printf("v = %d, index = %d, count = %d, value = ", v, index, count);
    if (index == -1) {
        printf("unknown\n");
    } else {
        printf("%d\n", a[index]);
    }

    v = 6;
    index = array_half_find(a, alen, v);
    count = array_half_find_count(a, alen, v);
    printf("v = %d, index = %d, count = %d, value = ", v, index, count);
    if (index == -1) {
        printf("unknown\n");
    } else {
        printf("%d\n", a[index]);
    }

    v = 3;
    count = array_half_find_count_optimize(a, alen, v);
    printf("v = %d, count = %d\n", v, count);
    assert(count == 4);

    exit(EXIT_SUCCESS);
}
