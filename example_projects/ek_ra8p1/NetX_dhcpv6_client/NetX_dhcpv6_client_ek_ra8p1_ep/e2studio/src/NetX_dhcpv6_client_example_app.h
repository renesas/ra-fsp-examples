/***********************************************************************************************************************
 * File Name    : NetX_dhcpv6_client_example_app.h
 * Description  : Public interface for the NetX DHCPv6 Client example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef NETX_DHCPV6_CLIENT_EXAMPLE_APP_H_
#define NETX_DHCPV6_CLIENT_EXAMPLE_APP_H_

#define EP_VERSION          "1.0"

#define MODULE_NAME         "NetX_dhcpv6_client"

#define BANNER_INFO         "\r\n********************************************************************************"\
                            "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                  *"\
                            "\r\n*   Example Project Version %s                                                *"\
                            "\r\n*   Flex Software Pack Version %d.%d.%d                                           *"\
                            "\r\n********************************************************************************"\
                            "\r\nRefer to readme.txt file for more details on Example Project and"\
                            "\r\nFSP User's Manual for more information about "MODULE_NAME" module\r\n"

#define EP_INFO             "\r\nThe example project demonstrates the basic functionality of NetX Duo DHCPv6 Client"\
                            "\r\nwith an Ethernet driver on Renesas RA MCUs based on Renesas FSP using"\
                            "\r\nAzure RTOS. The DHCPv6 Client is created and started. On successful receiving"\
                            "\r\nof an IPv6 address from the DHCPv6 Server, it pings the DNS Server to verify the"\
                            "\r\nreceived IPv6 address. Error and info messages will be printed on on the terminal\r\n"

/* Single entry-point called in dhcp_client_thread_entry.c */
void dhcp_client_thread_entry_run(void);

#endif /* NETX_DHCPV6_CLIENT_EXAMPLE_APP_H_ */
