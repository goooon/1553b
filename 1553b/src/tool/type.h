#ifndef API_TYPE_H
#define API_TYPE_H
#include <stdio.h> 
#include <stdlib.h>
//for C6000
/*
 char 8 bits 
 short 16 bits 
 int 32 bits 
 float 32 bits
 long 40 bits
 long long 64 bits
 double 64 bits*/
typedef signed int      sint;
typedef unsigned int    uint;

typedef signed char     s8;
typedef unsigned char   u8;
typedef short           s16;
typedef unsigned short  u16;
typedef signed int      s32;
typedef unsigned int    u32;
typedef long            s40;
typedef unsigned long   u40;
typedef long long       s64;
typedef unsigned long long u64;

typedef double          f64;
typedef float           f32;


typedef s8              sbyte;
typedef u8              ubyte;
typedef s16             sword;
typedef u16             uword;
typedef s32             snumb;
typedef u32             unumb;
typedef long            slong; //40bit
typedef unsigned long   ulong;
typedef f32             realf;
typedef f64             reald;


typedef char			b32;
#ifndef bool
#define bool char
#endif
#ifndef false
#define false 0
#endif
#ifndef true
#define true 1
#endif
typedef char            achar;
typedef u32             result;
typedef const achar     cachar;
//typedef const wchar     cwchar;
typedef const achar*    castr;
//typedef const wchar*    cwstr;
typedef u32             color;
#define vTrue  1
#define vFalse 0
// class hex {
// 	u32 v;
// 	hex(u32 u):v(u){}
// 	hex(s32 s):v(s){}
// 	operator u32() { return v; }
// 	operator u32&() { return v; }
// 	
// };

#define ARRAY_SIZE(a) sizeof(a)/sizeof(a[0])
#define BIT(n) (1<<(n))
#endif
