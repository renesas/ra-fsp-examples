/***********************************************************************************************************************
 * File Name    : secure_flash_functions.h
 * Description  : Contains definitions of secure functions running out of secure flash
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
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
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
