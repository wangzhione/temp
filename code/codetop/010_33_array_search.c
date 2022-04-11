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

/* describe : 33. 搜索旋转排序数组

   整数数组 nums 按升序排列, 数组中的值 互不相同.

   在传递给函数之前, nums 在预先未知某个下标 k (0 <= k < nums.length) 上进行了旋转, 
   使数组变为 [nums[k], nums[k+1], ..., nums[n-1], nums[0], nums[1], ..., nums[k-1]] (下标 从0开始 计数).
   例如, [0, 1, 2, 4, 5, 6, 7] 在下标 3 处旋转后可能变为 [4, 5, 6, 7, 0, 1, 2].

   给你旋转后的数组 nums 和 一个整数 target, 如果 nums 中存在这个目标值 target, 则返回它的下标, 否则 返回 -1.
 */

int search_middlen(int * nums, int left, int right, int target) {
    while (left <= right) {
        int middlen = (left+right)/2;
        if (target == nums[middlen]) {
            return middlen;
        }
        if (target < nums[middlen]) {
            right = middlen-1;
        } else {
            left = middlen+1;
        }
    }
    return -1;
}

int searchk(int * nums, int numsSize) {
    if (nums == NULL || numsSize <= 0 ) {
        return -1;
    }

    int left = 0;
    int right = numsSize-1;

    while (left <= right) {
        // 如果整个区域递增, 直接返回 | 这些思路非常巧妙
        if (nums[left] <= nums[right]) {
            return left;
        }

        int middlen = left + (right-left)/2;

        if (nums[left] <= nums[middlen]) {
            left = middlen+1;
        } else {
            // nums[middlen] 也有可能
            right = middlen;
        }
    }

    return -1;
}

int search(int * nums, int numsSize, int target) {
    // 异常检测
    if (nums == NULL || numsSize <= 0) {
        return -1;
    }

    int left = 0;
    int right = numsSize-1;

    //   /                /
    //  /     旋转之后-> /
    // /                    /
    // 
    // [left, right] 一定存在 a[left] > a[right] 并且 a[left] > { 翻转部分 a[right] 集合}
    // 假定三个点 left , k, right. k 是翻转点, 同样 a[k] 最大
    // 因此 查找一共分为 2 种情况, middlen 在 k 左边, middlen 在 k 的右边
    // 

    while (left <= right) {
        // 为什么不是 (left+right)/2
        // 当 right 为能表示的最大数时, 继续算加法会溢出, 而算减法不会溢出
        int middlen = left + (right-left)/2;
        if (nums[middlen] == target) {
            return middlen;
        }

        if (nums[left] <= nums[middlen]) {
            if (nums[left] <= target && target < nums[middlen]) {
                right = middlen - 1;
            } else {
                left = middlen + 1;
            }
        } else {
            if (nums[right] >= target && target > nums[middlen]) {
                left = middlen+1;
            } else {
                right = middlen-1;
            }
        }
    }

    return -1;
}

// build:
// gcc -g -O3 -Wall -Wextra -Werror -o 010_33_array_search 010_33_array_search.c
//
int main(void) {
    int nums1[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
    int left1 = 0;
    int right1 = 7;
    int target1 = 1;
    int index = search_middlen(nums1, left1, right1, target1);
    printf("index = %d\n", index);

    int nums2[] = {4,5,6,7,0,1,2};
    int target = 0;
    int k = searchk(nums2, sizeof(nums2)/sizeof(int));
    printf("k = %d, a[k] = %d\n", k, nums2[k]);
    index = search(nums2, sizeof(nums2)/sizeof(int), target);
    printf("index = %d\n", index);

    exit(EXIT_SUCCESS);
}
