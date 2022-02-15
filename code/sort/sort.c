#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

/*

    排序算法, 默认从小到大 升序

    https://zhuanlan.zhihu.com/p/42586566

    https://github.com/MisterBooo/Article

*/

#define ARRAY_LEN (20)

static void array_init(int a[], int len) {
    for (int i = 0; i < len; i++) {
        a[i] = rand();
    }
}

static bool array_assert(const char * name, int index, int old[], int a[], int len) {
    int i, j;

    // 大小检验
    for (i = 1; i < len; i++) {
        if (a[i-1] > a[i]) {
            fprintf(stderr, "sort %s 第 %d 组 len = %d [%d > %d] 测试失败.\n", name, index, len, a[i-1], a[i]);
            return false;
        }
    }

    // 数据检验
    bool check[len];
    memset(check, 0, sizeof(check));

    for (i = 0; i < len; i++) {
        int value = old[i];
        int exists = false;
        for (j = 0; j < len; j++) {
            if (a[j] == value) {
                // 之前已经算过了, 跳过去
                if (!check[j]) {
                    exists = true;
                    check[j] = true;
                    break;
                } 
            }
        }
        if (!exists) {
            fprintf(stderr, "sort %s 第 %d 组 len = %d 测试失败 [%d : %d]\n", name, index, len, i, value);
            return false;
        }
    }

    return true;
}

typedef void (* sort_f)(int a[], int len);

static bool array_test(const char * name, int index, sort_f fsort, int a[], int len) {
    int i;
    int old[len];
    memcpy(old, a, sizeof(int) * len);

    fsort(a, len);

    bool status = array_assert(name, index, old, a, len);
    if (status) {
        return true;
    }

    fprintf(stderr, "sort %s 第 %d 组 len = %d 测试失败\n", name, index, len);
    fprintf(stderr, "原始:");
    for (i = 0; i < len; i++) {
        fprintf(stderr, " %d", old[i]);
    }
    fprintf(stderr, "\n");
    fprintf(stderr, "现在:");
    for (i = 0; i < len; i++) {
        fprintf(stderr, " %d", a[i]);
    }
    fprintf(stderr, "\n");
    return false;
}

static void sort_test(sort_f fsort, const char * name) {
    int len = ARRAY_LEN;
    int a[len];
    bool status;

    fprintf(stdout, "sort %s 测试开始 ", name);
    for (int i = 0; i < len; i++) {
        array_init(a, len);
        status = array_test(name, i, fsort, a, len);
        fprintf(stdout, " %d%s", i, status ? "√" : "x");
        assert(status);
    }
    fprintf(stdout, "\n");
    fprintf(stdout, "sort %s 测试通过\n", name);
}

#define sort_test_assert(fsort) sort_test(fsort, #fsort);

#include "sort_bubble.c"
#include "sort_selection.c"
#include "sort_insertion.c"
#include "sort_shell.c"
#include "sort_merge.c"
#include "sort_quick.c"
#include "sort_heap.c"

//
// build : gcc -g -O3 -Wall -Wextra -Werror -o sort sort.c
//
int main(void) {
    unsigned seed = (unsigned)time(NULL);
    srand(seed);
    fprintf(stdout, "sort seed = %u\n", seed);

    sort_test_assert(sort_bubble);
    sort_test_assert(sort_bubble_upgrade);
    
    sort_test_assert(sort_selection);

    sort_test_assert(sort_insertion);
    sort_test_assert(sort_insertion_upgrade);

    sort_test_assert(sort_shell);

    sort_test_assert(sort_merge);
    sort_test_assert(sort_merge_non_recursive);

    sort_test_assert(sort_quick);

    sort_test_assert(sort_heap);

    exit(EXIT_SUCCESS);
}

// ulimit -c unlimited

// cat /proc/sys/kernel/core_pattern
// |/usr/share/apport/apport %p %s %c %d %P %E
// cat /proc/sys/kernel/core_uses_pid
// 0
// su root
// echo "1" > /proc/sys/kernel/core_uses_pid 
// echo "core-%e-%p-%t" > /proc/sys/kernel/core_pattern

/*
 %p - insert pid into filename 添加 pid
 %u - insert current uid into filename 添加当前 uid
 %g - insert current gid into filename 添加当前 gid
 %s - insert signal that caused the coredump into the filename 添加导致产生core的信号
 %t - insert UNIX time that the coredump occurred into filename 添加 core 文件生成时的unix时间
 %h - insert hostname where the coredump happened into filename 添加主机名
 %e - insert coredumping executable name into filename 添加命令名
 */