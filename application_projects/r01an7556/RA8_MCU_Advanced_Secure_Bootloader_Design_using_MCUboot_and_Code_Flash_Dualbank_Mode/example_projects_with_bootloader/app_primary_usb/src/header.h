/***********************************************************************************************************************
 * File Name    : header.h
 * Description  : Contains application image header information and related function prototypes
 ***********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
************************************************************************************************************************/

#ifndef HEADER_H_
#define HEADER_H_

#include "downloader_thread.h"

#define PRIMARY_IMAGE_START_ADDRESS         0x02010000
#define PRIMARY_IMAGE_END_ADDRESS           0x020F7FFF
#define SECONDARY_IMAGE_START_ADDRESS       0x02210000
#define SECONDARY_IMAGE_END_ADDRESS         0x022F7FFF
#define FLASH_BLOCK_SIZE                    (32 * 1024)
#define SECONDARY_IMAGE_NUM_BLOCKS          ((SECONDARY_IMAGE_END_ADDRESS - SECONDARY_IMAGE_START_ADDRESS + 1U) / FLASH_BLOCK_SIZE)

typedef enum e_enable_disable
{
    DISABLE,
    RE_ENABLE
} enable_disable_t;

void threads_and_interrupts(enable_disable_t enable_disable);
void display_image_slot_info(void);

#endif /* HEADER_H_ */
