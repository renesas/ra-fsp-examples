/***********************************************************************************************************************
 * File Name    : non_secure_flash_functions.h
 * Description  : Contains test functions defined to run out of non_secure flash regions
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
