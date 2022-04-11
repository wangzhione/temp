#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

// build:
// gcc -O2 -g -Wall -Wextra -Werror -o pthread_stack pthread_stack.c -lpthread
int main(void) {
    size_t stacksize;
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    if (pthread_attr_getstacksize(&attr, &stacksize)) {
        fprintf(stderr, "pthread_attr_getstacksize error = %d, %s", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    printf("pthread_attr_getstacksize = %zu\n", stacksize);

    pthread_attr_destroy(&attr);

    exit(EXIT_SUCCESS);
}
