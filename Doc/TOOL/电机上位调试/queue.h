/*
 * File: queue.h
 * Simple circular queue implement
 *
 * 2013.03.10 Created by Bruce.zhu
 */
#ifndef _CIR_QUEUE_H_
#define _CIR_QUEUE_H_

#define QUEUE_SIZE 1024*5

typedef struct{
	int front;
	int rear;
	UINT data[QUEUE_SIZE];
}cir_queue;


class CQueue
{
public:

	CQueue();
	virtual	~CQueue();

	void init_queue();
	int queue_empty();
	int queue_full();
	int en_queue(UINT ch);
	int de_queue(UINT *data);
	int queue_front(UINT *data);

private:

	cir_queue m_queue;

};



















#endif /* _CIR_QUEUE_H_ */

