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

/* describe : 31. 下一个排列

   整数数组的一个排列就是将其所有成员以序列或线性顺序排列.

   - 例如 arr = { 1, 2, 3 }, 以下这些都可以视作 arr 的排列:
     { 1, 2, 3 }, { 1, 3, 2 }, { 3, 1, 2 }, { 2, 3, 1 }

   整数数组的下一个排列是指整数的下一个字典序更大的排列. 
   更正式地, 如果数组的所有排列根据其字典顺序从小到大排列在一个容器中,
   那么数组的下一个排列就是在这个有序容器中排在他后面的那个排列. 如果不存在
   下一个更大的排列, 那么这个数必须重排为字典序最小的排列(即, 其元素按升序排列).

   - 例如 arr = { 1, 2, 3 } 的下一个排列是 { 1, 3, 2 }
   - 类似地 arr = { 2, 3, 1 } 的下一个排列是 { 3, 1, 2 }
   - 而 arr = { 3, 2, 1 } 的下一个排列是 { 1, 2, 3 }, 因为 { 3, 2, 1 } 不存在一个字典序更大的排列.

   给你一个整数数组 nums, 找出 nums 的下一个排列.

   必须原地修改, 只允许使用额外常熟空间.

   示例1:
   输入: nums = { 1, 2, 3 }
   输出: { 1, 3, 2 }

   示例2:
   输入: nums = { 3, 2, 1 }
   输出: { 1, 2, 3 }

   示例3:
   输入: nums = { 1, 1, 4 }
   输出: { 1, 5, 1 }
 */

int nums_compare(const void * left, const void * right) {
    return *(const int *)left - *(const int *)right;
}

void nextPermutation2(int * nums, int numsSize) {
    if (nums == NULL || numsSize <= 1) {
        return;
    }

    int temp;
    int i = numsSize-2;
    while (i >= 0) {
        if (nums[i] < nums[i+1]) {
            int second = i+1;

            int j = second;
            while (++j < numsSize) {
                if (nums[j] > nums[i] && nums[j] < nums[second]) {
                    second = j;
                }
            }

            temp = nums[i];
            nums[i] = nums[second];
            nums[second] = temp;
            
            // 开始排序 nums+i+1, numsSize-i-1
            if (numsSize > i+2) {
                qsort(nums+i+1, numsSize-i-1, sizeof(int), nums_compare);
            }
            return;
        }
        i--;
    }

    // 没有找到, 开始找最小排列, revert
    for (i = 0; i < numsSize/2; i++) {
        temp = nums[i];
        nums[i] = nums[numsSize-1-i];
        nums[numsSize-1-i] = temp;
    }
}

void nextPermutation(int * nums, int numsSize) { 
    if (numsSize <= 1 || nums == NULL) {
        return;
    }

    int temp;
    int i = numsSize - 2;
    while (i >= 0 && nums[i] >= nums[i+1]) {
        i--;
    }

    if (i >= 0) {
        int j = numsSize - 1;
        while (j >= 0 && nums[i] >= nums[j]) {
            j--;
        }
        temp = nums[i];
        nums[i] = nums[j];
        nums[j] = temp;
    }

    // 漂亮
    int left = i+1;
    int right = numsSize - 1;
    while (left < right) {
        temp = nums[left];
        nums[left++] = nums[right];
        nums[right--] = temp;
    }
}

void nextPermutation2(int * nums, int numsSize) { 
    if (numsSize <= 1 || nums == NULL) {
        return;
    }

    int temp;
    int i = numsSize - 2;
    while (i >= 0 && nums[i] >= nums[i+1]) {
        i--;
    }

    if (i >= 0) {
        int j = numsSize - 1;
        while (j >= 0 && nums[i] >= nums[j]) {
            j--;
        }
        temp = nums[i];
        nums[i] = nums[j];
        nums[j] = temp;
    }

    // 漂亮
    while (++i < --numsSize) {
        temp = nums[i];
        nums[i] = nums[numsSize];
        nums[numsSize] = temp;
    }
}

static void test_nextPermutation(int * nums, int numsSize) {
    int i = 0;

    printf("{ %d", nums[0]);
    for (i = 1; i < numsSize; i++) {
        printf(", %d", nums[i]);
    }

    nextPermutation(nums, numsSize);

    printf(" } -> { %d", nums[0]);
    for (i = 1; i < numsSize; i++) {
        printf(", %d", nums[i]);
    }
    printf(" }\n");
}

// build:
// gcc -g -O3 -Wall -Wextra -Werror -o 012_31_array_next_permutation 012_31_array_next_permutation.c
//
int main(void) {
    int nums1[] = { 1, 2, 3 };
    test_nextPermutation(nums1, sizeof nums1/ sizeof(int));

    int nums2[] = { 3, 2, 1 };
    test_nextPermutation(nums2, sizeof nums2/ sizeof(int));

    int nums3[] = { 1, 1, 5 };
    test_nextPermutation(nums3, sizeof nums3/ sizeof(int));

    int nums4[] = { 1, 3, 2 };
    test_nextPermutation(nums4, sizeof nums4/ sizeof(int));

    int nums5[] = { 2, 3, 1 };
    test_nextPermutation(nums5, sizeof nums5/ sizeof(int));

    exit(EXIT_SUCCESS);
}

/**
 * base -- 指向要排序的数组的第一个元素的指针。
 * nitems -- 由 base 指向的数组中元素的个数。
 * size -- 数组中每个元素的大小，以字节为单位。
 * compar -- 用来比较两个元素的函数。
 * 
 * void qsort(void *base, size_t nitems, size_t size, int (*compar)(const void *, const void*))
 * /