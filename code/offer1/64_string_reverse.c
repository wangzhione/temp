#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* describe : 

   输入一个英文句子, 翻转句子中单词的顺序, 使单词内字符的顺序不变. 为简单起见,
   标点符号和普通字母一样处理. 例如输入字符串 "I am a student.", 则输出 "student. a am I"
 */

static void string_reverse_partial(char * start, char * end) {
    while (start < end) {
        char c = *start;
        *start++ = *end;
        *end-- = c;
    }
}

void string_reverse(char * str) {
    if (str == NULL) {
        return;
    }

    size_t len = strlen(str);
    string_reverse_partial(str, str+len-1);
}

void word_reverse(char * str) {
    if (str == NULL) {
        return;
    }

    size_t len = strlen(str);
    if (len <= 1) {
        return;
    }

    // ' ' 字符分割
    char * start;
    char * end;
    char * s = str;

    do {
        // 确定 start
        while (*s != 0 && *s == ' ')
            s++;
        if (*s == 0)
            break;
        start = s;

        // 确定 end
        while (*s != 0 && *s != ' ')
            s++;
        end = s-1;

        string_reverse_partial(start, end);
    } while (*s != 0);
    
    printf("str = [%s]\n", str);

    string_reverse_partial(str, str+len-1);
}

void string_left_rotate(char * str, int n) {
    if (str == NULL || *str == 0 || n <= 0) {
        return;
    }

    int len = (int)strlen(str);
    if (len <= 1) {
        return;
    }

    n %= len;
    if (n == 0) {
        return;
    }

    // 开始翻转
    char * nstart = str;
    char * nend = str+n-1;
    char * start = str+n;
    char * end = str+len-1;

    string_reverse_partial(nstart, nend);
    string_reverse_partial(start, end);
    string_reverse_partial(nstart, end);
}

void string_left_rotate_optimize(char * str, int n) {
    if (str == NULL || *str == 0 || n <= 0) {
        return;
    }

    int len = (int)strlen(str);
    if (len <= 1) {
        return;
    }

    n %= len;
    if (n == 0) {
        return;
    }

    // [0, n-1] 区间翻转
    string_reverse_partial(str, str+n-1);

    for (int i = 0; i < n; i++) {
        char anchor = str[i];
        int j = i + n;
        while (j < len) {
            str[j-n] = str[j];
            j += n;
        }
        str[j-n] = anchor;
    }
}

// build:
// gcc -g -O3 -Wall -Wextra -Werror -o 64_string_reverse 64_string_reverse.c
//
int main(void) {
    char str[] = "I am a student.";

    printf("str = [%s]\n", str);
    word_reverse(str);
    printf("str = [%s]\n", str);

    char str2[] = "abcdefg";
    printf("str2 = [%s]\n", str2);
    string_left_rotate(str2, 2);
    printf("str2 = [%s]\n", str2);

    char str3[] = "abcdefg";
    printf("str3 = [%s]\n", str3);
    string_left_rotate_optimize(str3, 2);
    printf("str3 = [%s]\n", str3);

    exit(EXIT_SUCCESS);
}
