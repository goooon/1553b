#ifndef API_TIMER_H
#define API_TIMER_H
#include "type.h"

void Timer_Setup(void);
void Timer_Config(u32 msecond,void(*msec_cb)(u32),u32 usecond,void(*usec_cb)(u32));
void Timer_DelayUS(u32 us);
void Timer_DelayMS(u32 ms);
#endif
