#include "heap.h"

/*
    问题: 找到数据流中第 K 大元素

    例如:
    3 | {4, 5, 8} -> 4
    3 | {4, 5, 8, 2} -> 4
    3 | {4, 5, 8, 2, 3} -> 4
    3 | {4, 5, 8, 2, 3, 5} -> 5
    3 | {4, 5, 8, 2, 3, 5, 10} -> 5
    3 | {4, 5, 8, 2, 3, 5, 10, 9} -> 8
    3 | {4, 5, 8, 2, 3, 5, 10, 9, 8} -> 8
 */

/*
    分析: 我们用 K 个元素 小顶堆 结构

    步骤: 
    0. 前置健壮性检查
    1. 创建一个 K 个元素小顶堆, 在其中添加 K 个元素
    2. 小顶堆堆顶值最小, 让它同待添加元素比较, 如果待添加元素值大直接替换走小顶堆下沉操作
    3. 数据流比较完毕, 小顶堆顶就是 第 K 大值

    复杂度:
        时间复杂度: O(n*log(K)), 其中向堆中添加元素时间复杂度为 O(log(K))
        空间复杂度: O(K), 优先队列中只用存储 K 个元素
 */

static int cmp(int * left, int * right) {
    return *left - *right;
}

int main(void) {
    int a[] = { 4, 5, 8, 2, 3, 5, 10, 9, 8 };
    int n = sizeof(a) / sizeof(*a);
    int k = 3;

    int i = 0;

    heap_t h = heap_create(cmp);
    IF(h == NULL && (k < 0 || k > n));

    for (; i < k; i++) {
        heap_push(h, a+i);
    }
    
    for (;;) {
        int * node = heap_top(h);
        printf("%d\n", *node);

        if (i >= n)
            break;

        if (cmp(node, a+i) < 0) {
            heap_pop_push(h, a+i);
        }
        
        i++;
    }

    heap_delete(h, NULL);

    // 检查原始数据是否错乱
    for (i = 0; i < n; i++) {
        printf("%d ", a[i]);
    }
    putchar('\n');

    exit(EXIT_SUCCESS);
}