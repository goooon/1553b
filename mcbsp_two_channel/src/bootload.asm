            .title  "Flash bootup utility for TMS320C6416"
            .option D,T
            .length 102
            .width  140

COPY_TABLE  .equ   0x64000400
EMIF_BASE   .equ   0x01A80000

            .sect ".bootload"
            .global _boot

_boot:      
;************************************************************************
;* Debug Loop -  Comment out B for Normal Operation
;************************************************************************

            zero B1
_myloop:   ;[!B1] B _myloop  
            nop  5
_myloopend: nop

;************************************************************************
;* Configure EMIF
;************************************************************************
        ;CONFIG THE EMIFB_GBLCTL
        MVKL      0x01A80000, A4
      ||MVKL      0x000120BC, B4
        MVKH      0x01A80000, A4
      ||MVKH      0x000120BC, B4
        STW       B4       ,*A4
        
        ;CONFIG THE EMIFB_CE1CTL
        MVKL      0x01A80004, A4
      ||MVKL      0xFFFFFF0F, B4
        MVKH      0x01A80004, A4
      ||MVKH      0xFFFFFF0F, B4
        STW       B4       ,*A4
        nop 4


;****************************************************************************
;* Copy code sections
;****************************************************************************
        mvkl  COPY_TABLE, a3   ; load table pointer
        mvkh  COPY_TABLE, a3

        ldw   *a3++, b1        ; Load entry point

copy_section_top:
        ldw   *a3++, b0        ; byte count 
        ldw   *a3++, a4        ; ram start address
        nop   3

 [!b0]  b copy_done            ; have we copied all sections?
        nop   5

copy_loop:
        ldb   *a3++,b5
        sub   b0,1,b0          ; decrement counter
 [ b0]  b     copy_loop        ; setup branch if not done
 [!b0]  b     copy_section_top
        zero  a1
 [!b0]  and   3,a3,a1
        stb   b5,*a4++
 [!b0]  and   -4,a3,a5         ; round address up to next multiple of 4
 [ a1]  add   4,a5,a3          ; round address up to next multiple of 4

;****************************************************************************
;* Jump to entry point
;****************************************************************************
copy_done:
        b    .S2 b1
        nop   5
