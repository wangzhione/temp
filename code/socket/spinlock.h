#pragma once

#include <stdatomic.h>

typedef volatile atomic_flag spinlock_t[1];

#define SPINLOCK_INIT (spinlock_t) { ATOMIC_FLAG_INIT }

inline void spinlock_init(volatile spinlock_t lock) {
    *lock = (atomic_flag) ATOMIC_FLAG_INIT;
}

inline void spinlock_lock(volatile spinlock_t lock) {
    while (atomic_flag_test_and_set(lock)) {}
}

inline _Bool spinlock_trylock(volatile spinlock_t lock) {
    return atomic_flag_test_and_set(lock) == 0;
}

inline void spinlock_unlock(volatile spinlock_t lock) {
    atomic_flag_clear(lock);
}
