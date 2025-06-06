/***********************************************************************************************************************
 * File Name    : header.h
 * Description  : Contains application image header information and related function prototypes
 ***********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
************************************************************************************************************************/

#ifndef HEADER_H_
#define HEADER_H_

#include "downloader_thread.h"

#define PRIMARY_IMAGE_START_ADDRESS         0x00005000
#define PRIMARY_IMAGE_END_ADDRESS           0x000427FF

#define SECONDARY_IMAGE_START_ADDRESS       0x00042800
#define SECONDARY_IMAGE_END_ADDRESS         0x0007FFFF

#define FLASH_BLOCK_SIZE                    (2 * 1024)

#define PRIMARY_IMAGE_NUM_BLOCKS            ((PRIMARY_IMAGE_END_ADDRESS - PRIMARY_IMAGE_START_ADDRESS + 1U) / FLASH_BLOCK_SIZE)
#define SECONDARY_IMAGE_NUM_BLOCKS          ((SECONDARY_IMAGE_END_ADDRESS - SECONDARY_IMAGE_START_ADDRESS + 1U) / FLASH_BLOCK_SIZE)

typedef enum e_enable_disable
{
    DISABLE,
    RE_ENABLE
} enable_disable_t;

void threads_and_interrupts(enable_disable_t enable_disable);
void display_image_slot_info(void);

#endif /* HEADER_H_ */
