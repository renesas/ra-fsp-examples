/***********************************************************************************************************************
 * File Name    : secure_sram_functions.h
 * Description  : Contains implementations of secure functions running out of secure sram
 ***********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef SECURE_SRAM_FUNCTIONS_H_
#define SECURE_SRAM_FUNCTIONS_H_

#include "secure_definitions.h"

/* tests from secure area */
SECURE_SRAM_PROGRAM bool s_readSecureFlash_usingSecureRamCode(void);
SECURE_SRAM_PROGRAM bool s_readSecureRam_usingSecureRamCode(void);
SECURE_SRAM_PROGRAM bool s_read_non_secureFlash_usingSecureRamCode(void);
SECURE_SRAM_PROGRAM bool s_read_non_secureRam_usingSecureRamCode(void);

SECURE_SRAM_PROGRAM bool s_readSecureFlash_fromSecureRam_usingSecureRamCode(void);
SECURE_SRAM_PROGRAM bool s_readSecureRam_fromSecureRam_usingSecureRamCode(void);
SECURE_SRAM_PROGRAM bool s_read_non_secureFlash_fromSecureRam_usingSecureRamCode(void);
SECURE_SRAM_PROGRAM bool s_read_non_secureRam_fromSecureRam_usingSecureRamCode(void);

SECURE_SRAM_PROGRAM bool s_writeSecureRam_usingSecureRamCode(void);
SECURE_SRAM_PROGRAM bool s_write_non_secureRam_usingSecureRamCode(void);
SECURE_SRAM_PROGRAM bool s_writeSecureFlash_usingSecureRamCode(void);
SECURE_SRAM_PROGRAM bool s_write_non_secureFlash_usingSecureRamCode(void);
SECURE_SRAM_PROGRAM void s_setup_the_writeBuffer(void);

#endif /* SECURE_SRAM_FUNCTIONS_H_ */
