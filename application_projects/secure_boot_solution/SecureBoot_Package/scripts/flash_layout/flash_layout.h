/***********************************************************************************************************************
* File Name    : flash_layout.h
* Description  : Memory partition related macro definition file
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
#ifndef __FLASH_LAYOUT_H__
#define __FLASH_LAYOUT_H__

/*
 *   Flash layout on Renesas RA6M3 devices in case of MCUBOOT_OVERWRITE_ONLY use case:
 *
 *   Section                            Offset              Size
 *
 *   BL2                                0x0000_0000          64KB
 *   Primary Image Area                 0x0001_0000         960KB
 *   Secondary Image Area               0x0010_0000         960KB
 *   Scratch Area                       0x001F_0000          32KB
 */

#define MAX(X,Y)                       ((X) > (Y) ? (X) : (Y))

/* Size of a Secure and of a Non-secure image */
#define FLASH_S_PARTITION_SIZE              (0)
#define FLASH_NS_PARTITION_SIZE             (0xF0000) /* 30 * 32KB block = 960KB */
#define FLASH_MAX_PARTITION_SIZE            ((FLASH_S_PARTITION_SIZE >   \
                                             FLASH_NS_PARTITION_SIZE) ? \
                                             FLASH_S_PARTITION_SIZE :    \
                                             FLASH_NS_PARTITION_SIZE)

#define FLASH_TOTAL_SIZE                    (0x00200000)    /*  2MB  */

/* Sector size of the flash hardware; same as FLASH0_SECTOR_SIZE */
#define FLASH_AREA_IMAGE_SECTOR_SIZE        (0x8000)    /* 32 KB */

/* Sector size of the flash hardware */
#define FLASH_BOOT_SECTOR_SIZE              (0x2000)    /* 8 KB */

/* Flash layout info for BL2 bootloader */
#define FLASH_BASE_ADDRESS                  (0)

/* Offset and size definitions of the flash partitions that are handled by the
 * bootloader. The image swapping is done between IMAGE_PRIMARY and
 * IMAGE_SECONDARY, SCRATCH is used as a temporary storage during image
 * swapping.
 */
#define FLASH_AREA_BL2_OFFSET               (0x0)
#define FLASH_AREA_BL2_SIZE                 (8 * FLASH_BOOT_SECTOR_SIZE)

#if !defined(MCUBOOT_IMAGE_NUMBER) || (MCUBOOT_IMAGE_NUMBER == 1)
#define FLASH_AREA_0_ID                     (1)
#define FLASH_AREA_0_OFFSET                 (FLASH_AREA_BL2_OFFSET + FLASH_AREA_BL2_SIZE)
#define FLASH_AREA_0_SIZE                   (FLASH_NS_PARTITION_SIZE)

#define FLASH_AREA_2_ID                     (FLASH_AREA_0_ID + 1)
#define FLASH_AREA_2_OFFSET                 (FLASH_AREA_0_OFFSET + FLASH_AREA_0_SIZE)
#define FLASH_AREA_2_SIZE                   (FLASH_NS_PARTITION_SIZE)

/* Scratch area */
#define FLASH_AREA_SCRATCH_ID               (FLASH_AREA_2_ID + 1)
#define FLASH_AREA_SCRATCH_OFFSET           (FLASH_AREA_2_OFFSET + FLASH_AREA_2_SIZE)

#ifndef MCUBOOT_OVERWRITE_ONLY
#define FLASH_AREA_SCRATCH_SIZE             (FLASH_AREA_IMAGE_SECTOR_SIZE)

/* The maximum number of status entries supported by the bootloader. */
/* The maximum number of status entries must be at least 2. For more
 * information see the MCUBoot issue:
 * https://github.com/JuulLabs-OSS/mcuboot/issues/427.
 */
#define BOOT_STATUS_MAX_ENTRIES             ((FLASH_S_PARTITION_SIZE + \
                                             FLASH_NS_PARTITION_SIZE) / \
                                             FLASH_AREA_SCRATCH_SIZE)

#else
#define FLASH_AREA_SCRATCH_SIZE             (0)

/* The maximum number of status entries supported by the bootloader. */
/* The maximum number of status entries must be at least 2. For more
 * information see the MCUBoot issue:
 * https://github.com/JuulLabs-OSS/mcuboot/issues/427.
 */
#define MCUBOOT_STATUS_MAX_ENTRIES             (0)

#endif


/* Maximum number of image sectors supported by the bootloader. */
#define MCUBOOT_MAX_IMG_SECTORS       (( FLASH_S_PARTITION_SIZE +  \
                                      FLASH_NS_PARTITION_SIZE) / \
                                      FLASH_AREA_IMAGE_SECTOR_SIZE)


#elif (MCUBOOT_IMAGE_NUMBER == 2)

#else
#error "Only MCUBOOT_IMAGE_NUMBER 1 and 2 are supported!"
#endif

/*  NV Counters definitions */
#define FLASH_NV_COUNTERS_AREA_OFFSET       (0x40100000)
#define FLASH_NV_COUNTERS_AREA_SIZE         (0x40)   /* 64 bytes */

/* Flash device name used by BL2
 * Name is defined in flash driver file: Driver_Flash.c
 */
#define FLASH_DEV_NAME               Driver_FLASH0

/* NV Counters definitions */
#define TFM_NV_COUNTERS_AREA_ADDR    FLASH_NV_COUNTERS_AREA_OFFSET
#define TFM_NV_COUNTERS_AREA_SIZE    (0x40) /* 64 bytes */
#define TFM_NV_COUNTERS_SECTOR_ADDR  FLASH_NV_COUNTERS_AREA_OFFSET
#define TFM_NV_COUNTERS_SECTOR_SIZE  FLASH_NV_COUNTERS_AREA_SIZE

#define TOTAL_ROM_SIZE               FLASH_TOTAL_SIZE
#define TOTAL_RAM_SIZE               (0xA0000)      /* 640 KB */


#endif /* __FLASH_LAYOUT_H__ */

