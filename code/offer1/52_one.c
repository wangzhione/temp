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

   输入一个整数 n, 求从 1 到 n 这个 n 个整数的十进制表示中 1 出现的次数.

   例如输入 12, 从 1 到 12 这些整数中包含 1 的数字有 1, 10, 11, 和 12, 1 一共出现 5 次

 Sum(n) = Sum(10^i) = i * 10^(i-1) + 1 其中 n = 10^i, 并且 i ∈ N
 Sum(10^i - 1) = i * 10^(i-1)

 Sum(a*n) = Sum(a * 10^i) 
 1' a = 1 时候, Sum(10^i) = i * 10^(i-1) + 1
 2' a 属于 [2, 9] 并且 a 属于 N 时候, 
    Sum(a * 10^i) = Sum(10^i - 1) + (最高位 1)Sum(10^i, 10^i + 10^(i-1) - 1) + (a-1)*Sum(10^i - 1)
                  = (a + 1)*Sum(10^i - 1) + Sum(10^i, 10^i + 10^(i-1) - 1)
                  = a*i*10^(i-1) + 10^i
                  = (a*i + 10)*10^(i-1)

Sum(10^i + b) = Sum(10^i-1) + (最高为 1)Sum(10^i, 10^i+b) + Sum(b) 其中 b 属于 N, i ∈ N
              = i*10^(i-1) + (1 + b) + Sum(b)

同样有
Sum(a * 10^i + b) = Sum(a * 10^i) + Sum(b) 其中 a 属于 N 并且 ∈ [2, 9], b ∈ N 并且 [0, 10^i)
                  = (a*i + 10)*10^(i-1) + Sum(b)
 */

int countDigitOnePartial(int n, int * sum) {
    // n 分解为 a * 10^i + b

    int i = 0;
    int p = 1;
    while (10*p <= n) {
        p *= 10;
        i++;
    }
    int a = n/p;
    int b = n-a*p;

    if (a == 1) {
        *sum += i*p/10+1;
    } else {
        *sum += a*i*p/10+p;
    }

    return b;
}

int countDigitOne(int n) {
    // 健壮性校验
    if (n <= 0) {
        return 0;
    }

    int sum = 0;
    do {
        // n 分解为 a * 10^i + b
        int i = 0;
        int p = 1;
        while (p <= n/10) {
            p *= 10;
            i++;
        }
        int a = n/p;
        int ap = a * p;
        // int b = n-a*p;
        n -= ap;
        if (a == 1) {
            sum += p/10*i + n + 1;
        } else {
            // sum += a*i*p/10+p;
            sum += ap/10*i + p;
        }
    } while (n > 0);

    return sum;
}

static int number1(int num) {
    int n = 0;
    while (num > 0) {
        if (num % 10 == 1) {
            n++;
        }
        num /= 10;
    }
    return n;
}

static int number1section(int num) {
    int sum = 0;
    for (int i = 1; i <= num; i++) {
        sum += number1(i);
    }
    return sum;
}

// build:
// gcc -g -O3 -Wall -Wextra -Werror -o 52_one 52_one.c
//
int main(void) {
    int i;
    int len = 10000;

    for (i = 0; i < len; i++) {
        int o1 = number1section(i);
        int o2 = countDigitOne(i);
        if (o1 != o2) {
            printf(" i = %d, o1 = %d, o2 = %d \n", i, o1, o2);
            break;
        }
    }

    if (i == len) {
        printf("%d 次测试通过\n", i);
    } else {
        printf("第 %d 次测试失败\n", i);
    }

    exit(EXIT_SUCCESS);
}
