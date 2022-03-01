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
 */

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
    int n = 0;
    for (int i = 1; i <= num; i++) {
        n += number1(i);
    }
    return n;
}

// build:
// gcc -g -O3 -Wall -Wextra -Werror -o 51_one 51_one.c
//
int main(void) {
    int data[] = { 0, 1, 10, 100, 1000, 10000, 100000, 1000000 };
    int len = sizeof data / sizeof *data;

    for (int i = 0; i < len; i++) {
        printf("%8d -> %-8d\n", data[i], number1section(data[i]));
    }

    exit(EXIT_SUCCESS);
}

/*
       0 -> 0       
       1 -> 1       
      10 -> 2       
     100 -> 21      
    1000 -> 301     
   10000 -> 4001    
  100000 -> 50001   
 1000000 -> 600001

 Sum(n) = Sum(10 ^ i) = i * 10^(i-1) + 1
 
 找到规律后可以用数学证明
*/