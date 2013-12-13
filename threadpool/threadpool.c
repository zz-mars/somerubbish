#include"threadpool.h"
//#define DBG_MSG
void * thread_run(void * arg)
{
	thread_pool * tp = (thread_pool *)arg;
	task_runner * tskr;
	u32 task_processed_by_me = 0;
	pthread_t tid = pthread_self();
	while(1){
		pthread_mutex_lock(&(tp->mutex));
		while(tp->tsk_head == NULL && tp->shutdown != 1){
#ifdef DBG_MSG
			printf("no task to do,thread now sleep!\n");
#endif
			pthread_cond_wait(&(tp->cond),&(tp->mutex));
		}
		if(tp->shutdown == 1){
#ifdef DBG_MSG
			printf("thread pool is going to shut down!\nThis thread will exit!\n");
#endif
			printf("----------------------------- %4d tasks processed by thread #%u -----------------------\n",task_processed_by_me,tid);
			pthread_mutex_unlock(&(tp->mutex));
			pthread_exit(NULL);
		}
		tskr = tp->tsk_head;
		tp->tsk_head = tskr->next;
		if(tp->tsk_head == NULL){
			tp->tsk_tail = NULL;
		}
		pthread_mutex_unlock(&(tp->mutex));
#ifdef DBG_MSG
		printf("thread now call task func with its arg!\n");
#endif
		(tskr->task_func)(tskr->arg);
		task_processed_by_me++;
		/* free a task_runner struct when it is processed */
		free(tskr);
		tskr = NULL;
	}
	pthread_exit(NULL);
}
u32 thread_pool_init(thread_pool * tp,int max_thread_num)
{
	u32 i,r = 0;
	pthread_attr_t attr;
	if(pthread_mutex_init(&tp->mutex,NULL) != 0){
		perror("pthread_mutex_init");
		r = 1;
		goto over;
	}
#ifdef DBG_MSG
	printf("thread pool mutex initialized!\n");
#endif
	if(pthread_cond_init(&tp->cond,NULL) != 0){
		perror("pthread_cond_init");
		r = 2;
		goto over;
	}
#ifdef DBG_MSG
	printf("thread pool condition variable initialized!\n");
#endif
	tp->tsk_head = NULL;
	tp->tsk_tail = NULL;
	tp->shutdown = 0;
	tp->thread_pool_sz = max_thread_num;
	tp->threads = (pthread_t *)malloc(max_thread_num*sizeof(pthread_t));
	if(tp->threads == NULL){
		perror("malloc for threads");
		r = 3;
		goto over;
	}
	if(pthread_attr_init(&attr) != 0){
		perror("pthread_attr_init");
		r = 4;
		goto over1;
	}
#ifdef DBG_MSG
	printf("thread attr initialized!\n");
#endif
	if(pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED) != 0){
		perror("pthread_attr_setdetachstate");
		r = 5;
		goto over1;
	}
#ifdef DBG_MSG
	printf("thread detached attr set!\n");
#endif
	for(i = 0;i < max_thread_num;i++){
		if(pthread_create(&tp->threads[i],&attr,thread_run,(void *)tp) != 0){
			perror("pthread_create");
			r = 6;
			goto over1;
		}
		printf("thread #%u created!\n",tp->threads[i]);
#ifdef DBG_MSG
		printf("thread #%d created!\n",i);
#endif
	}
	if(pthread_attr_destroy(&attr) != 0){
		perror("pthread_attr_destroy");
		r = 7;
		goto over1;
	}
	goto over;
over1:
	free(tp->threads);
over:
	return r;
}
u32 thread_pool_add_tsk(thread_pool * tp,call_back * tsk_func,void * arg)
{
	task_runner * tskr = (task_runner *)malloc(sizeof(task_runner));
	if(tskr == NULL){
		perror("malloc for task runner");
		return 1;
	}
	tskr->task_func = tsk_func;
	tskr->arg = arg;
	tskr->next = NULL;
	pthread_mutex_lock(&(tp->mutex));
	if(tp->tsk_head == NULL){
#ifdef DBG_MSG
		printf("add 1st task to thread pool!\n");
#endif
		tp->tsk_head = tskr;
		tp->tsk_tail = tskr;
	}else{
#ifdef DBG_MSG
		printf("add a new task to thread pool!\n");
#endif
		tp->tsk_tail->next = tskr;
		tp->tsk_tail = tskr;
	}
	pthread_mutex_unlock(&(tp->mutex));
	pthread_cond_signal(&(tp->cond));
#ifdef DBG_MSG
	printf("wake up a new thread in thread pool!\n");
#endif
	return 0;
}
u32 thread_pool_destroy(thread_pool ** ptp)
{
	thread_pool * tp = *ptp;
	task_runner * tskr;
	if(tp->shutdown == 1){
		printf("already destroyed!\n");
		return 0;
	}
#ifdef DBG_MSG
	printf("thread pool is going to be destroyed!\n");
#endif
	tp->shutdown = 1;
	/* sleep for a while,
	 * wake all the threads and let them exit */
	pthread_cond_broadcast(&(tp->cond));
	sleep(1);
	free(tp->threads);
	tskr = NULL;
	while(tp->tsk_head != NULL){
		tskr = tp->tsk_head;
		tp->tsk_head = tp->tsk_head->next;
		free(tskr);
	}
	pthread_mutex_destroy(&(tp->mutex));
	pthread_cond_destroy(&(tp->cond));
	free(tp);
	*ptp = NULL;
	return 0;
}
void threadrun(void * arg)
{
	int i = *(int*)arg;
	pthread_t tid = pthread_self();
	printf("thread %u now processing task %d\n",tid,i);
	i = 1000000000;
	while(i>0){
		i /= 2;
	}
	return;
}
#define TASK_NUM	10000
#define THREAD_NUM	5
int main()
{
	int i;
	int tmp[TASK_NUM];
	thread_pool * tp = (thread_pool*)malloc(sizeof(thread_pool));
	thread_pool_init(tp,THREAD_NUM);
	for(i = 0;i < TASK_NUM;i++){
		tmp[i] = i;
#ifdef DBG_MSG
		printf("task #%d added to thread pool!\n",i);
#endif
		thread_pool_add_tsk(tp,threadrun,(void*)&tmp[i]);
	}
	sleep(2);
	thread_pool_destroy(&tp);
	return 0;
}
