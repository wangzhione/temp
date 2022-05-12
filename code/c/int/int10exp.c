#include <stdio.h>
#include <stdlib.h>

// gcc -g -O3 -Wall -Wextra -Werror -o int10exp int10exp.c
// gcc -g -O3 -Wall -Wextra -o int10exp int10exp.c
// gcc -g -Wall -Wextra -o int10exp int10exp.c
int main(void) {
    int expmax = 1000000000;

    printf("expmax * 10 / 10 = %d\n", (expmax * 10) / 10);

    /*
     gcc : expmax * 10 / 10 = 1000000000
     lc  : expmax * 10 / 10 = 141006540
     go  : expmax * 10 / 10 = 141006540
     */

    exit(EXIT_SUCCESS);
}
