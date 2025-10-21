/***********************************************************************************************************************
 * File Name    : block_media_ram_disk.h
 * Description  : Header file for RAM disk media driver.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef BLOCK_MEDIA_RAM_DISK_H
#define BLOCK_MEDIA_RAM_DISK_H

/* Remove the usb_pmsc driver related macros */
#undef STRG_SECTSIZE
#undef STRG_MEDIASIZE
#undef STRG_TOTALSECT
#undef RAMDISK_MEDIATYPE
#undef RAMDISK_SIGNATURE
#undef RAMDISK_CLSTSIZE
#undef RAMDISK_FATNUM

/* Macro definitions */
#define STRG_SECTSIZE                  (0x0200UL)                           /* 512 bytes per sector */
#define STRG_MEDIASIZE                 (32UL * 1024UL)                      /* Media size */
#define STRG_TOTALSECT                 (STRG_MEDIASIZE / STRG_SECTSIZE)
#define RAMDISK_MEDIATYPE              (0xF8u)                              /* Fixed media */
#define RAMDISK_SIGNATURE              (0xAA55u)
#define RAMDISK_CLSTSIZE               (0x01u)
#define RAMDISK_FATNUM                 (0x02u)

/* If the number of data areas of clusters is smaller than that of value 4085 (4096-11), it is FAT12.
 * If the number of data areas of clusters is smaller than that of value 65525 (65536-11), it is FAT16.
 * Otherwise it is FAT32 */
#if STRG_TOTALSECT < 4096
    #define RAMDISK_FATLENGTH           (0x155ul)       /* FAT12 */
#else   /* STRG_TOTALSECT < 4096 */
    #if STRG_TOTALSECT < 65536
        #define RAMDISK_FATLENGTH       (0x100ul)       /* FAT16 */
    #else   /* STRG_TOTALSECT < 65536 */
        #define RAMDISK_FATLENGTH       (0x80ul)        /* FAT32 */
    #endif  /* STRG_TOTALSECT < 65536 */
#endif  /* STRG_TOTALSECT < 4096 */

#define RAMDISK_FATSIZE   (((STRG_TOTALSECT - 8) / RAMDISK_FATLENGTH) + 1)
#define RAMDISK_DIRSIZE   (16ul)                        /* Root directory size */

#endif /* BLOCK_MEDIA_RAM_DISK_H */
