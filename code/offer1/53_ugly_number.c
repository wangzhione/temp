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
    printf("别这样2 a = %d, b = %d, c = %d\n", a, b, c);
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
    int * ugly = calloc(index,  sizeof(int));
    printf("别这样1 index = %d\n", index);
    *ugly = 1;

    // 下一个丑数位置
    int * next = ugly;

    // T2, T3, T5
    int * ugly_t2 = ugly;
    int * ugly_t3 = ugly;
    int * ugly_t5 = ugly;

    int min = 1;
    while (++next - ugly < index) {
        min = ugly_min(*ugly_t2*2, *ugly_t3*3, *ugly_t5*5);
        *next = min;

        printf("别这样3 min = %d, %zd\n", min, next - ugly);

        // 思路太巧妙了.
        while (*ugly_t2*2 <= min)
            ugly_t2++;
        while (*ugly_t3*3 <= min)
            ugly_t3++;
        while (*ugly_t5*5 <= min)
            ugly_t5++;
    }

    free(ugly);

    return min;
}

int nthUglyNumber0(int n) {
    // 假定 n 就是属于 [1, 1690]
    int ugly[n];
    int * ugly2 = ugly;
    int * ugly3 = ugly;
    int * ugly5 = ugly;

    int * next = ugly;

    ugly[0] = 1;
    while (++next - ugly < n) {
        // ugly 2 3 5 指针变化
        int num2 = *ugly2 * 2;
        int num3 = *ugly3 * 3;
        int num5 = *ugly5 * 5;
        
        // min 最小值
        int min = num2 < num3 ? num2 : num3;
        if (min > num5) {
            min = num5;
        }
        *next = min;

        if (min == num2) {
            ugly2++;
        } 
        if (min == num3) {
            ugly3++;
        } 
        if (min == num5) {
            ugly5++;
        }
    }

    return next[-1];
}

int nthUglyNumber(int n) {
    // 假定 n 就是属于 [1, 1690]
    int ugly[n];
    int * ugly2 = ugly;
    int * ugly3 = ugly;
    int * ugly5 = ugly;

    int * next = ugly+1;

    ugly[0] = 1;
    while (next - ugly < n) {
        // ugly 2 3 5 指针变化
        int num2 = *ugly2 * 2;
        int num3 = *ugly3 * 3;
        int num5 = *ugly5 * 5;
        
        // min 最小值
        int min = num2 < num3 ? num2 : num3;
        if (min > num5) {
            min = num5;
        }

        *next++ = min;

        while (*ugly2 * 2 <= min)
            ugly2++;
        while (*ugly3 * 3 <= min)
            ugly3++;
        while (*ugly5 * 5 <= min)
            ugly5++;
    }

    return next[-1];
}

// build:
// gcc -g -O3 -Wall -Wextra -Werror -o 53_ugly_number 53_ugly_number.c
//
int main(void) {
    int number = 1, number2;
    int index = 10;

    // index = 1500 ugly 859963392
    // number = GetUglyNumber(index);
    // number = GetUglyNumberOptimize(index);
    printf("index = %d ugly %d\n", index, number);

    index = 10;
    number = nthUglyNumber(index);
    number2 = nthUglyNumber0(index);
    printf("number = %d, number2 = %d\n", number, number2);

    index = 1500;
    number = nthUglyNumber(index);
    number2 = nthUglyNumber0(index);
    printf("number = %d, number2 = %d\n", number, number2);

    exit(EXIT_SUCCESS);
}
