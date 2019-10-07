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
#include "flash_hp_ep.h"

void R_BSP_WarmStart(bsp_warm_start_event_t event);
void readfromRTT(void);

/*******************************************************************************************************************//**
 * The RA Configuration tool generates main() and uses it to generate threads if an RTOS is used.  This function is
 * called by main() when no RTOS is used.
 **********************************************************************************************************************/
void hal_entry(void)
{
	fsp_err_t err = FSP_SUCCESS;
	fsp_pack_version_t version;
	uint32_t read_data = RESET_VALUE;

	/* version get API for FLEX pack information */
	R_FSP_VersionGet(&version);

	APP_PRINT(BANNER_1);
	APP_PRINT(BANNER_2);
	APP_PRINT(BANNER_3, EP_VERSION);
	APP_PRINT(BANNER_4, version.major, version.minor, version.patch);
	APP_PRINT(BANNER_5);
	APP_PRINT(BANNER_6);
	APP_PRINT("\r\n\nThis example project demonstrates basic functionalities of Flash HP");
	APP_PRINT("\r\ndriver.An RTT input command triggers the Initialization,Erase,Blank Check,");
	APP_PRINT("\r\nFlash Write,Flash Read operations in normal operating mode for Code/Data flash");
	APP_PRINT("\r\non predefined block and outputs the status/data to RTT viewer.");
	/* Setup MCU port settings after C runtime environment and system clocks are setup*/
	R_BSP_WarmStart(BSP_WARM_START_POST_C);

	/* Open Flash_HP */
	err = R_FLASH_HP_Open(&g_flash_ctrl, &g_flash_cfg);
	/* Handle Error */
	if (FSP_SUCCESS != err)
	{
		APP_PRINT("\r\n Flah_HP_Open API failed");
		APP_ERR_TRAP(err);
	}

	/* Setup Default  Block 0 as Startup Setup Block */
	err = R_FLASH_HP_StartUpAreaSelect(&g_flash_ctrl, FLASH_STARTUP_AREA_BLOCK0, true);
	if (err != FSP_SUCCESS)
	{
		APP_PRINT("\r\n Flah_HP_StartUpAreaSelect API failed");
		APP_ERR_TRAP(err);
	}
	APP_PRINT("\r\n\nChoose an option from the below menu");
	APP_PRINT("\r\n Press 1 for Code Flash Operations \r\n Press 2 for Data Flash Operations \r\n Press 3 for EXIT");

	unsigned char rByte[BUFF_SIZE] = { RESET_VALUE };
	while (EXIT != read_data)
	{

		if (APP_CHECK_DATA)
		{
			APP_READ(rByte);
			read_data = (uint32_t)(atoi((char *)(rByte)));
			switch (read_data)
			{
			case CODE_FLASH:
				APP_PRINT("\n\rEntering to code flash operations");
				err = flash_hp_code_flash_operations();
				if( FSP_SUCCESS != err)
				{
					flash_hp_deinit();
					APP_ERR_TRAP(err);
				}
				break;

			case DATA_FLASH:
				APP_PRINT("\n\rEntering to data flash operations");
				err = flash_hp_data_flash_operations();
				if (FSP_SUCCESS != err)
				{
					flash_hp_deinit();
					APP_ERR_TRAP(err);
				}
				break;
			case EXIT:
				APP_PRINT("\n\rExiting the flash_hp, User has to restart the application");
				flash_hp_deinit();
				break;
			default:
				APP_PRINT("\r\nChoose an option from the below menu");
				APP_PRINT("\r\n Press 1 for Code Flash Operations \r\n Press 2 for Data Flash Operations \r\n Press 3 for EXIT");
				break;
			}
			if ((CODE_FLASH <= read_data) && (EXIT > read_data))
			{
				APP_PRINT("\r\n Enter any key (other than 1,2,3) to go back to the main menu\r\n");
			}
			read_data = RESET_VALUE;
			memset(rByte,RESET_VALUE,BUFF_SIZE);
		}
	}

}

/*******************************************************************************************************************//**
 * This function is called at various points during the startup process.  This implementation uses the event that is
 * called right before main() to set up the pins.
 *
 * @param[in]  event    Where at in the start up process the code is currently at
 **********************************************************************************************************************/
void R_BSP_WarmStart(bsp_warm_start_event_t event)
{
	if (BSP_WARM_START_POST_C == event)
	{
		/* C runtime environment and system clocks are setup. */

		/* Configure pins. */
		R_IOPORT_Open(&g_ioport_ctrl, &g_bsp_pin_cfg);
	}
}
