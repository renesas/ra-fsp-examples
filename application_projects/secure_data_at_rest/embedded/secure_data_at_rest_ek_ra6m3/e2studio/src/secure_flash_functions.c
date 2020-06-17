/***********************************************************************************************************************
 * File Name    : secure_flash_functions.c
 * Description  : Contains implementations of secure functions running out of secure flash
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


#include <stdbool.h>
#include "secure_flash_functions.h"
#include "non_secure_flash_functions.h"
#include "hal_data.h"
#include "common_utils.h"
#include "DAR_utilities.h"

/*
 * secure data
 * If user want the secure variable to be manipulated by other non-secure or secure modules, create static variable
 * and provide secure access interface for these variables
 */
SECURE_BSS volatile     uint32_t s_dataBss;

/*
 * secure data
 * If user wants to hide the secure variables from any other module (non-secure or secure alike), declare them as static variables
 * and do not provide access functions to these variables
 */
SECURE_DATA volatile uint32_t s_dataInit2 = SECURE_DATA_VALUE;

/*
 * secure data
 * to verify secure data can not be read/write from non-secure program
 * define several global variables and DO NOT provide secure access function for them (because non-secure program can call secure functions)
 */

SECURE_CONST const      uint32_t s_dataConst = SECURE_DATA_CONST_VALUE;
SECURE_DATA  volatile   uint32_t s_dataInit = SECURE_DATA_VALUE;
SECURE_DATA  volatile   uint32_t s_dataWritten;

#define FAW_START	(0x100000) // flash access window start
#define FAW_END		(0x200000)  // flash access window ends

/*
 * this is a helper function to verify write to secure SRAM from non-secure program is not allowed
 */
uint32_t get_s_dataWritten()
{
	return s_dataWritten;
}

/*******************************************************************************************************************//**
 * @brief s_secureDataInit function
 * This function initializes the secure variables.
 *
 * return: void
 **********************************************************************************************************************/
void SECURE_PROGRAM  s_secureDataInit(void)
{
    /* need to do this here since the sections are not yet covered by the standard
     * BSP initialization
    */
    s_dataBss = 0;
    s_dataInit = SECURE_DATA_VALUE;

    SECURE_PADDING;
}

/***********************************************************************************************************************
* Function Name: secure_sram_section_copy
* Description  : Zero out input section
* Arguments    : psource -
*                    Address of where to copy data from
*                pdest -
*                    Address of where to copy data to
*                bytes -
*                    Size of section in bytes
* Return Value : none
***********************************************************************************************************************/
void secure_sram_section_copy (uint8_t * psource, uint8_t * pdest, uint32_t bytes)
{
    uint32_t index = 0;
    for (index = 0U; index < bytes; index++,pdest++,psource++)
    {
        *pdest = *psource;
    }
}

/*******************************************************************************************************************//**
 * @brief s_writeSecureRam_usingSecureFlashCode function
 * This function shows secure flash program can write to secure sram region.
 *
 * return: void
 **********************************************************************************************************************/
bool s_writeSecureRam_usingSecureFlashCode(void)
{

    setTestDatavalueData(SECURE_DATA_WRITE_TEST_VALUE);
    s_dataWritten = getvalueData();

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
 * @brief s_write_non_secureRam_usingSecureFlashCode function
 * This function shows secure flash program can write to non-secure SRAM region
 *
 * return: void
 **********************************************************************************************************************/

bool s_write_non_secureRam_usingSecureFlashCode(void)
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
 * @brief s_readSecureRam_usingSecureFlashCode function
 * This function shows secure flash program can read secure SRAM region.
 *
 * return: void
 **********************************************************************************************************************/

bool s_readSecureRam_usingSecureFlashCode(void)
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
 * @brief s_read_non_secureRam_usingSecureFlashCode function
 * This function shows secure flash program can read non-secure SRAM region.
 *
 * return: void
 **********************************************************************************************************************/

bool s_read_non_secureRam_usingSecureFlashCode(void)
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
 * @brief s_readSecureFlash_usingSecureFlashCode function
 * This function shows secure flash program can read secure flash region.
 *
 * return: void
 **********************************************************************************************************************/

bool s_readSecureFlash_usingSecureFlashCode(void)
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
 * @brief s_read_non_secureFlash_usingSecureFlashCode function
 * This function shows secure flash program can read non-secure flash region.
 *
 * return: void
 **********************************************************************************************************************/

bool s_read_non_secureFlash_usingSecureFlashCode(void)
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
 * @brief s_setup_faw function
 * This function sets up the FAW region between FAW_START and FAW_END.
 *
 * return: void
 **********************************************************************************************************************/

bool s_setup_faw(void)
{
	static volatile fsp_err_t err;

	err = R_FLASH_HP_Open(&g_flash0_ctrl, &g_flash0_cfg);
	APP_ERR_TRAP(err);

	/*Setup the access window to be between FAW_START and FAW_END */
	err = R_FLASH_HP_AccessWindowSet(&g_flash0_ctrl, FAW_START, FAW_END);
	APP_ERR_TRAP(err);

	err = R_FLASH_HP_Close(&g_flash0_ctrl);
	APP_ERR_TRAP(err);

    SECURE_PADDING;

    if(FSP_SUCCESS == err )
    {
        return(true);
    }
    else
    {
        return(false);
    }
}

/*******************************************************************************************************************//**
 * @brief s_reset_faw function
 * This function clears the FAW window (disables FAW).
 *
 * return: void
 **********************************************************************************************************************/

bool s_reset_faw(void)
{
    static volatile fsp_err_t  err;

    /* Configure and open the Flash API */
    err = R_FLASH_HP_Open(&g_flash0_ctrl, &g_flash0_cfg);
    APP_ERR_TRAP(err);

    /* Remove any prior access window */
    err = R_FLASH_HP_AccessWindowClear(&g_flash0_ctrl);
    APP_ERR_TRAP(err);

    /* close flash API */
    err = R_FLASH_HP_Close(&g_flash0_ctrl);
    APP_ERR_TRAP(err);

    SECURE_PADDING;
    if (FSP_SUCCESS == err)
    {
    	return true;
    }
    else
    {
        return(false);
    }

}

