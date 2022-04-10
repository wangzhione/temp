#include <stdio.h>
#include <errno.h>
#include <wchar.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* describe : 3. 无重复字符的最长子串

   给定一个字符串 S, 请你找出其中不含有重复字符的最长子串的长度
 */

int lengthOfLongestSubstring0(char * s) {
    if (s == NULL || *s == 0) {
        return 0;
    }

    int now;
    int max = 1;
    int hash[255];

    // 暴力循环
    for (char * i = s; *i != 0; i++) {
        now = 1;
        memset(hash, 0, sizeof hash);
        hash[*i+128] = 1;

        for (char * j = i+1; *j != 0 && hash[*j+128] == 0; j++) {
            hash[*j+128] = 1;
            if (max < ++now) {
                max = now;
            }
        }
    }

    return max;
}

int lengthOfLongestSubstring(char * s) {
    if (s == NULL || *s == 0) {
        return 0;
    }

    // 记录第一个数据
    char * substrings[255] = {};

    char * begin = s;
    char * end = s;

    int val = *begin+128;
    int max = 1;
    int now = 1;
    substrings[val] = s;

    while ((val = *++end + 128) != 128) {
        char * middlen = substrings[val];
        // 如果没有记录
        if (middlen == NULL) {
            substrings[val] = end;
            if (max < ++now) {
                max = now;
            }
        } else {
            // 开始跳跃和清理
            // [begin, end) -> [begin, middlen], (middlen, end) -> (middlen, end]
            now = end - middlen;
            // [begin, middlen] 需要 clear
            while (begin <= middlen) {
                substrings[*begin+128] = NULL;
                ++begin;
            }
            // 重新填充
            substrings[val] = end;
        }
    }

    return max;
}

// build:
// gcc -g -O3 -Wall -Wextra -Werror -o 001_substring 001_substring.c
//
int main(void) {
    char * str;
    int length;

    str = "abcabcbb";
    length = lengthOfLongestSubstring(str);
    printf("[%s] -> %d\n", str, length);

    str = "bbbbb";
    length = lengthOfLongestSubstring(str);
    printf("[%s] -> %d\n", str, length);

    str = "pwwkew";
    length = lengthOfLongestSubstring(str);
    printf("[%s] -> %d\n", str, length);

    exit(EXIT_SUCCESS);
}
