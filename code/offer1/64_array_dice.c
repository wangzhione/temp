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

   把 n 个骰子扔在地上, 所有骰子朝上一面徳点数之和为 s. 输入 n, 
   打印出 所有 s 徳所有可能的值出现徳概率.
 */

#define DICE_INT (6)

static void weight_print(int * a, int left, int right) {
    int i;
    int sum = 0;
    for (i = left; i <= right; i++) {
        sum += a[i];
    }

    printf("dice probability sum weight = %d\n", sum);
    for (i = left; i <= right; i++) {
        printf("%4d : %d/%d\n", i, a[i], sum);
    }
    printf("\n");
}

void dice_print(int n) {
    if (n <= 0) {
        printf("param unknown error n = %d\n", n);
        return;
    }
    
    // a[0] 和 a[1] 会交替迭代
    int a[2][DICE_INT * n + 1];
    memset(a, 0, sizeof a);

    int index = 0;

    // 第一次循环
    int i;
    for (i = 1; i <= DICE_INT; i++) {
        a[index][i] = 1;
    }
    if (n <= 1) {
        return weight_print(a[index], 1, DICE_INT);
    }

    int j = 1;
    while (++j <= n) {
        int now = 1 - index;

        int left = j - 1;
        int right = (j - 1) * DICE_INT;

        for (i = left; i <= right; i++) {
            for (int k = 1; k <= DICE_INT; k++) {
                a[now][i+k] += a[index][i];
            }
        }

        index = now;
    }

    return weight_print(a[index], n, n*DICE_INT);
}

// build:
// gcc -g -O3 -Wall -Wextra -Werror -o 64_array_dice 64_array_dice.c
//
int main(void) {
    dice_print(-1);
    dice_print(0);
    dice_print(1);
    dice_print(2);
    exit(EXIT_SUCCESS);
}
