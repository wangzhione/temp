#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

struct T1 {
    int8_t a;
    int64_t b;
    int16_t c;
};

// gcc -g -O3 -Wall -Wextra -o sizeof sizeof.c
int main(void) {
    printf("sizeof(T1) = %zu\n", sizeof(struct T1));

    exit(EXIT_SUCCESS);
}
