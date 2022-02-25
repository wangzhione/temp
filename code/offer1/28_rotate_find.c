#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>

/*
 题目:

 把一个数组最开始若干个元素搬到数组的末尾, 我们称之为数组的旋转. 
 输入一个递增排序的数组的一个旋转, 输出旋转数组的最小元素. 例如数组
 { 3, 4, 5, 1, 2 } 的一个旋转, 该数组最小值为 1.
 */

int rotate_find(int a[], int len) {
    if (a == NULL || len <= 0) {
        return INT_MIN;
    }

    if (len == 1) {
        return a[0];
    }

    if (len == 2) {
        return a[0] < a[1] ? a[0] : a[1];
    }

    if (len == 3) {
        int v = a[0] < a[1] ? a[0] : a[1];
        if (v > a[2]) {
            v = a[2];
        }
        return v;
    }

    // 尝试二分查找
    /*
       /
      /  
          \
           \
     */

    // a[0], a[len-1], a[mid] mid = len/2
    int mid = len/2;
    if (a[0] == a[mid] && a[0] == a[len-1]) {
        int v = a[0];
        for (int i = 1; i < len; i++) {
            if (v > a[i]) {
                v = a[i];
            }
        }
        return v;
    }

    // 顺序情况
    if (a[0] <= a[len-1]) {
        return a[0];
    }

    if (a[0] <= a[mid]) {
        if (a[mid] > a[mid+1]) {
            // mid 是最大值索引, 自然 mid+1 是最小值索引
            return a[mid+1];
        }
        // 说明 [0, mid] 区间, 数据是递增, 最小值不在这个范围
        return rotate_find(a+mid+1, len-mid-1);
    }
    if (a[mid] < a[mid-1]) {
        // mid-1 如果比 mid 大说明刚好 mid 是旋转点
        return a[mid];
    }
    // 说明 [0, mid] 包含了 最小值区间
    return rotate_find(a, mid-1);
} 

int rotate_find_index(int a[], int len) {
    if (a == NULL || len <= 0) {
        return -1;
    }

    int low = 0;
    int high = len - 1;
    int mid = low;

    while (a[low] >= a[high]) {
        if (high - low == 1) {
            mid = high;
            break;
        }

        mid = (low + high) / 2;
        // 如果下标为 low, high 和 mid 指向三个数字相等. 则只能顺序查找
        if (a[low] == a[high] && a[low] == a[mid]) {
            int min = low;
            for (int i = low + 1; i <= high; i++) {
                if (a[min] > a[i]) {
                    min = i;
                }
            }
            return min;
        }

        if (a[mid] >= a[low])
            low = mid;
        else if (a[mid] <= a[high])
            high = mid;
    }
}

// build:
// gcc -g -O3 -Wall -Wextra -Werror -o 28_rotate_find 28_rotate_find.c
//
int main(void) {
    // int a[] = { 3, 4, 5, 1, 2 };
    // assert(min == 1);

    // int a[] = { 1, 0, 1, 1, 1 };

    int a[] = { 0, 1, 2, 3, 4, 5, 6 };
    int len = sizeof(a) / sizeof(int);

    printf("a : ");
    for (int i = 0; i < len; i++) {
        printf(" %d", a[i]);
    }
    printf("\n");

    int min = rotate_find(a, len);

    printf("min = %d\n", min);

    exit(EXIT_SUCCESS);
}