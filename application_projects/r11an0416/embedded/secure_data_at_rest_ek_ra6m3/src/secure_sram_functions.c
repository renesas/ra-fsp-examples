/***********************************************************************************************************************
 * File Name    : secure_sram_functions.c
 * Description  : Contains implementations of secure functions running out of secure sram
 ***********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include <stdbool.h>
#include "secure_sram_functions.h"
#include "non_secure_flash_functions.h"
#include "secure_flash_functions.h"
#include "hal_data.h"
#include "secure_definitions.h"

#include "DAR_utilities.h"


extern uint8_t ns_writeBuffer[FLASH_WRITE_LENGTH];

/*******************************************************************************************************************//**
 * @brief s_writeSecureRam_usingSecureRamCode function
 * This function shows secure SRAM program can write to secure sram region.
 *
 * return: void
 **********************************************************************************************************************/

bool s_writeSecureRam_usingSecureRamCode(void)
{
    setTestDatavalueData(SECURE_DATA_WRITE_TEST_VALUE);
    s_dataWritten = getvalueData();

    SECURE_PADDING;

    if(SECURE_DATA_WRITE_TEST_VALUE == s_dataWritten)
    {
        return(true);
    }
    else
    {
        return(false);
    }
}

/*******************************************************************************************************************//**
 * @brief s_write_non_secureRam_usingSecureRamCode function
 * This function shows secure SRAM program can write to non-secure SRAM region
 *
 * return: void
 **********************************************************************************************************************/

bool s_write_non_secureRam_usingSecureRamCode(void)
{
	dataWritten = SECURE_DATA_WRITE_TEST_VALUE;
	setTestDatavalueData(dataWritten);

    SECURE_PADDING;

    if(SECURE_DATA_WRITE_TEST_VALUE == getvalueData())
    {
        return(true);
    }
    else
    {
        return(false);
    }
}

/*******************************************************************************************************************//**
 * @brief s_readSecureRam_usingSecureRamCode function
 * This function shows secure SRAM program can read secure SRAM data s_dataInit when access function is not provided.
 * return: void
 **********************************************************************************************************************/

bool s_readSecureRam_usingSecureRamCode(void)
{

    setTestDatavalueBss(s_dataBss);
    setTestDatavalueData(s_dataInit);

    SECURE_PADDING;

    if((0 == getvalueBss()) && (SECURE_DATA_VALUE == getvalueData()))
    {
        return(true);
    }
    else
    {
            return(false);
    }
}

/*******************************************************************************************************************//**
 * @brief s_read_non_secureRam_usingSecureRamCode function
 * This function shows secure SRAM program can read non-secure SRAM region.
 *
 * return: void
 **********************************************************************************************************************/

bool s_read_non_secureRam_usingSecureRamCode(void)
{
    setTestDatavalueBss(getDataBss());
    setTestDatavalueData(u_dataInit);

    SECURE_PADDING;

    if((0 == getvalueBss()) && (DATA_VALUE == getvalueData()))
    {
        return(true);
    }
    else
    {
        return(false);
    }
}

/*******************************************************************************************************************//**
 * @brief s_readSecureFlash_usingSecureRamCode function
 * This function shows secure SRAM program can read secure flash region.
 *
 * return: void
 **********************************************************************************************************************/

bool s_readSecureFlash_usingSecureRamCode(void)
{
    setTestDatavalueConst(s_dataConst);
    SECURE_PADDING;

    if(SECURE_DATA_CONST_VALUE == getvalueConst())
    {
        return(true);
    }
    else
    {
        return(false);
    }
}

/*******************************************************************************************************************//**
 * @brief s_read_non_secureFlash_usingSecureRamCode function
 * This function shows secure SRAM program can read non-secure flash region.
 *
 * return: void
 **********************************************************************************************************************/

bool s_read_non_secureFlash_usingSecureRamCode(void)
{
    setTestDatavalueConst(u_dataConst);
    SECURE_PADDING;

    if(DATA_CONST_VALUE == getvalueConst())
    {
        return(true);
    }
    else
    {
        return(false);
    }
}

/*******************************************************************************************************************//**
 * @brief s_writeSecureFlash_usingSecureRamCode function
 * This function uses the secure SRAM program to program FLASH_WRITE_LENGTH or SECURE_FLASH_WRITE_LENGTH bytes to
 * secure flash memory starting at FLASH_WRITE_TEST_BLOCK1.
 * 1. show secure SRAM program can write to secure flash region if this region is included in the FAW region
 *
 * 2. show secure SRAM program cannot write to secure flash region if this region is not included in the FAW region
 *
 * return: void
 **********************************************************************************************************************/

bool s_writeSecureFlash_usingSecureRamCode(void)
{
	return !flash_write(FLASH_WRITE_TEST_BLOCK1, ns_writeBuffer);
}

/*******************************************************************************************************************//**
 * @brief s_write_non_secureFlash_usingSecureRamCode function
 * This function writes FLASH_WRITE_LENGTH or SECURE_FLASH_WRITE_LENGTH bytes of data to the non-secure flash region
 * starting from 0xf0000 using secure SRAM program
 * If the FAW is setup from FAW_START to FAW_END, the flash erase and write will fail.
 * If the FAW is not setup (no write protected region exists), the flash erase and write will succeed.
 * If the FAW region does cover the flash block starting from 0xf0000 - for example, if the FAW is setup to 0xf0000 to 0x1FFFFF,
 * .....................................................................................the flash erase and write will also succeed.
 *  input: writeBuffer - the data to be written to the flash.
 *
 * return: void
 **********************************************************************************************************************/

bool s_write_non_secureFlash_usingSecureRamCode(void)
{
	return !flash_write(FLASH_WRITE_TEST_BLOCK2, ns_writeBuffer);
}
