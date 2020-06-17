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
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

// For use in C++
#ifdef __cplusplus
extern "C" {
#endif

#if !defined MEMORY_MAP_H
#define MEMORY_MAP_H


/* ****************************************************************************************************************
 * Code Flash memory map
 * ****************************************************************************************************************
 */

#define  ADDR_1ST_RX             0x00010000
#define  ADDR_1ST_RX_MT_PAGE     0x00018000
#define  FIRST_RX_MT_PAGE_SIZE  (0x00008000 - 1)
#define  ADDR_1ST_RX_MT          0x0001FC00
#define  TOTAL_SIZE_1ST_RX       0x10000

#define  ADDR_OEM_CERT           0x0000E000
#define  SIZE_OEM_CERT           0x00002000

#define  FAW_START           	0x00100000
#define  FAW_END           		0x00200000

#define  ADDR_OEM_RX             0x001E8000
#define  ADDR_OEM_RX_MT_PAGE     0x001F8000
#define  OEM_RX_MT_PAGE_SIZE    (0x00008000 - 1)
#define  ADDR_OEM_RX_MT          0x001FFC00


// The following addresses must (magically) match the addresses used in the security kernel loader (scatter) file.
#define  ADDR_KERNEL_START       0x0000
#define  ADDR_SEED_IN_KERNEL     0x0500
#define  ADDR_DEV_CERT_IN_KERNEL 0x0540
#define  ADDR_DEV_KEYS_IN_KERNEL 0x0698
#define  ADDR_KERNEL_MT          0xDE00
#define  SIZE_OF_KERNEL_MT       0x0200

#define  ADDR_MAIN_MT            0x20000
#define  SIZE_OF_MAIN_MT         0x8000

#define  SIZE_OF_1ST_RX_MT       0x00400
#define  OFFSET_1ST_RX_MT        (ADDR_1ST_RX_MT - ADDR_1ST_RX_MT_PAGE)

#define  ADDR_PENDING_UPDATE     0x108000

#define  SIZE_OF_OEM_RX_MT       0x00000400
#define  OFFSET_OEM_RX_MT        (ADDR_OEM_RX_MT - ADDR_OEM_RX_MT_PAGE)

#define CODE_FLASH_START_ADDR    0x00000000
#define CODE_FLASH_END_ADDR      0x001FFFFF
#define CODE_FLASH_SIZE          (CODE_FLASH_END_ADDR - CODE_FLASH_START_ADDR  + 1)


/* ****************************************************************************************************************
 * Config Flash memmory map
 * ****************************************************************************************************************
 */

// Configuration area register address blocks
#define FSPR_CONFIG_ADDR_START  0x0100A160
#define FSPR_CONFIG_ADDR_END    0x0100A16F
#define OSIS_CONFIG_AREA_START  0x0100A150
#define OSIS_CONFIG_AREA_END    0x0100A15F
#define FAW_CONFIG_ADDR_START   0x0100A160
#define FAW_CONFIG_ADDR_END     0x0100A16F

/* ****************************************************************************************************************
 * RAM memory map
 * ****************************************************************************************************************
 */

#define KERNEL_RAM_START_ADDR   0x1FFE0000
#define APP_RAM_START_ADDR      0x1FFE0010
#define RAM_END_ADDR            0x2007FFFF
#define APP_RAM_SIZE           (RAM_END_ADDR - APP_RAM_START_ADDR  + 1)
#endif

#ifdef __cplusplus
}
#endif
