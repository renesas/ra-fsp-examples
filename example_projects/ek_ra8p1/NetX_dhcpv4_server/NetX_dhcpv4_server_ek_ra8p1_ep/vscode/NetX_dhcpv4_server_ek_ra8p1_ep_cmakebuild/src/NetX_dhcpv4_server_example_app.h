/***********************************************************************************************************************
 * File Name    : NetX_dhcpv4_server_example_app.h
 * Description  : Public interface for the NetX DHCPv4 server example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef NETX_DHCPV4_SERVER_EXAMPLE_APP_H_
#define NETX_DHCPV4_SERVER_EXAMPLE_APP_H_

#define EP_VERSION      "1.0"

#define MODULE_NAME     "NetX_dhcpv4_server"

#define BANNER_INFO     "\r\n********************************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                  *"\
                        "\r\n*   Example Project Version %s                                                *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                                           *"\
                        "\r\n********************************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" module\r\n"

#define EP_INFO         "\r\nThe example project demonstrates the basic functionality of the NetX Duo DHCPv4 server"\
                        "\r\nwith an Ethernet driver on Renesas RA MCUs based on Renesas FSP using Azure RTOS."\
                        "\r\nThe user can start and stop the DHCPv4 server and display DHCPv4 client information."\
                        "\r\nThe status of each operation, information messages, and error messages are displayed"\
                        "\r\non the terminal."

#define EP_MENU         "\r\n\r\nMain menu:"\
                        "\r\n1. Enter 1 to start the DHCPv4 server"\
                        "\r\n2. Enter 2 to stop the DHCPv4 server"\
                        "\r\n3. Enter 3 to display DHCPv4 client information"\
                        "\r\nUser input: "

/* Single entry-point called in dhcpv4_server_thread_entry.c */
void dhcpv4_server_thread_entry_run(void);

#endif /* NETX_DHCPV4_SERVER_EXAMPLE_APP_H_ */
