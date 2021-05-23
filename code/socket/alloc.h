#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "system.h"

inline void * check(void * ptr, size_t size) {
    if (unlikely(!ptr)) {
        fprintf(stderr, "check memory collapse %zu\n", size);
        fflush(stderr);
        abort();
    }
    return ptr;
}

inline void * malloc_(size_t size) {
    return check(malloc(size), size);
}

inline char * strdup_(const char * str) {
    if (likely(str)) {
        size_t n = strlen(str) + 1;
        return memcpy(malloc_(n), str, n);
    }
    return NULL;
}

inline void * calloc_(size_t num, size_t size) {
    return check(calloc(num, size), size);
}

inline void * realloc_(void * ptr, size_t size) {
    return check(realloc(ptr, size), size);
}

// :) 包裹内存分配层, 些许感怀 ~ 可以通过 define ALLOC_OFF 关闭
//
#ifndef ALLOC_OFF
#  undef    malloc
#  define   malloc  malloc_
#  undef    strdup
#  define   strdup  strdup_
#  undef    calloc
#  define   calloc  calloc_
#  undef    realloc
#  define   realloc realloc_
#endif
