#include <stdio.h>
#include <errno.h>
#include <wchar.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* describe : 

   超级丑数是一个正整数, 并满足所有质因数出现在质数数组 primes 中.
   给你一个整数 n 和一个整数数组 primes, 返回第 n 个超级丑数.
   日暮数据保证 第 n 个超级丑数在 32-bit 带符号整数范围内.

    1 <= n <= 106
    1 <= primes.length <= 100
    2 <= primes[i] <= 1000
    题目数据 保证 primes[i] 是一个质数
    primes 中的所有值都 互不相同 ，且按 递增顺序 排列

    https://leetcode-cn.com/problems/super-ugly-number/
 */

int ugly_primes_min(int ** ugly_pointer, int * primes, int primesSize) {
    // [] 优先级高于 *
    int min = *ugly_pointer[0] * primes[0];
    for (int i = 1; i < primesSize; i++) {
        int num = *ugly_pointer[i] * primes[i];
        if (min > num) {
            min = num;
        }
    }
    return min;
}

int nthSuperUglyNumber(int n, int * primes, int primesSize) {
    int i;
    int res, min, num;
    int * ugly = malloc(n * sizeof(int));
    int ** ugly_pointer = malloc(primesSize * sizeof(int *));
    int * next = ugly;

    // 数据初始化
    ugly[0] = 1;
    for (i = 0; i < primesSize; i++) {
        ugly_pointer[i] = ugly;
    }

    while (++next - ugly < n) {
        min = *ugly_pointer[0] * primes[0];
        for (i = 1; i < primesSize; i++) {
            num = *ugly_pointer[i] * primes[i];
            if (min > num) {
                min = num;
            }
        }

        *next = min;

        // 指针移动
        printf("min = %4d | ", min);
        for (i = 0; i < primesSize; i++) {
            printf(" %4d ", *ugly_pointer[i]*primes[i]);
            if (min == *ugly_pointer[i]*primes[i]) {
                ugly_pointer[i]++;
            }
        }
        printf("\n");
    }

    res = next[-1];

    free(ugly_pointer);
    free(ugly);

    return res;
}

// build:
// gcc -g -O3 -Wall -Wextra -Werror -o 53_ugly_number_3 53_ugly_number_3.c
//
int main(void) {
    int n;
    int primes1[] = { 2, 7, 13, 19 };

    n = 12;
    int res = nthSuperUglyNumber(n, primes1, (int)(sizeof primes1 / sizeof *primes1));
    printf("n = %4d, res = %4d\n", n, res);
    assert(res == 32);

    int primes2[] = {2, 3, 5};
    n = 1;
    res = nthSuperUglyNumber(n, primes2, (int)(sizeof primes2 / sizeof *primes2));
    printf("n = %4d, res = %4d\n", n, res);
    assert(res == 1);

    exit(EXIT_SUCCESS);
}
