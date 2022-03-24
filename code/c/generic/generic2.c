#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CONCAT(X, Y) CONCAT_(X, Y)
#define CONCAT_(X, Y) X ## Y

#define ARGN(...) ARGN_(__VA_ARGS__)
#define ARGN_(_0, _1, _2, /*...*/ N, ...) N

#define NARG(...)                                           \
ARGN(__VA_ARGS__ COMMA(__VA_ARGS__) 3, 2, 1, 0)
#define HAS_COMMA(...)                                      \
ARGN(__VA_ARGS__, 1, 1, 0)

#define SET_COMMA(...) ,

#define COMMA(...) SELECT_COMMA(                            \
    HAS_COMMA(__VA_ARGS__),                                 \
    HAS_COMMA(__VA_ARGS__ ()),                              \
    HAS_COMMA(SET_COMMA __VA_ARGS__),                       \
    HAS_COMMA(SET_COMMA __VA_ARGS__ ())                     \
)

#define SELECT_COMMA(_0, _1, _2, _3)                        \
SELECT_COMMA_(_0, _1, _2, _3)
#define SELECT_COMMA_(_0, _1, _2, _3)                       \
COMMA_ ## _0 ## _1 ## _2 ## _3

#define COMMA_0000 ,
#define COMMA_0001
#define COMMA_0010 ,
#define COMMA_0011 ,
#define COMMA_0100 ,
#define COMMA_0101 ,
#define COMMA_0110 ,
#define COMMA_0111 ,
#define COMMA_1000 ,
#define COMMA_1001 ,
#define COMMA_1010 ,
#define COMMA_1011 ,
#define COMMA_1100 ,
#define COMMA_1101 ,
#define COMMA_1110 ,
#define COMMA_1111 ,

#define PRINT(stream, error, fmt, ...)                                  \
fprintf(stderr, "[%ld]["#stream"][%s:%s:%d][%d:%s]"fmt"\n", time(NULL), \
    __FILE__, __func__, __LINE__, error, strerror(error), ##__VA_ARGS__)

#define PERROR(error, fmt, ...)                                         \
PRINT(stderr, error, fmt, ##__VA_ARGS__)

#define PERRNO(fmt, ...)                                                \
PERROR(errno, fmt, ##__VA_ARGS__)

#define PERR_SELECT_1(...)                                              \
PERRNO

#define PERR_SELECT_2(...)                                              \
PERROR

#define PERR_SELECT(...)                                                \
CONCAT(PERR_SELECT_, NARG(__VA_ARGS__))(__VA_ARGS__)

#define PERR(...)                                                       \
PERR_SELECT(__VA_ARGS__)(__VA_ARGS__)

// _Generic 和 CONCAT 冲突
//
#define PERR_SELECT_NARG(errorfmt, ...)                                 \
_Generic((errorfmt), int : 1, default: 2)

#define PERR_SELECT2(...)                                                \
CONCAT(PERR_SELECT_, PERR_SELECT_NARG(__VA_ARGS__))(__VA_ARGS__)

#define PERR2(...)                                                       \
PERR_SELECT(__VA_ARGS__)(__VA_ARGS__)

// build :
// gcc -g -O2 -Wall -Wextra -Werror -o generic2 generic2.c
int main(void) {

    printf("%d\n", PERR_SELECT_NARG(1, "你好"));
    printf("%d\n", PERR_SELECT_NARG("世界"));

    PERR(1, "你好");
    PERR("世界");

    PERR2(2, "世界");
    PERR2("你好");

    exit(EXIT_SUCCESS);
}
