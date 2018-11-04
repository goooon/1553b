.\zlg.out                  /* input COFF file */
-b                                      /* create binary image */
-image                                  /* create complete ROM image */
-zero                                   /* reset address origin to 0 */
-order L                                /*Little Endian*/
-map .\zlg.map                          /* create a hex map file */                           
-memwidth 8                             /* Width of ROM/Flash memory -map appl2.map create a hex map file */

-boot   
-bootorg 0x64000400                     /* address of the boot/copy-table */
-bootsection .bootload 0x64000000       /* section containing our asm boot routine */

ROMS
{
    FLASH: org = 0x64000000, len = 0x00010000, romwidth = 8, files = {.\zlg.bin} /*64kB*/
}

