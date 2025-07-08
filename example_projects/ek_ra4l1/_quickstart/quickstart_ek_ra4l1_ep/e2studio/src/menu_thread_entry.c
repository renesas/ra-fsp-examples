/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
/**********************************************************************************************************************
 * File Name    : menu_thread_entry.c
 * Version      : .
 * Description  : .
 *********************************************************************************************************************/
#include "menu_thread.h"
#include "jlink_console.h"
#include "common_init.h"

#include "menu_main.h"
/*#include "menu_main.c"*/
/* Menu Thread entry function */
/* pvParameters contains TaskHandle_t */
/**********************************************************************************************************************
 * Function Name: menu_thread_entry
 * Description  : .
 * Argument     : pvParameters
 * Return Value : .
 *********************************************************************************************************************/
void menu_thread_entry(void *pvParameters)

{
    FSP_PARAMETER_NOT_USED (pvParameters);
    jlink_console_init();
    common_init();

    /* TODO: add your own code here */
    while (1)
    {
        vTaskDelay (1);
        main_display_menu();
    }
}
/**********************************************************************************************************************
 End of function menu_thread_entry
 *********************************************************************************************************************/

