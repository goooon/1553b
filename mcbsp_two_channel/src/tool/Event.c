#include "Event.h"
#include "tracer.h"
#include "Queue.h"

static QueueNode eventNodes[256];
static StaticQueue staticQueue;

#define __disable_irq() IRQ_globalDisable()
#define __enable_irq() IRQ_globalEnable()

void Event_Init(){
    Queue_Init(&staticQueue,eventNodes,256);
}

bool Event_Post(EventType type,u32 hex,u32 dec,void* param)
{
	QueueNode n;
	n.dec = dec;
	n.hex = hex;
	n.type = type;
	n.param = param;
	__disable_irq();
	if(!Queue_inQueue(&staticQueue, &n)){
		__enable_irq();
		LOG_E("eventQueue.inQueue() Failed");
		return false;
	}
	__enable_irq();
	return true;
}

bool Event_Read(EventType* type,u32* hex,u32* dec,void** param)
{
	 QueueNode n;
	 __disable_irq();
	 if(!Queue_outQueue(&staticQueue,&n)){//queue is empty
		 __enable_irq();
		 //LOG_E("eventQueue.outQueue() Failed");
		 return false;
	 }
	 *type = (EventType)n.type;
	 *hex = n.hex;
	 *dec = n.dec;
	 *param = n.param;
	 __enable_irq();
	 return true;
}
