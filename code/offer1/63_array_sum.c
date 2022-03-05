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

   属于一个递增的数组和一个数字 s, 在数组中查找两个数, 使得他们的和正好是 s.
   如果有多对数字的和等于 s, 输出任意一对即可.
 */

void array_printf(int a[], int len) {
    printf("array[%d]: ", len);
    for (int i = 0; i < len; i++) {
        printf("%4d", a[i]);
    }
    printf("\n");
}

struct return_value {
    bool exist; // false 不存在
    int left;   // 左边索引
    int right;  // 右边索引
};

struct return_value array_sum_find(int a[], int len, int v) {
    if (a == NULL || len < 2) {
        return (struct return_value){ .exist = false };
    }

    int sum = a[0] + a[1];
    if (sum == v) {
        return (struct return_value){ 
            .exist = true,
            .left = 0,
            .right = 1,
        };
    } 
    if (v < sum) {
        printf("sum = %d, %d + %d\n", sum, a[0], a[1]);
        return (struct return_value){ .exist = false };
    }

    sum = a[len-2] + a[len-1];
    if (sum == v) {
        return (struct return_value){ 
            .exist = true,
            .left = len-2,
            .right = len-1,
        };
    }
    if (v > sum) {
        return (struct return_value){ .exist = false };
    }

    // 开始两边夹逼查找
    int left = 0, right = len-1;
    do {
        sum = a[left] + a[right];
        printf("sum = %d, %d + %d\n", sum, a[left], a[right]);
        if (v == sum) {
            return (struct return_value){ 
                .exist = true,
                .left = left,
                .right = right,
            };
        } 
        if (v > sum) {
            left++;
        } else {
            right--;
        }
    } while (left < right);

    return (struct return_value){ .exist = false };
}

/*
 题目二: 输入一个正数 s, 打印出所有的和为 s 的连续正数序列(至少含有两个数).
 例如输入 15, 由于 1+2+3+4+5=4+5+6=7+8=15, 所以结果打印出 3 个连续序列 1~5, 4~6 和 7~8
 */

int find_sequence(int num) {
    int count = 0;

    if (num <= 2) {
        return count;
    }

    int left = 1;
    int right = left+1;

    int sum = left+right;

    // 结束点
    int end = (1+num)/2;
    // left < right && right <= end -> right <= end
    // 为什么可以精简掉 left < right 呢.
    // 1' 在 sum [left, right] < num 时候, 此刻 right -> right+1, left < right 仍然成立
    // 2' 在 sum [left, right] = num 时候, 此刻分为两种情况讨论
    //    2.1' left < right - 1 时候, left -> left+1, 此刻仍然有 left < right
    //    2.2' left = right - 1 时候, 此刻 left -> left + 1 进入 sum < num 分支, 
    //         因为 end = (1+num)/2 可以证明 此刻 right = end 随后结束
    // 3' 在 sum [left, right] > num 时候, left -> left+1, 
    //    3.1' left < right - 1 时候, 同 2.1'
    //    3.2' left = right - 1 时候, 此刻 left -> left+1, 由于 end 存在, 所以下一次 sum > num 结束
    while (right <= end) {
        printf("left = %d, right = %d, sum = %d\n", left, right, sum);
        if (sum < num) {
            right++;
            sum += right;
        } else {
            if (sum == num) {
                printf("num %3d : [%d, %d]\n", ++count, left, right);
            }

            sum -= left;
            left++;
        }
    }

    return count;
}

// build:
// gcc -g -O3 -Wall -Wextra -Werror -o 63_array_sum 63_array_sum.c
//
int main(void) {
    int a[] = { 1, 2, 4, 7, 11, 15 };
    int alen = sizeof a / sizeof *a;

    array_printf(a, alen);

    int sum = 15;
    struct return_value res = array_sum_find(a, alen, sum);

    if (res.exist) {
        printf("a[%d] + a[%d] = %d + %d = %d\n", res.left, res.right, a[res.left], a[res.right], sum);
    } else {
        printf("sum = %d not found\n", sum);
    }

    int num = 15;
    int count = find_sequence(num);
    printf("num = %d, count = %d\n", num, count);

    num = 16;
    count = find_sequence(num);
    printf("num = %d, count = %d\n", num, count);

    num = 4;
    count = find_sequence(num);
    printf("num = %d, count = %d\n", num, count);

    num = 21;
    count = find_sequence(num);
    printf("num = %d, count = %d\n", num, count);

    num = 20;
    count = find_sequence(num);
    printf("num = %d, count = %d\n", num, count);

    exit(EXIT_SUCCESS);
}
