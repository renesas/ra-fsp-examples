/***********************************************************************************************************************
 * File Name    : dhcp_client_thread_entry.c
 * Description  : Contains the entry function of the DHCP Client Thread.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "dhcp_client_thread.h"
#include "NetX_dhcpv6_client_example_app.h"

/*******************************************************************************************************************//**
 * @brief     This function is the DHCP Client thread entry.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void dhcp_client_thread_entry(void)
{
    dhcp_client_thread_entry_run();
}
