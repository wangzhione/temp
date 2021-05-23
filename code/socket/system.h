#pragma once

//
// 辽阔的夜, 黝黑而深邃 ~
//
// 我在模拟跨平台路上走过太多弯路. 可能也有点收获, 那就是认怂 ~ 
// 
// 如果真条条大道通罗马, 为什么不试试其中一条呢 ~
//
// 
// 苏堤春晓
// 明 杨周
// 
// 柳暗花明春正好，
// 重湖雾散分林鸟。
// 何处黄鹂破暝烟，
// 一声啼过苏堤晓。
// 
//

#include <stdint.h>
#include <stddef.h>

//
// 约定: BEST NEW VERSION 操作系统 Linux + 编译工具 GCC
//
#if defined __linux__ && defined __GNUC__

#if defined __x86_64__
#  define ISX64
#endif

//
// 大小端检测 : ISBIG defined 表示大端
//
#if defined __BIG_ENDIAN__ || defined __BIG_ENDIAN_BITFIELD
#  define ISBIG
#endif

#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#define PACKED(declare)             \
declare                             \
__attribute__((__packed__))

#endif

// small - 转本地字节序(小端)
inline uint32_t small(uint32_t x) {
#  ifdef ISBIG
    uint8_t t;
    uint8_t * p = (uint8_t *)&x;
    t = p[0]; p[0] = p[3]; p[3] = t;
    t = p[1]; p[1] = p[2]; p[2] = t;
#  endif
    return x;
}
