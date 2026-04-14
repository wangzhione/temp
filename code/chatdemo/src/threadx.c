#include "threadx.h"

#if defined(_WIN32)
// Windows 线程启动上下文, 用于把统一入口函数和参数传给 _beginthreadex
typedef struct thread_start_ctx {
    // 真实线程入口函数
    thread_routine_f routine;
    // 传递给线程入口的参数
    void * arg;
} thread_start_ctx_t;

static unsigned __stdcall
thread__entry(void * arg) {
    thread_start_ctx_t * ctx = (thread_start_ctx_t *)arg;
    thread_routine_f routine = ctx->routine;
    void * routine_arg = ctx->arg;

    free(ctx);
    (void)routine(routine_arg);
    return 0;
}
#endif

int
mutex_init(mutex_t * mutex) {
    if (mutex == nullptr) {
        return -EINVAL;
    }

#if defined(_WIN32)
    InitializeCriticalSection(mutex);
    return 0;
#else
    {
        int rc = pthread_mutex_init(mutex, nullptr);
        return rc == 0 ? 0 : -rc;
    }
#endif
}

void
mutex_destroy(mutex_t * mutex) {
    if (mutex == nullptr) {
        return;
    }

#if defined(_WIN32)
    DeleteCriticalSection(mutex);
#else
    pthread_mutex_destroy(mutex);
#endif
}

void
mutex_lock(mutex_t * mutex) {
    if (mutex == nullptr) {
        return;
    }

#if defined(_WIN32)
    EnterCriticalSection(mutex);
#else
    pthread_mutex_lock(mutex);
#endif
}

void
mutex_unlock(mutex_t * mutex) {
    if (mutex == nullptr) {
        return;
    }

#if defined(_WIN32)
    LeaveCriticalSection(mutex);
#else
    pthread_mutex_unlock(mutex);
#endif
}

int
thread_start(thread_t * thread, thread_routine_f routine, void * arg) {
    if (thread == nullptr || routine == nullptr) {
        return -EINVAL;
    }

#if defined(_WIN32)
    thread_start_ctx_t * ctx = (thread_start_ctx_t *)calloc(1, sizeof(*ctx));
    uintptr_t handle;

    if (ctx == nullptr) {
        return -ENOMEM;
    }
    ctx->routine = routine;
    ctx->arg = arg;

    handle = _beginthreadex(nullptr, 0, thread__entry, ctx, 0, nullptr);
    if (handle == 0) {
        free(ctx);
        return -(errno != 0 ? errno : EAGAIN);
    }
    *thread = (HANDLE)handle;
    return 0;
#else
    {
        int rc = pthread_create(thread, nullptr, routine, arg);
        return rc == 0 ? 0 : -rc;
    }
#endif
}

void
thread_join(thread_t thread) {
#if defined(_WIN32)
    WaitForSingleObject(thread, INFINITE);
    CloseHandle(thread);
#else
    pthread_join(thread, nullptr);
#endif
}


