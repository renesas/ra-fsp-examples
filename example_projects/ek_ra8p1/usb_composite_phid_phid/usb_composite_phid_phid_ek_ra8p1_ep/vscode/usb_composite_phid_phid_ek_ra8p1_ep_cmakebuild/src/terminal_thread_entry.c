/***********************************************************************************************************************
 * File Name    : terminal_thread_entry.c
 * Description  : Contains macros and functions used in terminal_thread_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "terminal_thread.h"
#include "usb_phid_phid_example_app.h"

/*******************************************************************************************************************//**
 * @brief       Terminal thread entry.
 * @param[in]   pvParameters.
 * @retval      None.
 **********************************************************************************************************************/
void terminal_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);

    terminal_thread_entry_run();
}
