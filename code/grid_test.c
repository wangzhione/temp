#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

int grid_find_priatial(int index, int grid[], int row, int column, int temp[]) {
    // 已经标记过直接返回
    if (temp[index] == 1) {
        return 0;
    }

    // 标记已经访问过了
    temp[index] = 1;

    int value = grid[index];
    if (value == 0) {
        // 如果这里是水, 我们不用遍历停止
        return 0;
    }

    // 如果这是陆地, 尝试其它四个方向试试
    int row_index = index / column;
    int column_index = index - row_index * column;
    printf("row_index = %d, column_index = %d\n", row_index, column_index);

    // 1. 上
    if (row_index-1 >= 0 && temp[(row_index-1)*column+column_index] == 0) {
        grid_find_priatial((row_index-1)*column+column_index, grid, row, column, temp);
    }

    // 2. 下
    if (row_index+1 < row && temp[(row_index+1)*column+column_index] == 0) {
        grid_find_priatial((row_index+1)*column+column_index, grid, row, column, temp);
    }

    // 3. 左
    if (column_index-1>=0 && temp[row_index*column+column_index-1] == 0) {
        grid_find_priatial(row_index*column+column_index-1, grid, row, column, temp);
    }

    // 4. 右
    if (column_index+1<column && temp[row_index*column+column_index+1] == 0) {
        grid_find_priatial(row_index*column+column_index+1, grid, row, column, temp);
    }

    return 1;
}

int grid_find(int grid[], int row, int column) {
    if (grid == NULL || row <= 0 || column <= 0) {
        return 0;
    }

    int count = 0;
    int * temp = calloc(row * column, sizeof(int));
    printf("row = %d, column = %d\n", row, column);
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < column; j++) {
            int index = i * column + j;
            // 已经搜索过就跳过
            if (temp[index] == 1) {
                continue;
            }
            count += grid_find_priatial(index, grid, row, column, temp);
        }
    }

    free(temp);
    return count;
}

// build : gcc -g -O2 -Wall -o grid_test grid_test.c 
//
int main() {
    // char str[3];
    // scanf("%s", str);
    int grid[] = { 
        1 , 1 , 0 , 0 , 0,
        1 , 1 , 0 , 0 , 0,
        0 , 0 , 1 , 0 , 0,
        0 , 0 , 0 , 1 , 1,
    };

    int row = 4;
    int column = sizeof(grid)/sizeof(int)/row;
    int count = grid_find(grid, row, column);
    
    printf("count = %d\n", count);

    assert(count == 3);

    return 0;
}