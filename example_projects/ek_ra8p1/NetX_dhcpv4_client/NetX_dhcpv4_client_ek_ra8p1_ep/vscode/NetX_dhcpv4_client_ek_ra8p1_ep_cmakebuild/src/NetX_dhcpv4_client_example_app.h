/***********************************************************************************************************************
 * File Name    : NetX_dhcpv4_client_example_app.h
 * Description  : Public interface for the NetX DHCPv4 client example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef NETX_DHCPV4_CLIENT_EXAMPLE_APP_H_
#define NETX_DHCPV4_CLIENT_EXAMPLE_APP_H_

#define EP_VERSION      "1.0"

#define MODULE_NAME     "NetX_dhcpv4_client"

#define BANNER_INFO     "\r\n********************************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                  *"\
                        "\r\n*   Example Project Version %s                                                *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                                           *"\
                        "\r\n********************************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO         "\r\nThe example project demonstrates the basic functionality of the NetX Duo DHCPv4 client"\
                        "\r\nwith an Ethernet driver on Renesas RA MCUs based on Renesas FSP using Azure RTOS."\
                        "\r\nThe DHCP client obtains an IP address from a DHCPv4 server and pings the server"\
                        "\r\nto verify network connectivity. It then restarts the DHCP client, obtains another"\
                        "\r\nIP address, pings the available DNS servers and gateways, and releases the IP address."\
                        "\r\nStatus information and error messages are displayed on the terminal.\r\n"

/* Single entry-point called in dhcp_client_thread_entry.c */
void dhcp_client_thread_entry_run(void);

#endif /* NETX_DHCPV4_CLIENT_EXAMPLE_APP_H_ */
