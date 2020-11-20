#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

extern bool rank_quick(int a[], int n, int k, int * v);
extern void print(int * a, int n);

extern void sort_quick(int a[], int n);

/*
 * rank k 
 * 无序数组中找第 k 大的数
 * 
 * cc -g -O2 -Wall -Wextra -Wno-sign-compare -o rank2 rank2.c
 */
int main(void) {
    int a[] = { 1, 6, 5, 4, 3, 2 };
    int n = sizeof a / sizeof * a;
    int k = 2;

    print(a, n);

    sort_quick(a, n);

    print(a, n);

    int value;
    bool ok = rank_quick(a, n, k, &value);
    printf("k = %d, ok = %d, value = %d\n", k, ok, value);

    exit(EXIT_SUCCESS);
}

void 
print(int * a, int n) {
    for (int i = 0; i < n; i++)
        printf("%d ", a[i]);

    putchar('\n');
}

/*
 * 简单点, 我们用快排思路来处理这类 第 k 大问题.
 * 
 * 快排中的 partition 算法, 每次都能确定一个位置. 这就是算法入口
 * 
 * 优点 : 
 *      快速, nlogk
 * 缺点 :
 *      递归, 最坏情况 n^2
 */

static int partition(int a[], int left, int right) {
    int v = a[left];

    while (left < right) {
        while (v >= a[right]) {
            if (left >= --right)
                goto ret_end;
        }
        a[left] = a[right];

        do {
            if (++left >= right)
                goto ret_end;
        } while (v <= a[left]);    
        a[right--] = a[left];
    }

ret_end:
    a[left] = v;
    return left;
}

static void sort_quick_partial(int a[], int left, int right) {
    while (left < right) {
        // 分割得到锚点, 左大, 右小
        int anchor = partition(a, left, right);
        
        // 递归处理左半部分 
        sort_quick_partial(a, left, anchor - 1);

        // 右半部分, 下次循环继续
        left = anchor + 1;
    }
}

void
sort_quick(int a[], int n) {
    if (a || n >= 0) {
        sort_quick_partial(a, 0, n - 1);
    }
}

static int rank_quick_select(int a[], int left, int right, int k) {
    int anchor = partition(a, left, right);
    int n = anchor - left + 1;
    if (n == k)
        return a[anchor];
    if (n > k)
        return rank_quick_select(a, left, anchor - 1, k);
    return rank_quick_select(a, anchor + 1, right, k - n);
}

/*
 * rank 通过小顶堆结构处理第 k 大的问题
 * a        : 原始数据
 * n        : 原始数据大小
 * k        : 第 k 大, 范围 [1, n]
 * v        : 返回第 k 大的具体数值
 * return   : false 标识没有找到, true 标识找到
 */
bool 
rank_quick(int a[], int n, int k, int * v) {
    // 异常情况, 没有找到, 直接返回
    if (!a || n <= 0 || k > n || k <= 0)
        return false;

    int value; 

    // 特殊情况, 特殊处理
    if (k == 1) {
        value = a[0];
        for (int i = 1; i < n; i++) {
            if (value < a[i])
                value = a[i];
        }
        return value;        
    } else {
        // 进入快速排序
        value = rank_quick_select(a, 0, n - 1, k);
    }

    if (v)
        *v = value;
    return true;
}
