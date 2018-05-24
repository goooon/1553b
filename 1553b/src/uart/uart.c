//Chip definition - Please change this accordingly
#define CHIP_6416 1

#include <csl.h>   
#include <csl_mcbsp.h>
#include <csl_edma.h>
#include <csl_gpio.h>
#include <csl_irq.h>  
#include "../tool/type.h"
#include "../uart/uart.h"
#include "../tool/tracer.h"

//Create buffers and aligning them on an L2 cache line boundary.
#pragma DATA_SECTION(xmitbuf,"xmit_buf");
u16 xmitbuf[32*16];	  
#pragma DATA_SECTION(recvbuf,"recv_buf");
u16 recvbuf[0x100];	

GPIO_Handle hGpio;
MCBSP_Handle hMcbsp0;			/* handle for McBSP0  */
EDMA_Handle hEdma12;			/* handle for EDMA 12 */
EDMA_Handle hEdma13;			/* handle for EDMA 13 */	

/**************************************************************/
/* void ConfigEDMAtx(EDMA_Handle hEdma,Uint32 mcbspXmtAddr):  */
/* set up EDMA transmit channel 12 for UART xmit              */
/**************************************************************/
void ConfigEDMAtx(EDMA_Handle hEdma,u32 mcbspXmtAddr,u32 len)
{	
	EDMA_configArgs(hEdma,
		/* OPT Setup */	
		 EDMA_OPT_RMK(
	 		EDMA_OPT_PRI_HIGH,		        
	 		EDMA_OPT_ESIZE_16BIT,	       
    		EDMA_OPT_2DS_NO,		    
    		EDMA_OPT_SUM_INC,		    
    		EDMA_OPT_2DD_NO,		    	
    		EDMA_OPT_DUM_NONE,		    	
    		EDMA_OPT_TCINT_YES,		    	
    		EDMA_OPT_TCC_OF(12),	    
			EDMA_OPT_TCCM_DEFAULT, 	       
			EDMA_OPT_ATCINT_DEFAULT,       
			EDMA_OPT_ATCC_DEFAULT, 	      
			EDMA_OPT_PDTS_DEFAULT, 	       
			EDMA_OPT_PDTD_DEFAULT, 	       
			EDMA_OPT_LINK_NO,		       
    		EDMA_OPT_FS_NO			    
  		),
		
		/* SRC Setup */  		    
		EDMA_SRC_RMK((u32) xmitbuf),     //xmitbuf address
		
		/* CNT Setup */
		EDMA_CNT_RMK(
			EDMA_CNT_FRMCNT_DEFAULT,
			EDMA_CNT_ELECNT_OF(len*10)
		),
		
		/* DST Setup */
		EDMA_DST_RMK(mcbspXmtAddr),  
		
		/* IDX Setup */
  		EDMA_IDX_RMK(0,0),
  		
  		/* RLD Setup */
  		EDMA_RLD_RMK(0,0)
		);
		    
} //End of ConfigEDMAtx

/**************************************************************/
/* void ConfigEDMArx(EDMA_Handle hEdma,Uint32 mcbspRcvAddr):  */
/* set up EDMA receive channel 13 for UART recv               */
/**************************************************************/
void ConfigEDMArx(EDMA_Handle hEdma,u32 mcbspRcvAddr,u32 len)
{
	EDMA_Handle hEdmaTable;
	
	EDMA_configArgs(hEdma,
		
		/* OPT Setup */
    	EDMA_OPT_RMK(
	 		EDMA_OPT_PRI_HIGH,	 	        
	 		EDMA_OPT_ESIZE_16BIT,	        
    		EDMA_OPT_2DS_NO,		    	
    		EDMA_OPT_SUM_NONE,		    
    		EDMA_OPT_2DD_NO,		    
    		EDMA_OPT_DUM_INC,		    
    		EDMA_OPT_TCINT_YES,		    
    		EDMA_OPT_TCC_OF(13),	    	
			EDMA_OPT_TCCM_DEFAULT, 	       
			EDMA_OPT_ATCINT_DEFAULT,        
			EDMA_OPT_ATCC_DEFAULT, 	       
			EDMA_OPT_PDTS_DEFAULT, 	        
			EDMA_OPT_PDTD_DEFAULT, 	        
    		EDMA_OPT_LINK_YES,		    
    		EDMA_OPT_FS_NO			    
  		),
  		
  		/* SRC Setup */
  		EDMA_SRC_RMK(mcbspRcvAddr),   
  		
  		/* CNT Setup */
  		EDMA_CNT_RMK(0, (len * 10)),
  		
  		/* DST Setup */
  		EDMA_DST_RMK((u32) recvbuf),   //recvbuf address
  		
  		/* IDX Setup */
  		EDMA_IDX_RMK(0,0),
  		
  		/* RLD Setup */
  		EDMA_RLD_RMK(0,0)
	);

	//open new EDMA link
	hEdmaTable = EDMA_allocTable(-1);
	
	EDMA_configArgs(hEdmaTable,
		
		/* OPT Setup */
    	EDMA_OPT_RMK(
	 		EDMA_OPT_PRI_HIGH,		    
	 		EDMA_OPT_ESIZE_16BIT,	   
    		EDMA_OPT_2DS_NO,		
    		EDMA_OPT_SUM_NONE,		
    		EDMA_OPT_2DD_NO,			
    		EDMA_OPT_DUM_INC,			
    		EDMA_OPT_TCINT_YES,		
    		EDMA_OPT_TCC_OF(13),	
			EDMA_OPT_TCCM_DEFAULT, 	    
			EDMA_OPT_ATCINT_DEFAULT,   
			EDMA_OPT_ATCC_DEFAULT, 	    
			EDMA_OPT_PDTS_DEFAULT, 	    
			EDMA_OPT_PDTD_DEFAULT, 	    
    		EDMA_OPT_LINK_YES,			
    		EDMA_OPT_FS_NO				
  		),
  		
  		/* SRC Setup */
  		EDMA_SRC_RMK(mcbspRcvAddr),
  		
  		/* CNT Setup */
  		EDMA_CNT_RMK(0, (len * 10)),
  		
  		/* DST Setup */
  		EDMA_DST_RMK((u32) recvbuf),  //recvbuf address
  		
  		/* IDX Setup */
  		EDMA_IDX_RMK(0,0),
  		
  		/* RLD Setup */
  		EDMA_RLD_RMK(0,0)
	);
	//link two EDMA
	EDMA_link(hEdma,hEdmaTable);
	//EDMA loop，because hEdma will be cleared each time
	EDMA_link(hEdmaTable,hEdmaTable);
		    
} //End of ConfigEDMArx
/*************************************************/
/* void ConfigMcBSP(MCBSP_Handle hMcbsp):        */
/* Setup for McBSP Configuration                 */
/*************************************************/
void ConfigMcBSP(MCBSP_Handle hMcbsp)
{

	MCBSP_Config mcbspCfg0 = {
		
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
    			MCBSP_RCR_RWDLEN2_8BIT,		    
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
	
	MCBSP_config(hMcbsp, &mcbspCfg0);
	
}	//end of Config_McBSP

/*******************************************************************/
/* void ProcessTransmitData(u8 *data)                           */
/*                                                                 */
/* This function expands each of the 8-bit ASCII characters in the */
/* transmit string "xmit_msg" into UART transmission 16-bit word   */
/* and place them in the transmit buffer "xmitbuf".  In addition,  */
/* 16-bit Start and 8-bit Stop framing words, respectively, are    */
/* inserted before and after each of the ASCII characters in the   */
/* buffer.                                                         */
/*******************************************************************/
void ProcessTransmitData(EDMA_Handle edma,MCBSP_Handle mcbsp,u8 *data,u32 len)
{
	int		i;
	short	cnt = 1;
	unsigned char 	xmit_char;
	unsigned short	*xmitbufptr;
	
	//point to Transmit Buffer
	xmitbufptr = (unsigned short *)xmitbuf;	
	
	//Process data BYTES in data[] and put in xmit buffer/
	for (i = 0; i < len; i++)
	{     
	    //Get transmit character (one byte) from xmit_msg[] and put in xmit buffer
		xmit_char	=	data[i];
		
		//Process each BYTE of transmit character
		for (cnt = -1; cnt < 9; cnt++)
		{
			if (cnt == -1)
				*xmitbufptr++ 	= 	0x0000;//start
			
		    else if (cnt == 8)
				*xmitbufptr++ 	= 	0xFFFF;//stop
			
			else if (xmit_char & (1 << cnt))
				*xmitbufptr++	=	0xFFFF;
			
			else
				*xmitbufptr++	=	0x0000;
								
		}	//end for cnt	      
		
	}	//end for i
	ConfigEDMAtx(edma,MCBSP_getXmtAddr(mcbsp),len);
	
	while (!MCBSP_xrdy(mcbsp));
	MCBSP_write(mcbsp,0xFFFF);

}	//end ProcessTransmitData


/*******************************************************************/
/* void transmitMsg(u8 *tx_msg)                                 */
/*                                                                 */
/* This function encode the transmit data and config EDMA.         */
/* Then tramsmit 0xffff from mcbsp .It will be create edma event   */
/* to start edma transmit "tx_msg"                                 */
/*******************************************************************/
void UART_transmitBytes(u8 *tx_msg,u32 len)
{	
    //one byte equals 10 shorts start + 8 + stop
	u8* sending = tx_msg;
	#define CHUNK ARRAY_SIZE(xmitbuf) / 10
	while(len >= CHUNK){
		ProcessTransmitData(hEdma12,hMcbsp0,sending, CHUNK);
		while(!EDMA_intTest(12)){}
	    EDMA_intClear(12);
		sending += CHUNK;
		len -= CHUNK;
	}
	if(len){
		ProcessTransmitData(hEdma12,hMcbsp0,sending, len);
		while(!EDMA_intTest(12)){}
	    EDMA_intClear(12);
	}
}
void UART_transmitWords(u16 *tx_msg,u32 len){
	u8* sending = (u8*)tx_msg;
	len <<= 1;
#define CHUNK ARRAY_SIZE(xmitbuf) / 10
	while(len >= CHUNK){
		ProcessTransmitData(hEdma12,hMcbsp0,sending, CHUNK);
		while(!EDMA_intTest(12)){}
	    EDMA_intClear(12);
		sending += CHUNK;
		len -= CHUNK;
	}
	if(len){
		ProcessTransmitData(hEdma12,hMcbsp0,sending, len);
		while(!EDMA_intTest(12)){}
	    EDMA_intClear(12);
	}
}
/*******************************************************************/
/* short VoteLogic(unsigned short)                                 */
/*                                                                 */
/* This function decoded the received character by testing the     */
/* center 4 bits of the baud.  A majority rule is used for the     */
/* decoding.                                                       */
/*******************************************************************/
short VoteLogic(unsigned short value)
{
	short returnvalue;

	switch	((value	>>	6)	& 0x0F)
	{
		case	0:
		case	1:			
		case	2:	
		case	3:				
		case	4:	
		case	5:	
		case	6:	
		case	8:	
		case	9:												
		case	10:
						returnvalue = 0;
						break;	
		case	7:	
		case	11:							
		case	12:	
		case	13:	
		case	14:							
        case	15:
        				returnvalue = 1 ;
        				break;
	}	//end switch
	
	return (returnvalue);

}	//end VoteLogic     

void UART_Store(UartBuffer* buff)
{
	int i;
	int s = buff->length * 10;
	u16* dst = buff->buffer + s;
	u16* src = recvbuf;
	if(s >= ARRAY_SIZE(buff->buffer)){
	    //buffer is full, ignore
		return;
	}
	for(i = 10;i != 0; --i){
	    *dst = *src;
		dst++;
		src++;
	}
	buff->length ++;
}
/*******************************************************************/
/* void ProcessReceiveData(u8* recv_data)                       */
/*                                                                 */
/* This function decodes the data in the receive buffer, "recvbuf" */
/* and strips the framing start (0x0000) and Stop (0xFFFF) words.  */
/* It calls the subroutine VoteLogic() to determine each bit of    */
/* the ASCII character.  It then puts the result in recv_msg.      */
/*******************************************************************/
void UART_ProcessReceiveData(u8* recv_data,u32 size,UartBuffer* buffer)
{
	int	i;
	u8 recv_char = 0;
	short cnt = -1;
	short recv_val;
	unsigned short	raw_data;
	unsigned short	*recvbufptr;    //receive buffer pointer

	//Point to the receive buffer
	recvbufptr	= (unsigned short *)buffer->buffer;

	//Process all data in the Receive buffer
	for (i = 0; i < size; i++)
	{
		recv_char = 0;
		
		//Process each UART frame
		for (cnt = -1; cnt < 9; cnt++)
		{
			if(cnt == -1 || cnt == 8)
			{
				//Ignore Start and Stop bits
				*recvbufptr++;
			}
			else
			{
				//Get 16-bit data from receive buffer
				raw_data	=	*recvbufptr;
				recvbufptr++;
				
				//get the value of the majority of the bits
				recv_val	=	VoteLogic(raw_data);
				
				//put received bit into proper place
				recv_char	+= recv_val << cnt;
			}	
		}	//end for cnt
	
		//A full BYTE is decoded. Put in result: recv_data[i]
		recv_data[i] = recv_char;
			
	}	//end for i
	
}	//end ProcessReceiveData


void UART_init(u32 recvBuffSize)
{
	//disable and clear the event interrupt
	IRQ_reset(IRQ_EVT_EDMAINT);
	
	//clear Parameter RAM of EDMA 
	EDMA_clearPram(0x00000000);
	
	//Open the EDMA channels - EDMA 12 for transmit, EDMA 13 for receive
	hEdma12 = EDMA_open(EDMA_CHA_XEVT0, EDMA_OPEN_RESET);
	hEdma13 = EDMA_open(EDMA_CHA_REVT0, EDMA_OPEN_RESET);          
	
	//Open the McBSP channel 0
	hMcbsp0 = MCBSP_open(MCBSP_DEV0, MCBSP_OPEN_RESET);
			
	//Configure the EDMA receive channels
	IRQ_disable(IRQ_EVT_DMAINT0);
	IRQ_clear(IRQ_EVT_DMAINT0);
	IRQ_enable(IRQ_EVT_DMAINT0);

	ConfigEDMArx(hEdma13,MCBSP_getRcvAddr(hMcbsp0),recvBuffSize);
	
	//enable EDMA-CPU interrupt tied to McBSP
	IRQ_enable(IRQ_EVT_EDMAINT);
	//enable EDMA channel interrupt to CPU
	EDMA_intEnable(13);
	EDMA_intEnable(12);
	    
	//Enable EDMA channels
	EDMA_enableChannel(hEdma12);
	EDMA_enableChannel(hEdma13);

	//Setup for McBSP
	ConfigMcBSP(hMcbsp0);

	//Start Sample Rate Generator: set /GRST = 1
	MCBSP_enableSrgr(hMcbsp0);
	
	//inserted wait time for McBSP to get ready 
	delayusec(1);
	
	//Wake up the McBSP as transmitter and receiver
	MCBSP_enableRcv(hMcbsp0);
	MCBSP_enableXmt(hMcbsp0);
	
	//Enable Frame Sync Generator for McBSP 0: set /FRST = 1
	MCBSP_enableFsync(hMcbsp0);
	//MCBSP_start(hMcbsp0,MCBSP_XMIT_START|MCBSP_RCV_START|MCBSP_SRGR_START|MCBSP_SRGR_FRAMESYNC,220);
	hGpio = GPIO_open(GPIO_DEV0,GPIO_OPEN_RESET);
	GPIO_reset(hGpio);
	GPIO_pinEnable(hGpio,GPIO_PIN1);
	GPIO_pinDirection(hGpio,GPIO_PIN1,GPIO_OUTPUT);
}
void UART_SetSendMode(bool send){
    GPIO_pinWrite(hGpio,GPIO_PIN1,send ? 1 : 0);
}
void UART_sendString(const char* s){
    int len = 0;
	const char* tmp = s;
	while(*tmp != 0){
	  tmp ++;
	  len++;
	}
    UART_SetSendMode(true);
	UART_transmitBytes((u8*)s,len);
	Timer_DelayUS(10);
	UART_SetSendMode(false);
}

void UART_sendBytes(u8 *tx_msg,u32 len){
    UART_SetSendMode(true);
	UART_transmitBytes(tx_msg,len);
	Timer_DelayUS(10);
	UART_SetSendMode(false);
}
int UART_sendFormat(const char* fmt,...){
    char abString[64];
	va_list pArg;
    va_start(pArg,fmt);
    vsprintf(abString,fmt,pArg);
    UART_sendString(abString);
    va_end(pArg);

    return 0;
}