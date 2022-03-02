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

   我们把只包含因子 2, 3和5的数称作丑数 (Ugly Number). 
   求按从小到大的顺序的第 1500 个丑数. 例如 6, 8, 都是丑数, 但 14 不是,
   因为它包含因子 7. 习惯上我们把 1 当做第一个丑数.
 */

bool IsUgly(int number) {
    while (number % 2 == 0)
        number /= 2;
    while (number % 3 == 0)
        number /= 3;
    while (number % 5 == 0)
        number /= 5;
    // 1 是第一个丑数
    return number == 1;
}

int GetUglyNumber(int index) {
    if (index <= 0) {
        return 0;
    }

    int number = 0;
    int uglyFound = 0;
    while (uglyFound < index) {
        if (IsUgly(++number)) {
            ++uglyFound;
        }
    }

    return number;
}

static int ugly_min(int a, int b, int c) {
    if (a < b) {
        return a < c ? a : c;
    }
    return b < c ? b : c;
}

int GetUglyNumberOptimize(int index) {
    if (index <= 0) {
        return 0;
    }

    if (index == 1) {
        return 1;
    }

    // 丑数数组
    int * ugly = malloc(index * sizeof(int));
    *ugly = 1;

    // 下一个丑数位置
    int * next = ugly;

    // T2, T3, T5
    int * ugly_t2 = ugly;
    int * ugly_t3 = ugly;
    int * ugly_t5 = ugly;

    while (++next - ugly < index) {
        int min = ugly_min(*ugly_t2*2, *ugly_t3*3, *ugly_t5*5);

        // 思路太巧妙了.
        while (*ugly_t2*2 <= min)
            ugly_t2++;
        while (*ugly_t3*3 <= min)
            ugly_t3++;
        while (*ugly_t5*5 <= min)
            ugly_t5++;

        *next = min;
    }

    free(ugly);

    return next[-1];
}

// build:
// gcc -g -O3 -Wall -Wextra -Werror -o 53_ugly_number 53_ugly_number.c
//
int main(void) {
    int number;
    int index = 1500;

    // index = 1500 ugly 859963392
    // number = GetUglyNumber(index);
    number = GetUglyNumberOptimize(index);
    printf("index = %d ugly %d\n", index, number);

    exit(EXIT_SUCCESS);
}
