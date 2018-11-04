#include "./timer.h"
#include <csl.h>
#include <csl_timer.h>
#include <csl_cache.h>
#include "./tracer.h"
typedef enum { NoIsrActive, Isr1Active, Isr2Active, Isr3Active} ISR_LIST;
static volatile ISR_LIST WhatIsrActive = NoIsrActive;

static volatile u32 Isr0 = 0;
static volatile u32 Isr1 = 0;
static volatile u32 Isr2 = 0;
static volatile u32 timer0value=0;
static volatile u32 timer1value=0;
static volatile u32 timer2value=0;
typedef void (*CB)(u32);
CB g_msec_cb;
CB g_usec_cb;
interrupt void PerformIsr0(void)
{
    timer0value++;
}

/*-----------------------------------------------------------------------------
* void PerformIsr2(void)
*
* Indicate that Isr2 is being performed, and increment the number of times
* it has been performed. Every so often, take a longer time than normal
* to introduce some variability into the system timing.
*----------------------------------------------------------------------------*/

interrupt void PerformIsr1(void)
{
    timer1value++;
	g_msec_cb(timer1value);
}

/*-----------------------------------------------------------------------------
* void PerformIsr3(void)
*
* Indicate that Isr3 is being performed, and increment the number of times
* it has been performed. Every so often, take a longer time than normal
* to introduce some variability into the system timing.
*----------------------------------------------------------------------------*/

interrupt void PerformIsr2(void)
{
    timer2value++;
	g_usec_cb(timer2value);
}

/* ----------------------------------------------------------------------*/
/*           Copyright (C) 2001 Texas Instruments Incorporated.          */
/*                           All Rights Reserved                         */
/*-----------------------------------------------------------------------*/

void Timer_Setup(void)
{
    IRQ_resetAll();                       /* Reset all maskable interrupts  */
  	IRQ_enable(IRQ_EVT_TINT0);            /* Enable timer0 -> CPU interrupt */
    IRQ_enable(IRQ_EVT_TINT1);            /* Enable timer1 -> CPU interrupt */
    IRQ_map(IRQ_EVT_TINT2,13);            /* Enable timer2 -> CPU interrupt */
    IRQ_enable(IRQ_EVT_TINT2);            /* Enable timer1 -> CPU interrupt */
    IRQ_nmiEnable();                      /* Enable non-maskable interrupt  */
} /* end SetupInterrupts() */

//clkin: 40M
//clkmode1: 1
//clkmode0: 0
//40M x 12 = 480M
//tms320c6416.pdf page68
//timer internal clock : 480M / 8 = 60M
//1 second == 1000 msec == 1000,000 usec <=> 60,000,000 ticks
//1 usec <=>60 ticks
#define U_SECOND(n) (60 * n)
#define M_SECOND(n) (60000 * n)
#define SECOND(n)   (60000000UL * n) //n should less than 71
void Timer_Config(u32 msecond,void(*msec_cb)(u32),u32 usecond,void(*usec_cb)(u32))
{
    TIMER_Handle timer0;
    TIMER_Handle timer1;
    TIMER_Handle timer2;

	g_msec_cb = msec_cb;
	g_usec_cb = usec_cb;
    
    timer0 = TIMER_open(TIMER_DEV0,0);//priority 14
    TIMER_configArgs( timer0, 0x000002c0, U_SECOND(1), 0x00000000);
    
	if(msec_cb){//priority 15
    	timer1 = TIMER_open(TIMER_DEV1,0);
    	TIMER_configArgs( timer1, 0x000002c0, M_SECOND(msecond), 0x00000000);
	}
    
	if(usec_cb){//highest priority 13
    	timer2 = TIMER_open(TIMER_DEV2,0);
    	TIMER_configArgs( timer2, 0x000002c0, U_SECOND(usecond), 0x00000000);  
	}
} /* end ConfigureAllTimers() */


void Timer_DelayUS(u32 us){
	timer0value = 0;
	while(timer0value < us){
	}
}
void Timer_DelayMS(u32 ms){
	u32 us = ms * 1000;
	timer0value = 0;
	while(timer0value < us){
	}
}
