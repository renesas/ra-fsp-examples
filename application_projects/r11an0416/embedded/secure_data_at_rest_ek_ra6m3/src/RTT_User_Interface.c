/***********************************************************************************************************************
 * File Name    : RTT_User_Interface.c
 * Description  : Contains RTT Viewer input and output control
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

#include "RTT_User_Interface.h"
#include "common_utils.h"
#include "menu_callbacks.h"

/* global definition */
void start_RTT_user_interface();

/*******************************************************************************************************************//**
 * @brief start_RTT_user_interface function
 * This function takes user input from the RTT Viewer and print the test result to the RTT Viewer
 *
 * return: void
 **********************************************************************************************************************/

void  start_RTT_user_interface()
{
	APP_PRINT("**************************\n\r");
	APP_PRINT("Choose an option from the below menu");
	APP_PRINT("\r\n Press 1 to view current security settings \r\n Press 2 for secure flash operation \r\n Press 3 for secure sram operation \r\n Press 4 for non-secure flash operation \r\n Press ~ for EXIT\r\n");
	APP_PRINT("**************************\n\r");

	unsigned char rByte[BUFF_SIZE] = { RESET_VALUE };
	while (EXIT != rByte[BUFF_INDEX])
	{
		if (APP_CHECK_DATA)
		{
			APP_READ(rByte);

			switch (rByte[BUFF_INDEX])
			{
			 case VIEW_SECURITY_SETTING:
				 read_secure_settings();
				 break;

			 case SECURE_FLASH_OPERATION:
				 APP_PRINT("**************************\n\r");
				 APP_PRINT("Choose an option from the below menu for secure flash operation");
				 APP_PRINT("\r\n Press a to read \r\n Press b to write \r\n Press c to setup faw \r\n Press d to reset faw \r\n");
				 APP_PRINT("**************************\n\r");
				 break;

			 case SECURE_FLASH_READ:
				 secure_code_read();
				 break;

			 case SECURE_FLASH_WRITE:
				 secure_code_write();
				 break;

			 case SET_UP_FAW:
				 setup_faw();
				 break;

			 case RESET_FAW:
				 reset_faw();
				 break;

			 case SECURE_SRAM_OPERATION:
				 APP_PRINT("**************************\n\r");
				 APP_PRINT("Choose an option from the below menu for secure sram operation");
				 APP_PRINT("\r\n Press g to read \r\n Press h to write\r\n");
				 APP_PRINT("**************************\n\r");
				 break;

			 case SECURE_SRAM_READ:
				 secure_sram_code_read();
				 break;

			 case SECURE_SRAM_WRITE:
				 secure_sram_code_write();
				 break;

			 case NON_SECURE_FLASH_OPERATION:
				 APP_PRINT("**************************\n\r");
				 APP_PRINT("Choose an option from the below menu for non_secure flash operation");
				 APP_PRINT("\r\n Press m to read \r\n Press n to write\r\n");
				 APP_PRINT("**************************\n\r");
				 break;

			 case NON_SECURE_FLASH_READ:
				 non_secure_code_read();
				 break;

			 case NON_SECURE_FLASH_WRITE:
				 non_secure_code_write();
				 break;

			 case EXIT:
				 APP_PRINT("\n\rExiting the secure data at rest.");
				 break;

			 default:
				 APP_PRINT("**************************\n\r");
				 APP_PRINT("Choose an option from the below menu");
				 APP_PRINT("\r\n Press 1 to view current security settings \r\n Press 2 for secure flash operation \r\n Press 3 for secure sram operation \r\n Press 4 for non-secure flash operation \r\n Press ~ for EXIT\r\n");
				 APP_PRINT("**************************\n\r");
				 break;
			}
			if ((VIEW_SECURITY_SETTING <= rByte[BUFF_INDEX]) && (EXIT > rByte[BUFF_INDEX]))
			{
			 APP_PRINT("Enter r to go back to the main menu\r\n\r\n");
			}
		}
	}

}
