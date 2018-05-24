#include "./delay.h"
static void delay(unsigned int delay)
{
    volatile unsigned int i, n;
    
    n = 0;
    for (i = 0; i < delay; i++)
    {
        n = n + 1;
    }
}
void delayusec(unsigned int d)
{
    delay(d * 21);
}
void swap(unsigned char* x, unsigned char* y) 
{
    unsigned char xx = *x;
	unsigned char yy = *y;
	*y = xx;
	*x = yy;
    //asm("swap %1, %0" : "+r" xx,"+r" yy);
}

unsigned char printf_buffer[50];
#define MOD(x, y) ((x) - (y) * ((unsigned long)(x) / (unsigned long)(y)))



void number(unsigned char* dst, unsigned long *offset, 
            long num, int base)
{
    long remain, i, len;
    unsigned long start;
    unsigned long end;
	unsigned char alphabet[] = {
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
    };
    if (num < 0)
    {
        num = -num;
        dst[(*offset)++] = '-';
    }
    
    start = *offset;
    while (num >= 0)
    {
        dst[(*offset)++] = alphabet[MOD(num, base)];
        remain = num / base;
        num = remain;
        if (num == 0)
            break;
    }
    end = *offset - 1;
    len = ((end - start) + 1) / 2;
    
    for (i = 0; i < len; i++)
        swap(dst + start + i, dst + end -i);
}

int doNothing(const char* fmt,...){
	/*char abString[64];
	va_list pArg;
    va_start(pArg,fmt);
    vsprintf(abString,fmt,pArg);
    UART_sendString(abString);
    va_end(pArg);*/
    return 0;
}
