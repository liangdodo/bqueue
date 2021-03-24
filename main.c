#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "bqueue.h"

//生产者线程
void* producer_thread(void *ptr){
	struct bqueue_t *bq=(struct bqueue_t*)ptr;
	int data,i=1001;
	
	printf("生产者已启动生产...\n");
	
	for(;;){
		//生产者隔5秒生产一个int数据 放入到队列
		sleep(5);
		bqueue_push(bq,&i);
		i++;
	}
	
	return NULL;
}

//消费者线程
void* consumer_thread(void *ptr){
	struct bqueue_t *bq=(struct bqueue_t*)ptr;
	int data;
	
	printf("消费者已启动消费...\n");
	
	for(;;){
		//从b队列里面取出一个元素放入到data变量中，若队列为空，则阻塞
		bqueue_pop(bq,&data);
		printf("消费者从队列里面拿出了一个元素:%d\n",data);
	}
	
	return NULL;
}


int main(int argc,char *argv[]){
	
	struct bqueue_t bqueue;//定义一个b队列
	pthread_t producer_tid,consumer_tid;//生产、消费者的线程tid
	
	//初始化一个数量为100b队列，队列中的每个数据元素为int类型（sizeof(int)）
	bqueue_init(&bqueue,sizeof(int),100);
	//创建生产者线程
	pthread_create(&producer_tid,NULL,producer_thread,&bqueue);
	//创建消费者线程
	pthread_create(&consumer_tid,NULL,consumer_thread,&bqueue);
	
	//等待线程结束
	pthread_join(producer_tid,NULL);
	pthread_join(consumer_tid,NULL);
	//销毁b队列
	bqueue_destroy(&bqueue);
	return 0;
}