#include <time.h>
#include <csl.h>
#include <csl_timer.h>
#include <csl_cache.h>
#include <csl_emif.h>
#include <csl_emifb.h>
#include <csl_emifa.h>
#include <csl_pll.h>
#include <csl_mcbsp.h>
#include <csl_edma.h>
#include <csl_gpio.h>
#include <csl_irq.h>  
#include "./tool/tracer.h"
#include "./BU61580/reg.h"
#include "./tool/timer.h"
#include "./tool/Event.h"
#include "./task/status.h"
#include "./uart/uart.h"
#include "./task/task.h"


////////////////////////////////////////////////////////////////////////////
//EMIFB �Ĵ������ú���
////////////////////////////////////////////////////////////////////////////
/*typedef struct {
  Uint32 gblctl;
  Uint32 cectl0;
  Uint32 cectl1;
  Uint32 cectl2;
  Uint32 cectl3;
  Uint32 sdctl;
  Uint32 sdtim;
  Uint32 sdext;
  Uint32 cesec0;
  Uint32 cesec1;
  Uint32 cesec2;
  Uint32 cesec3;
} EMIFB_Config;*/

//0xC03FFF1F
//0x083FFF1F
//0x3F3EEF1F
//0x3F3EFF17
#define WRSETUP  0x3  //1111b
#define WRSTRB   0x1F //11,1111b
#define WRHLD 	 0x1  //11b
#define RDSETUP  0x6  //1111b
#define TA		 0x3  //11b
#define RDSTRB   0x2F //11,1111b
#define MTYPE    0x1  //001b
#define WRHLDMSB 0x1  //1b
#define RDHLD    0x7  //011b

//0x30C1EA17
#define CE0  ( (WRSETUP<<28) | (WRSTRB<<22) | (WRHLD<<20) | (RDSETUP<<16) | (TA<<14) | (RDSTRB<<8) | (MTYPE <<4) | (WRHLDMSB<<3) | RDHLD )
//0x30C10A19  from webpage
static EMIFB_Config myEmifbCfg={
	0x00080780, 
	CE0, //CE0CTL��
	0x0000041f, //CE1CTL:
	0xFFF20317, //CE2CTL��Rsetup=2��Rstrobe=3��hold=1��16λ�첽
	0xFFFFFF03, //CE3CTL
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000, //CESEC0
	0x00000000, //CESEC1
	0x00000000, //CESEC2
	0x00000000 //CESEC3 
};

bool g_buint = false;
interrupt void PerformExtInt4(void)
{
	//LOG_I("ext_int4_isr\n");
	g_buint = true;
}

extern MCBSP_Handle hMcbsp0;		//handle for McBSP0
extern EDMA_Handle hEdma12;			//handle for EDMA 12
extern EDMA_Handle hEdma13;			//handle for EDMA 13	
interrupt void uartRcvInt(void);
//
//Global Variables
volatile int transmit_done = TRUE;
u8 xmit_msg[64] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09};
u8 recv_msg[64] = {0}; 
/*******************************************************************/
/* EDMA Data Transfer Completion ISRs                              */
/*******************************************************************/


typedef struct _UartFiller{
	UartBuffer data;
	u32 watcher;
}UartFiller;

UartFiller gUartDblBuffer[2];
volatile u32        gUartReceiving = 0;
volatile u32        gUartBuffReady = 0;
interrupt void uartRcvInt(void){
	if(EDMA_intTest(13))
	{
	    UART_Store(&gUartDblBuffer[gUartReceiving].data);
		EDMA_intClear(13);
	}
}

void onMSecond(u32 ticks){
	 UartFiller* pReady;
	 UartFiller* pBusy = &gUartDblBuffer[gUartReceiving];
     if(pBusy->watcher == pBusy->data.length){
		if(pBusy->watcher != 0){
			pReady = &gUartDblBuffer[1 - gUartReceiving];
			if(pReady->watcher == 0){
				gUartBuffReady ++;
			}
		}
	 }
	 else{
	 	pBusy->watcher = pBusy->data.length;
	}
}

extern far void vectors();

void main(void)
{
    u32 i;
	u16 reg0;
	u32 dataSend = 1;
	UartFiller* pUartBuffer;

	gUartDblBuffer[0].watcher = 0;
	gUartDblBuffer[0].data.length = 0;
	gUartDblBuffer[1].watcher = 0;
	gUartDblBuffer[1].data.length = 0;
	gUartReceiving = 0;
	
	CSL_init(); 
	IRQ_setVecs(vectors);
	IRQ_resetAll();
	Event_Init();
	Status_Init();
	EMIFB_config(&myEmifbCfg);

	Timer_Setup();
	Timer_Config(1,onMSecond,0,0);
	IRQ_globalEnable(); 

	UART_init(1);

	BU_Initialize();
	//BU interrupt for dsp
	IRQ_map(IRQ_EVT_EXTINT4,12);
	IRQ_reset(IRQ_EVT_EXTINT4);
	IRQ_enable(IRQ_EVT_EXTINT4);

	BU_enableIrq();

	BU_SetRspVectorWord(0x4321);
    BU_SetRspSync(0x5678);
	
	UART_sendBytes("start 1553b\n",12);
	LOG_I("entering loop\n");
	for(;;)
	{
		if(g_buint){
			g_buint = false;
			reg0 = BU_RegGetVal(BU_INTSTATUS);
			LOG_P("Int Status0x%x:\n",reg0);

#define Trigger(reg0,bitIndex,handler,msg) if(BIT(bitIndex) & reg0){printf(#msg##":");handler();}
			//Trigger(reg0,15,handleNull, RESERVED);
			Trigger(reg0,14,handleNull, RAM PARITY ERROR);
			Trigger(reg0,13,handleNull, BC/RT TRANSMITTER TIMEOUT);
			Trigger(reg0,12,handleNull, BC/RT COMMAND STACK ROLLOVER);
			Trigger(reg0,11,handleNull, MT COMMAND STACK ROLLOVER);
			Trigger(reg0,10,handleNull, MT DATA STACK ROLLOVER);
			Trigger(reg0,9, handleNull, HANDSHAKE FAILURE);
			Trigger(reg0,8, handleNull, BC RETRY);
			Trigger(reg0,7, handleNull, RT ADDRESS PARITY ERROR);
			Trigger(reg0,6, handleNull, TIME TAG ROLLOVER);
			Trigger(reg0,5, handleNull, RT CIRCULAR BUFFER ROLLOVER);
			Trigger(reg0,4, handleTranEOM, BC MSG/RT SUBADDRESS CONTROL WORD EOM);
			Trigger(reg0,3, handleNull, BC END OF FRAME);
			Trigger(reg0,2, handleNull, FORMAT ERROR);
			Trigger(reg0,1, handleNull, BC STATUS SET/RT MODE CODE/MT PATTERN TRIGGER);
			Trigger(reg0,0, handleRecvEOM, END OF MESSAGE);
		}
		if(gUartBuffReady){
		    i = gUartBuffReady;
		    gUartBuffReady = 0;
			gUartReceiving = 1 - gUartReceiving;
			pUartBuffer = &gUartDblBuffer[1-gUartReceiving];
			dataSend = pUartBuffer->data.length > ARRAY_SIZE(recv_msg) ? ARRAY_SIZE(recv_msg) : pUartBuffer->data.length;
			UART_ProcessReceiveData(recv_msg,dataSend,&pUartBuffer->data);
			pUartBuffer->data.length = 0;
			pUartBuffer->watcher = 0;
			BU_SendData(recv_msg,dataSend);
			//LOG_I("recv %d count %d\r\n",i,dataSend);
			//i = BU_GetStatusRegister();
			BU_SetServiceRequest(true); //combined with vector word
		}
	}
}
//�ӵ�ַ��14
//ģ��������Ϣ����һ�ιر�һ��

//test
//:apptag(11) subtype(2)   type(1)   version(3)                   
//:5 - 15       4            3        0 - 2      
//:pkg name or seq numb(14)          seqTag(2)   
//: 2   -    15                       0,1       
//:data
//:8n-bit





//��ʶ��
//0000 1110 0100 1000B      0x0E48
//�����п���(������)
//0000 0000 0000 0011B      0x0003
//����
//0000 0000 0000 0000B      0x0002
//����
//1111 0000 1111 0000B      0xF0F0
/*
FFFF200000000000000000AAAA
FFFF200100000000000000AAAA
FFFF203000000000000000AAAA
FFFF30123456789ABCDEF0AAAA
FFFF31123456789ABCDEF0AAAA
FFFF32123456789ABCDEF0AAAA
FFFF33123456789ABCDEF0AAAA
FFFF34123456789ABCDEF0AAAA
FFFFE0123456789ABCDEF0AAAA
FFFFE1123456789ABCDEF0AAAA
FFFFE2123456789ABCDEF0AAAA
FFFF99123456789ABCDEF0AAAA
FFFF9A123456789ABCDEF0AAAA
*/