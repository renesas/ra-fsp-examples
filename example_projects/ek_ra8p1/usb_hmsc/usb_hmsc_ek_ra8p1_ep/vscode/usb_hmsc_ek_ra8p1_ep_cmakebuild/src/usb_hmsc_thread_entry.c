/***********************************************************************************************************************
 * File Name    : usb_hmsc_thread_entry.c
 * Description  : Contains the USB HMSC thread entry function.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "usb_hmsc_example_app.h"

/*******************************************************************************************************************//**
 * @brief     This function is the USB HMSC thread entry.
 * @param[in] pvParameters              Thread input parameter.
 * @retval    None.
 **********************************************************************************************************************/
void usb_hmsc_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);
    usb_hmsc_thread_entry_run();
}
