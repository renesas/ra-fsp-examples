/***********************************************************************************************************************
 * File Name    : non_secure_flash_functions.h
 * Description  : Contains test functions defined to run out of non_secure flash regions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef non_secure_flash_FUNCTIONS_H_
#define non_secure_flash_FUNCTIONS_H_

#include "secure_definitions.h"

#define NUM_OF_FLASH_SECTOR     (1)
#define FLASH_SECTOR_SIZE_32KB	(32*1024)	// 32kB sector

uint32_t getDataBss(void);
uint32_t getDataConst(void);
uint32_t getDataInit(void);
uint32_t getvalueConst(void);
uint32_t getvalueData(void);
uint32_t getvalueBss(void);

extern volatile uint32_t dataWritten;
extern volatile    uint32_t u_dataInit;
extern const       uint32_t u_dataConst;
void setTestDatavalueBss(uint32_t valueBss);
void setTestDatavalueData(uint32_t valueData);
void setTestDatavalueConst(uint32_t valueConst);
void ns_setup_the_writeBuffer(void);

/* tests from non_secure area */
bool  readSecureFlash_using_non_secureFlashCode(void);
bool  readSecureRam_using_non_secureFlashCode(void);
bool  read_non_secureFlash_using_non_secureFlashCode(void);
bool  read_non_secureRam_using_non_secureFlashCode(void);


bool writeSecureRam_using_non_secureFlashCode(void);
bool write_non_secureRam_using_non_secureFlashCode(void);
bool writeSecureFlash_using_non_secureFlashCode(uint8_t *writeBuffer);
bool write_non_secureFlash_using_non_secureFlashCode(uint8_t *writeBuffer);


bool flash_write(uint32_t Flash_address, uint8_t *writeBuffer);

#endif /* non_secure_flash_FUNCTIONS_H_ */
