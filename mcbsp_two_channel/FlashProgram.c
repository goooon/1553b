#include "SM29LV320.h"

/************************************************************************/
/* PROCEDURE:    Check_Toggle_Ready                                     */
/*                                                                      */
/* During the internal program cycle, any consecutive read operation    */
/* on DQ6 will produce alternating 0's and 1's (i.e. toggling between   */
/* 0 and 1). When the program cycle is completed, DQ6 of the data will  */
/* stop toggling. After the DQ6 data bit stops toggling, the device is  */
/* ready for next operation.                                            */
/*                                                                      */
/* Input:                                                               */
/*           Dst        must already set-up by the caller               */
/*                                                                      */
/* Output:                                                              */
/*           None                                                       */
/************************************************************************/
void Check_Toggle_Ready ()  // *Dst可以是任意地址
{
  Uint8 Loop = 1;
  Uint8 PreData;
  Uint8 CurData;
  unsigned long TimeOut = 0;

  PreData = *((Uint8 *) FLASH_BASE);
  PreData = PreData & 0x40;
  while ((TimeOut< 0x0FFFFFFF) && (Loop))
  {
    CurData = *((Uint8 *) FLASH_BASE);
    CurData = CurData & 0x40;
    if (PreData == CurData)
      Loop = 0;   /* ready to exit the while loop */
    PreData = CurData;
    TimeOut++;
  }
}

/************************************************************************/
/* PROCEDURE:   Check_Data_Polling                                      */
/*                                                                      */
/* During the internal program cycle, any attempt to read DQ7 of the    */
/* last byte loaded during the page/byte-load cycle will receive the    */
/* complement of the true data.  Once the program cycle is completed,   */
/* DQ7 will show true data.                                             */
/*                                                                      */
/* Input:                                                               */
/*           Dst        must already set-up by the caller               */
/*           True       Data is the original (true) data                */
/*                                                                      */
/* Output:                                                              */
/*           None                                                       */
/************************************************************************/

void Check_Data_Polling (Uint8 *Dst, Uint8 TrueData)
{
  Uint8 Loop = 1;
  Uint8 CurrData;
  unsigned long TimeOut = 0;

  TrueData = TrueData &  0x80;
  while ((TimeOut< 0x0FFFFFFF) && (Loop))
  {
    CurrData = *Dst;
    CurrData = CurrData & 0x80;
    if (TrueData == CurrData)
      Loop = 0;   /* ready to exit the while loop  */
    TimeOut++;
  }
}

/* Erase chip*/
void FLASH_erase_chip()
{
     Uint8 *pdata;
     pdata =  (Uint8 *)FLASH_BASE;

    /* Start sector erase sequence */
    *((Uint8 *) FLASH_CTLAAA) = 0xaa;
    *((Uint8 *) FLASH_CTL555) = 0x55;
    *((Uint8 *) FLASH_CTLAAA) = 0x80;
    *((Uint8 *) FLASH_CTLAAA) = 0xaa;
    *((Uint8 *) FLASH_CTL555) = 0x55;
    *((Uint8 *) FLASH_CTLAAA) = 0x10;
    
    /* Wait for erase to complete */
    //Check_Toggle_Ready();
     while (1)
       if (*pdata & 0x80)
          break;

    /* Put back in read mode */
    *((Uint8 *) FLASH_BASE) = 0xf0;
}

/* Erase a segment of Flash memory */
void FLASH_erase(Uint32 start, Uint32 length)
{
    Int16 i;
    Uint8 *pdata;
    Uint32 sector_base, end;

    /* Calculate extents of range to erase */
    end = start + length - 1;

    /* Walk through each sector, erase any sectors within range */
    sector_base =  FLASH_BASE;
    for (i = 0; i <  FLASH_SECTORS; i++)
    {
        if ( ( ( sector_base >= start ) || ( sector_end[i] >= start ) ) &&
             ( ( sector_base <= end )   || ( sector_end[i] <= end ) ) )
        {
          /* Start sector erase sequence */
          *((Uint8 *) FLASH_CTLAAA) = 0xaa;
          *((Uint8 *) FLASH_CTL555) = 0x55;
          *((Uint8 *) FLASH_CTLAAA) = 0x80;
          *((Uint8 *) FLASH_CTLAAA) = 0xaa;
          *((Uint8 *) FLASH_CTL555) = 0x55;
    
            /* Start erase at sector address */
            pdata = (Uint8 *)sector_base;
            *pdata = 0x30;

            /* Wait for erase to complete */
            Check_Toggle_Ready();
            /* while (1)
                if (*pdata & 0x80)
                     break;*/

            /* Put back in read mode */
            *((Uint8 *) FLASH_BASE) = 0xf0;
        }

        /* Advance to next sector */
        sector_base = sector_end[i] + 1;
    }
}

/* Read data from a data range in Flash */
void FLASH_read(Uint32 src, Uint32 dst, Uint32 length)
{
    Uint8 *psrc, *pdst;
    Uint32 i;

    /* Establish source and destination */
    psrc = (Uint8 *)src;
    pdst = (Uint8 *)dst;
    for (i = 0; i < length; i++)
    {
        *pdst++ = *psrc++;
    }
}

/* Write data to a data range in Flash */
void FLASH_write(Uint32 src, Uint32 dst, Uint32 length)
{
    Uint8 *psrc, *pdst;
    Uint32 i;

    /* Establish source and destination */
    psrc = (Uint8 *)src;
    pdst = (Uint8 *)dst;
    for (i = 0; i < length; i++)
    {
        // Program one 8-bit word0x
        *((Uint8 *) FLASH_CTLAAA) = 0xaa;
        *((Uint8 *) FLASH_CTL555) = 0x55;
        *((Uint8 *) FLASH_CTLAAA) = 0xa0;
        
        *pdst = *psrc;

        // Wait for operation to complete
        //Check_Toggle_Ready();
         while(1)
             if (*pdst == *psrc)
                 break;

        pdst++;
        psrc++;
    }

    /* Put back in read mode */
    *((Uint16 *) FLASH_BASE) = 0xf0;
}


Uint32 Verify_Flash(Uint32 src,Uint32 dst, Uint32 Length)
{
  Uint32 i=0;  
  Uint32 nError =0;
  Uint8 *psrc, *pdst;
  Uint8 srcData,dstData;

  /* Establish source and destination */
  psrc = (Uint8 *)src;
  pdst = (Uint8 *)dst;
  for (i = 0; i < Length; i++)
  {
      srcData = *psrc++;
      dstData = *pdst++;
      if (srcData != dstData)
      {
        nError++;
        //printf("Verify Error at Src:%8x=%8x\n",psrc,srcData);
        //printf("Verify Error at Dst:%8x=%8x\n",pdst,dstData);
      }
       
    	if(nError>10)
				return nError;
  }
 	return nError;   
}



