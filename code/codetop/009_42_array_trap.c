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

/* describe : 42. 接雨水

   给定 n 个非负整数表示每个宽度为 1 的柱子的高度图, 计算按此排列的柱子, 下雨之后能接多少雨水.
 */

int trap(int * height, int heightSize) {
    if (height == NULL || heightSize <= 1) {
        return 0;
    }

    int ans = 0;
    int left = 0, right = heightSize - 1;
    int left_max = 0, right_max = 0;
    while (left < right) {
        if (height[left] < height[right]) {
            if (height[left] >= left_max) {
                left_max = height[left];
            } else {
                ans += left_max - height[left];
            }
            left++;
        } else {
            if (height[right] >= right_max) {
                right_max = height[right];
            } else {
                ans += right_max - height[right];
            }
            right--;
        }
    }

    return ans;
}

// build:
// gcc -g -O3 -Wall -Wextra -Werror -o 009_42_array_trap 009_42_array_trap.c
//
int main(void) {

    exit(EXIT_SUCCESS);
}

/*
 题解:
 非常类似: https://leetcode-cn.com/problems/best-time-to-buy-and-sell-stock/

 买卖股票
 */