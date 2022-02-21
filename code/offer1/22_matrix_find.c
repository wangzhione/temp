#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

bool matrix_find(int matrix[], int rows, int columns, int value) {
    if (matrix == NULL || rows <= 0 || columns <= 0) {
        // 没有数据, 数据异常, 默认返回找不到
        return false;
    }

    /*
     find 7

     1. 
        1  2  8  9 
        2  4  9 12 
        4  7 10 13 
        6  8 11 15 
     
     2.
      (0, 3) -> 9 > 7
        1  2  8 
        2  4  9 
        4  7 10 
        6  8 11

     3. 
       (0, 2) -> 8 > 7
        1  2 
        2  4 
        4  7 
        6  8 

    4.
       (0, 1) -> 2 < 7
        2  4 
        4  7 
        6  8 

    5.
       (1, 1) -> 4 < 7
        4  7 
        6  8 

    6.
       (2, 1) -> 7 = 7
       OK
     */

    int row = 0, column = columns - 1;
    int step = (rows - 1) * columns;
    do {
        // [左上角最小值, 右下角最大值]
        if (value < matrix[row * columns] || value > matrix[step + column]) {
            return false;
        }

        // 和右上角比对
        int anchor = matrix[row * columns + column];
        if (anchor == value) {
            // 找到了
            return true;
        } 
        if (anchor > value) {
            // 在左边
            column--;
        } else {
            // 在下面
            row++;
        }
    } while (row < rows && column >= 0);

    return false;
}

// build:
// gcc -g -O2 -Wall -Wextra -Werror -o 22_matrix_find 22_matrix_find.c
//
int main(void) {
    int rows = 4;
    int columns = 4;
    // rows * columns
    int matrix[] = {  1, 2, 8, 9 , 2, 4, 9, 12 , 4, 7, 10, 13 ,  6, 8, 11, 15 };

    int i, j;

    for (i = 0; i < rows; i++) {
        for (j = 0; j < columns; j++) {
            printf("%2d ", matrix[i * rows + j]);
        }
        printf("\n");
    }

    bool status;
    int value;

    value = -1;
    status = matrix_find(matrix, rows, columns, value);
    assert(status == false);

    value = 4;
    status = matrix_find(matrix, rows, columns, value);
    assert(status == true);

    value = 7;
    status = matrix_find(matrix, rows, columns, value);
    assert(status == true);

    value = 100;
    status = matrix_find(matrix, rows, columns, value);
    assert(status == false);

    value = 2;
    status = matrix_find(matrix, rows, columns, value);
    assert(status == true);

    exit(EXIT_SUCCESS);
}

/*
 这个题目更优解, 利用工程算法组合.

 例如 4 个顶点 + 中间点 都需要考虑. 来筛选. 

 然后考虑 退化成二分查找 
 */
