/***********************************************************************************************************************
 * File Name    : block_media_ram_disk.h
 * Description  : Header file for RAM-disk media driver.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#ifndef BLOCK_MEDIA_RAM_DISK_H
#define BLOCK_MEDIA_RAM_DISK_H

/* Undefining the usb_pmsc driver related macros.*/
#undef STRG_SECTSIZE
#undef USB_MEDIA_ADDRESS
#undef STRG_MEDIASIZE
#undef STRG_TOTALSECT
#undef RAMDISK_MEDIATYPE
#undef RAMDISK_SIGNATURE
#undef RAMDISK_CLSTSIZE
#undef RAMDISK_FATNUM

/* macro definitions */
#define STRG_SECTSIZE                  (0x0200UL)      /* 512 bytes per sector */
#define USB_MEDIA_ADDRESS              (0x22008000UL)  /* Media Head Address */
#define STRG_MEDIASIZE                 (32UL * 1024UL) /* Media size */
#define STRG_TOTALSECT                 (STRG_MEDIASIZE / STRG_SECTSIZE)
#define RAMDISK_MEDIATYPE              (0xF8u)        /* Fixed media */
#define RAMDISK_SIGNATURE              (0xAA55u)
#define RAMDISK_CLSTSIZE               (0x01u)
#define RAMDISK_FATNUM                 (0x02u)

/* If the number of data areas of clusters is smaller than that of value 4085(4096-11), it is FAT12.
 * If the number of data areas of clusters is smalle than that of value 65525(65536-11), it is FAT16.
 * Otherwise it is FAT32. */

#if STRG_TOTALSECT < 4096
    #define RAMDISK_FATLENGTH           (0x155ul)     /* FAT12 */
#else   /* STRG_TOTALSECT < 4096 */
    #if STRG_TOTALSECT < 65536
        #define RAMDISK_FATLENGTH       (0x100ul)     /* FAT16 */
    #else   /* STRG_TOTALSECT < 65536 */
        #define RAMDISK_FATLENGTH       (0x80ul)     /* FAT32 */
    #endif  /* STRG_TOTALSECT < 65536 */
#endif  /* STRG_TOTALSECT < 4096 */

#define RAMDISK_FATSIZE   (((STRG_TOTALSECT - 8) / RAMDISK_FATLENGTH) + 1)
#define RAMDISK_DIRSIZE   (16ul)           /* root directory size */


#endif /* BLOCK_MEDIA_RAM_DISK_H */
