#include "Queue.h"
#include "tracer.h"
void Queue_Init(StaticQueue* q,QueueNode* node,u32 size){
	QueueNode* n,*e;
	q->_in = 0;
	q->_out = 0;
	q->_itemSize = sizeof(QueueNode);
	q->_size = size;
	q->node = node;
	n = node;
	e = n + size;
	for(;n != e;){
		n->type = 0;
		n++;
	}
}
b32 Queue_isEmpty(StaticQueue* q)
{
	b32 ret = q->_in == q->_out;
	return ret;
}
b32 Queue_isFull(StaticQueue* q)
{
	b32 ret = (q->_in + 1) % q->_size == q->_out;
	return ret;
}
QueueNode* Queue_outlet(StaticQueue* q)
{
	LOG_A(!(q->_in == q->_out));
	return &q->node[q->_out];
}
QueueNode* Queue_inlet(StaticQueue* q)
{
	LOG_A(!((q->_in + 1) % q->_size == q->_out));
	return &q->node[q->_in];
}
QueueNode* Queue_getAt(StaticQueue* q,s32 index)
{
	s32 len = (q->_in - q->_out + q->_size) % q->_size;
	LOG_A(index < len);
	return &q->node[(q->_out + index) % q->_size];
}
u32 Queue_getLength(StaticQueue* q)
{
	u32 len = (q->_in - q->_out + q->_size) % q->_size;
	return len;
}
b32 Queue_inQueue(StaticQueue* q,QueueNode* in)
{
	if ((q->_in + 1) % q->_size == q->_out)
	{
		 return vFalse;
	}
	else
	{
		q->node[q->_in] = *in;
		q->_in += (q->_size + 1);
		q->_in %= q->_size;
	}
	return vTrue;
}
b32 Queue_outQueue(StaticQueue* q,QueueNode* out)
{
	if (q->_in == q->_out)
		return vFalse;
	*out = q->node[q->_out];
	q->_out += 1;
	q->_out %= q->_size;
	return vTrue;
}
