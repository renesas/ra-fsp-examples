/**********************************************************************************************************************
 * File Name    : sntp_client_ep.h
 * Description  : Contains macros, data structures and functions used in the sntp_client_thread_entry.c
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *********************************************************************************************************************/

#ifndef SNTP_CLIENT_EP_H_
#define SNTP_CLIENT_EP_H_

/* EP Banner Info */
#define EP_INFO "\r\nThis project demonstrates the real-time clock functionality without the RTC hardware\r\n"\
		"present on the RA MCU, using the Renesas FSP NetX Duo SNTP stack and Azure RTOS with an\r\n"\
		"Ethernet interface. Initially, the system time is set based on the compiler's build time and\r\n"\
		"the Azure RTOS tick count. To start with the DHCP client obtains the IP address from the DHCP\r\n"\
		"server for RA board IP assignment. The RA board's IP address, obtained via DHCP, is displayed\r\n"\
		"on the Jlink RTTViewer upon successful execution of the DHCP client functionality. An SNTP client\r\n"\
		"is created, and the SNTP server URL's IP address is resolved through the DNS stack functions.\r\n"\
		"The SNTP client then requests the server to provide network timestamp updates. Upon successfully\r\n"\
		"receiving a timestamp from the server, the system timestamp is read and compared with the received\r\n"\
		"timestamp, and the system time is updated if there is a difference.\r\n"\
		"Status, informational, and error messages (if any occur) will be displayed on the Jlink RTTViewer.\r\n\n\n"

/**********************************************************************************************************************
 * Macro definitions
 *********************************************************************************************************************/
/* Timeout options */
#define LINK_ENABLE_WAIT_TIME      (1000U)
#define INIT_MODE_EVENT_WAIT_TIME  (3000U)
#define RETRY_MODE_EVENT_WAIT_TIME (500U)

/* Timeout for DNS resolution */
#define NETWORK_TIMEOUT         (60 * TX_TIMER_TICKS_PER_SECOND)

/* DNS Buffer Size*/
#define IP_V4_SIZE              (4U)

/* Maximum number of DNS servers */
#define MAX_DNS_SERVERS         (5U)

/* SNTP Client time update event */
#define SNTP_UPDATE_EVENT       (1U)

/* To enable UNICAST/BROADCAST mode */
#define UNICAST                 (1U)

/* Maximum server URLs */
#define SERVER_URL_MAX_VAL      (4U)

/* Sntp server url length */
#define SERVER_URL_LEN          (50U)

/**********************************************************************************************************************
 * Typedef definitions
 *********************************************************************************************************************/
/* Enum for sntp client mode */
typedef enum sntp_client_mode
{
    INIT_MODE,
    RETRY_MODE
}sntp_client_mode_t;

#endif /* SNTP_CLIENT_EP_H_ */
