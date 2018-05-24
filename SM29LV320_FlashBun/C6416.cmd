
-c
-heap  0x1000
-stack 0x2000

-l rts6400.lib    /* C and C++ run-time library support */
-l csl6416.lib    /* 如果设置CSL函数库路径*/


MEMORY
{
    SRAM	:	origin = 0x00000000,len = 0x000E0000
	FLASH	:	origin = 0x64000000,len = 0x04000000
    Bin   : origin = 0x000E0000,len = 0x00020000
    SDRAM : origin = 0x80000000,len = 0x00FFFFFF   /*EMIFA_CE0,4M*32bit SDRAM*/
}
SECTIONS
{
	.vector		>	SRAM
	.text:isr	>	SRAM
	.text		  >	SRAM
	.cinit	  >	SRAM
	.const	  >	SRAM
	.far		  >	SRAM
	.stack	  >	SRAM
	.cio		  >	SRAM
	.bss		  >	SRAM
	.sysmem	  >	SRAM
	.data		  >	SRAM
    .switch   > SRAM
    .codebuf  > SDRAM
}