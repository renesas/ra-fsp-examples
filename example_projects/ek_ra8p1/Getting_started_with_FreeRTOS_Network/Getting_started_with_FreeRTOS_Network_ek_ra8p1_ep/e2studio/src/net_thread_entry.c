/***********************************************************************************************************************
 * File Name    : net_thread_entry.c
 * Description  : Contains data structures and functions used in net_thread_entry.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2024 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "net_thread.h"
#include "freertos_network_example_app.h"

/*******************************************************************************************************************//**
 * @brief     This function is the FreeRTOS network thread entry.
 * @param[in] pvParameters              Thread input parameter.
 * @retval    None.
 **********************************************************************************************************************/
void net_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);

    freertos_network_example_app_run();
}
