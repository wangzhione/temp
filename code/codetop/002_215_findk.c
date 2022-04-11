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

/* describe : 215. 数组中的第K个最大元素

   
 */

int findKthLargest(int * nums, int numsSize, int k) {
    assert(nums != NULL && numsSize >= 1);

    if (numsSize == 1) {
        return nums[0];
    }

    if (k <= 1) {
        // 默认找最大值
        int max = nums[0];
        for (int i = 1; i < numsSize; i++) {
            if (max < nums[i]) {
                max = nums[i];
            }
        }
        return max;
    }

    if (k >= numsSize) {
        // 默认找最小值
        int min = nums[0];
        for (int i = 1; i < numsSize; i++) {
            if (min > nums[i]) {
                min = nums[i];
            }
        }
        return min;
    }

    // 开始查找了. 首先切分
    int * left = nums;
    int * right = nums + numsSize - 1;
    int anchor = *left;
    do {
        while (left < right && anchor >= *right)
            right--;
        if (left >= right)
            break;
        *left++ = *right;

        while (left < right && anchor <= *left)
            left++;
        if (left >= right)
            break;
        *right-- = *left;
    } while (left < right);
    *left = anchor;

    // 已经找到了
    int kth = (int)(left - nums) + 1;
    if (k == kth) {
        return anchor;
    }

    if (kth > k) {
        return findKthLargest(nums, kth-1, k);
    }
    // kth < k
    return findKthLargest(nums+kth, numsSize-kth, k-kth);
}

// build:
// gcc -g -O3 -Wall -Wextra -Werror -o 002_215_findk 002_215_findk.c
//
int main(void) {
    int nums1[] = { 3, 2, 1, 5, 6, 4 };
    int k1 = 2;
    int kth1 = findKthLargest(nums1, sizeof(nums1)/sizeof(int), k1);
    printf("k = %d, kth = %d\n", k1, kth1);


    int nums2[] = { 3, 2, 3, 1, 2, 4, 5, 5, 6 };
    int k2 = 4;
    int kth2 = findKthLargest(nums2, sizeof(nums2)/sizeof(int), k2);
    printf("k = %d, kth = %d\n", k2, kth2);

    exit(EXIT_SUCCESS);
}
