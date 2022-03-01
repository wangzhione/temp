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

   1   2   3   4
   5   6   7   8
   9   10  11  12
   13  14  15  16

   -> 1 2 3 4 8 12 16 15 14 13 9 5 6 7 11 10
*/

void matrix_printf(int * matrix, int rows, int columns) {
    if (matrix == NULL || rows <= 0 || columns <= 0) {
        return;
    }

    printf("matrix(%p)[%d][%d]:\n", matrix, rows, columns);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            printf("%-4d", matrix[i * columns + j]);
        }
        printf("\n");
    }
}

void matrix_circle(int * matrix, int rows, int columns) {
    if (matrix == NULL || rows <= 0 || columns <= 0) {
        return;
    }

    printf("circle(%p)[%d][%d]:\n", matrix, rows, columns);

    bool loop;
    int step = 0;
    do {
        loop = false;

        /*
        i → j
        ↑    ↓
        w ← k
        */
        int i = step * columns + step;
        int j = step * columns + (columns - 1 - step);
        int k = (rows - 1 - step) * columns + (columns - 1 - step);
        int w = (rows - 1 - step) * columns + step;

        // i → j
        if (i <= j) {
            loop = true;
            for (int x = i; x <= j; x++) {
                printf("%d ", matrix[x]);
            }
        }
        // i
        // ↓
        // k
        if (j < k) {
            loop = true;
            for (int x = step + 1; x <= rows - 1 - step; x++) {
                printf("%d ", matrix[x * columns + (columns - 1 - step)]);
            }
        }
        // w ← k
        if (k > w) {
            loop = true;
            for (int x = k - 1; x >= w; x--) {
                printf("%d ", matrix[x]);
            }
        }
        // i
        // ↑
        // w
        if (i < w) {
            loop = true;
            for (int x = rows - 1 - step - 1; x >= step + 1; x--) {
                printf("%d ", matrix[x * columns + step]);
            }
        }

        step++;
    } while (loop);

    printf("\n");
}

// build:
// gcc -g -O3 -Wall -Wextra -Werror -o 41_matrix 41_matrix.c
//
int main(void) {
    int matrix[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    int rows = 4;
    int columns = 4;

    matrix_printf(matrix, rows, columns);

    matrix_circle(matrix, rows, columns);

    exit(EXIT_SUCCESS);
}

/*
matrix(0x7ffda8f71490)[4][4]:
1   2   3   4   
5   6   7   8   
9   10  11  12  
13  14  15  16  
circle(0x7ffda8f71490)[4][4]:
1 2 3 4 8 12 16 15 14 13 9 5 6 7 11 10
 */