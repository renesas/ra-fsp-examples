/***********************************************************************************************************************
 * File Name    : usbx_pcdc_acm_thread_entry.c
 * Description  : Contains data structures and functions used in usbx_pcdc_acm_thread_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "usbx_pcdc_acm_example_app.h"

/*******************************************************************************************************************//**
 * @brief     This function is the USBX PCDC ACM example thread entry.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void usbx_pcdc_acm_thread_entry(void)
{
    usbx_pcdc_acm_example_app_run();
}
