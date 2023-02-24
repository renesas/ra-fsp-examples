/***********************************************************************************************************************
 * File Name    : non_secure_flash_functions.c
 * Description  : Contains test routines running out of non_secure flash regions
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

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "non_secure_flash_functions.h"
#include "secure_flash_functions.h"
#include "hal_data.h"
#include "common_utils.h"
#include "DAR_utilities.h"

/* global variable */
/* temporary variable used to exercise the data access to different regions*/
typedef struct  {
	volatile uint32_t valueBss;
	volatile uint32_t valueData;
	volatile uint32_t valueConst;
} Non_Secure_TestData;

/*
 * non-secure global variables
 * to verify non-secure data can be read/write from non-secure and non-secure program
 * define several global variables and DO NOT provide access function
 */
const       uint32_t u_dataConst = DATA_CONST_VALUE;
volatile    uint32_t u_dataInit = DATA_VALUE;
volatile    uint32_t dataWritten;

/* local variable */
static Non_Secure_TestData testData;
static volatile    uint32_t u_dataBss = 0;
static uint8_t readBuffer[FLASH_WRITE_LENGTH];
uint8_t ns_writeBuffer[FLASH_WRITE_LENGTH];

/*******************************************************************************************************************//**
 * @brief ns_setup_the_writeBuffer function
 * This function fills the buffer to write to secure and non-secure flash
 *
 * return: void
 **********************************************************************************************************************/
void ns_setup_the_writeBuffer(void)
{
    volatile int i=0;

    for (i=0; i<FLASH_WRITE_LENGTH; i++)
            ns_writeBuffer[i]=FLASH_WRITE_TEST_DATA;
}
/*get and set function for static variables*/

/* get Bss test data*/
uint32_t getDataBss()
{
	return u_dataBss;
}
/* set Bss test data*/
void setTestDatavalueBss(uint32_t valueBss)
{
	testData.valueBss = valueBss;
}
/* set test data */
void setTestDatavalueData(uint32_t valueData)
{
	testData.valueData = valueData;
}
/* set test constant */
void setTestDatavalueConst(uint32_t valueConst)
{
	testData.valueConst = valueConst;
}
/* get test constant*/
uint32_t getvalueConst()
{
	return testData.valueConst;
}
/* get test data */
uint32_t getvalueData()
{
	return testData.valueData;
}
/* get Bss test data */
uint32_t getvalueBss()
{
	return testData.valueBss;
}

/*******************************************************************************************************************//**
 * function: writeSecureRam_using_non_secureFlashCode
 * description:
 * write DATA_WRITE_TEST_VALUE to secure sram location
 * show non-secure flash program can not write to secure sram location
 *
 * return: bool
 **********************************************************************************************************************/

bool writeSecureRam_using_non_secureFlashCode(void)
{
	/* write to secure SRAM variable from non-secure flash program*/
    s_dataWritten = DATA_WRITE_TEST_VALUE;

    /* use the secure access function to retrieve value written */
    testData.valueData = get_s_dataWritten();

    /* test to prove that secure SRAM variable is not written */
    if(DATA_WRITE_TEST_VALUE == testData.valueData)
    {
        return(true);
    }
    else
    {
        return(false);
    }
}

/*******************************************************************************************************************//**
 * function: write_non_secureRam_using_non_secureFlashCode
 * description:
 * write DATA_WRITE_TEST_VALUE to non-secure sram location
 * show non-secure flash program can write to non-secure sram location
 *
 * return: bool
 **********************************************************************************************************************/

bool write_non_secureRam_using_non_secureFlashCode(void)
{

    dataWritten = DATA_WRITE_TEST_VALUE;
    setTestDatavalueData(dataWritten);

    if(DATA_WRITE_TEST_VALUE == getvalueData())
    {
        return(true);
    }
    else
    {
        return(false);
    }

}
/*******************************************************************************************************************//**
 * function: flash_write
 * description:
 * write FLASH_WRITE_LENGTH or SECURE_FLASH_WRITE_LENGTH bytes to certain flash address
 * return: bool
 **********************************************************************************************************************/

bool flash_write(uint32_t Flash_address, uint8_t *writeBuffer)
{
	static volatile fsp_err_t  err = 0;
	uint32_t end = 0;
	flash_result_t blank_check_result = FLASH_RESULT_BLANK;

	/* check SecurityMPU settings */
    uint16_t secMpuAc = 0;  /* variable to hold the Security MPU access register setting */
    uint32_t start = 0;

    s_readSecureMpuSettings(&secMpuAc);

    /* PC region 0 */
    if (!(secMpuAc & (1<<8u)))
    {
        err = R_FLASH_HP_Open(&g_flash0_ctrl, &g_flash0_cfg);
        APP_ERR_TRAP(err);
        __disable_fault_irq();

        s_readRegion0StartAddress(&start);
        s_readRegion0EndAddress(&end);

        if (Flash_address> end )
        {
            /*the flash write address in located in non-secure region */
            /* the erase will succeed if the flash erase/write area in inside FAW */
            if (FSP_SUCCESS == R_FLASH_HP_Erase(&g_flash0_ctrl, Flash_address, NUM_OF_FLASH_SECTOR))
            {
                /*blank check will succeed if the flash region is in non-secure region */
                if (FSP_SUCCESS == R_FLASH_HP_BlankCheck(&g_flash0_ctrl, Flash_address, FLASH_SECTOR_SIZE_32KB, &blank_check_result))
                {
                    if (FLASH_RESULT_BLANK == blank_check_result)
                    {
                        if (FSP_SUCCESS == R_FLASH_HP_Write(&g_flash0_ctrl, (uint32_t)writeBuffer, Flash_address, FLASH_WRITE_LENGTH))
                        {
                            /*Read code flash data */
                            memcpy(readBuffer, (uint8_t *) Flash_address, FLASH_WRITE_LENGTH);
                            if(!memcmp(writeBuffer, readBuffer, FLASH_WRITE_LENGTH))
                            {
                                err = R_FLASH_HP_Close(&g_flash0_ctrl);
                                APP_ERR_TRAP(err);
                                __enable_fault_irq();
                                return false;
                            }
                        }
                    }
                }
            }
        }
        else
        {
            /*the flash write address in located in secure region */
            /* the erase will succeed if the flash erase/write area in inside FAW */
           if (FSP_SUCCESS == R_FLASH_HP_Erase(&g_flash0_ctrl, Flash_address, NUM_OF_FLASH_SECTOR))
           {
               /* flash write should succeed if erase is successful */
             if (FSP_SUCCESS == R_FLASH_HP_Write(&g_flash0_ctrl, (uint32_t)writeBuffer, Flash_address, FLASH_WRITE_LENGTH))
               {
                   err = R_FLASH_HP_Close(&g_flash0_ctrl);
                   APP_ERR_TRAP(err);
                   __enable_fault_irq();
                   return false;
               }
           }
        }
        err = R_FLASH_HP_Close(&g_flash0_ctrl);
        APP_ERR_TRAP(err);
        __enable_fault_irq();
    }
   else
   {
       APP_PRINT("Security MPU not enabled, check BSP Configuration\n\r");
   }

	/* when one of the above operation fails, system will return flash_write fail */
    /* notice that this function uses a reverse logic for true false */
    return true;

}

/*******************************************************************************************************************//**
 * function: writeSecureFlash_using_non_secureFlashCode
 * description:
 * write FLASH_WRITE_LENGTH or SECURE_FLASH_WRITE_LENGTH bytes to secure flash region starting at FLASH_WRITE_TEST_BLOCK1
 * 1. show non-secure flash program can write to secure flash region if this region is included in the FAW region
 *    this can be confirmed when FAW region is not setup
 * 2. show non-secure flash program cannot write to secure flash region if this region is not included in the FAW region
 *    this can be confirmed if FAW region is setup to FAW_STRAT to FAW_END
 * return: bool
 **********************************************************************************************************************/

bool writeSecureFlash_using_non_secureFlashCode(uint8_t *writeBuffer)
{
	return !flash_write(FLASH_WRITE_TEST_BLOCK1, writeBuffer);
}
/*******************************************************************************************************************//**
 * @brief write_non_secureFlash_using_non_secureFlashCode function
 * This function writes FLASH_WRITE_LENGTH or SECURE_FLASH_WRITE_LENGTH bytes of data to the
 * non-secure flash region starting from FLASH_WRITE_TEST_BLOCK2.
 * If the FAW is setup from FAW_START to FAW_END, the flash erase and write will fail.
 * If the FAW is not setup (no write protected region exists), the flash erase and write will succeed.
 * If the FAW region does cover the flash block starting from 0xf0000 - for example, if the FAW is setup to 0xf0000 to 0x1FFFFF,
 * .....................................................................................the flash erase and write will also succeed.
 * input: writeBuffer - the data to be written to the flash.
 *
 * return: bool
 **********************************************************************************************************************/
bool write_non_secureFlash_using_non_secureFlashCode(uint8_t *writeBuffer)
{
	return !flash_write(FLASH_WRITE_TEST_BLOCK2, writeBuffer);
}

/*******************************************************************************************************************//**
 * @brief readSecureFlash_using_non_secureFlashCode function
 * This function reads two secure SRAM variables from the non-secure flash region.
 *
 * return: bool
 **********************************************************************************************************************/

bool readSecureFlash_using_non_secureFlashCode(void)
{
	testData.valueConst = s_dataConst;

    if(SECURE_DATA_CONST_VALUE == testData.valueConst)
    {
        return(true);
    }
    else
    {
        return(false);
    }
}

/*******************************************************************************************************************//**
 * @brief readSecureRam_using_non_secureFlashCode function
 * This function reads two secure SRAM variables from the secure region.
 * The first secure variable s_dataBss is a private variable for secure_functions.c.
 * This function shows non-secure function can get the value of a secure varaible via the access function provided by the secure module
 * The second secure variable s_dataInit is a global data located in the secure flash region
 * This functions shows non-secure function cannot read secure SRAM data.
 *
 * return: bool
 **********************************************************************************************************************/

bool readSecureRam_using_non_secureFlashCode(void)
{
    testData.valueBss = s_dataBss;
    testData.valueData = s_dataInit;

    if((0 == testData.valueBss) || (SECURE_DATA_VALUE != testData.valueData))
    {
        return(true);
    }
    else
    {
        return(false);
    }
}
/*******************************************************************************************************************//**
 * @brief read_non_secureFlash_using_non_secureFlashCode function
 * This function reads a non_secure flash variable and shows non-secure flash program can read non-secure flash region.
 *
 * return: bool
 **********************************************************************************************************************/
bool read_non_secureFlash_using_non_secureFlashCode(void)
{
    setTestDatavalueConst(u_dataConst);
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
 * @brief read_non_secureRam_using_non_secureFlashCode function
 * This function reads a non-secure sram variable and shows non-secure flash program can read non-secure sram region.
 *
 * return: bool
 **********************************************************************************************************************/

bool read_non_secureRam_using_non_secureFlashCode(void)
{
    setTestDatavalueBss(getDataBss());
    setTestDatavalueData(u_dataInit);

    if((0 == getvalueBss()) && (DATA_VALUE == getvalueData()))
    {
        return(true);
    }
    else
    {
        return(false);
    }
}




