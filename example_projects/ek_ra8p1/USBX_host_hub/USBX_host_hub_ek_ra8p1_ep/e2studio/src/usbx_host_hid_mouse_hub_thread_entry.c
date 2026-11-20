/***********************************************************************************************************************
 * File Name    : usbx_host_hid_mouse_hub_thread_entry.c
 * Description  : Contains data structures and functions used in usbx_host_hid_mouse_hub_thread_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "usbx_host_hub_example_app.h"

/*******************************************************************************************************************//**
 * @brief     This function is the HID Mouse HUB thread entry.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void usbx_host_hid_mouse_hub_thread_entry(void)
{
    usbx_host_hid_mouse_hub_example_app_run();
}
