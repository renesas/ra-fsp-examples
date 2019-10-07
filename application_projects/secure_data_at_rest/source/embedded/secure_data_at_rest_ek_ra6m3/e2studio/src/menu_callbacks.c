/***********************************************************************************************************************
 * File Name    : menu_callbacks.c
 * Description  : Contains user command processing
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
#include "stdio.h"
#include "menu_callbacks.h"
#include "secure_flash_functions.h"
#include "non_secure_flash_functions.h"
#include "secure_sram_functions.h"
#include "RTT_user_interface.h"
#include "common_utils.h"
#include "DAR_utilities.h"


/******************************************************************************
 global definitions
 ******************************************************************************/

/******************************************************************************
 local definitions
 ******************************************************************************/

static uint8_t writeBuffer[FLASH_WRITE_LENGTH];    /* buffer for flash writing */
static void provision_test_variables(void);
static volatile bool success = false;

/*******************************************************************************************************************//**
 * @brief provision_test_variables function
 * This function set the test variables to support secure and non-secure program validation
 *
 * return: void
 **********************************************************************************************************************/
static void provision_test_variables()
{
    setTestDatavalueConst(DUMMY_VALUE);
    setTestDatavalueBss(DUMMY_VALUE);
    setTestDatavalueData(DUMMY_VALUE);
}

/*******************************************************************************************************************//**
 * @brief setup_the_writeBuffer function
 * This function fills the buffer to write to secure and non-secure flash
 *
 * return: void
 **********************************************************************************************************************/

static void setup_the_writeBuffer()
{
    volatile int i=0;

    for (i=0; i<FLASH_WRITE_LENGTH; i++)
            writeBuffer[i]=FLASH_WRITE_TEST_DATA;
}

/*******************************************************************************************************************//**
 * @brief read_secure_settings function
 * This function reads and prints security mpu and faw settings:
 *  reads and prints secure program region 0 setting (flash secure program)
 *  reads and prints secure program region 1 setting (sram secure program)
 *  reads and prints secure data region 0 setting (secure flash data)
 *  reads and prints secure data region 1 setting (secure sram data)
 *  reads and prints secure data region 2 setting (security function 1)
 *  reads and prints secure data region 3 setting (security function 2)
 *
 * return: void
 **********************************************************************************************************************/

void read_secure_settings(void)
{

    uint16_t secMpuAc = 0;  /* variable to hold the Security MPU access register setting */
    uint32_t start = 0;
    uint32_t end = 0;
    uint32_t fawe = 0;
    uint32_t faws = 0;

    s_readSecureMpuSettings(&secMpuAc);

    APP_PRINT("\r\n**************************\n\r Security MPU settings\n\r\n\r");
    /* PC region 0 */
    if (secMpuAc & (1<<8u))
    {
        APP_PRINT("PC Region 0:\t\tdisabled\n\r");
    }
    else
    {
        s_readPc0StartAddress(&start);
        s_readPc0EndAddress(&end);
        APP_PRINT("PC Region 0:\t\tenabled\t[0x%8X | 0x%8X]\n\r", (unsigned int)start, (unsigned int)end);
    }

    /* PC region 1 */
    if (secMpuAc & (1<<9u))
    {
    	APP_PRINT("PC Region 1:\t\tdisabled\n\r");
    }
    else
    {
        s_readPc1StartAddress(&start);
        s_readPc1EndAddress(&end);
        APP_PRINT("PC Region 1:\t\tenabled\t[0x%8X | 0x%8X]\n\r", (unsigned int)start, (unsigned int)end);
    }

    /* Region 0 */
    if (secMpuAc & (1<<0u))
    {
    	APP_PRINT("Secure Region 0:\tdisabled\n\r");
    }
    else
    {
        s_readRegion0StartAddress(&start);
        s_readRegion0EndAddress(&end);
        APP_PRINT("Secure Region 0:\tenabled\t[0x%8X | 0x%8X]\n\r", (unsigned int)start, (unsigned int)end);
    }

    /* region 1 */
    if (secMpuAc & (1<<1u))
    {
    	APP_PRINT("Secure Region 1:\tdisabled\n\r");
    }
    else
    {
        s_readRegion1StartAddress(&start);
        s_readRegion1EndAddress(&end);
        APP_PRINT("Secure Region 1:\tenabled\t[0x%8X | 0x%8X]\n\r", (unsigned int)start, (unsigned int)end);
    }

    /* region 2 */
    if (secMpuAc & (1<<2u))
    {
    	APP_PRINT("Secure Region 2:\tdisabled\n\r");
    }
    else
    {
        s_readRegion2StartAddress(&start);
        s_readRegion2EndAddress(&end);
        APP_PRINT("Secure Region 2:\tenabled\t[0x%8X | 0x%8X]\n\r", (unsigned int)start, (unsigned int)end);
    }

    /* region 3 */
    if (secMpuAc & (1<<3u))
    {
    	APP_PRINT("Secure Region 3:\tdisabled\n\r");
    }
    else
    {
        s_readRegion3StartAddress(&start);
        s_readRegion3EndAddress(&end);
        APP_PRINT("Secure Region 3:\tenabled\t[0x%8X | 0x%8X]\n\r", (unsigned int)start, (unsigned int)end);
    }

    /* flash access window */
    s_read_faw_settings(&faws, &fawe);

    if (faws == fawe)
    {
    	APP_PRINT("\nAccess Window:\t\tdisabled\n\r");
    }
    else
    {
    	APP_PRINT("\nAccess Window:\t\tenabled\t[0x%8X | 0x%8X]\n\r", (unsigned int)faws, (unsigned int)fawe);
    }
}


/*******************************************************************************************************************//**
 * @brief secure_code_read function
 * This function exercises data reading using secure flash code:
 *  read secure flash variable
 *  read secure sram variable
 *  read non-secure flash variable
 *  read non-secure sram variable
 *
 * return: void
 **********************************************************************************************************************/

void secure_code_read(void)
{
	APP_PRINT("**************************\n\r");
	APP_PRINT("\nSecure code reads...running from secure flash\n\r");

    /* reset variable to enable valid test */
    provision_test_variables();
    success = s_readSecureFlash_usingSecureFlashCode();

    if(success)
    {
    	APP_PRINT("\nPASS! secure flash program can read secure flash\n\r");
    }
    else
    {
    	APP_PRINT("\n\n!FAIL! secure flash program cannot read secure flash\n\r");
    }

    /* reset variable to enable valid test */
    provision_test_variables();
    success = s_readSecureRam_usingSecureFlashCode();

    if(success)
    {
    	APP_PRINT("\nPASS! secure flash program can read secure ram\n\r");
    }
    else
    {
    	APP_PRINT("\n!FAIL! secure flash program cannot read secure ram\n\r");
    }
    /* reset variable to enable valid test */
    provision_test_variables();
    success = s_read_non_secureFlash_usingSecureFlashCode();

    if(success)
    {
    	APP_PRINT("\nPASS! secure flash program can read non-secure flash\n\r");
   }
    else
    {
    	APP_PRINT("\n!FAIL! secure flash program cannot read non-secure flash\n\r");
    }

    /* reset variable to enable valid test */
    provision_test_variables();
    success = s_read_non_secureRam_usingSecureFlashCode();

    if(success)
    {
    	APP_PRINT("\nPASS! secure flash program can read non-secure ram\n\r");
    }
    else
    {
    	APP_PRINT("\n!FAIL! secure flash program cannot read non-secure ram\n\r");
    }
}

/*******************************************************************************************************************//**
 * @brief secure_sram_code_read function
 * This function exercise data reading using secure sram code:
 *  read secure flash variable
 *  read secure sram variable
 *  read non-secure flash variable
 *  read non-secure sram variable
 *
 * return: void
 **********************************************************************************************************************/

void secure_sram_code_read(void)
{
	APP_PRINT("\n**************************\n\r");
	APP_PRINT("\nSecure sram code reads...running from secure sram\n\r");

    /* reset variable to enable valid test */
    provision_test_variables();
    success = s_readSecureFlash_usingSecureRamCode();

    if(success)
    {
    	APP_PRINT("\nPASS! secure sram program can read secure flash\n\r");
    }
    else
    {
    	APP_PRINT("\n!FAIL! secure sram program cannot read secure flash\n\r");
    }
    /* reset variable to enable valid test */
    provision_test_variables();
    success = s_readSecureRam_usingSecureRamCode();

    if(success)
    {
    	APP_PRINT("\nPASS! secure sram program can read secure ram\n\r");
    }
    else
    {
    	APP_PRINT("\n!FAIL! secure sram program cannot read secure ram\n\r");
    }

    /* reset variable to enable valid test */
    provision_test_variables();
    success = s_read_non_secureFlash_usingSecureRamCode();

    if(success)
    {
    	APP_PRINT("\nPASS! secure sram program can read non-secure flash\n\r");
    }
    else
    {
    	APP_PRINT("\n!FAIL! secure sram program cannot read non-secure flash\n\r");
    }

    /* reset variable to enable valid test */
    provision_test_variables();
    success = s_read_non_secureRam_usingSecureRamCode();

    if(success)
    {
    	APP_PRINT("\nPASS! secure sram program can read non-secure ram\n\r");
    }
    else
    {
    	APP_PRINT("\n!FAIL! secure sram program cannot read non-secure ram\n\r");
    }

}

/*******************************************************************************************************************//**
 * @brief non_secure_code_read function
 * This function exercises data reading using non-secure flash code:
 *  read secure flash variable
 *  read secure sram variable
 *  read non-secure flash variable
 *  read non-secure sram variable
 *
 * return: void
 **********************************************************************************************************************/

void non_secure_code_read(void)
{
	APP_PRINT("**************************\n\r");
	APP_PRINT("\nnon-secure code reads...running from non-secure flash\n\r");

    /* reset variable to enable valid test */
    provision_test_variables();
    success = readSecureFlash_using_non_secureFlashCode();

    if(success)
    {
    	APP_PRINT("\n!FAIL! non-secure program can read secure flash (not good)\n\r");
    }
    else
    {
    	APP_PRINT("\nPASS! non-secure program cannot read secure flash\n\r");
    }

    /* reset variable to enable valid test */
    provision_test_variables();
    success = readSecureRam_using_non_secureFlashCode();

    if(success)
    {
    	APP_PRINT("\nPASS! non-secure program cannot read secure sram \n\r");
    }
    else
    {
    	APP_PRINT("\nFAIL! non-secure program can read secure ram data \n\r");
    }

    /* reset variable to enable valid test */
    provision_test_variables();
    success = read_non_secureFlash_using_non_secureFlashCode();

    if(success)
    {
    	APP_PRINT("\nPASS! non-secure program can read non-secure flash\n\r");
    }
    else
    {
    	APP_PRINT("\n!FAIL! non-secure program cannot read non-secure flash\n\r");
    }

    /* reset variable to enable valid test */
    provision_test_variables();
    success = read_non_secureRam_using_non_secureFlashCode();

    if(success)
    {
    	APP_PRINT("\nPASS! non-secure program can read non-secure sram\n\r");
    }
    else
    {
    	APP_PRINT("\n!FAIL! non-secure program cannot read non-secure sram\n\r");
   }

    /* reset variable to enable valid test */
    provision_test_variables();
}
/*******************************************************************************************************************//**
 * @brief secure_code_write function
 * This function exercise data writing using secure flash code:
 *  write to secure sram
 *  write to non-secure sram
 *
 * return: void
 **********************************************************************************************************************/

void secure_code_write(void)
{
	APP_PRINT("**************************\n\r");
	APP_PRINT("\nSecure code writes...running from secure flash\n\r");

    /* reset variable to enable valid test */
    setTestDatavalueData(DUMMY_VALUE);
    success = s_writeSecureRam_usingSecureFlashCode();

    if(success)
    {
    	APP_PRINT("\nPASS! secure flash program can write secure sram data\n\r");
    }
    else
    {
    	APP_PRINT("\n!FAIL! secure flash program cannot write secure sram data\n\r");
    }

    /* reset variable to enable valid test */
    setTestDatavalueData(DUMMY_VALUE);
    success = s_write_non_secureRam_usingSecureFlashCode();

    if(success)
    {
    	APP_PRINT("\nPASS! secure flash program  can write non-secure sram data\n\r");
   }
    else
    {
    	APP_PRINT("\n!FAIL! secure flash program cannot write non-secure sram data\n\r");
   }

  }

/*******************************************************************************************************************//**
 * @brief secure_sram_code_write
 * This function exercises data writing using secure sram code:
 *  write to secure sram
 *  write to non-secure sram
 *  write to secure flash
 *  write to non-secure flash
 *
 * return: void
 **********************************************************************************************************************/

void secure_sram_code_write(void)
{
    uint32_t fawe = 0;
    uint32_t faws = 0;

    APP_PRINT("**************************\n\r");
    APP_PRINT("\nSecure sram code writes...running from secure sram\n\r");

    /*setup FAW from 0x100000 to 0x200000 to protect the secure flash data region and portions of the non-secure flash region
    * then write to the secure flash block located at 0xb0000 to show
    * so secure sram code cannot write to the secure data region (0x400 to 0xDFFFF)*/

    success = s_setup_faw();
    /* confirm flash access window */
    s_read_faw_settings(&faws, &fawe);
    if (faws == fawe)
    {
    	APP_PRINT("\nAccess Window:\t\tsetup failed\n\r");
        APP_ERR_TRAP(1);
    }
    else
    {
    	APP_PRINT("\nAccess Window:\t\tenabled\t[0x%8X | 0x%8X]\n\r", (unsigned int)faws, (unsigned int)fawe );
    }

    /* reset variable to enable valid test */
    setTestDatavalueData(DUMMY_VALUE);
    success = s_writeSecureRam_usingSecureRamCode();

    if(success)
    {
    	APP_PRINT("\nPASS! secure sram program can write secure sram data\n\r");
    }
    else
    {
    	APP_PRINT("\n!FAIL! secure sram program cannot write secure sram data\n\r");
    }

    /* reset variable to enable valid test */
    setTestDatavalueData(DUMMY_VALUE);
    success = s_write_non_secureRam_usingSecureRamCode();

    if(success)
    {
    	APP_PRINT("\nPASS! secure sram program can write non-secure sram data\n\r");
    }
    else
    {
    	APP_PRINT("\n!FAIL! secure sram program cannot write non-secure sram data\n\r");
    }

    /* prepare flash write */
    s_setup_the_writeBuffer();
    success = s_writeSecureFlash_usingSecureRamCode();
    if(success)
    {
    	APP_PRINT("\n!FAIL! secure sram program can write to secure flash which is unmodifiable by FAW\n\r");
    }
    else
    {
    	APP_PRINT("\nPASS! secure sram program cannot write to secure flash region 0xb0000\n\r       which is unmodifiable by FAW\n\r");
    }

    /*write to the non-secure flash block located at 0x120000 to show
    * secure SRAM code can write to the non-secure flash region if
    * this non-secure flash region is modifiable by FAW (within FAW region from 0x100000 to 0x200000)*/
    s_setup_the_writeBuffer();
    success = s_write_non_secureFlash_usingSecureRamCode();
    if(success)
    {
    	APP_PRINT("\n!FAIL! secure sram program cannot program non-secure flash \n\r      which should be modifiable based on FAW setting.\n\r");
    }
    else
    {
    	APP_PRINT("\nPASS! secure sram program can write to non-secure flash 0x120000\n\r       which is modifiable by FAW\n\r");
    }

}

/*******************************************************************************************************************//**
 * @brief non_secure_code_write function
 * This function exercises data writing using non-secure flash code:
 *  write to secure sram (show non-secure flash code cannot write to secure sram)
 *  write to non-secure sram (show non-secure flash code can write to non-secure sram)
 *  write to secure flash
 *      show non-secure flash code can not write to secure flash if the secure flash is not part of the FAW region
 *      show non-secure flash code can write to secure flash if the secure flash is part of the FAW region
 *  write to non-secure flash
 *      show non-secure flash can write to non-secure flash if the non-secure flash is part of FAW region
 *      show non-secure flash can not write to non-secure flash if the non-secure flash is not part of the FAW region
 *
 * return: void
 **********************************************************************************************************************/

void non_secure_code_write(void)
{
    uint32_t fawe = 0;
    uint32_t faws = 0;

    APP_PRINT("**************************\n\r");
    APP_PRINT("\nnon-secure flash code tests...running from non-secure flash\n\r");

    /*setup FAW from 0x100000 to 0x200000 to protect the secure flash data region and portions of the non-secure flash region
       then write to the secure flash block located at 0xb0000 to show
       so non-secure code cannot write to the secure data region (0x400 to 0xDFFFF) */
    success = s_setup_faw();
    /* confirm flash access window */
    s_read_faw_settings(&faws, &fawe);
    if (faws == fawe)
    {
    	APP_PRINT("\nAccess Window:\t\tsetup failed\n\r");
        APP_ERR_TRAP(1);
    }
    else
    {
    	APP_PRINT("\nAccess Window:\t\tenabled\t[0x%8X | 0x%8X]\n\r", (unsigned int)faws, (unsigned int)fawe);
    }
    setTestDatavalueData(DUMMY_VALUE);
    success = writeSecureRam_using_non_secureFlashCode();

    if(success)
    {
    	APP_PRINT("\n!FAIL! non-secure flash program can write secure sram data(not good)\n\r");
    }
    else
    {
    	APP_PRINT("\nPASS! non-secure flash program cannot write secure sram data\n\r");
    }
    setTestDatavalueData(DUMMY_VALUE);
    success = write_non_secureRam_using_non_secureFlashCode();

    if(success)
    {
    	APP_PRINT("\nPASS! non-secure flash program can write to non-secure sram data\n\r");
    }
    else
    {
    	APP_PRINT("\n!FAIL! non-secure flash program cannot write non-secure sram data\n\r");
    }
    setup_the_writeBuffer();

    success = writeSecureFlash_using_non_secureFlashCode(writeBuffer);

    if(success)
    {
    	APP_PRINT("\nPASS! non-secure flash program cannot write to secure flash region 0xb0000\n\r       which is unmodifiable by FAW\n\r");
    }
    else
    {
    	APP_PRINT("\n!FAIL! Non-secure flash program can write secure flash data. \n\r");
    }

    /*write to the non-secure flash block located at 0x120000 to show
      non-secure code can write to the non-secure flash region if
      this non-secure flash region is modifiable by FAW (within FAW region from 0x100000 to 0x200000) */

    success = write_non_secureFlash_using_non_secureFlashCode(writeBuffer);
    if(success)
    {
    	APP_PRINT("\nPASS! non-secure flash program can write to non-secure flash region 0x120000\n\r       which is modifiable by FAW\n\r");
    }
    else
    {
		APP_PRINT("\n!FAIL! Non-secure program cannot write to non-secure flash data \n\r      which is modifiable by FAW\n\r");
    }

}

/*******************************************************************************************************************//**
 * @brief setup_faw
 * This function sets up the FAW from 0x100000 to 0x200000
 *
 * return: void
 **********************************************************************************************************************/

void setup_faw(void)
{
	APP_PRINT("**************************\n\r");
	APP_PRINT("\nSetup Flash Access Window...running from flash\n\r");

    success = s_setup_faw();
    if(success)
    {
    	APP_PRINT("\nPASS secure program => faw is setup from 0x100000 to 0x1FFFFF\n\r");
    }
    else
    {
    	APP_PRINT("\n!FAIL! secure program => faw setup failed\n\r");
    }
    APP_PRINT("**************************\n\r");
}

/*******************************************************************************************************************//**
 * @brief reset_faw function
 *  This function clears the FAW region so FAW is disabled
 *
 * return: void
 **********************************************************************************************************************/
void reset_faw(void)
{
	APP_PRINT("**************************\n\r");
	APP_PRINT("\nReset Flash Access Window...running from flash\n\r");

    success = s_reset_faw();

    if(success)
    {
    	APP_PRINT("PASS secure program => faw is reset\n\r");
    }
    else
    {
    	APP_PRINT("FAIL! secure program => faw reset failed\n\r");
    }
    APP_PRINT("**************************\n\r");
}

