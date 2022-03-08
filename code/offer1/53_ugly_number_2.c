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

   https://leetcode-cn.com/problems/ugly-number-iii/
   https://leetcode-cn.com/problems/ugly-number-iii/solution/1201chou-shu-iii-by-jinhei-irdx/
   https://leetcode-cn.com/problems/ugly-number-iii/solution/cer-fen-jie-fa-zhe-dao-ti-shi-zen-yao-be-b7r9/

   给出四个整数: n, a, b, c 请你设计一个算法来找出第 n 个丑数.

   丑数是可以被 a 或 b 或 c 整除的正整数. 
 */

/* 思路 : 
   
   难点在于内存空间会被玩坏了. 这里只能借助数学力量.

   首先设 a 和 b 的最小公倍数为 gcd_ab, a 和 c 的最小公倍数是 gcd_ac,
   b 和 c 的最小公倍数 gcd_bc, a b c 三者最小的公倍数 gcd_abc.

   那么 1 到 x 即 [1, x] 中有多少个丑数呢?

   即:
   有 x / a 个数可以被 a 整除
   有 x / b 个数可以被 b 整除
   有 x / c 个数可以被 c 整除
   有 x / gcd_ab 个数可以同时被 a 和 b 整除
   有 x / gcd_ac 个数可以同时被 a 和 c 整除
   有 x / gcd_bc 个数可以同时被 b 和 c 整除
   有 x / gcd_abc个数可以同时被 a, b c 整除
   则:
   [1, x] 的丑数数量为:
   x/a + x/b + x/c - x/gcd_ab - x/gcd_ac - x/gcd_bc + x/gcd_abc

   分析: 容斥原理
   ∣A∪B∪C∣=∣A∣+∣B∣+∣C∣−∣A∩B∣−∣A∩C∣−∣B∩C∣+∣A∩B∩C∣
   在计算可以被 a 整除和可以被 b 整除的数时候, 可以同时被 a和b 整除的数
   被计算了两次. a和c, b和c 也类似. 但其中同时被 a, b和c 整除的数
   3 - 3 没有了, 需要再补上. 即最后的 + x/gcd_abc

   因此上面题目可以转化为 [1, x] 范围内查找一个数为 mid, 即 [1, mid] 中含有
   丑数的数量等于 n 且前一个数丑数小于 n.

   算法步骤
   1. 求出 gcd_ab gcd_ac gcd_bc gcd_abc
   2. 求 [1, x] 的丑数数量
   3. 用二分进行查找看那个数满足.
 */

long gcd(int numa, int numb) {
    // 1. 若 a % b 得 余数 c
    // 2. 若 c == 0, 则 b 为两数最大公约数
    // 3. 若 c != 0, 则 a = b, b = c goto 1
    int a, b, c;

    assert(numa > 0 && numb > 0);
    if (numa == numb) {
        return numa;
    }

    if (numa > numb) {
        a = numa;
        b = numb;
    } else {
        a = numb;
        b = numa;
    }

    while ((c = a % b) != 0) {
        a = b;
        b = c;
    }

    return (long)numa/b*(long)numb;
}

/*
    1 <= n, a, b, c <= 10^9
    1 <= a * b * c <= 10^18
    本题结果在 [1, 2 * 10^9] 的范围内
 */
int nthUglyNumber(int n, int a, int b, int c){
    if (n < 1 || a < 1 || b < 1 || c < 1) {
        return 0;
    }

    long gcd_ab = gcd(a, b);
    long gcd_ac = gcd(a, c);
    long gcd_bc = gcd(b, c);
    long gcd_abc = gcd(gcd_ab, c);

    printf("gcd_ab = %ld, gcd_ac = %ld, gcd_bc = %ld, gcd_abc = %ld\n", gcd_ab, gcd_ac, gcd_bc, gcd_abc);

#define NUMX(x) ((x)/a+(x)/b+(x)/c-(x)/gcd_ab-(x)/gcd_ac-(x)/gcd_bc+(x)/gcd_abc)

    // 确定大致区间
    int low = 1;
    int high = a < b ? a : b;
    if (c < high) {
        high = c;
    }
    high = n * high;

    while (low <= high) {
        int middle = (high-low)/2+low;
        int num = NUMX(middle);
        if (num < n) {
            low = middle + 1;
        } else {
            high = middle - 1;
        }
    }

    return low;

#undef  NUMX
}

//求出0-x之间有多少个a b c的倍数
//最大公约数
int gcd2(int a, int b)
{
    return a % b == 0 ? b : gcd2(b, a % b);
}
//最小公倍数
long lcm (long a, long b)
{
    return a / gcd2(a, b) * b;
}

// build:
// gcc -g -O3 -Wall -Wextra -o 53_ugly_number_2 53_ugly_number_2.c
// gcc -g -O3 -Wall -Wextra -Werror -o 53_ugly_number_2 53_ugly_number_2.c
//
int main(void) {
    int ride;
    
    // error: division by zero [-Werror=div-by-zero]
    // ride = 1 % 0;
    // 非法指令 (核心已转储)

    int n;
    int a, b, c;

    n = 3, a = 2, b = 3, c = 5;
    ride = nthUglyNumber(n, a, b, c);
    printf("n = %d, a = %d, b = %d, c = %d, ride = %d\n", n, a, b, c, ride);
    assert(ride == 4);

    n = 4, a = 2, b = 3, c = 4;
    ride = nthUglyNumber(n, a, b, c);
    printf("n = %d, a = %d, b = %d, c = %d, ride = %d\n", n, a, b, c, ride);
    assert(ride == 6);

    n = 5, a = 2, b = 11, c = 13;
    ride = nthUglyNumber(n, a, b, c);
    printf("n = %d, a = %d, b = %d, c = %d, ride = %d\n", n, a, b, c, ride);
    assert(ride == 10);

    // 存在数据越界
    // n = 1000000000, a = 2, b = 217983653, c = 336916467;
    // gcd_ab = 435967306, gcd_ac = 673832934, gcd_bc = -671709793, gcd_abc = -1343419586

    n = 1000000000, a = 2, b = 217983653, c = 336916467;
    ride = nthUglyNumber(n, a, b, c);
    printf("n = %d, a = %d, b = %d, c = %d, ride = %d\n", n, a, b, c, ride);
    
    long ab = lcm(a, b);
    long ac = lcm(a, c);
    long bc = lcm(b, c);
    long abc = lcm(ab, c);
    printf("ab = %ld, ac = %ld, bc = %ld, abc = %ld\n", ab, ac, bc, abc);
    
    assert(ride == 1999999984);

    exit(EXIT_SUCCESS);
}
