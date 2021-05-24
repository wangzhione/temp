#pragma once

#include <stdatomic.h>

inline void atomic_flag_lock(volatile atomic_flag * lock) {
    while (atomic_flag_test_and_set(lock)) {}
}

inline void atomic_flag_unlock(volatile atomic_flag * lock) {
    atomic_flag_clear(lock);
}

inline _Bool atomic_flag_trylock(volatile atomic_flag * lock) {
    return atomic_flag_test_and_set(lock) == 0;
}
