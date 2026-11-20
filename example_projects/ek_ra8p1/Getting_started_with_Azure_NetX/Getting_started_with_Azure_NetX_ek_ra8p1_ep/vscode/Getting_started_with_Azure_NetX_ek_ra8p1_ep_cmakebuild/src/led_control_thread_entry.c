/***********************************************************************************************************************
 * File Name    : led_control_thread_entry.c
 * Description  : Contains the entry function of the LED Control Thread.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2024 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "led_control_thread.h"
#include "Getting_started_with_Azure_NetX_example_app.h"

/*******************************************************************************************************************//**
 * @brief     This function is the LED Control thread entry.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void led_control_thread_entry(void)
{
    led_control_thread_entry_run();
}
