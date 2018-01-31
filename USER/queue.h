/*
 * File: queue.h
 * Simple circular queue implement
 *
 * 2013.03.10 Created by Bruce.zhu
 */
#ifndef _CIR_QUEUE_H_
#define _CIR_QUEUE_H_

#include "stm32f2xx.h"
#include "string.h"

#define QUEUE_SIZE 64


typedef struct data_type
{
	u8  index;
	s16 x;
	s16 y;
}data_type;

typedef struct{
	int front;
	int rear;
	data_type data[QUEUE_SIZE];
}cir_queue;


void init_queue(cir_queue *Q);
int queue_empty(cir_queue *Q);
int queue_full(cir_queue *Q);
int en_queue(cir_queue *Q, data_type* x);
int de_queue(cir_queue *Q, data_type *data);
int queue_front(cir_queue *Q, data_type *data);


#endif /* _CIR_QUEUE_H_ */

