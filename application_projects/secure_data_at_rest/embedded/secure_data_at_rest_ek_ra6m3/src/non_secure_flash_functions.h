/***********************************************************************************************************************
 * File Name    : non_secure_flash_functions.h
 * Description  : Contains test functions defined to run out of non_secure flash regions
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
