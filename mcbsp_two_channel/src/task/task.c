#include "./task.h"
#include "../BU61580/BU61580.h"
#include "../task/status.h"
#include "../uart/uart.h"
/*
TABLE 50. RT MODE BLOCK STATUS WORD
BIT DESCRIPTION
15(MSB) EOM
14 SOM
13 CHANNEL B/A*
12 ERROR FLAG
11 RT-RT FORMAT ERROR
10 FORMAT ERROR
9 NO RESPONSE TIMEOUT
8 LOOP TEST FAIL
7 CIRCULAR BUFFER ROLLOVER
6 ILLEGAL COMMAND WORD
5 WORD COUNT ERROR
4 INCORRECT DATA SYNC
3 INVALID WORD
2 RT-RT GAP/SYNCH/ADDRESS ERROR
1 RT-RT 2ND COMMAND ERROR
0(LSB) COMMAND WORD CONTENTS ERROR
*/
#define EOM 0x8000
#define SOM 0x4000
#define CHANNEL 0x2000
static u16 g_vectorWord = 0;
typedef struct _SubAddrDesc{
    u16 stackAddr;
	u16 recvBase;
	u16 sendBase;
	u16 bcstBase;
}SubAddrDesc;
static SubAddrDesc subAddrDesc[2] = {
    {STACK_A_ADDR,ARECV_BUFF_BASE,ASEND_BUFF_BASE,ABCST_BUFF_BASE},
	{STACK_B_ADDR,BRECV_BUFF_BASE,BSEND_BUFF_BASE,BBCST_BUFF_BASE},
};

#define BlockDesc_IsEOM(blk) (0x8000 & blk.status)
#define BlockDesc_IsSOM(blk) (0x4000 & blk.status)
#define BlockDesc_GetAddr(blk) ((blk.recvCommand >> 11) & 0x1F) //31 is broadcast address
#define BlockDesc_GetSubAddr(blk)  ((blk.recvCommand >> 5) & 0x1F) //0 or 31 is for mode code switch
#define BlockDesc_GetDataLen(blk)  (((blk.recvCommand) & 0x1F) == 0 ? 32 : ((blk.recvCommand) & 0x1F)) //data count 0 is 32 bytes;
#define BlockDesc_GetModeCode(blk) ((blk.recvCommand) & 0x1F) //mode code 			
																		
#define BlockDesc_IsReqRTTran(blk) (0x400 & blk.recvCommand) //T/R*
#define BlockDesc_IsReqRTRecv(blk) (!(BlockDesc_IsReqRTTran(blk)))

//note: T/R*:1  mode code:10000b   send vector word
//      T/R*:0  mode code:10001b   sync

void BU_SendOne(u32 subAddr);
void BU_SetBusy(bool isBcst,bool isTransmit,u8 subAddr);
typedef struct _BlockDesc{
    u16 status;
	u16 timeTag;
	u16 dataBlockPtr;
	u16 recvCommand;
}BlockDesc;
void handleNull(){
	printf("not handled\n");
}
void handleTranEOM(){
	printf("handleTranEOM() need reload\n");
}
static u16 g_rtReceived[32];
static u16 g_rtSending[32];
static u16 g_rt0Sending[32];

void BU_Send20(u8* data,u32 size){
   u8 rsp;
   if(data[3] == 0){
   		LOG_I("0\n");
		g_rt0Sending[0] = 0;
		RSP_UART(&data[3],1);
	}
   else if(data[3] == 1){
        LOG_I("%d\n",data[4]);
        //LOG_I("0x%2x\n",data[4]);
		RSP_UART(&data[4],1);
		g_rt0Sending[0] = data[4];
	}
	else {
	    LOG_I("107\n");
		g_rt0Sending[0] = 107;
		rsp = 107;
		RSP_UART(&rsp,1);
	}
}
void BU_Send3xa(u8* data,u32 size){
   u8 status = 0;
   g_rt0Sending[0] = 0;
   LOG_I("%d\n",status);
   RSP_UART(&status,1);
   LOG_I("0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x\n",
       data[3],data[4],data[5],data[6],data[7],data[8],data[9],data[10]);
   RSP_UART(&data[3],8);
   LOG_I("0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x\n",
       data[3],data[4],data[5],data[6],data[7],data[8],data[9],data[10]);
   RSP_UART(&data[3],8);
   LOG_I("0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x\n",
       data[3],data[4],data[5],data[6],data[7],data[8],data[9],data[10]);
   RSP_UART(&data[3],8);
}
void BU_Send3xb(u8* data,u32 size){
   u8 status = 0;
   g_rt0Sending[0] = 0;
   LOG_I("%d\n",status);
   RSP_UART(&status,1);
   LOG_I("0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x\n",
       data[3],data[4],data[5],data[6],data[7],data[8],data[9],data[10]);
   RSP_UART(&data[3],8);
   LOG_I("0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x\n",
       data[3],data[4],data[5],data[6],data[7],data[8],data[9],data[10]);
   RSP_UART(&data[3],8);
   LOG_I("0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x\n",
       data[3],data[4],data[5],data[6],data[7],data[8],data[9],data[10]);
   RSP_UART(&data[3],8);
   LOG_I("0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x,0x%2x\n",
       data[3],data[4],data[5],data[6],data[7],data[8],data[9],data[10]);
   RSP_UART(&data[3],8);
}
void BU_SendEx9(u8* data,u32 size){
    u8 status = 0;
	g_rt0Sending[0] = data[2];
    LOG_I("%d\n",data[2]);
	RSP_UART(&data[2],1);
}
void BU_SendData(u8* data,u32 size){
    int i = 0;
	u16 d = 0;
	u8 status = 0;
	int displayByteMode = false;//guzhihe: display for BU word or byte mode: true false
	BU_SetRspVectorWord(data[2]);
	

	if(displayByteMode){
		for(i = 0; i < 32; ++i){
	    	g_rtSending[i] = data[2 + i];
		}
	}
	else{
		for(i = 0; i < (size + 1) / 2; ++i){
			g_rtSending[i] = (data[2 + 2 * i] << 8) | data[2 + 2 * i + 1];
		}
	}
	if(size != 13){
	       status = 108;
		   g_rt0Sending[0] = 108;
		   LOG_I("%d\n",status);
		   RSP_UART(&status,1);
		   return;
    }
	if(data[0] != 0xFF || data[1] != 0xFF || data[11] != 0xAA || data[12] != 0xAA){
	       status = 109;
		   g_rt0Sending[0] = 109;
		   LOG_I("%d\n",status);
		   RSP_UART(&status,1);
		   return;
	}
    //BU_SendData(data,size);
	switch(data[2]){
	    case 0x20:
		BU_Send20(data,size);
		break;
		case 0x30:
		case 0x31:
		case 0x32:
		BU_Send3xa(data,size);
		break;
		case 0x33:
		case 0x34:
		BU_Send3xb(data,size);
		break;
		case 0xE0:
		case 0xE1:
		case 0xE2:
		case 0x99:
		case 0x9A:
		BU_SendEx9(data,size);
		break;
		default:
		   status = 107;
		   g_rt0Sending[0] = 107;
		   LOG_I("%d\n",status);
		   RSP_UART(&status,1);
		break;
	};
}
void BU_SetRspVectorWord(u16 val){
	BU_MemSetVal(0x0110 + 0x10,val);
	g_vectorWord = val;
}
struct CtlData{
   unsigned int ver : 3;
   unsigned int type : 1;
   unsigned int subtype : 1;
   unsigned int appTag : 11;
   unsigned int seqTag : 2;
   unsigned int pkgIdx  : 14;
   unsigned int length : 16;
   unsigned char data[1];
};
#define STATUS_SUCC 0
#define STATUS_ERROR_LENGTH 101
#define STATUS_ERROR_VERSION 102
#define STATUS_ERROR_TYPE 103
#define STATUS_ERROR_SUBTYPE 104
#define STATUS_ERROR_APPTAG 105
#define STATUS_ERROR_PKGIDX 106
#define STATUS_ERROR_CMD_TYPE 107
#define STATUS_ERROR_CMD_LENGHT 108
#define STATUS_ERROR_CMD_CODE 109
#define STATUS_ERROR_CMD_SWITCH 110
#define STATUS_ERROR_CMD_MOTOR 111
#define STATUS_ERROR_CMD_CAMERA 112
#define STATUS_ERROR_CMD_HOT 113
#define STATUS_ERROR_CMD_DATA 114
#define STATUS_ERROR_CMD_MEMORY 119
#define STATUS_ERROR_CMD_INJECT 120
struct ACKData{
	unsigned int ackStatus : 8; //STATUS_ERROR_
	unsigned int ctlPkgCnt : 16;
	unsigned int ctlPkgIdx : 16;
	unsigned int ctlPkgSuccCnt : 8;
};
void handleRecvEOM(){
    u16 val,i;
	u16 stackptr;
	u16 buffBase;
	u16 cfg1;
	BlockDesc blockDesc;
	SubAddrDesc* pSubAddrDesc;
	stackptr = BU_RegGetVal(BU_CMDSTACKPOINT);
	cfg1 = BU_RegGetVal(BU_CFG1);
	val = cfg1 & 0x2000;
	Status_SetStackPointer(val, stackptr);
	pSubAddrDesc = val ? &subAddrDesc[STACK_B] : &subAddrDesc[STACK_A];
	
	blockDesc.status      = BU_MemGetVal(stackptr + 0);
	blockDesc.timeTag     = BU_MemGetVal(stackptr + 1);
	blockDesc.dataBlockPtr= BU_MemGetVal(stackptr + 2);
	blockDesc.recvCommand = BU_MemGetVal(stackptr + 3);
	buffBase = BlockDesc_IsReqRTTran(blockDesc) ? pSubAddrDesc->sendBase : pSubAddrDesc->recvBase;

	LOG_P(" Block:Time(0x%04x) %s %s:%s addr[%d:%d] ",
		blockDesc.timeTag,
		BlockDesc_IsEOM(blockDesc) ? "EOM":"",
		BlockDesc_IsSOM(blockDesc) ? "SOM":"",
		BlockDesc_IsReqRTTran(blockDesc) ? "Tran":"Recv",
		BlockDesc_GetAddr(blockDesc),
		BlockDesc_GetSubAddr(blockDesc));
	
	val = BlockDesc_GetSubAddr(blockDesc);
	if(val == 0 || val == 0x31){ //for mode code
		i = BlockDesc_GetModeCode(blockDesc);
		val = BU_MemGetVal(blockDesc.dataBlockPtr);
		LOG_P("%s modeCode(0x%x) dataWord(0x%x) => ",BlockDesc_IsReqRTTran(blockDesc) ? "T":"R",i,val);
	    if(BlockDesc_IsReqRTTran(blockDesc)){
			if(0x10 == i){
				LOG_P("curr vector word(0x%x)[1]:0x%x\n",i,g_vectorWord);
				//todo
				UART_SetSendMode(true);
			    UART_transmitBytes((u8*)"Recevie vector word 0x%x\n",g_vectorWord);
				Timer_DelayUS(10);
			    UART_SetSendMode(false);
			}
		}
		else {
			if(0x11 == i){
				LOG_P("sync(0x%x)[1]:0x%x",i,val);
				//todo
			    UART_sendString((u8*)"Recevie sync word\n");
			}
		}
	}
	else{
		LOG_P(" Mem(0x%x-0x%x)[%d]:",
			blockDesc.dataBlockPtr,
			buffBase + ((blockDesc.dataBlockPtr + BlockDesc_GetDataLen(blockDesc) - 1) - buffBase) % BUFF_SIZE,
			BlockDesc_GetDataLen(blockDesc));
		if(BlockDesc_IsReqRTRecv(blockDesc)){
			for(i = 0;i < BlockDesc_GetDataLen(blockDesc); ++ i){
			    val = ((blockDesc.dataBlockPtr + i) - buffBase) % BUFF_SIZE;
				val = BU_MemGetVal(buffBase + val);
				g_rtReceived[i] = (val >> 8) | (val << 8);
				LOG_P("%02x ",val);
			}
			UART_SetSendMode(true);
			UART_transmitBytes((u8*)g_rtReceived,2 * BlockDesc_GetDataLen(blockDesc));
			Timer_DelayUS(10);
			UART_SetSendMode(false);
		}
		else{
			if(val == 1){
				for(i = 0;i < BlockDesc_GetDataLen(blockDesc); ++ i){
			    	val = buffBase + ((blockDesc.dataBlockPtr + BlockDesc_GetDataLen(blockDesc) + i) - buffBase) % BUFF_SIZE;
			    	BU_MemSetVal(val,g_rt0Sending[0]);
					LOG_P("%04x ",g_rt0Sending[0]);
				}
			}
			else{

		    	for(i = 0;i < BlockDesc_GetDataLen(blockDesc); ++ i){
			    	val = buffBase + ((blockDesc.dataBlockPtr + BlockDesc_GetDataLen(blockDesc) + i) - buffBase) % BUFF_SIZE;
			    	BU_MemSetVal(val,g_rtSending[i]);
					LOG_P("%04x ",g_rtSending[i]);
				}
			}
		}
	}
	LOG_P("\r\n");
	if(val == 23){
	}
	else if(val == 20){
	}
	else if(val == 21){
	}
	else if(val == 22){
	}
	else{
	}
}
