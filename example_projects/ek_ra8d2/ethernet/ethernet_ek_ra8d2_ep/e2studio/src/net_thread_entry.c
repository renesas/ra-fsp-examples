/***********************************************************************************************************************
 * File Name    : net_thread_entry.c
 * Description  : Contains data structures and functions used in net_thread_entry.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "net_thread.h"
#include "ethernet_example_app.h"

/*******************************************************************************************************************//**
 * @brief     This function is the Ethernet thread entry.
 * @param[in] pvParameters              Thread input parameter.
 * @retval    None.
 **********************************************************************************************************************/
void net_thread_entry(void * pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);

    ethernet_example_app_run();
}
