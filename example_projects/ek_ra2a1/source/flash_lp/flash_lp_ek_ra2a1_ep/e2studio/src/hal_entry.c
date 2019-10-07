/***********************************************************************************************************************
 * File Name    : hal_entry.c
 * Description  : Contains data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
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

#include "common_utils.h"
#include "flash_lp_ep.h"

void R_BSP_WarmStart(bsp_warm_start_event_t event);
uint8_t block_num_input_and_validation(uint8_t flash);

/*******************************************************************************************************************//**
 * @brief The RA Configuration tool generates main() and uses it to generate threads if an RTOS is used.  This function is
 *        called by main() when no RTOS is used.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
void hal_entry(void)
{
	fsp_err_t err = FSP_SUCCESS;
	fsp_pack_version_t version = {RESET_VALUE};
	unsigned char rByte[BUFF_LEN] = {RESET_VALUE};
	uint8_t menu_input = RESET_VALUE;	// User input for menu options
	uint8_t block_num = RESET_VALUE;	// User input data/code flash block number

	/* version get API for FLEX pack information */
	R_FSP_VersionGet(&version);

	/* Example Project information printed on the Console */
	APP_PRINT(BANNER_1);
	APP_PRINT(BANNER_2);
	APP_PRINT(BANNER_3,EP_VERSION);
	APP_PRINT(BANNER_4,version.major, version.minor, version.patch );
	APP_PRINT(BANNER_5);
	APP_PRINT(BANNER_6);
	APP_PRINT("\r\n\nThis example project demonstrates basic functions of Flash LP driver.");
	APP_PRINT("\r\nAn RTT input from the user triggers Initialization, Erase, Blank Check,");
	APP_PRINT("\r\nFlash Write and Flash Read operations. User has to choose between the ");
	APP_PRINT("\r\nData Flash or Code Flash operation, and give the input for block number.");
	APP_PRINT("\r\nThe data and status output can be seen on the RTTviewer\r\n");

	/* Setup MCU port settings after C runtime environment and system clocks are setup*/
	R_BSP_WarmStart(BSP_WARM_START_POST_C);

	/* Menu for User Selection */
	APP_PRINT("\r\nSelect from the below menu options\r\n");
	APP_PRINT("\r\nMenu");
	APP_PRINT("\r\nEnter 1 for Data Flash Operations");
	APP_PRINT("\r\nEnter 2 for Code Flash Operations\r\n");

	while (true)
	{
		if (APP_CHECK_DATA)
		{
			APP_READ(rByte);
			menu_input = (uint8_t) atoi((char*) rByte);
			switch (menu_input)
			{
			case DATA_FLASH_OPERATIONS:
				block_num = block_num_input_and_validation(DATA_FLASH_OPERATIONS);
				/* Open Flash_LP driver */
				err = flash_lp_init();
				/* Error handle */
				if (FSP_SUCCESS != err)
				{   /* Flash_LP module init failed */
					APP_ERR_PRINT("\r\n ** FLASH_LP INIT FAILED ** \r\n");
					APP_ERR_TRAP(err);
				}
				else
				{
					APP_PRINT("\r\n ** FLASH_LP INIT SUCCESSFUL ** \r\n");
				}
				APP_PRINT("\r\n ** Performing Data Flash operations **\r\n");
				err = flash_lp_data_flash_operations (block_num);
				/* Error handle */
				if (FSP_SUCCESS != err)
				{
					APP_ERR_PRINT("** Flash_LP Data FLASH Operations failed **  \r\n");
					flash_lp_deinit();
					APP_ERR_TRAP(err);
				}
				else
				{
					APP_PRINT("\r\n** Flash_LP Data FLASH Operations Successful**  \r\n")
				}
				/* Close Flash_LP driver */
				err = flash_lp_deinit();
				/* Error handle */
				if (FSP_SUCCESS != err)
				{   /* Flash_LP module init failed */
					APP_ERR_PRINT("\r\n ** FLASH_LP DE-INIT FAILED ** \r\n");
					APP_ERR_TRAP(err);
				}
				else
				{
					APP_PRINT("\r\n ** FLASH_LP DE-INIT SUCCESSFUL ** \r\n");
				}
				break;
			case CODE_FLASH_OPERATIONS:
				block_num = block_num_input_and_validation (CODE_FLASH_OPERATIONS);
				/* Open Flash_LP driver */
				err = flash_lp_init ();
				/* Error handle */
				if (FSP_SUCCESS != err)
				{   /* Flash_LP module init failed */
					APP_ERR_PRINT("\r\n ** FLASH_LP INIT FAILED ** \r\n");
					APP_ERR_TRAP(err);
				}
				else
				{
					APP_PRINT("\r\n ** FLASH_LP INIT SUCCESSFUL ** \r\n");
				}
				APP_PRINT("\r\n ** Performing Code Flash operations **\r\n");
				err = flash_lp_code_flash_operations (block_num);
				/* Error handle */
				if (FSP_SUCCESS != err)
				{
					APP_ERR_PRINT("\r\n** Flash_LP Code FLASH Operations failed **  \r\n");
					flash_lp_deinit ();
					APP_ERR_TRAP(err);
				}
				else
				{
					APP_PRINT("** Flash_LP Code FLASH Operations Successful**  \r\n")
				}
				/* Close Flash_LP driver */
				err = flash_lp_deinit ();
				/* Error handle */
				if (FSP_SUCCESS != err)
				{   /* Flash_LP module init failed */
					APP_ERR_PRINT("\r\n ** FLASH_LP DE-INIT FAILED ** \r\n");
					APP_ERR_TRAP(err);
				}
				else
				{
					APP_PRINT("\r\n ** FLASH_LP DE-INIT SUCCESSFUL ** \r\n");
				}
				break;
			default:
				/* Menu for User Selection */
				APP_PRINT("\r\nSelect from the below menu options\r\n");
				APP_PRINT("\r\nMenu");
				APP_PRINT("\r\nEnter 1 for Data Flash Operations");
				APP_PRINT("\r\nEnter 2 for Code Flash Operations\r\n");
				break;
			}
			if ((DATA_FLASH_OPERATIONS <= menu_input) && (CODE_FLASH_OPERATIONS >= menu_input))
			{
				APP_PRINT ("\r\nEnter any other key (except '1' and '2') to go back to the main menu\r\n");
			}
		}
	}
}

/*******************************************************************************************************************//**
 * @brief       Takes block number input from user and validates it w.r.t. data/code flash.
 * @param[IN]  	flash					Macro for Data Flash or Code Flash.
 * @retval      block number          	Block number for data/code flash operations further.
 **********************************************************************************************************************/
uint8_t block_num_input_and_validation(uint8_t flash_type)
{
	uint8_t block_num = RESET_VALUE;	// User input data/code flash block number
	char rByte[BUFF_LEN] = {'\0'};	// RTT Input buffer
	unsigned num_bytes = RESET_VALUE;	// Number of bytes read by RTT

	/* Checks whether the function is called for Data-Flash or Code-Flash input */
	if (DATA_FLASH_OPERATIONS == flash_type)
	{
		/* Data-Flash block number input and validation */
		APP_PRINT("\r\nEnter block number between 0 and 7 for Data-Flash operations:\r\n");
		while ((FLASH_DF_BLOCK0 >= FLASH_DF_BLOCK(block_num)) || (FLASH_DF_BLOCK7 < FLASH_DF_BLOCK(block_num)))
		{
			while (BYTES_RECEIVED_ZERO == num_bytes)
			{
				if (APP_CHECK_DATA)
				{
					num_bytes = APP_READ(rByte);
					if (BYTES_RECEIVED_ZERO == num_bytes)
					{
						APP_PRINT("\r\nInvalid Input\r\n");
					}
				}
			}

			/* Checking whether input is numerical value or not */
			if(('0' <= rByte[ARRAY_INDEX]) && ('9' >= rByte[ARRAY_INDEX]) && ('\n' == rByte[ARRAY_INDEX + 1]))
			{
				/* Converting block number string to integer */
				block_num = (uint8_t)atoi(rByte);
			}
			else
			{
				APP_PRINT("\r\nEntered Data-Flash block number is not valid \r\n");
				APP_PRINT("Please enter block number between  0 and 7 \r\n");
				num_bytes = RESET_VALUE;
				continue;
			}

			/* Data-Flash block number validation */
			if ((FLASH_DF_BLOCK0 > FLASH_DF_BLOCK(block_num)) || (FLASH_DF_BLOCK7 < FLASH_DF_BLOCK(block_num)))
			{
				APP_PRINT("\r\nEntered Data-Flash block number is not valid \r\n");
				APP_PRINT("Please enter block number between  0 and 7 \r\n");
				num_bytes = RESET_VALUE;
			}
			else
			{
				break;
			}
		}
	}
	else if (CODE_FLASH_OPERATIONS == flash_type)
	{
		/* Code-Flash block number input and validation */
		APP_PRINT("\r\nEnter block number between 10 and 126 for Code-Flash operations\r\n");
		while ((FLASH_CF_BLOCK10 > FLASH_CF_BLOCK(block_num)) || (FLASH_CF_BLOCK126 < FLASH_CF_BLOCK(block_num)))
		{
			while (BYTES_RECEIVED_ZERO == num_bytes)
			{
				if (APP_CHECK_DATA)
				{
					num_bytes = APP_READ(rByte);
					if (BYTES_RECEIVED_ZERO == num_bytes)
					{
						APP_PRINT("\r\nInvalid Input\r\n");
					}
				}
			}

			/* Checking whether input is numerical value or not */
			if(('0' <= rByte[ARRAY_INDEX]) && ('9' >= rByte[ARRAY_INDEX]))
			{
				if(('.' != rByte[ARRAY_INDEX + 2]) && ('.' != rByte[ARRAY_INDEX + 3]) &&
						(('\n' == rByte[ARRAY_INDEX + 2]) || ('\n' == rByte[ARRAY_INDEX + 3])))
				{
					/* Converting block number string to integer */
					block_num = (uint8_t)atoi(rByte);
				}
				else
				{
					APP_PRINT("\r\nEntered Code-Flash block number is not valid \r\n");
					APP_PRINT("Please enter block number between  10 and 126 \r\n");
					num_bytes = RESET_VALUE;
					continue;
				}
			}
			else
			{
				APP_PRINT("\r\nEntered Code-Flash block number is not valid \r\n");
				APP_PRINT("Please enter block number between  10 and 126 \r\n");
				num_bytes = RESET_VALUE;
				continue;
			}

			/* Code-Flash block number validation */
			if ((FLASH_CF_BLOCK10 > FLASH_CF_BLOCK(block_num)) || (FLASH_CF_BLOCK126 < FLASH_CF_BLOCK(block_num)))
			{
				APP_PRINT("\r\nEntered Code-Flash block number is not valid \r\n");
				APP_PRINT("Please enter block number between 10 and 126 \r\n");
				num_bytes = RESET_VALUE;
			}
			else
			{
				break;
			}
		}
	}
	else
	{
		/* do nothing */
	}

	return block_num;
}

/*******************************************************************************************************************//**
 * This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart(bsp_warm_start_event_t event) {
	if (BSP_WARM_START_POST_C == event) {
		/* C runtime environment and system clocks are setup. */

		/* Configure pins. */
		R_IOPORT_Open(&g_ioport_ctrl, &g_bsp_pin_cfg);
	}
}
