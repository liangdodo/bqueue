#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bqueue.h"

int bqueue_init(struct bqueue_t *bq,int data_size,int max_length){
	struct bqueue_node_t *p=NULL;
	bq->length=0;
	bq->max_length=max_length;
	bq->data_size=data_size;
	
	//初始化互斥信号量
	sem_init(&bq->mutex,0,1);
	//初始化同步信号量
	sem_init(&bq->sync_push,0,max_length);
	sem_init(&bq->sync_pop,0,0);
	
	p= bqueue_make_node(bq,NULL);
	
	if(NULL==p){
		return -1;
	}
	
	bq->head=bq->tail=p;
	return 0;
}

void bqueue_destroy(struct bqueue_t *bq){
	bqueue_clear(bq);
	bqueue_free_node(bq->head);
	bq->head=bq->tail=NULL;
	//销毁互斥信号量
	sem_destroy(&bq->mutex);
	//销毁同步信号量
	sem_destroy(&bq->sync_push);
	sem_destroy(&bq->sync_pop);
}

void bqueue_clear(struct bqueue_t *bq){
	int i=0;
	
	for(i=0;i < bq->length;i++){
		bqueue_pop(bq,NULL);
	}
	
	bq->tail=bq->head;
}

struct bqueue_node_t * bqueue_make_node(struct bqueue_t *bq,void *data){
	struct bqueue_node_t *p=NULL;
	void *q=NULL;
	
	p=(struct bqueue_node_t *)malloc(sizeof(struct bqueue_node_t));
	
	if(NULL==p){
		return NULL;
	}
	
	p->next=NULL;
	
	if(NULL!=data){
		q=malloc(bq->data_size);
		
		if(NULL==q){
			free(p);
			return NULL;
		}
		
		memcpy(q,data,bq->data_size);
	}
	
	p->data=q;
	return p;
}

void bqueue_free_node(struct bqueue_node_t *bq_node){
	if(bq_node->data!=NULL){
		free(bq_node->data);
	}
	
	bq_node->next=NULL;
	free(bq_node);
}



int bqueue_push(struct bqueue_t *bq,void *data){
	struct bqueue_node_t *bq_node=NULL;
	
	sem_wait(&bq->sync_push);
	sem_wait(&bq->mutex);
	bq_node=bqueue_make_node(bq,data);
	
	if(NULL==bq_node){
		sem_post(&bq->mutex);
		sem_post(&bq->sync_push);
		return -1;
	}
	
	bq->tail->next=bq_node;
	bq->tail=bq_node;
	bq->length++;
	//printf("%d入队\n",*(int*)data);
	sem_post(&bq->mutex);
	sem_post(&bq->sync_pop);
	return 0;
}


int bqueue_pop(struct bqueue_t *bq,void *data){
	struct bqueue_node_t * bq_node=NULL;
	sem_wait(&bq->sync_pop);
	sem_wait(&bq->mutex);
	bq_node=bq->head->next;
	
	if(NULL!=data){
		memcpy(data,bq_node->data,bq->data_size);
	}
	
	bq->head->next=bq_node->next;
	bqueue_free_node(bq_node);
	bq->length--;
	
	if(bq->length <=0){
		bq->tail=bq->head;
	}
	
	//printf("%d出队\n",*(int*)data);
	sem_post(&bq->mutex);
	sem_post(&bq->sync_push);
	return 0;
}

int bqueue_length(struct bqueue_t *bq){
	return bq->length;
}

int bqueue_head(struct bqueue_t *bq,void *buf){
	struct bqueue_node_t *p=NULL;
	p=bq->head->next;
	
	if(NULL==p){
		return -1;
	}
	
	memcpy(buf,p->data,bq->data_size);
	return 0;
}

int bqueue_tail(struct bqueue_t *bq,void *buf){
	struct bqueue_node_t *p=NULL;
	p=bq->tail;
	
	if(NULL==p){
		return -1;
	}
	
	memcpy(buf,p->data,bq->data_size);
	return 0;
}

void bqueue_traverse(struct bqueue_t *bq,void (*call)(void *data)){
	int i=0;
	struct bqueue_node_t *p=bq->head->next;
	
	for(i=0; i < bq->length;i++){
		call(p->data);
		p=p->next;
	}
}

int bqueue_get_free(struct bqueue_t *bq){
	int val=0;
	sem_getvalue(&(bq->sync_push),&val);
	return val;
}