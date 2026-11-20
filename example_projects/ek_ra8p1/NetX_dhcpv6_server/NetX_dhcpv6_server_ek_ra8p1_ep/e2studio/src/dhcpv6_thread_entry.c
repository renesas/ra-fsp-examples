/***********************************************************************************************************************
 * File Name    : dhcpv6_thread_entry.c
 * Description  : Contains the entry function of the DHCPv6 Thread.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "dhcpv6_thread.h"
#include "NetX_dhcpv6_server_example_app.h"

/* DHCPv6 Server thread entry function */
void dhcpv6_thread_entry(void)
{
    dhcpv6_thread_entry_run();
}
