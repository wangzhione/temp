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
    int sum = 0;
    for (int i = 1; i <= num; i++) {
        sum += number1(i);
    }
    return sum;
}

// build:
// gcc -g -O3 -Wall -Wextra -Werror -o 51_one 51_one.c
//
int main(void) {
    int data[] = { 0, 1, 10, 100, 1000, 10000, 20000, 30000, 40000, 50000, 90000, 100000, 1000000 };
    int len = sizeof data / sizeof *data;

    printf("data  : \n");
    for (int i = 0; i < len; i++) {
        printf("%8d -> %-8d\n", data[i], number1section(data[i]));
    }

    int data2[] = { 10000, 10001, 10002, 10010, 10230, 230, 20000, 22345, 2345, 33333, 30000, 3333 };
    int len2 = sizeof data2 / sizeof *data2;

    printf("data2 : \n");
    for (int i = 0; i < len2; i++) {
        printf("%8d -> %-8d\n", data2[i], number1section(data2[i]));
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

/*
       0 -> 0       
       1 -> 1       
      10 -> 2       
     100 -> 21      
    1000 -> 301     
   10000 -> 4001    
   20000 -> 18000   
   30000 -> 22000   
   40000 -> 26000   
   50000 -> 30000   
   90000 -> 46000   
  100000 -> 50001   
 1000000 -> 600001  
 
 Sum(n) = Sum(10^i) = i * 10^(i-1) + 1 其中 n = 10^i, 并且 i ∈ N
 Sum(10^i - 1) = i * 10^(i-1)

 Sum(a*n) = Sum(a * 10^i) 
 1' a = 1 时候, Sum(10^i) = i * 10^(i-1) + 1
 2' a 属于 [2, 9] 并且 a 属于 N 时候, 
    Sum(a * 10^i) = Sum(10^i - 1) + (最高位 1)Sum(10^i, 10^i + 10^(i-1) - 1) + (a-1)*Sum(10^i - 1)
                  = (a + 1)*Sum(10^i - 1) + Sum(10^i, 10^i + 10^(i-1) - 1)
                  = a*i*10^(i-1) + 10^i
                  = (a*i + 10)*10^(i-1)
 */

/*
data2 : 
   10000 -> 4001    
   10001 -> 4003    
   10002 -> 4004    
   10010 -> 4013    
   10230 -> 4384    
     230 -> 153     
   20000 -> 18000   
   22345 -> 19775   
    2345 -> 1775

Sum(10^i + b) = Sum(10^i-1) + (最高为 1)Sum(10^i, 10^i+b) + Sum(b) 其中 b 属于 N, i ∈ N
              = i*10^(i-1) + (1 + b) + Sum(b)

同样有
Sum(a * 10^i + b) = Sum(a * 10^i) + Sum(b) 其中 a 属于 N 并且 ∈ [2, 9], b ∈ N
                  = (a*i + 10)*10^(i-1) + Sum(b)
                  
 */