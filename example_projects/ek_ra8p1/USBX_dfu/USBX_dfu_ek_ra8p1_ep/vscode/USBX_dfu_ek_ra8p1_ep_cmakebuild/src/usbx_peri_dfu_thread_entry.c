/***********************************************************************************************************************
 * File Name    : usbx_peri_dfu_thread_entry.c
 * Description  : Contains data structures and functions used in usbx_peri_dfu_thread_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "usbx_dfu_example_app.h"

/*******************************************************************************************************************//**
 * @brief     This function is the USBX PERI DFU thread entry.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void usbx_peri_dfu_thread_entry(void)
{
    usbx_peri_dfu_example_app_run();
}
