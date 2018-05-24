/****************************************************************************/
/*  C6455.cmd                                                               */
/*  Copyright (c) 2010 Texas Instruments Incorporated                       */
/*													                                     						*/
/*    Description: This file is a sample linker command file that can be    */
/*                 used for linking programs built with the C compiler and  */
/*                 running the resulting .out file on an C6455              */
/*                 device.  Use it as a guideline.  You will want to        */
/*                 change the memory layout to match your specific C6xxx    */
/*                 target system.  You may want to change the allocation    */
/*                 scheme according to the size of your program.            */
/*                                                                          */
/****************************************************************************/
-c
-heap  0x1000
-stack 0x2000

-l rts6400.lib    /* C and C++ run-time library support */
-l csl6416.lib    /* 如果设置CSL函数库路径*/

MEMORY
{
    BOOTRAM:       o = 00000000h    l = 00000400h  
    IRAM:          o = 00000400h    l = 000FFC00h	  /*Internal SRAM*/  
	SDRAM :        o = 80000000h    l = 00FFFFFFh   /*EMIFA_CE0,4M*32bit SDRAM*/
		    
    /*EMIFB CE1 , FLASH 16M*8bit*/						
    /*FLASH_BOOT:  o = 64000000h   l = 00000400h*/
    /*FLASH_REST:  o = 64000400h   l = 000ffc00h*/		/* 1M BYTE FLASH */

}


SECTIONS
{
    .bootload      >  BOOTRAM
    .vector        >  IRAM
    .text          >  IRAM
    .cinit         >  IRAM
    .const         >  IRAM
    .stack         >  IRAM  
    .bss           >  IRAM
    .data          >  IRAM
    .far           >  IRAM
    .switch        >  IRAM
    .sysmem        >  IRAM
    .cio           >  IRAM
   
        
}



    