#include <stdio.h>
#include <stdlib.h>

#define EA 1
#define EW 2

#if EA != EW

#define EC EA

#define EA EC : case EW

#endif

int main(void) {
    int a = 2;

    switch (a) {
    case EA:
        printf("EA a = %d\n", a);
        break;
    
    default:
        printf("default a = %d\n", a);
        break;
    }

    exit(EXIT_SUCCESS);
}