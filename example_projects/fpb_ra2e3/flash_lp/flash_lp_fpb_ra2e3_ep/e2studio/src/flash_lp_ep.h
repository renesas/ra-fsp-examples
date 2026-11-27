/***********************************************************************************************************************
 * File Name    : flash_lp_ep.h
 * Description  : Contains declarations of data structures and functions used in flash_lp_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef FLASH_LP_EP_H_
#define FLASH_LP_EP_H_

#include <stdbool.h>
#include "hal_data.h"

/* Macro for the reset value */
#define RESET_VALUE                 (0x00)

/* Macros for buffer */
#define BUFFER_SIZE					(16U)

#define INITIAL_VALUE               ('\0')

/* Block size and block number */
#define BLOCK_SIZE					(64U)
#define NUM_BLOCKS					(1U)

/* Macros for data flash */
#define FLASH_DF_BLOCK0				(0x40100000)

#if defined(BOARD_RA2E1_EK) || defined(BOARD_RA2L2_EK)
#define FLASH_DF_BLOCK_END			(0x40100FFF)    /* Block 3 */
#elif defined(BOARD_RA2E2_EK) || defined(BOARD_RA2E3_FPB) || defined(BOARD_RA2T1_FPB)
#define FLASH_DF_BLOCK_END          (0x401007FF)    /* Block 1 */
#elif defined(BOARD_RA0E1_FPB) || defined(BOARD_RA0L1_FPB)
#define FLASH_DF_BLOCK_END          (0x401003FF)    /* Block 3 */
#elif defined(BOARD_RA0E2_FPB)
#define FLASH_DF_BLOCK_END          (0x401007FF)    /* Block 7 */
#else
#define FLASH_DF_BLOCK_END 			(0x40101FFF)	/* Block 7 */
#endif /* Address block end */

#if defined(BOARD_RA0E1_FPB) || defined(BOARD_RA0E2_FPB) || defined(BOARD_RA0L1_FPB)
#define FLASH_DF_BLOCK_SIZE			(256U)          /* Block size 256B */
#define FLASH_DF_BLOCK(x)			((FLASH_DF_BLOCK0) + (FLASH_DF_BLOCK_SIZE) * (uint32_t)(x))
#else
#define FLASH_DF_BLOCK_SIZE         (1024U)         /* Block size 1KB */
#define FLASH_DF_BLOCK(x)           ((FLASH_DF_BLOCK0) + (FLASH_DF_BLOCK_SIZE) * (uint32_t)(x))
#endif /* Block size and calculator address for block (x) */

/* Macros for code flash */
#define FLASH_CF_BLOCK_SIZE			(2048U)         /* Block size 2KB */
#define FLASH_CF_BLOCK0				(0x00000000)
#define FLASH_CF_BLOCK10			(0x00005000)

#if defined(BOARD_RA2E1_EK) || defined(BOARD_RA0E2_FPB) || defined(BOARD_RA2L2_EK)
#define FLASH_CF_BLOCK_PRE_END		(0x0001F7FF)
#define FLASH_CF_BLOCK_END			(0x0001FFFF)    /* Block 63 */
#elif defined(BOARD_RA2E2_EK) || defined(BOARD_RA2E3_FPB) || defined(BOARD_RA0E1_FPB)\
	  || defined(BOARD_RA2T1_FPB) || defined(BOARD_RA0L1_FPB)
#define FLASH_CF_BLOCK_PRE_END      (0x0000F7FF)
#define FLASH_CF_BLOCK_END          (0x0000FFFF)    /* Block 31 */
#elif defined(BOARD_RA4W1_EK) || defined(BOARD_RA2A2_EK)
#define FLASH_CF_BLOCK_PRE_END		(0x0007F7FF)
#define FLASH_CF_BLOCK_END			(0x0007FFFF)    /* Block 255 */
#else
#define FLASH_CF_BLOCK_PRE_END		(0x0003F7FF)
#define FLASH_CF_BLOCK_END			(0x0003FFFF)    /* Block 127 */
#endif /* Block end and block pre-end */

#define FLASH_CF_BLOCK(x)			((FLASH_CF_BLOCK0) + (FLASH_CF_BLOCK_SIZE) * (uint32_t)(x))
#define FLASH_CF_START_ADDR(x)		(FLASH_CF_BLOCK(x))
#define FLASH_CF_END_ADDR(x)		((FLASH_CF_BLOCK(x)) + (2) * (FLASH_CF_BLOCK_SIZE))

/* Extern global variable - defined in flash_lp_ep.c */
extern flash_result_t g_blank_check_result;

/* Function declarations */
fsp_err_t flash_lp_init(void);
fsp_err_t flash_lp_erase(uint32_t block_addr);
fsp_err_t flash_lp_write(uint8_t * src_address, uint32_t flash_address);
fsp_err_t flash_lp_access_window_set(uint32_t block_addr);
fsp_err_t flash_lp_access_window_clear(void);
void flash_lp_deinit(void);
fsp_err_t flash_lp_blank_check(uint32_t block_addr);

#endif /* FLASH_LP_EP_H_ */
