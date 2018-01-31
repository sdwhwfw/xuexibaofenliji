/*
 * File: queue.c
 * First created: 2013.03.10
 */

#include "stdafx.h"
#include "queue.h"


CQueue::CQueue()
{
}

CQueue::~CQueue()
{
}


void CQueue::init_queue()
{
	m_queue.front = m_queue.rear = 0;
}

int CQueue::queue_empty()
{
	return m_queue.front == m_queue.rear;
}

int CQueue::queue_full()
{
	if(m_queue.front == (m_queue.rear + 1) % QUEUE_SIZE)
		return 1;
	else
		return 0;
}

int CQueue::en_queue(UINT data)
{
	if(queue_full())
		return 0;

	m_queue.data[m_queue.rear] = data;

	m_queue.rear = (m_queue.rear + 1) % QUEUE_SIZE;
	return 1;
}

int CQueue::de_queue(UINT *data)
{
	if(queue_empty())
		return 0;

	*data = m_queue.data[m_queue.front];

	m_queue.front = (m_queue.front + 1) % QUEUE_SIZE;
	return 1;
}


int CQueue::queue_front(UINT *data)
{
	if(queue_empty())
		return 0;

	*data = m_queue.data[m_queue.front];

	return 1;
}





