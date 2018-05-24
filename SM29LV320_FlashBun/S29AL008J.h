#ifndef  FLASH_
#define  FLASH_

#ifdef __cplusplus
extern "C" {
#endif

#include "tistdtypes.h"

/* Flash address definitions */
// #define  FLASH_BASE          (Uint32)0xB0000000  // CE3
 #define  FLASH_SECTORSIZE    (Uint32)0x010000   // 64K

// #define  FLASH_PAGES         (Uint32)0x1
// #define  FLASH_PAGESIZE      (Uint32)0x400000
// #define  FLASH_SIZE          (Uint32)0x400000   //4 MByte = 64*64k
// #define  FLASH_CTL555        (Uint32)(FLASH_BASE + 0x555)
// #define  FLASH_CTLAAA        (Uint32)(FLASH_BASE + 0xaaa)
// #define  FLASH_SUPPORT       1

#define  FLASH_BASE          (Uint32)0x64000000  
#define  FLASH_CTLAAA        (Uint32)(FLASH_BASE + 0xAAA)
#define  FLASH_CTL555        (Uint32)(FLASH_BASE + 0x555)
#define  FLASH_SECTORS       (Uint32)0x13       // 19
#define  FLASH_SIZE          (Uint32)0x100000   // 1 MByte = 15*64k+32+8+8+16=1204kB


/* Constant table containing end address of each sector */
// TOP boot,S29AL008J70TFI01
/* static Uint32 sector_end[ FLASH_SECTORS] = {
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
      FLASH_BASE + 0x0f7fff, // Sector 15 ,32kb
      FLASH_BASE + 0x0f8fff, // Sector 16 ,8kb
      FLASH_BASE + 0x0fbfff, // Sector 17 ,8kb
      FLASH_BASE + 0x0fffff, // Sector 18 ,16kb
 };*/

// Bottom boot,S29AL008J70TFI02
static Uint32 sector_end[ FLASH_SECTORS] = {
     FLASH_BASE + 0x003fff, // Sector  0 ,16kb 
     FLASH_BASE + 0x005fff, // Sector  1 ,8kb 
     FLASH_BASE + 0x007fff, // Sector  2 ,8kb
     FLASH_BASE + 0x00ffff, // Sector  3 ,32kb
     FLASH_BASE + 0x01ffff, // Sector  4 ,64kb
     FLASH_BASE + 0x02ffff, // Sector  5 ,64kb
     FLASH_BASE + 0x03ffff, // Sector  6 ,64kb
     FLASH_BASE + 0x04ffff, // Sector  7 ,64kb
     FLASH_BASE + 0x05ffff, // Sector  8 ,64kb
     FLASH_BASE + 0x06ffff, // Sector  9 ,64kb
     FLASH_BASE + 0x07ffff, // Sector 10 ,64kb
     FLASH_BASE + 0x08ffff, // Sector 11 ,64kb
     FLASH_BASE + 0x09ffff, // Sector 12 ,64kb
     FLASH_BASE + 0x0affff, // Sector 13 ,64kb
     FLASH_BASE + 0x0bffff, // Sector 14 ,64kb
     FLASH_BASE + 0x0cffff, // Sector 15 ,64kb
     FLASH_BASE + 0x0dffff, // Sector 16 ,64kb
     FLASH_BASE + 0x0effff, // Sector 17 ,64kb
     FLASH_BASE + 0x0fffff, // Sector 18 ,64kb
};

/* Erase a segment of Flash memory */
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

