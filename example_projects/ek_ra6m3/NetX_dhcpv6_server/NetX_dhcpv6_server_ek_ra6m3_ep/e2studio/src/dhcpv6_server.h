/***********************************************************************************************************************
 * File Name    : dhcpv6_server.h
 * Description  : Contains data structures and functions used in dhcpv6_server.
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

#ifndef DHCP_V6_SERVER_H_
#define DHCP_V6_SERVER_H_

#define LINK_ENABLE_WAIT_TIME (1000U)

/* RTT input selection options to start, and print leased IPs info. */
#define DHCPV6_SERVER_START               (1U)
#define PRINT_DHCPV6_LEASED_IP_INFO       (2U)

#define DHCPV6_SERVER_MENU               "\r\nDHCPV6 SERVER Menu options :"\
                                         "\r\n1. Enter 1 to start dhcpv6 server"\
                                         "\r\n2. Enter 2 to display dhcpv6 leased IPs info"\
                                         "\r\nUser Input : \r\n"\

/* macros to print info, error and trap the error.*/
#define PRINT_INFO_STR(str)  (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_INFO_STR, sizeof(str), (str)))
#define PRINT_ERR_STR(str)   (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_STR, sizeof(str), (str)))
#define ERROR_TRAP(err)      (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_TRAP, sizeof(UINT *), &(err)))

#define EP_INFO        "\r\nThis project demonstrates the basic functionality of Netx Duo DHCPv6 server\r\n"\
                       "with ethernet driver on Renesas RA MCUs based on Renesas FSP using AzureRTOS.\r\n"\
                       "DHCPV6 Server is created and based on user input, user can start, stop and print \r\n"\
                       "DHCPV6 clients info.\r\n"\
                       "On successful completion of each operation, the success status is displayed\r\n"\
                       "on RTT viewer. Error and info messages will be printed on JlinkRTTViewer.\r\n\n\n"

#define FILL_NXD_IPV6_ADDRESS(ipv6,f0,f1,f2,f3,f4,f5,f6,f7) ({do { \
                                                                       (ipv6).nxd_ip_address.v6[0] = (((uint32_t)(f0) << 16) & 0xFFFF0000) | ((uint32_t)(f1) & 0x0000FFFF);\
                                                                       (ipv6).nxd_ip_address.v6[1] = (((uint32_t)(f2) << 16) & 0xFFFF0000) | ((uint32_t)(f3) & 0x0000FFFF);\
                                                                       (ipv6).nxd_ip_address.v6[2] = (((uint32_t)(f4) << 16) & 0xFFFF0000) | ((uint32_t)(f5) & 0x0000FFFF);\
                                                                       (ipv6).nxd_ip_address.v6[3] = (((uint32_t)(f6) << 16) & 0xFFFF0000) | ((uint32_t)(f7) & 0x0000FFFF);\
                                                                       (ipv6).nxd_ip_version       = NX_IP_VERSION_V6;\
                                                                   } while(0);})

/* structure to store leased IPs info.*/
typedef struct ipv6_info
{
    NXD_ADDRESS ipv6_address;
    USHORT mac_high;
    ULONG mac_low;
}ipv6_info_t;

/* Function declaration.*/
void str_ipv6(UCHAR * str, NXD_ADDRESS ipv6);
void str_mac(UCHAR * str,USHORT mac_msw, ULONG mac_lsw);

#endif /* DHCP_V6_SERVER_H_ */
