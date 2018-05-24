// 用于配置Bu61580

#include "BU61580.h"
#include "../tool/timer.h"
#include "../tool/tracer.h"



/*
15(MSB) RX: DOUBLE BUFFER ENABLE
14 TX: EOM INT
13 TX: CIRC BUF INT
12 TX: MEMORY MANAGEMENT 2 (MM2)
11 TX: MEMORY MANAGEMENT 1 (MM1)
10 TX: MEMORY MANAGEMENT 0 (MM0)
9 RX: EOM INT
8 RX: CIRC BUF INT
7 RX: MEMORY MANAGEMENT 2 (MM2)
6 RX: MEMORY MANAGEMENT 1 (MM1)
5 RX: MEMORY MANAGEMENT 0 (MM0)
4 BCST: EOM INT
3 BCST: CIRC BUF INT
2 BCST: MEMORY MANAGEMENT 2 (MM2)
1 BCST: MEMORY MANAGEMENT 1 (MM1)
0(LSB) BCST: MEMORY MANAGEMENT 0 (MM0)
*/

#define ASUB_ADDR_CFG_BASE 0x1a0
#define BSUB_ADDR_CFG_BASE 0x220
//subaddress_buffer_scheme.bmp
#if BUFF_SIZE == 0x200
#define BU_RTSUBADDRCTRL_VAL 0xEF7B
#define ASUB_ADDR_CFG_VAL 0x0C63 //NO DBL BUFFER, 512 SIZE
#define BSUB_ADDR_CFG_VAL 0x0C63 //NO DBL BUFFER, 512 SIZE
#else //0x100
#define BU_RTSUBADDRCTRL_VAL 0xEB5A
#define ASUB_ADDR_CFG_VAL 0x0842 //NO DBL BUFFER, 256 SIZE
#define BSUB_ADDR_CFG_VAL 0x0842 //NO DBL BUFFER, 256 SIZE
#endif



// 设置内部16个寄存器
void BU_RegSetVal(Uint16 Addr,Uint16 Val )
{
  *(volatile Uint16 *)(BU_REG+(Addr<<SHIFT_POS)) =  Val;
  Timer_DelayUS(4);       //delay 4us
}

//设置内部存储器取值
void BU_MemSetVal(Uint16 Addr,Uint16 Val )
{
  *(volatile Uint16 *)(BU_MEM+(Addr<<MEM_SHIFT)) = Val;
  Timer_DelayUS(4);       //delay 4us
}

//读取寄存器值，零等待模式，读取两次返回正确值
Uint16 BU_RegGetVal(Uint16 Addr)
{
  Uint32 Val=0x00;
  Val=*((volatile  Uint16 *)(BU_REG+(Addr<<SHIFT_POS)));;    // 零等待模式，读取两次返回正确值
  Timer_DelayUS(4);                          //延时4us
  Val=*((volatile  Uint32 *)(BU_REG+(Addr<<SHIFT_POS)));    
  Timer_DelayUS(4); 
  Val = (Val>>16)&0XFFFF; 
  return Val;
}

// 读取内存值，零等待模式，读取两次返回正确值
Uint16 BU_MemGetVal(Uint16 Addr)
{
  Uint32 Val=0x00;
  
  Val=*((Uint16 *)(BU_MEM+(Addr<<MEM_SHIFT)));    // 零等待模式，读取两次返回正确值
  Timer_DelayUS(4);                              //延时4us
  Val=*((Uint32 *)(BU_MEM+(Addr<<MEM_SHIFT)));    
  Timer_DelayUS(4); 
  Val = (Val >> 16)&0XFFFF; 
  return Val;
}

// 读取内存值，连续读取
Uint16 BU_MemGetValOne(Uint16 Addr)
{
  Uint16 Val=0x00;
  
  Val=*((Uint16 *)(BU_MEM+(Addr<<MEM_SHIFT)));    // 循环读取，本次读取返回上次的取值
  Timer_DelayUS(4);                           //延时4us
  
  Val = Val&0XFFFF; 
  return Val;
}

Uint16 BU_GetStatusRegister()
{
	return BU_RegGetVal(BU_RTSTATUSWORD);
}
void BU_SetServiceRequest(bool request)
{
	BU_RegSetVal(BU_CFG1, 0x8D80);
}
#define ASEND_CMD_BASE 0x160



//基于BU61580的MILSTD1553B总线设计.pdf
//acehg.pdf
b32 BU_Initialize(){
    u32 i = 0;
	//printf("BU_Initialize\n");
	BU_RegSetVal(BU_RST,5);//R3:reset(0) and interrupt reset(2) ,page 31, page 42
	BU_RegSetVal(BU_CFG3,0x8000);//R7:#3启动增强模式(15)
	
	BU_MemSetVal(0x100,STACK_A_ADDR); //初始化堆栈A指针位置 	page 42
	BU_MemSetVal(0x104,STACK_B_ADDR); //初始化堆栈B指针位置   page 42
	for(i = 0; i < 0xff; i ++){
		BU_MemSetVal(STACK_A_ADDR + i, 0);//初始化堆栈A,清零
		BU_MemSetVal(STACK_B_ADDR + i, 0);//初始化堆栈B,清零
	}
	for(i = 0; i < 32; i ++){//初始化lookup table
		BU_MemSetVal(0x140 + i, ARECV_BUFF_BASE);//区域A收子地址0-31均存放于0x0400起始的地址内
		BU_MemSetVal(0x160 + i, ASEND_BUFF_BASE);//区域A送子地址0-31均存放于0x0800起始的地址内
		BU_MemSetVal(0x180 + i, ABCST_BUFF_BASE);//区域A收广播子地址0-31均存放于0x0c00起始的地址内 
		BU_MemSetVal(0x1a0 + i, ASUB_ADDR_CFG_VAL);//区域A子地址0-31缓冲区均设为256 
		BU_MemSetVal(0x1c0 + i, BRECV_BUFF_BASE);//区域B收子地址0-31均存放于0x0600起始的地址内 
		BU_MemSetVal(0x1e0 + i, BSEND_BUFF_BASE);//区域B送子地址0-31均存放于0x0a00起始的地址内
		BU_MemSetVal(0x200 + i, BBCST_BUFF_BASE);//区域B收广播子地址0-31均存放于0x0e00起始的地址内
		BU_MemSetVal(0x220 + i, BSUB_ADDR_CFG_VAL);//区域B子地址0-31缓冲区均设为256
	}
	BU_RegSetVal(BU_RTSUBADDRCTRL,BU_RTSUBADDRCTRL_VAL); //R4 设置缓冲区大小
	//step13 Busy bit lookup table
	for(i = 0; i < 8 ; i++)      // All Busy bit lookup table set 'no busy' 8  
	{
	    BU_MemSetVal(0x240 + i, 0); 
	}
	for(i = 0; i < 8; i++){
		BU_MemSetVal(0x108 + i, 0xFFFF);
	}
	//step14 Command illegalization Table,  未定义的指令为非法， P157,Table80
    for(i = 0;i<256;i++)                // 0x300-0x3ff非法
    {
        BU_MemSetVal(0x300 + i, 0xFFFF); //初始化先将所有指令置为非法
    }
	#define BUG_CFG2_VAL 0xBa97
	BU_RegSetVal(BU_CFG2, BUG_CFG2_VAL);//R2:  (0xB817/
					//Enhanced interrupts(bit15=1)          
					//RAM Parity Enable(bit14=0)
					//BUSY LOOK UP TABLE ENABLE(bit13=1)
					//RX SA DOUBLE BUFFER ENABLE(bit12=1)              
					
					//OVERWRITE INVALID DATA(bit11=1)
					//256-WORD BOUNDER Disable(bit10=0) 
					//TTR2 (bit9) //TTR1(bit8)              
					
					//TTR0 (bit7)                                 //CLEAR TIME TAG ON SYNCHRONIZE(bit6=0 NOT )
					//LOAD TIME TAG ON SYNCHRONIZE(bit5=0 NOT)    //INTERRUPT STATUS AUTO CLEAR(bit4=1)
					
					//InteruptType:(Level bit3=1;Pluse bit3=0)   //CLEAR SERVICE REQ.(bit2=1)
					//ENHANCED RT MEMORY MANG.(bit1=1)           //SEPARATE BROADCAST DATA(bit0=1)
	BU_RegSetVal(BU_CFG3, 0x8089);//R7 (0x805D)
                 //ENHANCE MODE ENABLE(bit15=1)                  
                 
                 //ILLEGALIZATION DISABLE(bit7)             //OVERRIDE MODE T/R ERROR(bit6=1)
                 //Alternate RT STATUS WORD enable(bit5=0) //ILLEGAL RX TRANSFER Disable(bit4=1)
                 
                 //BUSY RX TRANSFER DISABLE(bit3 =1)    //RTFAIL-FLAG WRAP ENABLE(bit2 =1)
                 //1553A MODE CODES ENABLE(bit2 = 0)    //ENHANCED MODE CODE HANDLING(bit0=1)
	BU_RegSetVal(BU_CFG4, 0x1000); //R8 (0X2100)
					//Mode Command Override Busy(bit13)
					//Expand BC Control Word Enable(bit12)
					//1ST Retry Alt/Same Bus* (bit8)
	BU_RegSetVal(BU_CFG5, 0x0854);//R9 (0x088e)
					// 16MHz(bit15=0)       
					
					// EXPANDED CROSSING ENABLED(bit11=1)
					
					// BroadCast Disabled(bit7) //RT Addr latch/Transspant(bit6)
					// RT Address4(bit5) //RT Address3(bit4)

					// RT Address2(bit3) //RT Address1(bit2)
                    // RT Address0(bit1)    // Program RT Address to 7(parity bit =0)
	BU_RegSetVal(BU_RTSTATUSWORD, 0x0000);//Re 
							
	BU_RegSetVal(BU_CFG1, 0x8F80); //R1: Enhanced RT Mode,  (0x8D80/0x8F80)
					//Active Area B/A (bit13),
					//s10~s8 (bit11~8=1111)
					//s6(bit7)
	BU_RegSetVal(BU_TIMETAG, 0); //R5
	BU_RegSetVal(BU_INTSTATUS,0); //R6
	#define BU_INTMASK_VAL 0x72B7
	BU_RegSetVal(BU_INTMASK,0);//R0:初始化中断屏蔽寄存器
							//EndOfMsg(0),RTModeCode(1),FmtErr(2),
							//SubAddr(4),RTBuffRollOver(5),TimeTagRollOver(6),RTAddrParityErr(7)
							//HandShake(9),
							//RTCmdSackRollOver(12),RTTransmitTimeout(13),RAMParityErrr(14)
	//IRQ_map(IRQ_EVT_EXTINT4,12);
	//IRQ_reset(IRQ_EVT_EXTINT4);
	//IRQ_disable(IRQ_EVT_EXTINT4);
	
	return vTrue;
}

void BU_disableIrq(){
	BU_RegSetVal(BU_INTMASK,0);
	//IRQ_disable(IRQ_EVT_EXTINT4);
}
void BU_enableIrq(){
	BU_RegSetVal(BU_INTMASK,BU_INTMASK_VAL);
	//IRQ_enable(IRQ_EVT_EXTINT4);
}

#define RTADDR 10
#define A_SEND(subAddr,len)  BU_MemSetVal(ASEND_CMD_BASE + subAddr, (RTADDR << 11) | (1 << 10) | (subAddr<<5)|len)

void BU_SetBusy(bool isBcst,bool isTransmit,u8 subAddr){
	u16 busyAddr = 0x240 | (isBcst ? 4 : 0) | (isTransmit ? 2 : 0) | ((subAddr & 0x10) ? 1 : 0);
	//u16 bitMask = 1 << (subAddr & 0xF);
	BU_MemSetVal(busyAddr,0);
}

void BU_SendOne(u32 subAddr){
	printf("send\r\n");
	Timer_DelayUS(4);
	BU_MemSetVal(ASEND_BUFF_BASE,0x1234);
	A_SEND(subAddr,1);
	Timer_DelayUS(1000000);
}

void BU_SetRspSync(u16 val){
	BU_MemSetVal(0x0110 + 0x11,val);
}
void BU_EnableModeInterupt(ModeType type,u16 modeCode){
    u16 idx = modeCode & 0x1F;
	u16 offset = type << 1 + idx >> 4;
	u16 val = BU_MemGetVal(0x108 + offset);
	BU_MemSetVal(0x108 + offset, val | (1 << idx));
}
void BU_DisableModeInterupt(ModeType type,u16 modeCode){
	u16 idx = modeCode & 0x1F;
	u16 offset = type << 1 + idx >> 4;
	u16 val = BU_MemGetVal(0x108 + offset);
	BU_MemSetVal(0x108 + offset, val & (~(1 << idx)));
}

    //0100H是堆栈指针A的地址；
	//0104H是堆栈指针B呢址。
	//0000-00FFH是256字的堆栈A空间，也是1553B接收信息描述符区，每描述符占4个字，一共可储存64个描述符。
				//描述符的4个字分别是块状态字、时间戳，接收数据块指针、接收命令字。     
	//0108-010FH是模式命令选择中断表，设置每一个接收、发送、广播接收、广播发送的模式命令时，是否产生中断。
		//0x108:interrupts for receive mode codes 0-15
		//0x109:interrupts for receive mode codes 16-31
		//0x10a:interrupts for transmit mode codes 0-15
		//0x10b:interrupts for transmit mode codes 16-31
		//0x10c:interrupts for broadcast receive mode codes 0-15
		//0x10d:interrupts for broadcast receive mode codes 16-31
		//0x10e:interrupts for broadcast transmit mode codes 0-15
		//0x10f:interrupts for broadcast transmit mode codes 16-31
	//0110-013FH是模式命令返回数据的固定地址，当RT接收到一个模式命令时，将储存于对应位置的数据自动返回给BC。
	//0140-01BFH和01C0-023FH分别是RT的A／B区域地址查询表。对于A区域，地址分为4部分：
		//0140-015FH分别对应子地址0到子地址31的接收数据缓冲区的首地址；
		//0160-017FH分别对应子地址0到子地址31的发送数据缓冲区的首地址；
		//0180-019FH分别对应子地址0到子地址31广播模式数据缓冲区的首地址；
		//01A0一01BFH分别对应子地址0到子地址31的子地址控制字，可以设置数据缓存区的大小和工作方式，
	             //如：单消息模式、双缓冲模式和循环缓冲模式。
	//0240-0247H是忙位查询表，其中每一位分别对应子地址0到子地址31的忙状态，0表示不忙，1表示忙。
	            //当RT收到的消息对应的子地址为忙时，将向BC端返回状态字的忙位置1。
	//0300-03FFH是非法化指令查询表，分为4部分：
		//0300-033FH对应广播接收非法化命令；
		//0340-037FH对应广播发送非法化命令；
		//0380-03BFH对应非广播式接收非法化命令；
		//03C0-03FFH对应非广播式发送非法化命令。
		//当RT收到非法化命令字时，将向BC返回对应的状态字。      
	//0260-02FFH和0400-0FFFH为数据缓存区。
				//这部分空间可以由子地址0到子地址31的子地址控制字设置每个子地址对应的数据缓存区空间的大小和工作方式。

//dguidehg.pdf  1-12

//1553A MODE CODES ENABLED = 0 : 
   //both subaddresses 0 and 31 to be mode code subaddresses
   //RT recognizes and responds to all MIL-STD-1553B mode codes, including those with or without, Data Words.
   //RT will decode for the MIL-STD-1553B "Transmit Status" and "Transmit Last Command" mode codes 
   //and will not update its internal RT Status Word Register as a result of
   //these commands, with the exception of setting the Message Error bit if the command is illegalized

//CLEAR SERVICE REQUEST: If this bit is logic "0," the Service Request RT Status Word bit may
	//controlled only by the host processor software. If the bit is logic "1," the Service Request bit may still be
	//set and cleared under software (register) control. In addition, the SERVICE REQUEST* Configuration
	//Register (#1) bit will automatically clear (go to logic 1) after the ACE RT has responded to a Transmit
	//Vector Word mode code command. That is, if the CLEAR SERVICE REQUEST bit is set to 1 while
	//SERVICE REQUEST* is set to 0, the ACE RT will respond with the Service Request Status bit set for all
	//commands until the RT responds to a Transmit Vector Word command. In this instance, the ACE will
	//respond with the Service Request still set in the Status Word for this message. Following this message,
	//SERVICE REQUEST* in the Configuration Register automatically clears to a logic "1." It stays logic "1"
	//(cleared) for subsequent messages until it is reasserted to a logic "0" by the host processor.

//ENHANCED MODE CODE HANDLING (bit 0 of Configuration Register #3) is not used, the pointers for receive subaddresses 0 and 31 (for
	//Synchronize with Data messages) generally get loaded with the same pointer value.
	//Similarly, the Lookup Table addresses for transmit subaddresses 0 and 31 (for Transmit Vector
	//Word messages) generally get loaded with the same pointer value.
	//If ENHANCED MODE CODE HANDLING is enabled. Data Words for these mode codes
	//are stored in locations 0111 (for Synchronize with data) and 0130 (for Transmit Vector Word).
    //Data Words for mode codes are stored in address locations 0110-013F, and
	//interrupt requests for individual mode codes may be enabled by means of a table in address locations 0108-010F. 
	//The desired mode code may be selected by setting the appropriate bit in the Mode Code
	//Selective Interrupt Table (address range 0108-010F).). When the specific mode code message
	//has been completed, the RT MODE CODE bit of the Interrupt Status Register will return logic "1".


	//BU-61580发送数据时，将对应的子地址的数据字送到初始化设定的查询表地址中，
	//同时将该子地址对应的矢量字置为“1”，表明RT的数据已有更新。
	//因为子地址控制字位设置了发送和接收产生中断，所以在中断处理程序中，
	//当命令字与发送子地址相同时，表明BU-61580已将需要发送的数据发送到1553B网络中，
	//此时，在中断服务程序中将该发送子地址的矢量字清“0”。
	//BC周期性的查询矢量字，只有对应子地址的矢量字为“1”，BC才从RT中将数据取走。

	//由于RT不具有发起总线消息的权限,形如RT→BC、RT→RT以RT作为数据源的消息格式的组织,即RT上传数据的过
	//程相比接收数据较为复杂。总线控制器组织传输的消息分为周期性消息和非周期性消息。
	//对RT子系统周期性上传的数据主处理器首先将数据填充至BU-61580内相应子地址发送数据缓冲区,
	//设置发送数据包个数,并将对应的子地址"发送忙"位清零等待BC取走数据。当对应的子地址产生发送中断时,
	//表明数据已完成上传,主处理器在中断处理程序中将发送数据包个数减1并在个数为零时将"发送忙"位重新置"1"
	//用于告诉BC当前时刻RT子系统没有更新数据。
	//对于非周期性消息,BC在原有周期性消息的间歇增加要求RT发送矢量字的模式命令。
	//如BC以低于10Hz的频率向RT发送询问指令,被查询的RT若有事件请求就将矢量字置位,
	//BC根据RT发送的矢量字来组织消息处理该事件。RT在判断事件请求被响应后则将矢量字复位。

//Busy bit lookup table
  //bit address:  15 14 13 12 11 10  9  8  7  6  5  4  3      2                    1    0
  //binary value:  0  0  0  0  0  0  1  0  0  1  0  0  0  BROADCAST/OWN ADDRESS*  T/R*  SA4   //0x240 ~ 0x247

  //C language u32 subAddrBusyFlag = (((u32)*(u16*)(0x240 | BROADCAST/OWN ADDRESS*<<2 | T/R*<<1 + 1)) << 16 ) | (*(u16*)(0x240 | BROADCAST/OWN ADDRESS*<<2 | T/R*<<1))

  //bit address :  15    14    13   12   11   10   9    8    7    6    5    4    3    2    1    0      
  //suaddr 0 ~ 3:  SA15 SA14 SA13 SA12 SA11 SA10 SA9  SA8  SA7  SA6  SA5  SA4  SA3  SA2  SA1  SA0      //0X240 for OWN ADDRESS*, R*, SA4(0)
  //suaddr 0 ~ 3:  SA31 SA30 SA29 SA28 SA27 SA26 SA25 SA24 SA23 SA22 SA21 SA20 SA19 SA18 SA17 SA16     //0X240 for OWN ADDRESS*, R*, SA4(1)


  //TRANSMIT VECTOR WORD (T/R* = 1; 10000)
	//MESSAGE SEQUENCE = TRANSMIT VECTOR WORD----STATUS/VECTOR WORD
		//The ACE transmits a Status Word followed by a Vector Word. If the ENHANCED MODE CODES
		//are enabled (bit 15 in Configuration Register #3 set to logic "1") the contents of the Vector Word
		//are obtained from RAM location 120 (hex). If ENHANCED MODE CODES are not enabled, the
		//single word data block in the shared RAM that is referenced by the lookup table pointer for transmit
		//subaddress 00000 or 11111.
		//ERROR CONDITIONS
			//1. Invalid Command. No response, command ignored.
			//2. Correct Command Followed by Data Word. No Status response. Set Message Error bit
				//(Status Word), High Word Count (BIT Word).
			//3. T/R* bit set to zero, no Data Word. No Status response. Set Message Error bit (Status
				//Word), and Low Word Count (BIT Word).
			//4. T/R* bit set to zero plus one Data Word. The ACE will respond with Status. The Data
				//Word will be stored in RAM location 0110 (or single-word data block for subaddress 0000 or 1111).
			//5. Zero T/R* bit and Broadcast Address, no Data Word. No Status response. Set Message
				//Error and Broadcast Command Received bits (Status Word), and Low Word Count (BIT word).
			//6. Zero T/R* bit and Broadcast Address, plus one Data Word. No Status response. Set
				//Broadcast Command Received bits (Status Word).
			//7. Broadcast Address. No Status response. Set Message Error and Broadcast Command
				//Received bits (Status Word), Command Word Contents Error (BIT word).

//data_buffer_management.bmp
//1553B_message.jpg
//memory map : page 230

//RT PSEUDO CODE EXAMPLE
