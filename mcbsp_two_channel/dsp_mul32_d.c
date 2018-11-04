/* ======================================================================== */
/*  TEXAS INSTRUMENTS, INC.                                                 */
/*                                                                          */
/*  NAME                                                                    */
/*      DSP_mul32_d.c -- Example file for DSP_mul32                         */
/*                                                                          */
/*  USAGE                                                                   */
/*      This file contains code for demonstrating the C64x DSPLIB           */
/*      DSP_mul32 function. All inputs to the function contain random       */
/*      data. The assembly version of DSP_mul32 is called and the output    */
/*      is compared with the reference C code using mem_compare() in        */
/*      support.c.                                                          */
/* ------------------------------------------------------------------------ */
/*          Copyright (C) 2003 Texas Instruments, Incorporated.             */
/*                          All Rights Reserved.                            */
/* ======================================================================== */
    
#include <stdio.h> 
#include <stdlib.h>

/* Header file for the memory compare function */
int mem_compare(const void *ptr1, const char *name1, 
                const void *ptr2, const char *name2, int len);

/* Header file for the DSPLIB function */
#include "DSP_mul32.h"    

/* Header file for the C function */
void DSP_mul32_c(const int * x, const int * y, int * r, short nx);

/* ======================================================================== */
/*  DSPLIB function-specific alignments. Refer to the                       */
/*  TMS320C64x DSP Library Programmer's Reference for details.              */
/* ======================================================================== */
#pragma DATA_ALIGN(x, 8);
#pragma DATA_ALIGN(y, 8);
#pragma DATA_ALIGN(r_asm, 8);
#pragma DATA_ALIGN(r_c, 8);

/* ======================================================================== */
/*  Constant dataset.                                                       */
/* ======================================================================== */
#define N    (256)


/* ======================================================================== */
/*  Initialize arrays with random test data.                                */
/* ======================================================================== */
int     test = 0;
const int  x[N] = 
{
    -0x28B1B34E,  0x6D7C99E9, -0x39A4E5D0,  0x1D8BCB1A,
     0x13D76F79,  0x1B0EDA61, -0x0F83242D,  0x0786B87B,
     0x7DEE2C04,  0x3590197F,  0x79100CAF,  0x7BE83CAE,
    -0x787BFE03, -0x03993C6F,  0x6D4D5779, -0x6D06ADD7,
    -0x273FFD38,  0x479C2A7E, -0x57E95F6A, -0x14061A86,
     0x3D350A35, -0x4AD01E69,  0x2DC5BCC8, -0x47817966,
    -0x6178F4FA, -0x66A80DBB, -0x01845AD6,  0x7E4AD81E,
    -0x2591E7AF, -0x058E56C8,  0x20283497,  0x075930AC,
    -0x744E2FF3,  0x4C126E1C,  0x4A39B855, -0x23DF0ABE,
     0x329BEE68,  0x55E6F008, -0x48C3ED15,  0x06FD328D,
    -0x6C2FA97B,  0x7F1DE157, -0x0CA70264,  0x18525C64,
    -0x249B0094, -0x7D4BC146, -0x3E37CC2B,  0x6BAFFF06,
    -0x688416C6, -0x5695E52D, -0x3F6BB4A3, -0x07747E49,
    -0x15103693,  0x07D0408C,  0x16DEA370, -0x3B99DBBD,
     0x44F432E6,  0x72535190, -0x510C5EE8, -0x51D0E3A2,
     0x09B275EB, -0x057892EA, -0x262FBD96,  0x4D4DC111,
     0x4337F58E,  0x48E6F2F4, -0x08D3908F, -0x3EC43E88,
    -0x1BC40F54,  0x046A7D96, -0x20A1B423, -0x3445E83A,
     0x10A3DA37,  0x2058602C,  0x20D5CBAD, -0x7D3FC74A,
     0x121F3242,  0x03CF73E4, -0x61837AF8,  0x04B0BCAC,
     0x7A7C1154, -0x54C45A30, -0x2FC58DF1,  0x72C63BDD,
    -0x625937A3, -0x559A10ED,  0x6B041184, -0x5470F862,
     0x07E60CEA,  0x74E508E4,  0x79EDF452, -0x1252DD83,
     0x7F84B830, -0x50123015,  0x1FD3BF2E, -0x3C4E8E2C,
     0x27E68F21, -0x03F040CE,  0x6A7F6308, -0x2A8C5DBD,
     0x5B2AE669, -0x3B1387C8,  0x381C06F1,  0x0A859317,
    -0x7FF79ADD, -0x2A0ED221, -0x60B7A9B9,  0x309D49F1,
     0x2BA69A97, -0x428E2EBA,  0x7818D9F2,  0x67D1498C,
     0x481C8CFF,  0x4B73D914, -0x3831D4A8,  0x4ED5D981,
     0x7C345398, -0x56C08252,  0x76F6C36B,  0x58A05619,
     0x1B6E49E0, -0x44334C5F,  0x5D35EFA6, -0x413FF6B0,
     0x51C55D26, -0x65700603,  0x2A98E7A4, -0x2165772B,
     0x7344B102,  0x4CA73F61,  0x42EA8435,  0x1CEE602B,
    -0x2F347D51,  0x1B5E19AB,  0x7E95BA18, -0x0DAD089B,
     0x787B5DB1, -0x0F85E29C,  0x4E2E1C3B, -0x0AB2CF08,
     0x56840E97,  0x15B5A74A, -0x09E5622E,  0x44887E36,
    -0x62DD54EC,  0x0BD1A0C9,  0x7F0A7E78, -0x1C0A679B,
    -0x55180BC8, -0x1B54C33A,  0x3D77C35E, -0x2CE205A2,
     0x25CCB2C6,  0x2C834876, -0x2B10092F, -0x605351C8,
    -0x5E05D4B0,  0x04CFDB13, -0x228CE071,  0x14FC0EA7,
     0x68562FD6,  0x69879BEC, -0x2B48A846,  0x05249363,
    -0x5CE76BEE, -0x693F7445, -0x3BC87678,  0x671A99C3,
    -0x68A5D97A, -0x0B43BE67, -0x4E801173, -0x37A8BA77,
     0x21368FAE,  0x4503FE3F,  0x2D218601,  0x0B2CF1BE,
     0x466694F6,  0x5D522B70,  0x4CC4CAD8,  0x65446BD7,
     0x56AF300C, -0x0020DCA4, -0x6E233A1D,  0x33C924F6,
     0x2F708FF5, -0x302B9C84,  0x1B2C640F, -0x523EB1E0,
     0x4E776654, -0x2A010B0F, -0x5216655D, -0x70F7977C,
     0x2D945F5A,  0x437E47C0, -0x4165D848, -0x702BB30C,
     0x77F6BF19,  0x25AD9A58,  0x441C6DBB,  0x4DF4B0B0,
    -0x6B943A64,  0x519D1ABF,  0x43B02A6A, -0x1F3114F3,
    -0x322B8BC8, -0x0AD50632,  0x5CCB8BB3, -0x376DD2AB,
    -0x3D56B80F, -0x3E19BE80, -0x57E82A86,  0x64F9ABBF,
     0x0C4141F0, -0x25C42458, -0x3C1AC305, -0x1B6DE4BC,
    -0x0F4A6342, -0x7BB4F0CE, -0x42FDCCDA, -0x1F67CDB4,
     0x042C1F57, -0x75BD0A22,  0x52D9BFBC, -0x41DB2861,
     0x173B8A1E, -0x066A50FC, -0x3D42F487, -0x3E5C9A2A,
    -0x353698E8,  0x4C6FC1C9,  0x17F24B5E,  0x5EECA8D8,
    -0x2788ADFA, -0x4DC069DF,  0x458DEBE6, -0x4766CB76,
     0x3A910734,  0x393C57AA, -0x4E3F1AC1,  0x1B88107A,
     0x780E9DF5,  0x225FEF4B,  0x6EEEEBC0, -0x34765419,
    -0x4A4A9F40,  0x105E7C05,  0x784C8D6B, -0x4C8946D3,
    -0x2111E304, -0x6DF26B0D, -0x3F392398,  0x7274E481,
     0x247F31A2, -0x1E8A525F, -0x3FF81327,  0x6905465C
};

const int  y[N] = 
{
    -0x1D6B6D29, -0x412B75AF, -0x7A8BB928, -0x3114FF80,
    -0x6C479621, -0x6CCDC782,  0x50DE71A7,  0x1B196EDB,
     0x2766C0EA,  0x68BB4D6D,  0x7C68BBE7,  0x6EF94523,
    -0x1264048E,  0x7F8F71F8, -0x6EA15F8A, -0x0B5949A1,
     0x12BEB819, -0x12D3EB4C,  0x2A8AA812, -0x25C3470E,
    -0x1CE1F591, -0x056D7B3F,  0x2E9FFBBE,  0x38E5251D,
    -0x7BC788F4, -0x736741B2,  0x13C9F32E, -0x20E496FF,
     0x14C86101, -0x5D863188, -0x227D0C98, -0x589A060C,
     0x589450BD, -0x42F1BCFE, -0x6BD27145, -0x73D15794,
     0x4D3B14D7, -0x3B3790FF, -0x061CF570,  0x4668349A,
    -0x2D9A85DD,  0x6EFC7A32,  0x6A0E40CE,  0x0BE70EA9,
    -0x4030AC36, -0x1D7DA402, -0x1B7DC483,  0x76087EE2,
    -0x72C59688,  0x59555630,  0x0372D1D1, -0x5153FB73,
    -0x30672E3F, -0x36F88DB0, -0x2B2946A1,  0x071C45BD,
    -0x486A0939,  0x10E62DD0, -0x4DCFF9B0,  0x0CCC0CD8,
     0x5BE51D6B,  0x1D82FE98, -0x788C08AA,  0x4FD48CF4,
    -0x1C8ADB83,  0x74311409,  0x4D0CA92B, -0x5E3862F1,
     0x0E4BF0F9,  0x7085747B,  0x7C47F482,  0x6EAD3589,
    -0x0565AADE, -0x7DB5C4E9,  0x75186359,  0x27B3BFCB,
    -0x6534DE31,  0x626B8B87, -0x16112E4E, -0x75A14C48,
     0x6CCA0DD5,  0x7042F296, -0x48A225CF, -0x454D0DB0,
    -0x63AED2E3,  0x1B6B9F4C,  0x1D29B26A,  0x3A0868FE,
     0x07D00ADC, -0x1D8BDC45, -0x09ADCCF2,  0x0AB72A06,
     0x2D5F6662, -0x3B2D4669,  0x50C50665,  0x5628D829,
    -0x764EC5C9,  0x019B30D7,  0x0EB3EA88, -0x08CB3535,
     0x12E2BA23,  0x3FABCF62, -0x16807621,  0x126FE683,
     0x5FDA586F, -0x20106FD0, -0x77FB84C5,  0x6D7F8D57,
    -0x2232477D, -0x74BE697C,  0x0BDF501F, -0x562BC3C9,
    -0x5C5BA165,  0x50BCEF3C,  0x27817663, -0x31BDAD24,
     0x510DD70C, -0x61AD401C, -0x1C59915F, -0x39E95125,
     0x07AEB624, -0x254FADD7, -0x61C80005, -0x0174730A,
    -0x3A4B9F65,  0x786A8669, -0x6DCDF4AB, -0x63F7DD91,
    -0x221DF7EB, -0x0D0B6433,  0x45C5034B, -0x14611AAF,
     0x48213953, -0x7613AD45, -0x56EB26FE, -0x3215D45A,
     0x39EDD96D,  0x058E990E, -0x5659110D, -0x7B06BDF9,
    -0x23024973,  0x087A7C71,  0x3F323CE8,  0x5164F849,
    -0x58CD495F,  0x20C02FDE,  0x03DFD799,  0x1AB107A6,
     0x139D22D2,  0x3EF6951B,  0x3E12B706,  0x2393A16C,
    -0x211B8297,  0x31482E1A,  0x1696E759, -0x58763731,
     0x20310264, -0x02C902BA,  0x34368A4B, -0x4EE9CE22,
     0x56117AEC,  0x0A0F1CBE, -0x316D3912, -0x3BFECA09,
     0x57BD4646, -0x4D74AD82,  0x0E0EC61F,  0x19D901B5,
    -0x5B279242,  0x248F96F2,  0x0168D155, -0x2C13EE8D,
    -0x097E818E, -0x6D63F711,  0x64B58456, -0x4B08DA03,
     0x2F004D4D,  0x418CE9D4,  0x01F5FD68, -0x51157107,
     0x1DD6E2A7,  0x5BC75B69, -0x4784A45C, -0x390A714C,
    -0x57AB8AEB, -0x5A1AC309,  0x62F33076,  0x08A82A14,
    -0x611891B4,  0x13261A1C, -0x3AAA5CF8,  0x0134B0E9,
    -0x6D12A48E,  0x7C4AD4F8,  0x2B7A2C64, -0x1B747E76,
    -0x5AE55A11, -0x257F256F,  0x4F62988A,  0x1E731F7A,
     0x13B330E5, -0x64BFD44F, -0x2629F6AF, -0x50E9CC75,
    -0x2B0D70B5, -0x2AFD44E2,  0x2A1AE1D9,  0x09EF6444,
     0x1FDEE47C,  0x3E79FC16,  0x2C6BA7FD,  0x030DF8B1,
    -0x1185B0EA, -0x78A9A7C5,  0x281CA6FE, -0x154CEE42,
     0x1E19E55C, -0x0D3A1A0A,  0x4E437379, -0x50B956D9,
    -0x0DC29475, -0x453E960C,  0x4F54EC66, -0x06968728,
     0x11696E83,  0x25FDF25F,  0x0B794DAD,  0x43A4B10F,
    -0x2554E2AA,  0x4C4145EC,  0x63DB76F0,  0x182D11B3,
     0x5487DE91,  0x110E60C6,  0x1203646F,  0x61D2D22F,
     0x37A44180, -0x46A132C8, -0x00FFC464,  0x1E87DD52,
     0x259B70C4, -0x19EB5972, -0x32D39A52,  0x0102C527,
     0x42A373F9,  0x4B1328CB, -0x480EFA58,  0x2D0B6276,
     0x14D6F84F,  0x2BBC47F4,  0x45A96730,  0x21E7630C,
    -0x07687255,  0x23A97ADA,  0x7BC290A0, -0x6320E0D9
};

int  r_asm[N];
int  r_c[N];


int main()
{   

/* ======================================================================== */
/*  Call hand-coded assembly version (located in DSP64x.lib library         */ 
/*  archive - see Project -> Build Options -> Linker -> Include Libraries)  */
/* ======================================================================== */   
    DSP_mul32(x, y, r_asm, N);
    
/* ======================================================================== */
/*  Call natural-C version                                                  */
/* ======================================================================== */
    DSP_mul32_c(x, y, r_c, N);

/* ======================================================================== */
/*  Compare outputs using mem_compare() in support.c.  If different,        */
/*  mem_compare() displays location of failures.                            */
/* ======================================================================== */
    test = mem_compare(r_asm, "r_asm", r_c, "r_c", sizeof(r_c));
    if (test == 0) 
    {
      printf("r_asm vs. r_c");
      printf("\nVerification successful.\n");
    }

}

/* ======================================================================== */
/*  End of file:  dsp_mul32_d.c                                             */
/* ------------------------------------------------------------------------ */
/*          Copyright (C) 2003 Texas Instruments, Incorporated.             */
/*                          All Rights Reserved.                            */
/* ======================================================================== */
