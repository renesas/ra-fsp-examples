/***********************************************************************************************************************
 * File Name    : NetX_dhcpv6_server_example_app.h
 * Description  : Public interface for the NetX DHCPv6 Server example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef NETX_DHCPV6_SERVER_EXAMPLE_APP_H_
#define NETX_DHCPV6_SERVER_EXAMPLE_APP_H_

/* Macro for the EP version */
#define EP_VERSION          "1.0"

/* Macro for the module name */
#define MODULE_NAME         "NetX_dhcpv6_server"

/* Macro for the banner information */
#define BANNER_INFO         "\r\n********************************************************************************"\
                            "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                  *"\
                            "\r\n*   Example Project Version "EP_VERSION\
                            "                                                *"\
                            "\r\n*   Flex Software Pack Version %d.%d.%d                                           *"\
                            "\r\n********************************************************************************"\
                            "\r\nRefer to readme.txt file for more details on Example Project and"\
                            "\r\nFSP User's Manual for more information about "MODULE_NAME" module\r\n"

/* Macro for the EP information */
#define EP_INFO             "\r\nThis project demonstrates the basic functionality of NetX Duo DHCPv6 Server"\
                            "\r\nwith an Ethernet driver on Renesas RA MCUs based on Renesas FSP using"\
                            "\r\nAzure RTOS. The DHCPv6 Server is created and, based on the user input, the user"\
                            "\r\ncan start the DHCPv6 Server and print the DHCPv6 clients info."\
                            "\r\nOn successful completion of each operation, the success status is displayed"\
                            "\r\non the terminal. Error and info messages are also printed there."

/* Macro for the EP menu */
#define EP_MENU             "\r\nDHCPv6 Server Menu options:"\
                            "\r\n1. Enter 1 to start DHCPv6 Server"\
                            "\r\n2. Enter 2 to display DHCPv6 leased IPs info"\
                            "\r\nUser input:"

/* Function declarations */
void dhcpv6_thread_entry_run(void);
void terminal_thread_entry_run(void);

#endif /* NETX_DHCPV6_SERVER_EXAMPLE_APP_H_ */
