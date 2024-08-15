/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/**********************************************************************************************************************
 * File Name    : menu_thread_entry.c
 * Version      : .
 * Description  : The main menu demo thread.
 *********************************************************************************************************************/

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "menu_thread.h"
#include "menu_main.h"
#include "common_init.h"
#include "common_data.h"
#include "board_cfg.h"

#define WAIT_TIME    (500u)            /* Wait time for task on Semaphore and Message Queue*/

extern TaskHandle_t board_mon_thread;
extern TaskHandle_t menu_thread;

/**********************************************************************************************************************
 * Function Name: menu_thread_entry
 * Description  : Main menu thread.
 * Argument     : pvParameters contains TaskHandle_t
 * Return Value : .
 *********************************************************************************************************************/
void menu_thread_entry (void * pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);

    SYSTEM_OK

    TickType_t Semphr_wait_ticks = pdMS_TO_TICKS(WAIT_TIME);

    if (NULL == board_mon_thread)
    {
        /* Fatal error - thread creation issue it is unsafe to continue */
        SYSTEM_ERROR
    }

    if (NULL == menu_thread)
    {
        /* Fatal error - thread creation issue it is unsafe to continue */
        SYSTEM_ERROR
    }

    /* Initialse subsystem */
    common_init();

    while (1)
    {
        vTaskDelay(WAIT_TIME);

        main_display_menu();
    }
}

/**********************************************************************************************************************
 * End of function menu_thread_entry
 *********************************************************************************************************************/
