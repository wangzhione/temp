// 线程与互斥锁抽象层, 兼容 Windows 与 POSIX 线程接口
#pragma once

#include "platform.h"

// 线程入口函数签名, 返回值当前未使用, 主要保持跨平台统一
typedef void * (* thread_routine_f)(void * arg);

#if defined(_WIN32)
// 跨平台线程句柄别名, Windows 下对应 HANDLE
typedef HANDLE thread_t;
// 跨平台互斥锁别名, Windows 下对应 CRITICAL_SECTION
typedef CRITICAL_SECTION mutex_t;
#else
// 跨平台线程句柄别名, POSIX 下对应 pthread_t
typedef pthread_t thread_t;
// 跨平台互斥锁别名, POSIX 下对应 pthread_mutex_t
typedef pthread_mutex_t mutex_t;
#endif

// 初始化互斥锁; 成功返回 0, 参数非法或底层调用失败返回负的错误码
extern int mutex_init(mutex_t * mutex);
extern void mutex_destroy(mutex_t * mutex);
extern void mutex_lock(mutex_t * mutex);
extern void mutex_unlock(mutex_t * mutex);

// 启动线程; 成功返回 0, 失败返回负的错误码
extern int thread_start(thread_t * thread, thread_routine_f routine, void * arg);
extern void thread_join(thread_t thread);

