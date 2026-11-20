/***********************************************************************************************************************
 * File Name    : terminal_thread_entry.c
 * Description  : Contains the entry function of the Terminal Thread.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "terminal_thread.h"
#include "NetX_dhcpv6_server_example_app.h"

/* Terminal Thread entry function */
void terminal_thread_entry(void)
{
    terminal_thread_entry_run();
}
