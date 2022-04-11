#include <stdio.h>
#include <stdlib.h>

#ifndef UNUSED
#define UNUSED(parameter) if (parameter) {}
#endif//UNUSED

// build:
// gcc -O1 -g -Wall -Wextra -Werror -o define define.c
int main(void) {
    for (int i = 0; i < 10; i++) {
        char echo[7];
        sprintf(echo, "echo %d", i);
        UNUSED(system(echo));
    }
    return 0;
}

// int main(void) {
//     for(int i = 0; i < 10; i++) {
//         char * echo = (char *)malloc(6 * sizeof(char));
//         sprintf(echo, "echo %d", i);
//         system(echo);
//     }
//     return 0;
// }