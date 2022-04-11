#include <time.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PRINT(stream, error, fmt, ...)                                  \
fprintf(stderr, "[%ld]["#stream"][%s:%s:%d][%d:%s]"fmt"\n", time(NULL), \
    __FILE__, __func__, __LINE__, error, strerror(error), ##__VA_ARGS__)

#define PERROR(error, fmt, ...) PRINT(stderr, error, fmt, ##__VA_ARGS__)

#define PERRNO(fmt, ...) PERROR(errno, fmt, ##__VA_ARGS__)

#define PERR(...)                                           \
SELECT(__VA_ARGS__)(__VA_ARGS__)

#define CONCAT(X, Y) CONCAT_(X, Y)
#define CONCAT_(X, Y) X ## Y

#define SELECT(...)                                         \
CONCAT(SELECT_, NARG(__VA_ARGS__))(__VA_ARGS__)

#define SELECT_1(...)                                          \
PERRNO

#define SELECT_2(...)                                          \
PERROR

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

// build :
// gcc -g -O2 -Wall -Wextra -Werror -o generic generic.c
int main(void) {

    PERROR(1, "I am here!");
    PERROR(errno, "Hello, 世界!");
    PERRNO("Hello, 世界!");
    PERRNO("Hello, 世界 = %d!", 2022);

    PERR(1, "你好");
    PERR("世界");

    exit(EXIT_SUCCESS);
}
