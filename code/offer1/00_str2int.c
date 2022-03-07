#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <limits.h>
#include <stdlib.h>
#include <stdint.h>

/*
  把一个字符串转换成整数
 */

#define NUMBER_SUCCESS  0    // 转换成功
#define NUMBER_INVALID -2    // 非法字符
#define NUMBER_BORDER  -3    // 数值越界

int str2int(const char * str, int * res) {
    if (str == NULL || *str == 0) {
        if (res) *res = 0;
        return NUMBER_SUCCESS;
    }

    long long num = 0;
    int minus = 0; // 1 标识负数
    if (*str == '+' || *str == '-') {
        minus = *str++ == '-' ? 1 : 0;
    }

    char c;
    while ((c = *str++) != 0) {
        if (c >= '0' && c <= '9') {
            // 合理字符
            num = 10 * num + c - '0';
        } else {
            return NUMBER_INVALID;
        }
    }
    num = minus ? -num : num;

    int value = (int)num;
    if (value != num) {
        return NUMBER_BORDER;
    }

    if (res) *res = value;
    return NUMBER_SUCCESS;
}

//
// build : gcc -g -O3 -Wall -Wextra -Werror -o 00_str2int 00_str2int.c
//
int main(void) {
    int code;
    int num;
    const char * str;

    num = 0;
    str = NULL;
    code = str2int(str, &num);
    printf("code = %d, str = %s, num = %d\n", code, str, num);
    assert(code == NUMBER_SUCCESS && num == 0);

    num = 0;
    str = "-1";
    code = str2int(str, &num);
    printf("code = %d, str = %s, num = %d\n", code, str, num);
    assert(code == NUMBER_SUCCESS && num == -1);

    num = 0;
    str = "1234A";
    code = str2int(str, &num);
    printf("code = %d, str = %s, num = %d\n", code, str, num);
    assert(code == NUMBER_INVALID);

    num = 0;
    str = "2147483647";
    code = str2int(str, &num);
    printf("code = %d, str = %s, num = %d\n", code, str, num);
    assert(code == NUMBER_SUCCESS);

    num = 0;
    str = "-2147483648";
    code = str2int(str, &num);
    printf("code = %d, str = %s, num = %d\n", code, str, num);
    assert(code == NUMBER_SUCCESS);

    num = 0;
    str = "-2147483649";
    code = str2int(str, &num);
    printf("code = %d, str = %s, num = %d\n", code, str, num);
    assert(code == NUMBER_BORDER);

    num = 0;
    str = "2147483648";
    code = str2int(str, &num);
    printf("code = %d, str = %s, num = %d\n", code, str, num);
    assert(code == NUMBER_BORDER);

    exit(EXIT_SUCCESS);
}