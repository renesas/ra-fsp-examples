/***********************************************************************************************************************
 * File Name    : mram_storage.h
 * Description  : Contains macro definitions for flash layout
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
************************************************************************************************************************/

#ifndef MRAM_STORAGE_H_
#define MRAM_STORAGE_H_

#include "common_utils.h"
#include "hal_data.h"

/* Users need to update the KUK_ADDRESS, DIRECT_ECC_PUB_KEY_ADDRESS and DIRECT_ECC_PRI_KEY_ADDRESS based on where they are stored */
#define KUK_ADDRESS                     (0x02030000U)
#define DIRECT_ECC_PUB_KEY_ADDRESS      (0x02020000U)
#define DIRECT_ECC_PRI_KEY_ADDRESS      (0x02020080U)

#define RESET_VALUE             (0x00)
#define BLOCK_SIZE            (32)

/* Number of bytes to read/write */
#define NUM_BYTES           (128)
/* Erase block size is 32 bytes, so the number of blocks must be aligned accordingly */
#define BLOCK_NUM           ((NUM_BYTES + (BSP_FEATURE_MRAM_PROGRAMMING_SIZE_BYTES - 1)) / \
                             BSP_FEATURE_MRAM_PROGRAMMING_SIZE_BYTES)

fsp_err_t store_new_ecc_key_pair_to_mram(void);
fsp_err_t mram_save_ecc_key_pair(void);
bool mram_is_blank(uint32_t mram_address, uint32_t num_bytes);

#endif /* MRAM_STORAGE_H_ */
