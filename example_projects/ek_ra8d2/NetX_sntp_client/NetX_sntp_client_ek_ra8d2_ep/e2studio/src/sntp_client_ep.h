/***********************************************************************************************************************
 * File Name    : sntp_client_ep.h
 * Description  : Contains macros, data structures and functions used in the sntp_client_thread_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef SNTP_CLIENT_EP_H_
#define SNTP_CLIENT_EP_H_

/* Example project banner info */
#define EP_INFO "This project demonstrates the basic functionality of the NetX Duo SNTP Client using the Ethernet\r\n"\
                "driver on Renesas RA MCUs with the Renesas FSP NetX Duo stack and Azure RTOS. The example project\r\n"\
                "uses the MCU internal RTC for system time and the DHCP client to obtain an IP address from the\r\n"\
                "DHCP server. Upon successfully receiving an IP address from the DHCP server, the success status\r\n"\
                "and the client IP address are displayed in the J-Link RTT Viewer. The SNTP Client is created, and\r\n"\
                "the SNTP server IP address is resolved through DNS stack functions using the provided server URL.\r\n"\
                "The SNTP client then requests the server to provide network timestamp updates. Upon successfully\r\n"\
                "receiving a timestamp from the server, the MCU internal RTC timestamp is read and compared with\r\n"\
                "the received timestamp. The RTC time is updated if there is any difference. Error and information\r\n"\
                "messages are printed on the J-Link RTT Viewer.\r\n\n\n"

/* Timeout options */
#define LINK_ENABLE_WAIT_TIME       (1000U)
#define INIT_MODE_EVENT_WAIT_TIME   (3000U)
#define RETRY_MODE_EVENT_WAIT_TIME  (500U)
/* Timeout for DNS resolution */
#define NETWORK_TIMEOUT             (60 * TX_TIMER_TICKS_PER_SECOND)
/* DNS buffer size */
#define IP_V4_SIZE                  (4U)
/* Maximum number of DNS servers */
#define MAX_DNS_SERVERS             (5U)
/* SNTP client time update event */
#define SNTP_UPDATE_EVENT           (1U)
/* Enable UNICAST or BROADCAST mode */
#define UNICAST                     (1U)
/* Maximum number of server URLs */
#define SERVER_URL_MAX_VAL          (4U)
/* SNTP server URL length */
#define SERVER_URL_LEN              (50U)

/* Enumeration for SNTP client modes */
typedef enum sntp_client_mode
{
    INIT_MODE,
    RETRY_MODE
} sntp_client_mode_t;

#endif /* SNTP_CLIENT_EP_H_ */
