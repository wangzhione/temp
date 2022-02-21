#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

/*
 题目: 
 请实现一个函数, 把字符串中每个空格替换成 "%20". 例如输入 "We are happy.",
 则输出 "We%20are%20happy.".
 */

static int space_replace_n(const char * tar, int * plen) {
    int n = 0;
    int len = 0;
    do {
        int c = *tar++;
        if (c == ' ')
            ++n;
        ++len;
    } while (*tar);
    *plen = len;
    return n;
}

char * space_replace(const char * tar) {
    if (tar == NULL || *tar == 0) {
        return NULL;
    }

    // 确定几个 ' '
    // <对于特殊双字节字符我们先不考虑>
    int len;
    int n = space_replace_n(tar, &len);

    char * source = malloc(len + n * 2 + 1);
    assert(source != NULL);
    if (n == 0) {
        return strcpy(source, tar);
    }

    // 特殊情况, 挨个替换
    int c;
    char * ptr = source;
    do {
        if ((c = *tar++) == ' ') {
            *ptr++ = '%';
            *ptr++ = '2';
            *ptr++ = '0';
        } else {
            *ptr++ = c;
        }
    } while (c != 0);

    return source;
}

// build:
// gcc -g -O3 -Wall -Wextra -Werror -o 23_replace 23_replace.c
//
int main(void) {
    char * source;
    const char * target ;

    target = "We are happy.";
    source = space_replace(target);
    printf("target = %s, source = %s\n", target, source);
    assert(source != NULL && strcmp(source, "We%20are%20happy.") == 0);
    free(source);

    target = NULL;
    source = space_replace(target);
    printf("target = %s, source = %s\n", target, source);
    assert(source == NULL);
    free(source);

    target = "Wearehappy.";
    source = space_replace(target);
    printf("target = %s, source = %s\n", target, source);
    assert(source != NULL && strcmp(source, "Wearehappy.") == 0);
    free(source);

    exit(EXIT_SUCCESS);
}
