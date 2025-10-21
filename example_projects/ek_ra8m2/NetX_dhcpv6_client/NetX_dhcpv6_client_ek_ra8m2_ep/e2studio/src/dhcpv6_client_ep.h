/***********************************************************************************************************************
 * File Name    : dhcpv6_client_ep.h
 * Description  : Contains data structures and functions used in dhcpv6_client_ep.h.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef DHCPV6_CLIENT_EP_H_
#define DHCPV6_CLIENT_EP_H_

/* Define parameters for creating the DHCPv6 client */
#define DHCPV6_IANA_ID          (0xC0DEDBAD)                 /* Identity association unique identifier */
#define DHCPV6_T1               (NX_DHCPV6_INFINITE_LEASE)   /* When the client must start the IPv6 address renewal */
#define DHCPV6_T2               (NX_DHCPV6_INFINITE_LEASE)   /* When the client must start theIPv6 address rebinding */
#define DHCP_EVENT              (0x1)
#define DHCPV6_EVENT_WAIT_TIME  (8000U)

/* Macros to print info, error and trap the error */
#define PRINT_INFO_STR(str)     (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_INFO_STR, sizeof(str), (str)))
#define PRINT_ERR_STR(str)      (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_STR, sizeof(str), (str)))
#define ERROR_TRAP(err)         (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_TRAP, sizeof(UINT *), &(err)))

#define EP_INFO                 "\r\nThis project demonstrates the basic functionality of NetX Duo DHCPv6 client\r\n"\
                                "with Ethernet driver on Renesas RA MCUs based on Renesas FSP using AzureRTOS.\r\n"\
                    		    "DHCP client is created and started. On successful receiving of an IP address from\r\n"\
								"DHCPv6 server, it pings the server to verify the received IP address.\r\n"\
								"On successful completion, the success status and IP address are displayed\r\n"\
								"on RTT Viewer. Error and info messages will be printed on RTT Viewer.\r\n\n\n"

/* Function declaration */
void str_ipv6(UCHAR * str, NXD_ADDRESS ipv6);

#endif /* DHCPV6_CLIENT_EP_H_ */
