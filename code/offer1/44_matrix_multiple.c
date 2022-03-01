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

   数组中出现次数超过一半的数字
 */

void matrix_printf(int matrix[], int len) {
    assert(matrix != NULL && len > 0);
    printf("matrix[%d]: \n", len);
    for (int i = 0; i < len; i++) {
        printf("%4d", matrix[i]);
    }
    printf("\n");
}

static int matrix_find_half_partial(int matrix[], int left, int right, int k) {
    int low = left, high = right;
    int anchor = matrix[low];
    while (low < high) {
        while (low < high && anchor <= matrix[high])
            high--;
        if (low >= high)
            break;
        matrix[low++] = matrix[high];

        while (low < high && anchor >= matrix[low])
            low++;
        if (low >= high)
            break;
        matrix[high--] = matrix[low];
    }

    // left, low-1, low, low+1, right

    if (k == low) {
        return low;
    }

    if (k > low) {
        return matrix_find_half_partial(matrix, low+1, right, k);
    } else {
        return matrix_find_half_partial(matrix, left, low-1, k);
    }
}

int matrix_find_half(int matrix[], int len) {
    if (matrix == NULL || len <= 0) {
        return -1;
    }

    // 一个数字超过了一半
    if (len <= 1) {
        return 0;
    }

    int mid = len/2;
    return matrix_find_half_partial(matrix, 0, len-1, mid);
}

int matrix_half(int matrix[], int len) {
    if (matrix == NULL || len <= 0) {
        return -1;
    }

    // 一个数字超过了一半
    if (len <= 1) {
        return 0;
    }

    int i;
    int num = 1;
    int index = 0;
    int anchor = matrix[0];
    for (i = 1; i < len; i++) {
        if (num == 0) {
            num = 1;
            anchor = matrix[index = i];
        } else if (matrix[i] == anchor) {
            num++;
        } else {
            num--;
        }
    }

    return index;
}

// build:
// gcc -g -O3 -Wall -Wextra -Werror -o 44_matrix_multiple 44_matrix_multiple.c
//
int main(void) {
    int matrix[] = { 1, 2, 3, 2, 2, 2, 5, 4, 2 };
    int len = sizeof matrix / sizeof *matrix;

    matrix_printf(matrix, len);

    int index = matrix_find_half(matrix, len);
    printf("index = %d, matrix[index] = %d\n", index, matrix[index]);

    index = matrix_half(matrix, len);
    printf("index = %d, matrix[index] = %d\n", index, matrix[index]);

    // 还得校验, 是否大于半数
    int anchor = matrix[index];
    int num = 0;
    for (int i = 0; i < len; i++) {
        if (matrix[i] == anchor) {
            num++;
        }
    }

    printf("num = %d, len = %d, num * 2 > len : %d\n", num, len, 2*num > len);

    exit(EXIT_SUCCESS);
}
