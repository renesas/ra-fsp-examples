/***********************************************************************************************************************
 * File Name    : pmsc_thread_entry.c
 * Description  : Contains data structures and functions used in pmsc_thread_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "pmsc_thread.h"
#include "usb_pmsc_example_app.h"

/*******************************************************************************************************************//**
* @brief     This function is the PMSC thread entry function.
* @param[in] pvParameters               Unused thread parameter.
* @retval    None
***********************************************************************************************************************/
void pmsc_thread_entry(void * pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);
    usb_pmsc_example_app_run();
}
