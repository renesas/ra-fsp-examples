/***********************************************************************************************************************
 * File Name    : usb_hhid_thread_entry.c
 * Description  : Contains data structures and functions used in usb_hhid_thread_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "usb_hhid_example_app.h"

/*******************************************************************************************************************//**
 * @brief     This function is the USB HHID example thread entry.
 * @param[in] pvParameters              Thread input parameter.
 * @retval    None.
 **********************************************************************************************************************/
void usb_hhid_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);
    usb_hhid_example_app_run();
}
