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

   输入一个整型数组, 数组里有正数也有负数. 数组中一个或连续的多个整数组成一个子数组.
   求所有子数组和的最大值. 要求时间复杂度为 O(n).

   例如输入的数组为 { 1, -2, 3, 10, -4, 7, 2, -5 }, 和最大的子数组为 { 3, 10, -4, 7, 2 },
   因此输出为该子数组的和 18.
 */

int FindGreatestSumOfSubArray(int * pData, int nLength) {
    if (pData == NULL || nLength <= 0) {
        // 空为 0, 什么都不获取
        return 0;
    }

    int nCurSum = 0;
    int nGreatestSum = 0;
    for (int i = 0; i < nLength; i++) {
        if (nCurSum <= 0) {
            if (pData[i] > 0) {
                nCurSum = pData[i];
            }
        } else {
            nCurSum += pData[i];
        }

        if (nCurSum > nGreatestSum) {
            nGreatestSum = nCurSum;
        }
    }

    return nGreatestSum;
}

int stock(int data[], int len) {
    if (data == NULL || len <= 0) {
        return 0;
    }

    int max = 0;
    int cur = 0;
    for (int i = 0; i < len; i++) {
        if (cur <= 0) {
            cur = data[i];
        } else {
            cur += data[i];
        }

        if (cur > max) {
            max = cur;
        }
    }

    return max;
}

// build:
// gcc -g -O3 -Wall -Wextra -Werror -o 50_stock 50_stock.c
//
int main(void) {
    int data[] = { 1, -2, 3, 10, -4, 7, 2, -5 };
    int len = sizeof data / sizeof *data;

    int sum = FindGreatestSumOfSubArray(data, len);
    printf("sum = %d\n", sum);

    sum = stock(data, len);
    printf("sum = %d\n", sum);

    exit(EXIT_SUCCESS);
}
