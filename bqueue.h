#ifndef _BQUEUE_H_
#define _BQUEUE_H_

#include <semaphore.h>
struct bqueue_node_t{
	void *data;
	struct bqueue_node_t *next;
};

struct bqueue_t{
	int length;
	int max_length;
	struct bqueue_node_t *head;
	struct bqueue_node_t *tail;
	int data_size;
	
	sem_t mutex;
	sem_t sync_push;
	sem_t sync_pop;
};


int bqueue_init(struct bqueue_t *bq,int data_size,int max_length);
void bqueue_destroy(struct bqueue_t *bq);
int bqueue_push(struct bqueue_t *bq,void *data);
int bqueue_pop(struct bqueue_t *bq,void *data);
int bqueue_length(struct bqueue_t *bq);
int bqueue_head(struct bqueue_t *bq,void *buf);
int bqueue_tail(struct bqueue_t *bq,void *buf);
void bqueue_clear(struct bqueue_t *bq);
struct bqueue_node_t * bqueue_make_node(struct bqueue_t *bq,void *data);
void bqueue_free_node(struct bqueue_node_t *bq_node);
void bqueue_traverse(struct bqueue_t *bq,void (*call)(void *data));
int bqueue_get_free(struct bqueue_t *bq);
#endif
