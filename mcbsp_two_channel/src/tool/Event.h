#ifndef __STM32F10x_EVENT_H
#define __STM32F10x_EVENT_H

#include "type.h"

typedef enum {
	NoEvent,
	
	Timer1,
	Timer2,
	Timer3,
	UART1,
	UART2,
	UART3,
	PAGE,
	KEYCODE,
	EXITR,
	WIFISucc,
	UDPSucc,
	UDPSending,
	UDPFail,
	WIFIFail,
	D61580RX,
	D61580TX
}EventType;
void Event_Init();
bool Event_Post(EventType type,u32 hex,u32 dec,void* param);
bool Event_Read(EventType* type,u32* hex,u32* dec,void** param);

#endif
