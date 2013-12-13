#ifndef _THREAD_POOL_H
#define _THREAD_POOL_H
#include"stdio.h"
#include"stdlib.h"
#include"unistd.h"
#include"pthread.h"
#include"stdint.h"
#define _PROTOTYPE(func,arg)	extern func arg
typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef void call_back(void*);
typedef struct _task_runner{
	call_back * task_func;
	void * arg;
	struct _task_runner * next;
}task_runner;
typedef struct _thread_pool{
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	task_runner * tsk_head;
	task_runner * tsk_tail;
	int shutdown;
	pthread_t * threads;
	int thread_pool_sz;
}thread_pool;
_PROTOTYPE(u32 thread_pool_init,(thread_pool * tp,int max_thread_sz));
_PROTOTYPE(u32 thread_pool_destroy,(thread_pool ** tp));
_PROTOTYPE(void * thread_run,(void * arg));
_PROTOTYPE(u32 thread_pool_add_tsk,(thread_pool * tp,call_back * tsk_func,void * arg));
#endif
