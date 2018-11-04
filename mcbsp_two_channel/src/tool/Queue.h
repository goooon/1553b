#ifndef __STM32F10x_QUEUE_H
#define __STM32F10x_QUEUE_H

#include "type.h"
	typedef struct _QueueNode
	{
		u32 type;
		u32 hex;
		u32 dec;
		void* param;
	}QueueNode;
	typedef struct _StaticQueue{
		s32 _in;
		s32 _out;
		u32 _itemSize;
		u32 _size;
		QueueNode* node;
	}StaticQueue;
	void Queue_Init(StaticQueue* q,QueueNode* n,u32 size);
	b32 Queue_isEmpty(StaticQueue* q);
	b32 Queue_isFull(StaticQueue* q);
	QueueNode* Queue_outlet(StaticQueue* q);
	QueueNode* Queue_inlet(StaticQueue* q);
	QueueNode* Queue_getAt(StaticQueue* q,s32 index);
	u32 Queue_getLength(StaticQueue* q);
	b32 Queue_inQueue(StaticQueue* q,QueueNode* in);
	b32 Queue_outQueue(StaticQueue* q,QueueNode* out);
#endif
