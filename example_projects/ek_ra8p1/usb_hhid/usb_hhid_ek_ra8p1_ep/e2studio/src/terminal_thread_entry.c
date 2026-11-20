/***********************************************************************************************************************
 * File Name    : terminal_thread_entry.c
 * Description  : Contains data structures and functions used in terminal_thread_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "terminal.h"

/*******************************************************************************************************************//**
 * @brief     This function is the Terminal thread entry.
 * @param[in] pvParameters              Thread input parameter.
 * @retval    None.
 **********************************************************************************************************************/
void terminal_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);
    terminal_thread_entry_run();
}
