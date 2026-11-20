/***********************************************************************************************************************
 * File Name    : usb_peri_vendor_thread_entry.c
 * Description  : Contains functions used in usb_peri_vendor_thread_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "usb_pvnd_example_app.h"

/*******************************************************************************************************************//**
 * @brief     This function is the USB PVND example thread entry.
 * @param[in] pvParameters              Thread input parameter.
 * @retval    None.
 **********************************************************************************************************************/
void usb_peri_vendor_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);
    usb_pvnd_example_app_run();
}
