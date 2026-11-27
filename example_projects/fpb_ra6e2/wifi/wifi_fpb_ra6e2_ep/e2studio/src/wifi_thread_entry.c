/***********************************************************************************************************************
 * File Name    : wifi_thread_entry.c
 * Description  : Contains data structures and functions used in wifi_thread_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "wifi_thread.h"
#include "wifi_example_app.h"

/* WiFi Thread entry function */
void wifi_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);
    wifi_example_app_run();
}
