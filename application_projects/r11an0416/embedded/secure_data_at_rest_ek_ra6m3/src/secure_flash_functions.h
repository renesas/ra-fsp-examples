/***********************************************************************************************************************
 * File Name    : secure_flash_functions.h
 * Description  : Contains definitions of secure functions running out of secure flash
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * Copyright [2020] Renesas Electronics Corporation and/or its affiliates.  All Rights Reserved.
 *
 * This software and documentation are supplied by Renesas Electronics America Inc. and may only be used with products
 * of Renesas Electronics Corp. and its affiliates ("Renesas").  No other uses are authorized.  Renesas products are
 * sold pursuant to Renesas terms and conditions of sale.  Purchasers are solely responsible for the selection and use
 * of Renesas products and Renesas assumes no liability.  No license, express or implied, to any intellectual property
 * right is granted by Renesas. This software is protected under all applicable laws, including copyright laws. Renesas
 * reserves the right to change or discontinue this software and/or this documentation. THE SOFTWARE AND DOCUMENTATION
 * IS DELIVERED TO YOU "AS IS," AND RENESAS MAKES NO REPRESENTATIONS OR WARRANTIES, AND TO THE FULLEST EXTENT
 * PERMISSIBLE UNDER APPLICABLE LAW, DISCLAIMS ALL WARRANTIES, WHETHER EXPLICITLY OR IMPLICITLY, INCLUDING WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT, WITH RESPECT TO THE SOFTWARE OR
 * DOCUMENTATION.  RENESAS SHALL HAVE NO LIABILITY ARISING OUT OF ANY SECURITY VULNERABILITY OR BREACH.  TO THE MAXIMUM
 * EXTENT PERMITTED BY LAW, IN NO EVENT WILL RENESAS BE LIABLE TO YOU IN CONNECTION WITH THE SOFTWARE OR DOCUMENTATION
 * (OR ANY PERSON OR ENTITY CLAIMING RIGHTS DERIVED FROM YOU) FOR ANY LOSS, DAMAGES, OR CLAIMS WHATSOEVER, INCLUDING,
 * WITHOUT LIMITATION, ANY DIRECT, CONSEQUENTIAL, SPECIAL, INDIRECT, PUNITIVE, OR INCIDENTAL DAMAGES; ANY LOST PROFITS,
 * OTHER ECONOMIC DAMAGE, PROPERTY DAMAGE, OR PERSONAL INJURY; AND EVEN IF RENESAS HAS BEEN ADVISED OF THE POSSIBILITY
 * OF SUCH LOSS, DAMAGES, CLAIMS OR COSTS.
 **********************************************************************************************************************/


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
