/***********************************************************************************************************************
 * File Name    : mram_ep.h
 * Description  : Contains macros, variables, and functions used in mram_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2025 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef MRAM_EP_H_
#define MRAM_EP_H_

#include <stdbool.h>
#include "hal_data.h"

/* Macro for the reset value */
#define RESET_VALUE         (0x00)

/* Start address for MRAM access in example project (avoid overwriting application code) */
#define MRAM_START_ADDR     (0x020FF000U)

/* Number of bytes to read/write */
#define NUM_BYTES           (128U)

/* Erase block size is 32 bytes, so the number of blocks must be aligned accordingly */
#define BLOCK_NUM           ((NUM_BYTES + (BSP_FEATURE_MRAM_PROGRAMMING_SIZE_BYTES - 1)) / \
                             BSP_FEATURE_MRAM_PROGRAMMING_SIZE_BYTES)

/* Switch commands */
#define MRAM_WRITE          (1U)
#define MRAM_READ           (2U)
#define MRAM_ERASE          (3U)
#define EXIT                (4U)

/* Function declarations */
fsp_err_t mram_init(void);
void mram_deinit(void);
fsp_err_t mram_write(uint32_t mram_address, uint8_t const * src_address, uint32_t num_bytes);
fsp_err_t mram_erase(uint32_t address, uint32_t num_blocks);
bool mram_is_blank(uint32_t mram_address, uint32_t num_bytes);

#endif /* MRAM_EP_H_ */
