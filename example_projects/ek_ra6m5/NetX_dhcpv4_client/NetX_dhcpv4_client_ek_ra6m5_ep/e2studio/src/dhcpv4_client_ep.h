/***********************************************************************************************************************
 * File Name    : dhcpv4_client_ep.h
 * Description  : Contains data structures and functions used in dhcpv4_client_ep.h.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#ifndef DHCPV4_CLIENT_EP_H_
#define DHCPV4_CLIENT_EP_H_

/* Define the wait limit for getting assigned an IP address.
 * NX_IP_PERIODIC_RATE is defined internally in NetX for converting
 * seconds to timer ticks. This timeout is defined for 2 seconds.
 */
#define WAIT_TO_BE_BOUND      (2*NX_IP_PERIODIC_RATE)
#define LINK_ENABLE_WAIT_TIME (1000U)
#define INITIAL_VALUE         '\0'
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
