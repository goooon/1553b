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
//EMIFB ????????ú???
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
	CE0, //CE0CTL??
	0x0000041f, //CE1CTL:
	0xFFF20317, //CE2CTL??Rsetup=2??Rstrobe=3??hold=1??16λ??
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
    int i = 0;
	int j = 0;
	
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

#define EDMA_TX_NUMB 17
#define EDMA_RX_NUMB 18
#define EDMA_CHA_XEVTx EDMA_CHA_XEVT2
#define EDMA_CHA_REVTx EDMA_CHA_REVT2

#define MCBSP_DEVx MCBSP_DEV2
#define GPIO_PINx GPIO_PIN3

GPIO_Handle hGpioN;
EDMA_Handle hEdmaX; /* handle for EDMA  */
EDMA_Handle hEdmaR; /* handle for EDMA  */
MCBSP_Handle hMcbsp; /* handle for McBSP */
#define BUFFER_SIZE 0x400
unsigned short tracebuf[BUFFER_SIZE];
unsigned short traceRecvBuf[BUFFER_SIZE];
void ConfigTraceMcBSP(void)
{
MCBSP_Config mcbspCfg1 = {

			/* SPCR Setup */
			MCBSP_SPCR_RMK(	
				MCBSP_SPCR_FREE_YES,		
				MCBSP_SPCR_SOFT_DEFAULT,			
				MCBSP_SPCR_FRST_DEFAULT,		
				MCBSP_SPCR_GRST_DEFAULT,		
				MCBSP_SPCR_XINTM_XRDY,			
				MCBSP_SPCR_XSYNCERR_DEFAULT,	
				MCBSP_SPCR_XRST_DEFAULT,		
				MCBSP_SPCR_DLB_OFF,			    
				MCBSP_SPCR_RJUST_RZF,			
				MCBSP_SPCR_CLKSTP_DISABLE,		
				MCBSP_SPCR_DXENA_OFF,			
				MCBSP_SPCR_RINTM_RRDY,			
				MCBSP_SPCR_RSYNCERR_DEFAULT,	
				MCBSP_SPCR_RRST_DEFAULT			
			),
			/* RCR Setup */	
			MCBSP_RCR_RMK(
  		  		MCBSP_RCR_RPHASE_DUAL,			
 		   		MCBSP_RCR_RFRLEN2_OF(0),		
    			MCBSP_RCR_RWDLEN2_16BIT,		    
    			MCBSP_RCR_RCOMPAND_MSB,		    
    			MCBSP_RCR_RFIG_YES,			    
    			MCBSP_RCR_RDATDLY_0BIT,		    
    			MCBSP_RCR_RFRLEN1_OF(8),	    
    			MCBSP_RCR_RWDLEN1_16BIT,	    
    			MCBSP_RCR_RWDREVRS_DISABLE 	    
    		),
  	
  			/* XCR Setup */
  			MCBSP_XCR_RMK(
            	MCBSP_XCR_XPHASE_DUAL,			
            	MCBSP_XCR_XFRLEN2_OF(0),		
    			MCBSP_XCR_XWDLEN2_16BIT,	
    			MCBSP_XCR_XCOMPAND_MSB,			
    			MCBSP_XCR_XFIG_YES,			    
    			MCBSP_XCR_XDATDLY_0BIT,			
    			MCBSP_XCR_XFRLEN1_OF(8),	 	
    			MCBSP_XCR_XWDLEN1_16BIT,		
				MCBSP_XCR_XWDREVRS_DISABLE		
			),
				
			/* SRGR Setup */    
			MCBSP_SRGR_RMK(
    			MCBSP_SRGR_GSYNC_FREE,			
    			MCBSP_SRGR_CLKSP_RISING,			
    			MCBSP_SRGR_CLKSM_INTERNAL,		
    			MCBSP_SRGR_FSGM_DXR2XSR,		
    			MCBSP_SRGR_FPER_DEFAULT,		
    			MCBSP_SRGR_FWID_DEFAULT,
				//current CPU CLK is 40M * 12 = 480M
    			/************************************************/
    			/* CLKGDV  控制波特率,计算方法如下:             */
    			/* 6416 McBSP的输入时钟CLKmcbsp为CPU CLK的1/4	*/
    			/* CLKGDV = (CLKmcbsp) / (16 * baudrate) -1     */
    			/************************************************/	    
    	 		MCBSP_SRGR_CLKGDV_OF(31)   //115200 --- 64
    	 									//230400 ----- 31/32       	 	
  			),
		
			/* MCR Setup */
  			MCBSP_MCR_DEFAULT,  	

			/* RCER Setup */
			MCBSP_RCERE0_DEFAULT,
			MCBSP_RCERE1_DEFAULT,
			MCBSP_RCERE2_DEFAULT,
			MCBSP_RCERE3_DEFAULT, 
		
			/* XCER Setup */
			MCBSP_XCERE0_DEFAULT,
			MCBSP_XCERE1_DEFAULT,
			MCBSP_XCERE2_DEFAULT,
			MCBSP_XCERE3_DEFAULT, 

		
			/* PCR Setup */
  			MCBSP_PCR_RMK(
    			MCBSP_PCR_XIOEN_SP,				    
    			MCBSP_PCR_RIOEN_SP,				   
    			MCBSP_PCR_FSXM_INTERNAL,			
    			MCBSP_PCR_FSRM_EXTERNAL,		
    			MCBSP_PCR_CLKXM_OUTPUT,				
    			MCBSP_PCR_CLKRM_OUTPUT,			
    			MCBSP_PCR_CLKSSTAT_0,			
    			MCBSP_PCR_DXSTAT_0,				  
    			MCBSP_PCR_FSXP_ACTIVELOW,		
    			MCBSP_PCR_FSRP_ACTIVELOW,		
    			MCBSP_PCR_CLKXP_RISING,			
				MCBSP_PCR_CLKRP_FALLING			
    		)
};
MCBSP_config(hMcbsp, &mcbspCfg1);
} 
void ConfigTraceEDMAR(int len)
{
	EDMA_Handle hEdmaTable;
	EDMA_configArgs(hEdmaR,
		
		/* OPT Setup */
    	EDMA_OPT_RMK(
	 		EDMA_OPT_PRI_HIGH,	 	        
	 		EDMA_OPT_ESIZE_16BIT,	        
    		EDMA_OPT_2DS_NO,		    	
    		EDMA_OPT_SUM_NONE,		    
    		EDMA_OPT_2DD_NO,		    
    		EDMA_OPT_DUM_INC,		    
    		EDMA_OPT_TCINT_YES,		    
    		EDMA_OPT_TCC_OF(EDMA_RX_NUMB),	    	
			EDMA_OPT_TCCM_DEFAULT, 	       
			EDMA_OPT_ATCINT_DEFAULT,        
			EDMA_OPT_ATCC_DEFAULT, 	       
			EDMA_OPT_PDTS_DEFAULT, 	        
			EDMA_OPT_PDTD_DEFAULT, 	        
    		EDMA_OPT_LINK_YES,		    
    		EDMA_OPT_FS_NO			    
  		),
  		
  		/* SRC Setup */
  		EDMA_SRC_RMK(MCBSP_getRcvAddr(hMcbsp)),   
  		
  		/* CNT Setup */
  		EDMA_CNT_RMK(0, (len * 10)),
  		
  		/* DST Setup */
  		EDMA_DST_RMK((u32) traceRecvBuf),   //recvbuf address
  		
  		/* IDX Setup */
  		EDMA_IDX_RMK(0,0),
  		
  		/* RLD Setup */
  		EDMA_RLD_RMK(0,0)
	);

	//open new EDMA link
	hEdmaTable = EDMA_allocTable(-1);
	
	EDMA_configArgs(hEdmaR,
		
		/* OPT Setup */
    	EDMA_OPT_RMK(
	 		EDMA_OPT_PRI_HIGH,		    
	 		EDMA_OPT_ESIZE_16BIT,	   
    		EDMA_OPT_2DS_NO,		
    		EDMA_OPT_SUM_NONE,		
    		EDMA_OPT_2DD_NO,			
    		EDMA_OPT_DUM_INC,			
    		EDMA_OPT_TCINT_YES,		
    		EDMA_OPT_TCC_OF(EDMA_RX_NUMB),	
			EDMA_OPT_TCCM_DEFAULT, 	    
			EDMA_OPT_ATCINT_DEFAULT,   
			EDMA_OPT_ATCC_DEFAULT, 	    
			EDMA_OPT_PDTS_DEFAULT, 	    
			EDMA_OPT_PDTD_DEFAULT, 	    
    		EDMA_OPT_LINK_YES,			
    		EDMA_OPT_FS_NO				
  		),
  		
  		/* SRC Setup */
  		EDMA_SRC_RMK(MCBSP_getRcvAddr(hMcbsp)),
  		
  		/* CNT Setup */
  		EDMA_CNT_RMK(0, (len * 10)),
  		
  		/* DST Setup */
  		EDMA_DST_RMK((u32) traceRecvBuf),  //recvbuf address
  		
  		/* IDX Setup */
  		EDMA_IDX_RMK(0,0),
  		
  		/* RLD Setup */
  		EDMA_RLD_RMK(0,0)
	);
	//link two EDMA
	EDMA_link(hEdmaR,hEdmaTable);
	//EDMA loop，because hEdma will be cleared each time
	EDMA_link(hEdmaTable,hEdmaTable);

}
void ConfigTraceEDMAX(int len)
{
    
	EDMA_configArgs(hEdmaX,
		 EDMA_OPT_RMK(
		 EDMA_OPT_PRI_HIGH, /* 1 */
		 EDMA_OPT_ESIZE_16BIT, /* 01 */
		 EDMA_OPT_2DS_NO, /* 0 */
		 EDMA_OPT_SUM_INC, /* 01 */
		 EDMA_OPT_2DD_NO, /* 0 */
		 EDMA_OPT_DUM_NONE, /* 00 */
		 EDMA_OPT_TCINT_YES, /* 1 */
		 EDMA_OPT_TCC_OF(EDMA_TX_NUMB), /* 14 */
		 EDMA_OPT_TCCM_DEFAULT, /* 0 */
		 EDMA_OPT_ATCINT_DEFAULT,/* 0 */
	 	 EDMA_OPT_ATCC_DEFAULT, /* 0 */
		 EDMA_OPT_PDTS_DEFAULT, /* 0 */
		 EDMA_OPT_PDTD_DEFAULT, /* 0 */
		 EDMA_OPT_LINK_NO, /* 0 */
		 EDMA_OPT_FS_NO /* 0 */
		 ),
		/* SRC Setup */
		EDMA_SRC_RMK((Uint32) tracebuf), /*xmitbuf address*/
		/* CNT Setup */
		EDMA_CNT_RMK(
		    EDMA_CNT_FRMCNT_DEFAULT,
		    EDMA_CNT_ELECNT_OF(len*10)
		),
		/* DST Setup */
		EDMA_DST_RMK(MCBSP_getXmtAddr(hMcbsp)),
		/* IDX Setup */
		EDMA_IDX_RMK(0,0),

		/* RLD Setup */
		EDMA_RLD_RMK(0,0)
	);
	EDMA_link(hEdmaX, EDMA_hNull); 

	
} /* End of ConfigEDMA() */
void ProcessTraceData(unsigned char* buff,int len)
{
	int i;
	short cnt = 1;
	unsigned char xmit_char;
	unsigned short *xmitbufptr;
	/* point to Transmit Buffer */
	xmitbufptr = (unsigned short *)tracebuf;
	/* Process data BYTES in xmit_msg[] and put in xmit buffer */
	for (i = 0; i < len; i++)
	{
		/* Get transmit character (one byte) from xmit_msg[] */
		 /* and put in xmit buffer */
		xmit_char = buff[i];
		/* Process each BYTE of transmit character */
		for (cnt = -1; cnt < 9; cnt++)
		{
			if (cnt == -1)
			*xmitbufptr++ = 0x0000;
			else if (cnt == 8 || cnt ==9)
			*xmitbufptr++ = 0xFFFF;
			else if (xmit_char & (1 << cnt))
			*xmitbufptr++ = 0xFFFF;
			else
			*xmitbufptr++ = 0x0000;

		} /* end for cnt */
	} /* end for i */
} /* end ProcessTransmitDataTest */
void sendDataToUart(unsigned char* buff,int len){

	ProcessTraceData(buff,len);
	ConfigTraceEDMAX(len);
	while(!MCBSP_xrdy(hMcbsp));
	MCBSP_write(hMcbsp,0xFFFF);
	//while(!EDMA_intTest(TX_NUMB)){}
	//EDMA_intClear(EDMA_TX_NUMB);
}

void sendStringToUart(char* str){
    int len = 0;
	char* p = str;
	while(*p ++ != 0)len ++;
	sendDataToUart(str,len);
}

void InitTrace(){
    int waittime  = 0;
	
    /*hGpioN = GPIO_open(GPIO_DEV0,GPIO_OPEN_RESET);
	GPIO_reset(hGpioN);
	GPIO_pinEnable(hGpioN,GPIO_PINx);
	GPIO_pinWrite(hGpioN,GPIO_PINx,1);
	GPIO_pinDirection(hGpioN,GPIO_PINx,GPIO_OUTPUT);
	GPIO_pinWrite(hGpioN,GPIO_PINx,1);*/
	/* process transmit data */
	/* disable and clear the event interrupt */

	/* clear Parameter RAM of EDMA */

	/* Open the EDMA channels ? EDMA 14 for transmit, */
	 /* EDMA 15 for receive */
	hEdmaX = EDMA_open(EDMA_CHA_XEVTx, EDMA_OPEN_RESET);
	hEdmaR = EDMA_open(EDMA_CHA_REVTx, EDMA_OPEN_RESET);

	/* Open the McBSP channel 1 */
	hMcbsp = MCBSP_open(MCBSP_DEVx, MCBSP_OPEN_RESET);
	/* Configure the EDMA channels */
	/* enable EDMA?CPU interrupt tied to McBSP */
	//IRQ_enable(IRQ_EVT_EDMAINT);
	/* enable EDMA channel interrupt to CPU */
	EDMA_intEnable(EDMA_TX_NUMB);
	EDMA_intDisable(EDMA_RX_NUMB);
	/* Enable EDMA channels */
	EDMA_enableChannel(hEdmaX);
	EDMA_enableChannel(hEdmaR);
	/* Setup for McBSP */
	ConfigTraceMcBSP();
    ConfigTraceEDMAR(10);
	/* Start Sample Rate Generator: set /GRST = 1 */
	MCBSP_enableSrgr(hMcbsp);
	for (waittime=0; waittime<0xF; waittime++);
	//Wake up the McBSP as transmitter and receiver
	MCBSP_enableRcv(hMcbsp);
	MCBSP_enableXmt(hMcbsp);
	
	//Enable Frame Sync Generator for McBSP 0: set /FRST = 1
	MCBSP_enableFsync(hMcbsp);
}
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

	/* disable and clear the event interrupt */
	IRQ_reset(IRQ_EVT_EDMAINT);
	/* clear Parameter RAM of EDMA */
	EDMA_clearPram(0x00000000);

	
	UART_init(1);

	InitTrace();


	BU_Initialize();
	//BU interrupt for dsp
	IRQ_map(IRQ_EVT_EXTINT4,12);
	IRQ_reset(IRQ_EVT_EXTINT4);
	IRQ_enable(IRQ_EVT_EXTINT4);

	BU_enableIrq();

	BU_SetRspVectorWord(0x4321);
    BU_SetRspSync(0x5678);
	//while(1)
	{
	   UART_sendBytes("start 1553b\n",12);
	   sendStringToUart("this is the text string\r\n");
	   delayusec(1000);
	}
	LOG_I("entering loop\n");

	for(;;)
	{
		if(g_buint){
			g_buint = false;
			reg0 = BU_RegGetVal(BU_INTSTATUS);
			UART_sendFormat("Int Status0x%x:\n",reg0);

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
		    UART_sendFormat("gUartBuffReady triggered\r\n");
			delayusec(1000);
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
//??????14
//????????????????ι?????

//test
//:apptag(11) subtype(2)   type(1)   version(3)                   
//:5 - 15       4            3        0 - 2      
//:pkg name or seq numb(14)          seqTag(2)   
//: 2   -    15                       0,1       
//:data
//:8n-bit





//?????
//0000 1110 0100 1000B      0x0E48
//?????п???(??????)
//0000 0000 0000 0011B      0x0003
//????
//0000 0000 0000 0000B      0x0002
//????
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