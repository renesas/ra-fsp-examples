/***********************************************************************************************************************
 * File Name    : dhcpv4_client_ep.h
 * Description  : Contains data structures and functions used in dhcpv4_client_ep.h.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef DHCPV4_CLIENT_EP_H_
#define DHCPV4_CLIENT_EP_H_

/* Define the wait limit for getting assigned an IP address.
 * NX_IP_PERIODIC_RATE is defined internally in NetX for converting
 * seconds to timer ticks. This timeout is defined for 2 seconds.
 */
#define WAIT_TO_BE_BOUND      (2*NX_IP_PERIODIC_RATE)
#define LINK_ENABLE_WAIT_TIME (1000U)
#define INITIAL_VALUE         ('\0')
#define MAX_DNS_SERVERS       (5)
#define MAX_GATEWAYS          (5)
#define IP_V4_SIZE            (4)
#define DHCP_EVENT            (0x1)

#define EP_INFO        "\r\nThis project demonstrates the basic functionality of Netx Duo DHCPv4 client\r\n"\
                       "with ethernet driver on Renesas RA MCUs based on Renesas FSP using AzureRTOS.\r\n"\
                       "DHCP Client is created and started. On successful receiving of IP address from \r\n"\
                       "DHCPv4 server, it pings the server to verify the received Ip address.\r\n"\
                       "On successful completion, the success status and IP address  are displayed\r\n"\
                       "on RTT viewer. Error and info messages will be printed on JlinkRTTViewer.\r\n\n\n"

#endif /* DHCPV4_CLIENT_EP_H_ */
