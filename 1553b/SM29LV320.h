#ifndef  FLASH_
#define  FLASH_

#ifdef __cplusplus
extern "C" {
#endif

#include "tistdtypes.h"

/* Flash address definitions */
#define  FLASH_BASE          (Uint32)0x64000000  
#define  FLASH_CTLAAA        (Uint32)(FLASH_BASE + 0xAAA)
#define  FLASH_CTL555        (Uint32)(FLASH_BASE + 0x555)
#define  FLASH_SECTORSIZE    (Uint32)0x010000   // 64K
#define  FLASH_SECTORSIZE1   (Uint32)0x002000   // 8K
#define  FLASH_SECTORS       (Uint32)71        // 71
#define  FLASH_SIZE          (Uint32)0x100000   // 4 MByte = 63*64k+8*8=4096kB


/* Constant table containing end address of each sector */
// SM29LV320
 static Uint32 sector_end[ FLASH_SECTORS] = {
      FLASH_BASE + 0x00ffff, // Sector  0 ,64kb 
      FLASH_BASE + 0x01ffff, // Sector  1 ,64kb 
      FLASH_BASE + 0x02ffff, // Sector  2 ,64kb
      FLASH_BASE + 0x03ffff, // Sector  3 ,64kb
      FLASH_BASE + 0x04ffff, // Sector  4 ,64kb
      FLASH_BASE + 0x05ffff, // Sector  5 ,64kb
      FLASH_BASE + 0x06ffff, // Sector  6 ,64kb
      FLASH_BASE + 0x07ffff, // Sector  7 ,64kb
      FLASH_BASE + 0x08ffff, // Sector  8 ,64kb
      FLASH_BASE + 0x09ffff, // Sector  9 ,64kb
      FLASH_BASE + 0x0affff, // Sector 10 ,64kb
      FLASH_BASE + 0x0bffff, // Sector 11 ,64kb
      FLASH_BASE + 0x0cffff, // Sector 12 ,64kb
      FLASH_BASE + 0x0dffff, // Sector 13 ,64kb
      FLASH_BASE + 0x0effff, // Sector 14 ,64kb
      FLASH_BASE + 0x0fffff, // Sector 15 ,64kb
      
      FLASH_BASE + 0x10ffff, // Sector 16 ,64kb 
      FLASH_BASE + 0x11ffff, // Sector 17 ,64kb 
      FLASH_BASE + 0x12ffff, // Sector 18 ,64kb
      FLASH_BASE + 0x13ffff, // Sector 19 ,64kb
      FLASH_BASE + 0x14ffff, // Sector 20 ,64kb
      FLASH_BASE + 0x15ffff, // Sector 21 ,64kb
      FLASH_BASE + 0x16ffff, // Sector 22 ,64kb
      FLASH_BASE + 0x17ffff, // Sector 23 ,64kb
      FLASH_BASE + 0x18ffff, // Sector 24 ,64kb
      FLASH_BASE + 0x19ffff, // Sector 25 ,64kb
      FLASH_BASE + 0x1affff, // Sector 26 ,64kb
      FLASH_BASE + 0x1bffff, // Sector 27 ,64kb
      FLASH_BASE + 0x1cffff, // Sector 28 ,64kb
      FLASH_BASE + 0x1dffff, // Sector 29 ,64kb
      FLASH_BASE + 0x1effff, // Sector 30, 64kb
      FLASH_BASE + 0x1fffff, // Sector 31 ,64kb

      FLASH_BASE + 0x20ffff, // Sector 32 ,64kb 
      FLASH_BASE + 0x21ffff, // Sector 33 ,64kb 
      FLASH_BASE + 0x22ffff, // Sector 34 ,64kb
      FLASH_BASE + 0x23ffff, // Sector 35 ,64kb
      FLASH_BASE + 0x24ffff, // Sector 36 ,64kb
      FLASH_BASE + 0x25ffff, // Sector 37 ,64kb
      FLASH_BASE + 0x26ffff, // Sector 38 ,64kb
      FLASH_BASE + 0x27ffff, // Sector 39 ,64kb
      FLASH_BASE + 0x28ffff, // Sector 40 ,64kb
      FLASH_BASE + 0x29ffff, // Sector 41 ,64kb
      FLASH_BASE + 0x2affff, // Sector 42 ,64kb
      FLASH_BASE + 0x2bffff, // Sector 43 ,64kb
      FLASH_BASE + 0x2cffff, // Sector 44 ,64kb
      FLASH_BASE + 0x2dffff, // Sector 45 ,64kb
      FLASH_BASE + 0x2effff, // Sector 46 ,64kb
      FLASH_BASE + 0x2fffff, // Sector 47 ,64kb

      FLASH_BASE + 0x30ffff, // Sector 48 ,64kb 
      FLASH_BASE + 0x31ffff, // Sector 49 ,64kb 
      FLASH_BASE + 0x32ffff, // Sector 50 ,64kb
      FLASH_BASE + 0x33ffff, // Sector 51 ,64kb
      FLASH_BASE + 0x34ffff, // Sector 52 ,64kb
      FLASH_BASE + 0x35ffff, // Sector 53 ,64kb
      FLASH_BASE + 0x36ffff, // Sector 54 ,64kb
      FLASH_BASE + 0x37ffff, // Sector 55 ,64kb
      FLASH_BASE + 0x38ffff, // Sector 56 ,64kb
      FLASH_BASE + 0x39ffff, // Sector 57 ,64kb
      FLASH_BASE + 0x3affff, // Sector 58 ,64kb
      FLASH_BASE + 0x3bffff, // Sector 59 ,64kb
      FLASH_BASE + 0x3cffff, // Sector 60 ,64kb
      FLASH_BASE + 0x3dffff, // Sector 61 ,64kb
      FLASH_BASE + 0x3effff, // Sector 62 ,64kb
      
      FLASH_BASE + 0x3f1fff, // Sector 63 ,8kb      
      FLASH_BASE + 0x3f3fff, // Sector 64 ,8kb      
      FLASH_BASE + 0x3f5fff, // Sector 65 ,8kb      
      FLASH_BASE + 0x3f7fff, // Sector 66 ,8kb      
      FLASH_BASE + 0x3f9fff, // Sector 67 ,8kb      
      FLASH_BASE + 0x3fbfff, // Sector 68 ,8kb      
      FLASH_BASE + 0x3fdfff, // Sector 69 ,8kb      
      FLASH_BASE + 0x3fffff  // Sector 70 ,8kb                        
 };

/* Erase chip*/
void FLASH_erase_chip();

/* Erase a sector of Flash memory */
void  FLASH_erase(Uint32 start, Uint32 length);

/* Read data from a range in Flash */
void  FLASH_read(Uint32 src, Uint32 dst, Uint32 length);

/* Write data to a data range in Flash */
void  FLASH_write(Uint32 src, Uint32 dst, Uint32 length);

/* usder define,leeq,2013.01.09*/
Uint32 Verify_Flash(Uint32 src,Uint32 dst, Uint32 Length);

#ifdef __cplusplus
}
#endif

#endif

