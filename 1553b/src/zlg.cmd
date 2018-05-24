-stack 0x1000
-heap 0x1000

MEMORY
{
    BOOTRAM:       o = 00000000h    l = 00000400h  
    IRAM:          o = 00000400h    l = 000FFC00h
	SDRAM :        o = 80000000h    l = 00FFFFFFh   
}
SECTIONS
{ 
  .bootload   > BOOTRAM
  .intvecs    > IRAM
  .switch     > IRAM
  .off_chip   > IRAM
  .bss        > IRAM
  .far        > IRAM
  .chip_image > IRAM
  .data       > IRAM
  .text       > IRAM
  .cio        > IRAM
  .cinit      > IRAM
  .boot       > IRAM
  .const      > IRAM
  .stack      > IRAM
  .sysmem     > IRAM
  "xmit_buf"  > IRAM
  "recv_buf"  > IRAM
}

/*MEMORY
{  
    IRAM:          o = 00000000h    l = 000FFC00h
	SDRAM :        o = 80000000h    l = 00FFFFFFh   
}
SECTIONS
{ 
  .intvecs    > IRAM
  .switch     > IRAM
  .off_chip   > IRAM
  .bss        > IRAM
  .far        > IRAM
  .chip_image > IRAM
  .data       > IRAM
  .text       > IRAM
  .cio        > IRAM
  .cinit      > IRAM
  .boot       > IRAM
  .const      > IRAM
  .stack      > IRAM
  .sysmem     > IRAM
  "xmit_buf"  > IRAM
  "recv_buf"  > IRAM
}*/