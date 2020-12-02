#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

// 数组数组长度
#define LEN(a)  (sizeof(a)/sizeof(*a))

void sort_insert(int * a, int n);
void sort_merge(int * a, int n);

void print(int * a, int n);

/*
 * merge 归并排序(默认升序)
 * 
 * cc -g -O2 -Wall -Wextra -Wno-sign-compare -o sort_merge sort_merge.c
 */
int main(void) {
    int a[] = {5, 2, 4, 6, 1, 3};

    print(a, LEN(a));

    sort_insert(a, LEN(a));

    print(a, LEN(a));

    sort_merge(a, LEN(a));

    print(a, LEN(a));

    exit(EXIT_SUCCESS);
}

void 
print(int * a, int n) {
    for (int i = 0; i < n; i++)
        printf("%d ", a[i]);

    putchar('\n');
}

void 
sort_insert(int * a, int n) {
    for (int i = 1; i < n; i++) {
        int j = i, v = a[i];
        if (v < a[j-1]) {
            do {
                a[j] = a[j-1];
                --j;
            } while(j > 0 && v < a[j-1]);
            a[j] = v;
        }
    }
}

static void sort_merge_partial_merge(int * a, int left, int right, int * tmp) {
    int mid = (left + right) >> 1;
    int i = left;       // 左边序列
    int j = mid + 1;    // 右边序列
    int k = 0;          // 临时索引记录

    // 特殊情况也可以优化成, 特殊二分查找插入排序 
    while (i <= mid && j <= right) {
        tmp[k++] = a[i] <= a[j] ? a[i++] : a[j++]; 
    }

    // 左边元素或右边元素插入
    if (i <= mid) {
        memcpy(tmp + k, a + i, (mid - i + 1) * sizeof(int));
    } else if (j <= right) {
        memcpy(tmp + k, a + j, (right - j + 1) * sizeof(int));
    }

    // 数据变更
    memcpy(a + left, tmp, right - left + 1);

    print(a, 6);
}

static void sort_merge_partial(int * a, int left, int right, int * tmp) {
    if (left < right) {
        int mid = (left + right) >> 1;
        // 左边归并排序, 使得左子序列有序
        sort_merge_partial(a, left, mid, tmp);
        // 右边归并排序, 使得右子序列有序
        sort_merge_partial(a, mid + 1, right, tmp);
        // 将两个有序子数组合并操作
        sort_merge_partial_merge(a, left, right, tmp);
    }
}

void 
sort_merge(int * a, int n) {
    if (!a || n <= 1) return;

    // 依赖 merge 算法
    int * tmp = malloc(((n + 1) >> 1) * sizeof(int));
    assert(tmp);

    sort_merge_partial(a, 0, n - 1, tmp);

    free(tmp);
}