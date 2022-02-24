#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define AGE_THRESHOLD 257

void sort_age(int ages[], int len) {
    if (ages == NULL || len <= 1) {
        return;
    }

    int i;
    int slot[AGE_THRESHOLD] = { 0 };
    for (i = 0; i < len; i++) {
        int age = ages[i];
        assert(age < AGE_THRESHOLD && age >= 0);
        slot[age]++;
    }

    int count = 0;
    for (i = 0; i < AGE_THRESHOLD && count < len; i++) {
        int icount = slot[i];
        for (int j = 0; j < icount; j++) {
            ages[count++] = i;
        }
    }
}

// build:
// gcc -g -O3 -Wall -Wextra -Werror -o 27_sort_age 27_sort_age.c
//
int main(void) {
    int ages[] = { 0, 1, 2, 3, 0, 3, 45, 66, 3, 56, 89, 90, 100, 3, 7 };
    int len = sizeof(ages) / sizeof(int);

    int i;

    printf("ages : ");
    for (i = 0; i < len; i++) {
        printf(" %d", ages[i]);
    }
    printf("\n");

    sort_age(ages, len);

    printf("sort : ");
    for (i = 0; i < len; i++) {
        printf(" %d", ages[i]);
    }
    printf("\n");

    exit(EXIT_SUCCESS);
}