/***********************************************************************************************************************
 * File Name    : secure_flash_functions.h
 * Description  : Contains definitions of secure functions running out of secure flash
 ***********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/


#ifndef SECURE_FLASH_FUNCTIONS_H_
#define SECURE_FLASH_FUNCTIONS_H_

#include "secure_definitions.h"
#include "stdbool.h"

/*
 * this is a helper function to verify write to secure SRAM from non-secure program is not allowed
 */
uint32_t get_s_dataWritten(void);

extern void SECURE_PROGRAM  s_secureDataInit(void);


/* tests from secure area */

/* secure programs from secure flash region */
SECURE_PROGRAM bool s_readSecureFlash_usingSecureFlashCode(void);
SECURE_PROGRAM bool s_readSecureRam_usingSecureFlashCode(void);
SECURE_PROGRAM bool s_read_non_secureFlash_usingSecureFlashCode(void);
SECURE_PROGRAM bool s_read_non_secureRam_usingSecureFlashCode(void);

SECURE_PROGRAM bool s_writeSecureRam_usingSecureFlashCode(void);
SECURE_PROGRAM bool s_write_non_secureRam_usingSecureFlashCode(void);

SECURE_PROGRAM void secure_sram_section_copy (uint8_t * psource, uint8_t * pdest, uint32_t bytes);

SECURE_PROGRAM bool s_setup_faw(void);
SECURE_PROGRAM bool s_reset_faw(void);
#endif /* SECURE_FLASH_FUNCTIONS_H_ */
