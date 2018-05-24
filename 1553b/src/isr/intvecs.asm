
;PUSH 
push    .macro    reg 
        stw    reg,*B15--[1] 
        .endm 

;POP 
pop    .macro    reg,delay 
        ldw    *++B15[1],reg 
        .if    delay>0 
        nop    delay 
        .endif 
        .endm 
        
        


	.sect	".intvecs"
	.align	32*8*4				; must be aligned on 256 word boundary
	.def _vectors
    .ref	_PerformIsr0
    .ref	_PerformIsr1
    .ref	_PerformIsr2
	.ref    _PerformExtInt4
	.ref    _uartRcvInt
    .def    rst
_vectors:			; Reset
rst:
	.ref	_c_int00			; program reset address
	MVKL 	_c_int00, B3
	MVKH	_c_int00, B3
	B		B3
	MVC 	PCE1, B0			; get address of interrupt vectors
	MVC 	B0, ISTP			; set table to point here
	NOP 	
	NOP 	
	NOP 	
	
	.align	32
	.def NMI_V
NMI_V:				; Non-maskable interrupt Vector
	B		$
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	
	.align	32
	.def AINT_V
AINT_V:				; Analysis Interrupt Vector (reserved)
	B		$
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	
	.align	32
	.def MSGINT_V
MSGINT_V:			; Message Interrupt Vector (reserved)
	B		$
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	
	.align	32
	.def INT4_V
INT4_V:				; Maskable Interrupt #4
	B		$
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	
	.align	32
	.def INT5_V
INT5_V:				; Maskable Interrupt #5
	B		$
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	
	.align	32
	.def INT6_V
INT6_V:				; Maskable Interrupt #6
	B		$
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	
	.align	32
	.def INT7_V
INT7_V:				; Maskable Interrupt #7
	B		$
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	
	.align	32
	.def INT8_V
INT8_V: ;_uartRcvInt
    push	b0 
    mvkl    .s2     _uartRcvInt,b0
    mvkh    .s2     _uartRcvInt,b0
    b       .s2     b0
    pop		b0, 4
    nop
    nop 	
	
	.align	32
	.def INT9_V
INT9_V:				; Maskable Interrupt #9
	B		$
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	
	.align	32
	.def INT10_V
INT10_V: 			; Maskable Interrupt #10
	B		$
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	
	.align	32
	.def INT11_V
INT11_V: 			; Maskable Interrupt #11
	B		$
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	NOP 	
	
	.align	32
	.def INT12_V
INT12_V: 			; Maskable Interrupt #12
    push	b0 
    mvkl    .s2     _PerformExtInt4,b0
    mvkh    .s2     _PerformExtInt4,b0
    b       .s2     b0
    pop		b0, 4
    nop
    nop
	
	.align	32
	.def INT13_V
INT13_V: 			; Maskable Interrupt #13
int13:
    push	b0 
    mvkl    .s2     _PerformIsr2,b0
    mvkh    .s2     _PerformIsr2,b0
    b       .s2     b0
    pop		b0, 4
    nop
    nop
	
	.align	32
	.def INT14_V
INT14_V: 			; Maskable Interrupt #14
int14:
    push	b0 
    mvkl    .s2     _PerformIsr0,b0
    mvkh    .s2     _PerformIsr0,b0
    b       .s2     b0
    pop		b0, 4
    nop
    nop
	.align	32
	.def INT15_V
INT15_V: 			; Maskable Interrupt #15 timerIsr
int15:
    push	b0 
    mvkl    .s2     _PerformIsr1,b0
    mvkh    .s2     _PerformIsr1,b0
    b       .s2     b0
    pop		b0, 4
    nop
    nop
	
; the remainder of the vector table is reserved
	.end
