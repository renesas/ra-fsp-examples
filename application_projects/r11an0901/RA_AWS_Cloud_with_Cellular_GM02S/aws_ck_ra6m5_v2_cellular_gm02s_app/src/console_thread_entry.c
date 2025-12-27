/***********************************************************************************************************************
 * File Name    : console_thread_entry.c
 * Description  : Contains data structures and functions used in Console related application
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2015 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#include "hal_data.h"
#include "common_init.h"
#include "common_data.h"
#include "common_utils.h"
#include "console_menu/console.h"
#include "board_cfg.h"
#include "console_thread.h"
#include "common_utils.h"
#include "console_menu/menu_main.h"

 uint32_t  g_console_status = RESET_VALUE;
 extern TaskHandle_t console_thread;
 void flash_init(void);
 void console_init(void);

 /**************************************************************************************
  * @brief Console Thread entry function
  * @param[in]  pvParameters contains TaskHandle_t
  * @retval
  **************************************************************************************/
void console_thread_entry(void *pvParameters) {
	FSP_PARAMETER_NOT_USED(pvParameters);

	console_init();
	flash_init();
	/* TODO: add your own code here */
	
    /* wait for application thread to finish MQTT connection */
    xTaskNotifyWait(pdFALSE, pdFALSE, (uint32_t* )&console_thread, portMAX_DELAY);
	while (1)
	{
		main_display_menu();
		vTaskDelay(100);
	}
}

/**************************************************************************************
 * @brief  Console Initialization
 * @param[in]
 * @retval
 **************************************************************************************/
void console_init(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Version get API for FLEX pack information */
    /* Initializing UART */
    err = uart_initialize();
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT ("\r\nUART INIT FAILED\r\n");
        APP_ERR_TRAP(err);
    }
    else
    {
       APP_DBG_PRINT ("\r\nUART INIT SUCCESS\r\n");
    }
}

/**************************************************************************************
 * @brief     Initialize flash
 * @param[in]
 * @retval
 **************************************************************************************/
void flash_init(void)
{
     fsp_err_t err = FSP_SUCCESS;
    /* Open Flash_HP */
    err = R_FLASH_HP_Open(&user_flash_ctrl, &user_flash_cfg);
    /* Handle Error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n Flah_HP_Open API failed");
        APP_ERR_TRAP(err);
    }
}
