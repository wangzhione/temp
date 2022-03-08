#include <stdio.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* describe : 

   写一个完整 string to int32_t
 */

#define NUMBER_SUCCESS  0    // 转换成功
#define NUMBER_INVALID -2    // 非法字符
#define NUMBER_BORDER  -3    // 数值越界

int str2int32(const char * str, int32_t * res) {
    if (str == NULL) {
        return NUMBER_INVALID;
    }

    if (*str == 0) {
        if (res) *res = 0;
        return NUMBER_SUCCESS;
    }

    // int32_t	int	Signed	32	4	-2,147,483,648	2,147,483,647
    // + - 1 字节, 后面 max size = 10
    // 如果 max size = 10, 那么 max size = 9 的时候最大值为 2,147,483,64

    int32_t num = 0;
    bool minus = false; // 标识负数
    if (*str == '+' || *str == '-') {
        minus = *str++ == '-';
    }

    char c;
    int count = 0;
    while (++count < 10 && (c = *str++) != 0) {
        if (c >= '0' && c <= '9') {
            // 合理字符
            num = 10 * num + c - '0';
        } else {
            return NUMBER_INVALID;
        }
    }
    if (c != 0) {
        if (num > 214748364) {
            // size = 9 时候最大值
            return NUMBER_BORDER;
        }
        c = *str++;
        if (*str != 0) {
            // size > 10 越界
            return NUMBER_BORDER;
        }
        if ((c >= '0' && c <= '7') || (c == '8' && minus)) {
            if (c == '8') {
                if (res) *res = -2147483648;
                return NUMBER_SUCCESS;
            }
            num = 10 * num + c - '0';
        } else {
            return NUMBER_BORDER;
        }
    }

    num = minus ? -num : num;
    if (res) *res = num;
    return NUMBER_SUCCESS;
}


// build:
// gcc -g -O3 -Wall -Wextra -Werror -o 67_string_int 67_string_int.c
//
int main(void) {
    int code;
    int32_t num;
    const char * str;

    num = 0;
    str = NULL;
    code = str2int32(str, &num);
    printf("code = %d, str = %s, num = %d\n", code, str, num);
    assert(code == NUMBER_INVALID && num == 0);

    num = 0;
    str = "-1";
    code = str2int32(str, &num);
    printf("code = %d, str = %s, num = %d\n", code, str, num);
    assert(code == NUMBER_SUCCESS && num == -1);

    num = 0;
    str = "1234A";
    code = str2int32(str, &num);
    printf("code = %d, str = %s, num = %d\n", code, str, num);
    assert(code == NUMBER_INVALID);

    num = 0;
    str = "2147483647";
    code = str2int32(str, &num);
    printf("code = %d, str = %s, num = %d\n", code, str, num);
    assert(code == NUMBER_SUCCESS && num == 2147483647);

    num = 0;
    str = "-2147483648";
    code = str2int32(str, &num);
    printf("code = %d, str = %s, num = %d\n", code, str, num);
    assert(code == NUMBER_SUCCESS && num == -2147483648);

    num = 0;
    str = "-2147483649";
    code = str2int32(str, &num);
    printf("code = %d, str = %s, num = %d\n", code, str, num);
    assert(code == NUMBER_BORDER);

    num = 0;
    str = "2147483648";
    code = str2int32(str, &num);
    printf("code = %d, str = %s, num = %d\n", code, str, num);
    assert(code == NUMBER_BORDER);

    exit(EXIT_SUCCESS);
}
