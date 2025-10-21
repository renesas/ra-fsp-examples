/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : blinky_thread_entry.c
 * Version      : 1.0
 * Description  : Blinky Thread, provided by FSP
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 14/10/2025 1.00     First Release
 *********************************************************************************************************************/
#include "blinky_thread.h"

/**********************************************************************************************************************
 * Function Name: blinky_thread_entry
 * Description  : Entry point for thread
 * Argument     : pvParameters
 * Return Value : None.
 *********************************************************************************************************************/
void blinky_thread_entry (void * pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);

    while (1)
    {
        /* Long delay to restrict CPU usage */
        vTaskDelay(configTICK_RATE_HZ * 10);
    }
}
/**********************************************************************************************************************
 End of function toupperstr
 *********************************************************************************************************************/
