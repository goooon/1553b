#ifndef _MY_UART_H
#define _MY_UART_H
#include <csl_mcbsp.h>
#include <csl_edma.h>
typedef struct _UartBuffer{
    u32 length;
	u16 buffer[0x400];
}UartBuffer;
void UART_ConfigMcBSP(MCBSP_Handle hMcbsp);
void UART_ProcessTransmitData(u8 *data);
void UART_transmitBytes(u8  *tx_msg,u32 len);
void UART_transmitWords(u16 *tx_msg,u32 len);
void UART_sendString(const char* s);
void UART_sendBytes(u8 *tx_msg,u32 len);
int  UART_sendFormat(const char* s,...);
void UART_ProcessReceiveData(u8* recv_data,u32 size,UartBuffer* buffer);
void UART_init(u32 uartBuffSize);
void UART_SetSendMode(bool send);
void UART_Store(UartBuffer* buff);
#endif
