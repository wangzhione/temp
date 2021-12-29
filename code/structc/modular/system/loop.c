#include "loop.h"

// loop 轮询器结构
struct loop {
    q_t rq;                // 读消息
    q_t wq;                // 写消息
    pthread_mutex_t mutex; // 消息切换锁
    node_f frun;           // 消息处理行为
    node_f fdie;           // 消息销毁行为
    sem_t block;           // 线程信号量
    pthread_t id;          // 运行的线程id
    volatile bool loop;    // true 线程正在运行
    volatile bool wait;    // true 线程空闲等待
};

// run - 消息处理行为
inline void run(loop_t p, void * m) {
    // 开始处理消息
    p->frun(m);
    p->fdie(m);
}

//
// loop_delete - 轮询对象销毁
// p        : 轮询对象
// return   : void
//
void 
loop_delete(loop_t p) {
    //
    // delete 执行必须在 push 之后, C 代码是在刀剑上跳舞 ~ 
    //
    if (p) {
        p->loop = false;
        sem_post(&p->block);
        // 等待线程结束, 然后退出
        pthread_end(p->id);
        sem_destroy(&p->block);
        q_delete(p->rq, p->fdie);
        q_delete(p->wq, p->fdie);
        pthread_mutex_destroy(&p->mutex);
        free(p);
    }
}

//
// loop_push - 消息压入轮询器
// p        : 轮询对象
// m        : 消息
// return   : void
//
void 
loop_push(loop_t p, void * m) {
    assert(p && m);
    pthread_mutex_lock(&p->mutex);
    q_push(p->rq, m);
    pthread_mutex_unlock(&p->mutex);
    if (p->wait) {
        p->wait = false;
        sem_post(&p->block);
    }
}

// loop_run - 轮询器执行的循环体
static void loop_run(loop_t p) {
    while (p->loop) {
        void * m = q_pop(p->rq);
        if (m) {
            run(p, m);
            continue;
        }

        // read q <- write q
        pthread_mutex_lock(&p->mutex);
        q_swap(p->rq, p->wq);
        pthread_mutex_unlock(&p->mutex);

        m = q_pop(p->rq);
        if (m) run(p, m);
        else {
            // 仍然没有数据, 开始睡眠
            p->wait = true;
            sem_wait(&p->block);
        }
    }
}

//
// loop_create - 创建轮询对象
// frun     : node_f 消息处理行为
// fdie     : node_f 消息销毁行为
// return   : 轮询对象
//
loop_t 
loop_create(void * frun, void * fdie) {
    loop_t p = malloc(sizeof(struct loop));
    if (pthread_mutex_init(&p->mutex, NULL)) {
        free(p);
        return NULL;
    }

    q_init(p->rq);
    q_init(p->wq);
    p->frun = frun;
    p->fdie = fdie;
    p->wait = p->loop = true;
    // 初始化 POSIX 信号量, 进程内线程共享, 初始值 0
    sem_init(&p->block, 0, 0);
    if (pthread_run(&p->id, loop_run, p)) {
        sem_destroy(&p->block);
        free(p->rq->data);
        free(p->wq->data);
        free(p); 
        pthread_mutex_destroy(&p->mutex);
        return NULL;
    }
    
    return p;    
}
