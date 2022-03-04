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

   一个整数数组除了两个数字之外, 其他的数字都出现了两次.
   请写程序找出这两个只出现一次的数字. 要求时间复杂度 O(n), 空间复杂度 O(1)
 */

void array_printf(int a[], int len) {
    printf("array[%d]: ", len);
    for (int i = 0; i < len; i++) {
        printf("%4d", a[i]);
    }
    printf("\n");
}

int array_xor(int a[], int len) {
    int xor = 0;
    for (int i = 0; i < len; i++) {
        xor ^= a[i];
    }
    return xor;
}

static unsigned first_binary(int v) {
    if (v == 0) {
        return 0;
    }

    unsigned b = 1u;

    while ((b & v) == 0) {
        b <<= 1;
    }

    return b;
}

void array_find_two(int a[], int len) {
    if (a == NULL || len < 2) {
        printf("异常 1 无法找到\n");
        return;
    }

    if (len == 2) {
        if (a[0] == a[1]) {
            printf("异常 2 重复数据异常\n");
            return;
        }
        printf("找到了 %d 和 %d\n", a[0], a[1]);
        return;
    }

    int xor = array_xor(a, len);
    // 取 xor 中其中 1 位, 做数据分组.
    int firstb = first_binary(xor);
    if (firstb == 0) {
        printf("异常 3 xor 数据为 0\n");
        return;
    }

    // 开始分组
    int blen = 0;
    int b[len];
    int clen = 0;
    int c[len];

    for (int i = 0; i < len; i++) {
        printf("i = %d, a[i] = %d, a[i] & firstb = %d, blen=%d, clen=%d\n", i, a[i], a[i] & firstb, blen, clen);
        if (a[i] & firstb) {
            b[blen++] = a[i];
        } else {
            c[clen++] = a[i];
        }
    }

    int bxor = array_xor(b, blen);
    array_printf(b, blen);
    array_printf(c, clen);
    int cxor = array_xor(c, clen);
    printf("bxor = %d, cxor = %d\n", bxor, cxor);
}

// build:
// gcc -g -O3 -Wall -Wextra -Werror -o 62_array_xor 62_array_xor.c
//
int main(void) {
    int a[] = { 2, 4, 3, 6, 3, 2, 5, 5, 4 };
    int alen = sizeof a / sizeof *a;

    array_printf(a, alen);

    int xor = array_xor(a, alen);
    printf("xor = %d\n", xor);

    int b[] = { 2, 4, 3, 6, 3, 2, 5, 5 };
    int blen = sizeof b / sizeof *b;

    array_printf(b, blen);

    // 位运算校验
    int32_t i = INT32_MIN;
    uint32_t j = 0x80000000;
    printf("%x & %x = %x : %d\n", i, j, i & j, (i & j) > 0);

    array_find_two(b, blen);

    exit(EXIT_SUCCESS);
}
