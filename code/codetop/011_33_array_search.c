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

/* describe : 搜索旋转排序数组

   https://www.cnblogs.com/lfri/p/12553209.html
 */

void test_search1(void);

void test_search2(void);

// build:
// gcc -g -O3 -Wall -Wextra -Werror -o 011_33_array_search 011_33_array_search.c
//
int main(void) {

    test_search1();
    test_search2();

    exit(EXIT_SUCCESS);
}

/*
 https://leetcode-cn.com/problems/search-in-rotated-sorted-array/submissions/
 33. 搜索旋转排序数组

 整数数组 nums 按升序排列, 数组中的值互不相同.

 在传递给函数之前, nums 在预先未知的某个下标 k (0 <= k < nums.length) 上进行了 旋转,
 使数组变为 [nums[k], nums[k+1]], ..., nums[n-1], nums[0], nums[1], ... nums[k-1]] (下标从0开始计数).
 例如, [0, 1, 2, 4, 5, 6, 7] 在下标 3 处经旋转后可能变为 [4, 5, 6, 7, 0, 1, 2]..,

 给你旋转后的数组 nums 和 一个整数 targe, 如果 nums 中存在这个目标值 target, 则返回他的下标. 
 否则返回他的下标, 否则返回 -1.
 */

int search1(int * nums, int numsSize, int target) {
    int left = 0;
    int right = numsSize - 1;

    while (left <= right) {
        int middlen = left + (right - left) / 2;
        int value = nums[middlen];
        if (value == target) {
            return middlen;
        }

        if (value >= nums[left]) {
            // [left, middlen] 升序
            if (nums[left] <= target && target < value) {
                right = middlen-1;
            } else {
                left = middlen+1;
            }
        } else {
            // [middlen, right] 升序
            if (value < target && target <=nums[right]) {
                left = middlen+1;
            } else {
                right = middlen-1;
            }
        }
    }

    return -1;
}



void test_search1(void) {
    int nums1[] = {4, 5, 6, 7, 0, 1, 2};
    int nums1Size = sizeof(nums1) / sizeof(int);
    int target = 0;
    int index = search1(nums1, nums1Size, target);
    printf("target = %d, index = %d\n", target, index);
    assert(index == 4);

    target = 3;
    index = search1(nums1, nums1Size, target);
    printf("target = %d, index = %d\n", target, index);
    assert(index == -1);

    int nums2[] = {1};
    int nums2Size = sizeof(nums2)/sizeof(int);
    target = 0;
    index = search1(nums2, nums2Size, target);
    printf("target = %d, index = %d\n", target, index);
    assert(index == -1);
}

/*
 https://leetcode-cn.com/problems/search-in-rotated-sorted-array-ii/
 81. 搜索旋转排序数组 II
 已知存在一个按非降序排列的整数数组 nums, 数组中的值不必互不相同.

 在传递给函数之前, nums 在预先未知的某个下标 k (0 <= k < nums.length) 上进行了旋转,
 使数组变为 [nums[k], nums[k+1], ..., nums[n-1], nums[0], nums[1], ... , nums[k-1]]
 (下标从0开始计数). 例如, [0, 1, 2, 4, 4, 4, 5, 6, 6, 7] 在下标 5 处经旋转后可能变为
 [4, 5, 6, 6, 7, 0, 1, 2, 4, 4].

 给你旋转后的数组 nums 和一个整数 target, 请你编写一个函数来判断给定的目标值是否存在于数组中.
 如果 nums 中存在这个目标值 target, 则返回 true, 否则返回 false.

 你必须尽可能减少整个步骤

 示例1:
 输入: nums = { 2, 4, 6, 0, 0, 1, 2 }, target = 0
 输出: true

 示例2:
 输入: nums = { 2, 5, 6, 0, 0, 1, 2 }, target = 3
 输出: false
 */

bool search2(int * nums, int numsSize, int target) {
    int left = 0;
    int right = numsSize;

    while (left <= right) {
        int middlen = left + (right - left)/2;
        int value = nums[middlen];
        if (value == target) {
            return true;
        }
        // 排除干扰项
        if (nums[left] == value) {
            left++;
            continue;
        }
        if (nums[left] < value) {
            // [left, middlen] 非降序
            if (nums[left] <= target && target < value) {
                right = middlen-1;
            } else {
                left = middlen+1;
            }
        } else {
            // [middlen, right] 非降序
            if (value < target && target <= nums[right]) {
                left = middlen+1;
            } else {
                right = middlen-1;
            }
        }
    }

    return false;
}

void test_search2(void) {
    int nums1[] = { 2, 4, 6, 0, 0, 1, 2 };
    int nums1Size = sizeof nums1 / sizeof (int);
    int target = 0;
    bool status = search2(nums1, nums1Size, target);
    printf("target = %d, status = %d\n", target, status);
    assert(status == true);

    int nums2[] = { 2, 5, 6, 0, 0, 1, 2 };
    int nums2Size = sizeof nums2 / sizeof (int);
    target = 3;
    status = search2(nums2, nums2Size, target);
    printf("target = %d, status = %d\n", target, status);
    assert(status == false);
}

/*
 https://leetcode-cn.com/problems/find-minimum-in-rotated-sorted-array/
 153. 寻找旋转排序数组中最小值

 已知一个长度为 n 的数组, 预先按照升序排列, 经有 1 到 n 次旋转后, 得到输入数组.
 例如, 原数组 nums = { 0, 1, 2, 4, 5, 6, 7 } 在变化后可能得到:
    * 若旋转 4 次, 则可以得到 [ 4, 5, 6, 7, 0, 1, 2 ]
    * 若旋转 7 次, 则可以得到 [ 0, 1, 2, 4, 5, 6, 7 ]
 
 注意, 数组 [ a[0], a[1], a[2], ..., a[n-1] ] 旋转一次结果为数组
 [ a[n-1], a[0], a[1], a[2], ... , a[n-2] ].

 给你一个元素值互不相同的数组 nums, 他原来是一个升序排列的数组, 
 并按上述情形进行了多次旋转. 请你找出返回数组中最小元素.

 你必须设计一个时间复杂度为 O(log n) 的算法解决此问题.

 示例1:
 输入: nums=[3, 4, 5, ,1, 2]
 输出: 1
 解释: 原数组为 [1, 2, 3, 4, 5], 旋转 3 次得到输入数组.

 示例2:
 输入: nums=[4, 5, 6, 7, 0, 1, 2]
 输出: 0
 解释: 原数组为 [0, 1, 2, 4, 5, 6, 7], 旋转 4 次得到输入数组.

 示例3:
 输入: nums=[11, 13, 15, 17]
 输出: 11
 解释: 原数组为 [11, 13, 15, 17], 旋转 4 次得到输入数组
 */

int search3(int * nums, int numsSize) {
    int left = 0;
    int right = numsSize - 1;

    while (left < right) {
        if (nums[left] < nums[right]) {
            break;
        }

        int middlen = left + (right - left)/2;
        if (nums[left] <= nums[middlen]) {
            left = middlen+1;
        } else {
            // middlen 有可能就为最小值
            right = middlen;
        }
    }

    return nums[left];
}

// https://leetcode-cn.com/problems/find-minimum-in-rotated-sorted-array-ii/submissions/
// 154. 寻找旋转排序数组中的最小值 II
int search4(int * nums, int numsSize) {
    int left = 0;
    int right = numsSize - 1;

    while (left < right) {
        int middlen = left + (right - left)/2;

        if (nums[middlen] == nums[right]) {
            right--;
            continue;
        }
        if (nums[middlen] < nums[right]) {
            right = middlen;
        } if (nums[middlen] > nums[right]) {
            left = middlen+1;
        } 
    }

    return nums[left];
}

/*
 nums[left] == nums[middlen] : 可以 left++
 nums[left] > nums[middlen] : 意味着 最小值在 [left, middlen] 中
 nums[left] < nums[middlen] : 最小值可能是 nums[left]也可能是 在 [middle+1, right] 中, 需要额外加步骤 
 上面是 left 和 middlen 判断逻辑. 

 但 middlen 和 right 判断时候:
 nums[middlen] == nums[right] : 可以 right--
 nums[middlen] < nums[right] : 意味着 [left, middlen] 范围内有最小值
 nums[middlen] > nums[right] : 意味着 [middlen+1, right] 范围内有最小值

 选取 left 和 right 不同, 居然不对称. 这是很少遇到的.
 有点意思. 
 */
int search5(int * nums, int numsSize) {
    int left = 0;
    int right = numsSize - 1;

    while (left < right) {
        int middlen = left + (right - left) / 2;
        if (nums[right] == nums[middlen]) {
            right--;
            continue;
        }

        if (nums[right] > nums[middlen]) {
            right = middlen;
        } else {
            left = middlen+1;
        }
    }

    return nums[left];
}