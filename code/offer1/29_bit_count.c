#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int bit_number(int num) {
    int bitn = 0;
    while (num) {
        num &= num - 1;
        bitn++;
    }
    return bitn;
}

// build: 
// gcc -g -O3 -Wall -Werror -Wextra -o 29_bit_count 29_bit_count.c
//
int main(void) {
    int bitn;
    int num;

    num = 0x1;
    bitn = bit_number(num);
    assert(bitn == 1);

    // 0x89 = 1000 1001
    num = 0x89;
    bitn = bit_number(num);
    assert(bitn == 3);

    num = -0x89; 
    bitn = bit_number(num);
    printf("num = %d, bitn = %d\n", num, bitn);
    assert(bitn == 30);

    num = -1;
    bitn = bit_number(num);
    printf("num = %d, bitn = %d\n", num, bitn);
    assert(bitn == 32);

    exit(EXIT_SUCCESS);
}
