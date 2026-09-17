/***********************************************************************************************************************
 * File Name    : slot_info.h
 * Description  : Contains macros for image slots, enum and function prototypes
 ***********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
************************************************************************************************************************/

#ifndef HEADER_H_
#define HEADER_H_

#include "hal_data.h"

fsp_err_t code_flash_lp_operations(void);
fsp_err_t display_image_slot_info(void);

#define SECONDARY_FIRMWARE_START_ADDRESS        0x11800
#define SECONDARY_FIRMWARE_END_ADDRESS          0x1F7FF
#define BLOCK_SIZE                              (128)
#define FLASH_BLOCK_SIZE                        (2 * 1024)
#define SECONDARY_FIRMWARE_NUM_BLOCKS           ((SECONDARY_FIRMWARE_END_ADDRESS - SECONDARY_FIRMWARE_START_ADDRESS + 1U )/ BLOCK_SIZE)

typedef enum e_enable_disable
{
    DISABLE,
    RE_ENABLE
}enable_disable_t;

#endif /* HEADER_H_ */
