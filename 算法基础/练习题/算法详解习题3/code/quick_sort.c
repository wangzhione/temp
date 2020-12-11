#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

// 数组数组长度
#define LEN(a)  (sizeof(a)/sizeof(*a))

void print(int * a, int n);

void quick_sort(int * a, int n);

static void insert_sort(int * low, int * high);
static void heap_sort(int * a, ptrdiff_t n);
static void quick_sort_partial(int * low, int * high, int depth);

/*
 * quick sort 快排
 * 
 * cc -g -O2 -Wall -Wextra -Wno-sign-compare -o quick_sort quick_sort.c
 */
int main(void) {
    int a[] = {5, 2, 4, 6, 1, 3, 10, 9, 8, 7, 2, 1};

    print(a, LEN(a));

    insert_sort(a, a + LEN(a) - 1);

    print(a, LEN(a));

    heap_sort(a, LEN(a));

    print(a, LEN(a));

    int b[] = { 5, 5, 4, 6, 7, 7, 7 };
    print(b, LEN(b));
    quick_sort_partial(b, b + LEN(b) - 1, 1);
    print(b, LEN(b));

    quick_sort(a, LEN(a));
    print(a, LEN(a));

    quick_sort(b, LEN(b));
    print(b, LEN(b));

    int c[] = {5, 2, 4, 6, 1, 3, 10, 9, 8, 7, 2, 1, 5, 5, 4, 6, 7, 7, 7};
    print(c, LEN(c));
    quick_sort(c, LEN(c));
    print(c, LEN(c));

    exit(EXIT_SUCCESS);
}

void 
print(int * a, int n) {
    for (int i = 0; i < n; i++)
        printf("%d ", a[i]);

    printf(" | n = %d, a = 0x%p\n", n, a);
}

/**
 * quick sort 优化策略
 * 
 * 简单一点我们以 int 数值升序为例
 * 
 * 1. 指针代替索引
 * 
 * 2. 基准值算法采用三数取中
 * 基准策略是快排的核心, 有筛法能保证稳定在 O(nlogn). 
 * 多数都是在 O(nlogn) ~ O(n^2) 区间, 但算法复杂度期望是 O(nlogn).
 * 我们这里取了一个工程实现较好思路. 三数取中, 这个对有序数组有较好效果.
 * 三数取中, 算法整体复杂度还是 O(nlogn) ~ O(n^2) 区间. 
 * 
 * 3. 混合使用 交换排序, 插入排序, 堆排序 策略
 * 
 * 4. 采用三路划分, 剔除重复元素
 *  
 *  { 等于 | 小于 | 大于 | 等于 | } -> { 小于 | 等于 | 大于 }
 * 
 * 5. 尾递归
 * 
 */

static inline void swap(int * a, int * b) {
    int t = *a;
    *a = *b;
    *b = t;
}

static void heap_adjust(int * a, ptrdiff_t n, ptrdiff_t i) {
    int node = a[i];
    for (ptrdiff_t j = 2*i+1; j < n; j = 2*i+1) {
        if (j+1 < n && a[j] < a[j+1])
            j++;
        
        // 已经是大顶堆直接结束
        if (a[j] <= node)
            break;

        a[i] = a[j];
        i = j;
    }
    a[i] = node;
}

static void heap_sort(int * a, ptrdiff_t n) {
    // 构建大顶堆
    for (ptrdiff_t i = n/2-1; i >= 0; i--)
        heap_adjust(a, n, i);

    // 数据交换
    while (--n > 0) {
        // 堆顶元素与末尾元素进行交换
        swap(a, a + n);
        // 重新对堆进行调整
        heap_adjust(a, n, 0);
    }
}

#define INT_SORT_INSERT    (16)

static void insert_sort(int * low, int * high) {
    for (int * lo = low + 1; lo <= high; lo++) {
        int v = *lo, * hi = lo - 1;
        if (v < *hi) {
            do {
                hi[1] = *hi;
            } while (--hi >= low && v < *hi);
            hi[1] = v;
        }
    }
}

static inline void swap_if_great(int * a, int * b) {
    if (*a > *b)
        swap(a, b);
}

/**
 * low, mid, high 三个未知上数据, 选取他们中间数据作为轴驱
 * mid = low + ((high - low) >> 1)
 */
static inline int quick_pivot(int * low, int * high) {
    int * mid = low + ((high - low) >> 1);

    swap_if_great(mid, high);
    swap_if_great(low, high);
    swap_if_great(mid, low);

    // 此时 *mid < *low < *high
    // low 就是三个位置的中值
    return *low;
}

static inline bool quick_sort_before(int * low, int * high, int depth) {
    ptrdiff_t n = high - low;

    if (n <= INT_SORT_INSERT) {
        switch (n) {
        case 0:
        case 1:
            return true;
        case 2:
            swap_if_great(low, high);
            return true;
        case 3:
            swap_if_great(low, low+1);
            swap_if_great(low, high);
            swap_if_great(low+1, high);
            return true;
        default:
            insert_sort(low, high);
            return true;    
        }
    }

    if (depth <= 0) {
        heap_sort(low, n);
        return true;
    }

    return false;
}

static void quick_sort_partial(int * low, int * high, int depth) {
    if (quick_sort_before(low, high, depth))
        return;

    // 开始 quick sort pivot 分割
    int pivot = quick_pivot(low, high);

    // 尝试三路分割 { 等于 | 小于 | 大于 | 等于 | } -> { 小于 | 等于 | 大于 }
    //
    // --------------------------------------
    // | 等于 | 小于 | ... | 大于 | 等于 |
    // +      +      +     +      +      +
    // low    p      i     j      q      high
    // --------------------------------------
    //
    // -------------------------
    // | 小于 | 等于 | 大于 |
    // +      +      +      +
    // low    i      j      high
    // -------------------------
    //
    // p 指向序列左边等于 pivot 元素的位置
    // q 指向序列右边等于 pivot 元素的位置
    // i 指向从左到右扫描的元素位置
    // j 指向从右往左扫描的元素位置
    //
    // 
    int * p = low, * q = high, * i = low, * j = high;
    do {
        while (i < j && *j >= pivot) {
            // 找到与锚点相等的元素, 于是交换到 q 所指向的位置
            if (*j == pivot) 
                swap(j, q--);
            j--;
        }
        *i = *j;

        while (i < j && *i <= pivot) {
            // 找到与锚点相等的元素, 于是交换到 q 所指向的位置 
            if (*i == pivot)
                swap(i, p++);
            i++;
        }
        *j = *i;
    } while (i < j);
    *i = pivot;

    print(low, high - low + 1);

    // 开始处理两边重复元素, 将其交换到序列中间
    if (i == p)
        i = low - 1;
    else {
        i--;
        while (--p >= low) 
            swap(i--, p);
    }
    if (j == q) 
        j = high + 1;
    else {
        j++;
        while (++q <= high) 
            swap(j++, q);
    }

    print(low, high - low + 1);

    // 开始递归处理
    quick_sort_partial(low, i, depth - 1);
    // 尾递归
    quick_sort_partial(j, high, depth);
}

#define INT_QUCICK_SORT_DEPATH      (32)

void quick_sort(int * a, int n) {
    if (a && n > 1)
        quick_sort_partial(a, a + n - 1, INT_QUCICK_SORT_DEPATH);
}