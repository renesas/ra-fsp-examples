
/***********************************************************************************************************************
 * File Name    : header.h
 * Description  : Contains application image header information and related function prototypes
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef HEADER_H_
#define HEADER_H_

#include "downloader_thread.h"

#if 0
#define PRIMARY_IMAGE_START_ADDRESS      0x00010000
#define PRIMARY_IMAGE_END_ADDRESS        0x0007FFFF
#define SECONDARY_IMAGE_START_ADDRESS    0x00080000
#define SECONDARY_IMAGE_END_ADDRESS      0x000EFFFF
#else
#define PRIMARY_IMAGE_START_ADDRESS      0x00018000
#define PRIMARY_IMAGE_END_ADDRESS        0x000F7FFF
#define SECONDARY_IMAGE_START_ADDRESS    0x60000000
#define SECONDARY_IMAGE_END_ADDRESS      0x600DFFFF

#endif
#define FLASH_BLOCK_SIZE                    (32 * 1024)
#define SECONDARY_IMAGE_NUM_BLOCKS          ((SECONDARY_IMAGE_END_ADDRESS - SECONDARY_IMAGE_START_ADDRESS + 1) / FLASH_BLOCK_SIZE)

typedef enum e_enable_disable
{
    DISABLE,
    RE_ENABLE
}enable_disable_t;

void ThreadsAndInterrupts(enable_disable_t EnableDisable);
void display_image_slot_info(void);

#endif /* HEADER_H_ */
