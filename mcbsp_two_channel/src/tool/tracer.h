#ifndef __STM32F10x_PRINT_H
#define __STM32F10x_PRINT_H
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdarg.h>
#include "./type.h"


#define ASCII_ON 1   //guzhihe

int UART_sendFormat(const char* fmt,...);
int doNothing(const char* fmt,...);

#ifndef ASCII_ON
/*#define LOG_V do{printf("V:"),__FILE__,__LINE__;}while(0);printf
#define LOG_I do{printf("I:"),__FILE__,__LINE__;}while(0);printf
#define LOG_W do{printf("W:"),__FILE__,__LINE__;}while(0);printf
#define LOG_E do{printf("E:"),__FILE__,__LINE__;}while(0);printf
#define LOG_F do{printf("F:"),__FILE__,__LINE__;}while(0);printf
#define LOG_A(expr) do{if(!expr){printf("A:"#expr" in %s(%d)",__FILE__,__LINE__);printf("\n");}}while(0)
#define LOG_P printf*/
#define LOG_V UART_sendFormat
#define LOG_I UART_sendFormat
#define LOG_W UART_sendFormat
#define LOG_E UART_sendFormat
#define LOG_F UART_sendFormat
#define LOG_A(expr) do{}while(0)
#define LOG_P UART_sendFormat
#define RSP_UART(data,n)  UART_sendBytes(data,n)
#else
#define LOG_V doNothing
#define LOG_I doNothing
#define LOG_W doNothing
#define LOG_E doNothing
#define LOG_F doNothing
#define LOG_A(expr) do{}while(0)
#define LOG_P doNothing
#define RSP_UART(data,n)  
#endif
#endif //__STM32F10x_PRINT_H
