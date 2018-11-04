#ifndef BU61580_DEF_
#define BU61580_DEF_

#include "tistdtypes.h"
#include "../tool/type.h"
#define BU_REG                           0x60000000      //BU-61580 Regs base-address(=EMIFB CE1 of DSP6416)
#define BU_MEM                           0x60008000      //BU-61580 Mems base-address(EA15=1 of DSP6416)  MEM/REG_N->BU61580's Pin5

#define BU_INTMASK                       0x00
#define BU_CFG1                          0x01
#define BU_CFG2                          0x02
#define BU_RST                           0x03
#define BU_CMDSTACKPOINT                 0x03
#define BU_RTSUBADDRCTRL                 0x04
#define BU_TIMETAG                       0x05
#define BU_INTSTATUS                     0x06
#define BU_CFG3                          0x07
#define BU_CFG4                          0x08
#define BU_CFG5                          0x09
#define BU_RTDATASTACKADDR               0x0a
#define BU_BCFRAMETIME                   0x0b
#define BU_BCTIMEREMAIN                  0x0c
#define BU_RTLASTCMD                     0x0d
#define BU_RTSTATUSWORD                  0x0e
#define BU_RTBITWORD                     0x0f
#define BU_TSTMODE0                      0x10
#define BU_TSTMODE1                      0x11
#define BU_TSTMODE2                      0x12
#define BU_TSTMODE3                      0x13
#define BU_TSTMODE4                      0x14
#define BU_TSTMODE5                      0x15
#define BU_TSTMODE6                      0x16
#define BU_TSTMODE7                      0x17

//#define BU_INTMASK_VAL                   0x0016
#define BU_CFG1_VAL                      0x8F80 // Enhanced RT Mode, Active Area A(bit15~12=1000)
#define BU_CFG2_VAL                      0xB817
#define BU_RST_VAL                       0x0001
#define BU_CFG3_VAL                      0x805D
#define BU_CFG4_VAL                      0x2100
#define BU_CFG5_VAL                      0x088E

#define BUMA_STACK_POINTER               0x0100   //RT Command Stack Pointer A 


#define SYNC_MODE_CODE                   0x0111
///////////////////
//RT look up table     
//Channel A Receive lookup pointer table  
#define BUMA_RX_SA0                       0x140   //Channel a Receive lookup pointer table -- subaddress Rx/bcst sa0~sa31: 0x140~0x15F
#define BUMA_RX_SA1                       0x141   //Channel a Receive lookup pointer table -- subaddress Rx/bcst sa1
#define BUMA_RX_SA20                      0x154   //Channel a Receive lookup pointer table -- subaddress Rx/bcst sa20

// Channel A transmit lookup pointer table -
#define BUMA_TX_SA0                       0x160   //Channel a transmit lookup pointer table -- subaddress Tx sa0~sa31    : 0x160~0x17F
#define BUMA_TX_SA1                       0x161   //Channel a transmit lookup pointer table -- subaddress Tx sa1
#define BUMA_TX_SA2                       0x162   //Channel a transmit lookup pointer table -- subaddress Tx sa2
#define BUMA_TX_SA3                       0x163   //Channel a transmit lookup pointer table -- subaddress Tx sa3
#define BUMA_TX_SA5                       0x165   //Channel a transmit lookup pointer table -- subaddress Tx sa5
#define BUMA_TX_SA14                      0x16E   //Channel a transmit lookup pointer table -- subaddress Tx sa14
#define BUMA_TX_SA15                      0x16F   //Channel a transmit lookup pointer table -- subaddress Tx sa15
#define BUMA_TX_SA20                      0x174   //Channel a transmit lookup pointer table -- subaddress Tx sa20
#define BUMA_TX_SA21                      0x175   //Channel a transmit lookup pointer table -- subaddress Tx sa21

//Channel A Broadcast lookup pointer table -- subaddress Bcst sa0
// #define BUMA_BCST_SA0                     0x180   //Channel a Broadcast lookup pointer table -- subaddress Bcst sa0~sa31    : 0x180~0x19F

///////////////////
//Subaddress Control Word
#define BUMA_SACW_SA0                     0x1A0   //Channel a subaddress control word -- subaddress Sacw sa0~sa31         : 0x1A0~0x1BF 
#define BUMA_SACW_SA1                     0x1A1   //Channel a subaddress control word -- subaddress Sacw sa1
#define BUMA_SACW_SA2                     0x1A2   //Channel a subaddress control word -- subaddress Sacw sa2
#define BUMA_SACW_SA3                     0x1A3   //Channel a subaddress control word -- subaddress Sacw sa3 
#define BUMA_SACW_SA5                     0x1A5   //Channel a subaddress control word -- subaddress Sacw sa5
#define BUMA_SACW_SA14                    0x1AE   //Channel a subaddress control word -- subaddress Sacw sa14
#define BUMA_SACW_SA15                    0x1AF   //Channel a subaddress control word -- subaddress Sacw sa15
#define BUMA_SACW_SA20                    0x1B4   //Channel a subaddress control word -- subaddress Sacw sa20
#define BUMA_SACW_SA21                    0x1B5   //Channel a subaddress control word -- subaddress Sacw sa21

//Busy bit lookup table,addr:0x240-0x247,共128bit,对应当前区的子地址控制字,0x248-0x25f not used.
#define BUSY_RX_SA0_15                    0x240  // SA0~SA15 接收忙
#define BUSY_TX_SA0_15                    0x242  // SA0~SA15 发送忙

#define BUSY_RX_SA16_31                   0x241  // SA16~SA31 接收忙
#define BUSY_TX_SA16_31                   0x243  // SA16~SA31 发送忙


// 接收和发送数据的DataBlock地址
// Receive Data Block
#define BUMA_RXBLOCK_SA1                  0x0800  // Rx/SA1 Data Block
#define BUMA_RXBLOCK_SA20                 0x0900  // Rx/SA1 Data Block

// Send Data Block
#define BUMA_TXBLOCK_SA1_PING             0x0540  // Tx/SA1 Data Block  Single Message, Ping Data Block
#define BUMA_TXBLOCK_SA1_PONG             0x0640  // Tx/SA1 Data Block  Single Message, Pong Data Block

#define BUMA_TXBLOCK_SA2                  0x0560  // Tx/SA2 Data Block
#define BUMA_TXBLOCK_SA3                  0x0740  // Tx/SA2 Data Block

#define BUMA_TXBLOCK_SA5_PING             0x0620  // Tx/SA5 Data Block
#define BUMA_TXBLOCK_SA5_PONG             0x0660  // Tx/SA5 Data Block

#define BUMA_TXBLOCK_SA14                 0x0580  // Tx/SA14 Data Block
#define BUMA_TXBLOCK_SA15                 0x0600  // Tx/SA15 Data Block

#define BUMA_TXBLOCK_SA20                 0x0700  // Tx/SA20 Data Block
#define BUMA_TXBLOCK_SA21                 0x0720  // Tx/SA21 Data Block


#define SHIFT_POS 1
#define MEM_SHIFT 1
#define BUFF_SIZE 0x100 //0x200
#define BSEND_BUFF_BASE 0x0400
#define BRECV_BUFF_BASE 0x0600
#define ASEND_BUFF_BASE 0x0800
#define ARECV_BUFF_BASE 0x0A00
#define ABCST_BUFF_BASE 0x0C00
#define BBCST_BUFF_BASE 0x0e00

#define STACK_A 0
#define STACK_B 1

#define STACK_A_ADDR 0x000
#define STACK_B_ADDR 0xf00

void   BU_RegSetVal(Uint16 Addr,Uint16 Val );
void   BU_MemSetVal(Uint16 Addr,Uint16 Val );
Uint16 BU_RegGetVal(Uint16 Addr);
Uint16 BU_MemGetVal(Uint16 Addr);
Uint16 BU_GetStatusRegister();
b32  BU_Initialize();
void BU_disableIrq();
void BU_enableIrq();
void BU_parseMessage();
void BU_SetRspVectorWord(u16 val);
void BU_SetRspSync(u16 val);
typedef enum _ModeType{
   SubAddrRecv = 0,
   SubAddrTran = 1,
   BrdcastRecv = 2,
   BrdcastTran = 3
}ModeType;
void BU_EnableModeInterupt(ModeType type,u16 modeCode);
void BU_DisableModeInterupt(ModeType type,u16 modeCode);
void BU_SendData(u8* data,u32 size);
void BU_SetServiceRequest(bool request);
#endif
