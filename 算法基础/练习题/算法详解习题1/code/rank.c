#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

extern bool rank_heap(int a[], int n, int k, int * v);
extern void print(int * a, int n);

/*
 * rank k 
 * 无序数组中找第 k 大的数
 * 
 * cc -g -O2 -Wall -Wextra -Wno-sign-compare -o rank rank.c
 */
int main(void) {
    int a[] = { 1, 6, 5, 4, 3, 2 };
    int n = sizeof a / sizeof * a;
    int k = 2;

    print(a, n);

    int value;
    bool ok = rank_heap(a, n, k, &value);
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
 * 简单点, 我们用堆结构来处理这类 第 k 大问题.
 * 
 * 1. 我们先通过原始数据构建一个 k 数量的小顶堆 
 * 2. 我们开始遍历原始 [k, n) 数据, 和 k 数量的小顶堆
 *    2.1 比较如果值比堆顶大, 修改堆顶值, 并重建
 * 3. 一直到结束, k 数量的小顶堆的堆顶就是第 k 大的数据
 * 
 * 优点 : 
 *      稳定, nlogk
 * 缺点 :
 *      需要维护小顶堆结构 
 */

// 维护一个小顶堆
static void heap_min_adjust_partial(int * m, int k, int i) {
    int node = m[i];
    for (int j = 2*i+1; j < k; j = 2*j+1) {
        if (j+1 < k && m[j] > m[j+1])
            j++;
        
        // 已经是小顶堆直接结束
        if (node <= m[j])
            break;

        m[i] = m[j];
        i = j;
    }
    m[i] = node;
}

// 构建一个小顶堆
static void heap_min_adjust(int * m, int k) {
    for (int i = k / 2 - 1; i >= 0; i--)
        heap_min_adjust_partial(m, k, i);
}


/*
 * 通过小顶堆结构处理第 k 大问题
 */
static int rank_heap_min(int a[], int n, int k) {
    int value;

    // 特殊情况, k 为 1 表示, 获取最大那个元素
    if (k == 1) {
        value = a[0];
        for (int i = 1; i < n; i++)
            if (value < a[i])
                value = a[i];
        return value;
    }

    // 开始构建 k 大小 小顶堆
    int * m = malloc(k * sizeof(int));
    memcpy(m, a, k * sizeof(int));
    heap_min_adjust(m, k);
    print(m, k);

    for (int i = k; i < n; i++) {
        // 遇到比小顶堆, 堆顶还大的元素, 开始重新调整
        if (m[0] < a[i]) {
            m[0] = a[i];
            heap_min_adjust_partial(m, k, 0);
        }
    }

    // 保留最终结果
    value = m[0];

    free(m);
    return value;
}

static void heap_max_adjust_partial(int * m, int k, int i) {
    int node = m[i];
    for (int j = 2*i+1; j < k; j = 2*j+1) {
        if (j+1 < k && m[j] < m[j+1])
            j++;
        
        // 已经是大顶堆直接结束
        if (node >= m[j])
            break;

        m[i] = m[j];
        i = j;
    }
    m[i] = node;
}

// 构建一个小顶堆
static void heap_max_adjust(int * m, int k) {
    for (int i = k / 2 - 1; i >= 0; i--)
        heap_max_adjust_partial(m, k, i);
}

/*
 * 通过大顶堆结构处理第 k 小问题
 */
static int rank_heap_max(int a[], int n, int k) {
    int value;

    // 特殊情况, k 为 1 表示, 获取最小那个元素
    if (k == 1) {
        value = a[0];
        for (int i = 1; i < n; i++)
            if (value > a[i])
                value = a[i];
        return value;
    }

    // 开始构建 k 大小 大顶堆
    int * m = malloc(k * sizeof(int));
    memcpy(m, a, k * sizeof(int));
    heap_max_adjust(m, k);

    for (int i = k; i < n; i++) {
        // 遇到比大顶堆, 堆顶还小的元素, 开始重新调整
        if (m[0] > a[i]) {
            m[0] = a[i];
            heap_max_adjust_partial(m, k, 0);
        }
    }

    // 保留最终结果
    value = m[0];

    free(m);
    return value;
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
rank_heap(int a[], int n, int k, int * v) {
    // 异常情况, 没有找到, 直接返回
    if (!a || n <= 0 || k > n || k <= 0)
        return false;

    int value;

    if (2 * k > n) {
        // 找第 k 大变为找 第 n - k 小的
        value = rank_heap_max(a, n, n - k);
    } else {
        value = rank_heap_min(a, n, k);
    }

    if (v)
        *v = value;
    return true;
}