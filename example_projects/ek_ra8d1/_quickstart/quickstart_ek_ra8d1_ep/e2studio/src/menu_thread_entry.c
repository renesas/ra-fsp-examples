/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/

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
