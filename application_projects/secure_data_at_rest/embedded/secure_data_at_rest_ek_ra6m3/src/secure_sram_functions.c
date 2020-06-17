/***********************************************************************************************************************
 * File Name    : secure_sram_functions.c
 * Description  : Contains implementations of secure functions running out of secure sram
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

#include <stdbool.h>
#include "secure_sram_functions.h"
#include "non_secure_flash_functions.h"
#include "secure_flash_functions.h"
#include "hal_data.h"
#include "secure_definitions.h"

#include "DAR_utilities.h"

SECURE_DATA uint8_t s_writeBuffer[SECURE_FLASH_WRITE_LENGTH];

/*******************************************************************************************************************//**
 * @brief s_setup_the_writeBuffer function
 * This function fills the buffer to write to secure and non-secure flash
 *
 * return: void
 **********************************************************************************************************************/

void s_setup_the_writeBuffer(void)
{
    volatile int i=0;

    for (i=0; i<SECURE_FLASH_WRITE_LENGTH; i++)
            s_writeBuffer[i]=FLASH_WRITE_TEST_DATA;
}
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
 * secure flash memory starting at 0xb0000.
 * 1. show secure SRAM program can write to secure flash region if this region is included in the FAW region
 *    this can be confirmed when FAW region is not setup
 * 2. show secure SRAM program cannot write to secure flash region if this region is not included in the FAW region
 *    this can be confirmed if FAW region is setup to 0x100000 to 0x200000
 * return: void
 **********************************************************************************************************************/

bool s_writeSecureFlash_usingSecureRamCode(void)
{
	return !flash_write(FLASH_WRITE_TEST_BLOCK1, s_writeBuffer);
}

/*******************************************************************************************************************//**
 * @brief s_write_non_secureFlash_usingSecureRamCode function
 * This function writes FLASH_WRITE_LENGTH or SECURE_FLASH_WRITE_LENGTH bytes of data to the non-secure flash region
 * starting from 0xf0000 using secure SRAM program
 * If the FAW is setup from 0x100000 to 0x1FFFFF, the flash erase and write will fail.
 * If the FAW is not setup (no write protected region exists), the flash erase and write will succeed.
 * If the FAW region does cover the flash block starting from 0xf0000 - for example, if the FAW is setup to 0xf0000 to 0x1FFFFF,
 * .....................................................................................the flash erase and write will also succeed.
 *  input: writeBuffer - the data to be written to the flash.
 *
 * return: void
 **********************************************************************************************************************/

bool s_write_non_secureFlash_usingSecureRamCode(void)
{
	return !flash_write(FLASH_WRITE_TEST_BLOCK2, s_writeBuffer);
}
