/***********************************************************************************************************************
 * File Name    : sntp_client_ep.h
 * Description  : Contains macros, data structures and functions used in the sntp_client_thread_entry.c
 ***********************************************************************************************************************/
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

#ifndef SNTP_CLIENT_EP_H_
#define SNTP_CLIENT_EP_H_

/* EP Banner Info. */
#define EP_INFO "\r\nThis project demonstrates the basic functionality of NetX Duo SNTP client\r\n"\
                "with ETHERNET driver on Renesas RA MCUs using Renesas FSP NetX Duo stack with Azure RTOS.\r\n"\
                "The EP utilize mcu_internal RTC for system time and DHCP client to get the IP address\r\n"\
                "from the DHCP server for RA board IP assignment. RA board's IP address obtained using DHCP is\r\n"\
                "displayed on Jlink RTTViewer upon successful DHCP client functionality execution.\r\n"\
                "SNTP Client is created and SNTP server URL's IP address is resolved through DNS stack functions.\r\n"\
                "SNTP client requests server to provide network time stamp upadtes from server.\r\n"\
                "Upon successful time stamp receive from server, MCU internal RTC timestamp is read and compared\r\n"\
                "with this timestamp and the RTC time stamp is updated if there is difference with received timestamp\r\n"\
                "Status, Information and Error(if any occur) messages will be displayed on Jlink RTTViewer.\r\n\n\n"

/* Timeout options. */
#define LINK_ENABLE_WAIT_TIME      (1000U)
#define INIT_MODE_EVENT_WAIT_TIME  (3000U)
#define RETRY_MODE_EVENT_WAIT_TIME (500U)
/* Timeout for DNS resolution. */
#define NETWORK_TIMEOUT        (60 * TX_TIMER_TICKS_PER_SECOND)
/* DNS Buffer Size*/
#define IP_V4_SIZE              (4U)
/* Maximum number of DNS servers. */
#define MAX_DNS_SERVERS         (5U)
/* SNTP Client time update event */
#define SNTP_UPDATE_EVENT       (1U)
/* To enable UNICAST/BROADCAST mode. */
#define UNICAST                 (1U)
/* Maximum server URLs*/
#define SERVER_URL_MAX_VAL      (4U)
/* sntp server url length. */
#define SERVER_URL_LEN          (50U)

/* enum for sntp client mode. */
typedef enum sntp_client_mode
{
    INIT_MODE,
    RETRY_MODE
}sntp_client_mode_t;

#endif /* SNTP_CLIENT_EP_H_ */
