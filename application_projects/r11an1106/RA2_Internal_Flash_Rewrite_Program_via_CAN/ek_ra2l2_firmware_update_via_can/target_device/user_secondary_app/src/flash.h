/***********************************************************************************************************************
 * File Name    : flash.h
 * Description  : Contains flash related macro definitions and function prototypes
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#include "common_utils.h"
#ifndef FLASH_H_
#define FLASH_H_

#define PRIMARY_IMAGE_START_ADDRESS         0x3800
#define PRIMARY_IMAGE_END_ADDRESS           0x117FF
#define SECONDARY_IMAGE_START_ADDRESS       0x11800
#define SECONDARY_IMAGE_END_ADDRESS         0x1F7FF
#define FLASH_BLOCK_SIZE                    (2 * 1024)
#define SECONDARY_IMAGE_NUM_BLOCKS          ((SECONDARY_IMAGE_END_ADDRESS - SECONDARY_IMAGE_START_ADDRESS + 1U )/ FLASH_BLOCK_SIZE)

/* This function initializes FLASH LP driver */
fsp_err_t flash_lp_init();
/* This function calls APIs to perform read/write/erase and other operations on code flash */
fsp_err_t code_flash_lp_operations();
fsp_err_t display_image_slot_info(const uint32_t address);
#endif /* FLASH_H_ */
