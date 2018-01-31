/*
 * File: queue.c
 * First created: 2013.03.10
 */
#include "queue.h"


void init_queue(cir_queue *Q)
{
	Q->front = Q->rear = 0;
}

int queue_empty(cir_queue *Q)
{
	return Q->front == Q->rear;
}

int queue_full(cir_queue *Q)
{
	if(Q->front == (Q->rear + 1) % QUEUE_SIZE)
		return 1;
	else
		return 0;
}

int en_queue(cir_queue *Q, data_type* data)
{
	if(queue_full(Q))
		return 0;

	memcpy(&(Q->data[Q->rear]), data, sizeof(data_type));

	Q->rear = (Q->rear + 1) % QUEUE_SIZE;
	return 1;
}

int de_queue(cir_queue *Q, data_type *data)
{
	if(queue_empty(Q))
		return 0;

	memcpy(data, &(Q->data[Q->front]), sizeof(data_type));

	Q->front = (Q->front + 1) % QUEUE_SIZE;
	return 1;
}


int queue_front(cir_queue *Q, data_type *data)
{
	if(queue_empty(Q))
		return 0;

	memcpy(data, &(Q->data[Q->front]), sizeof(data_type));

	return 1;
}

