/***********************************************************************************************************************
 * File Name    : ptp_ep.h
 * Description  : Contains macros, data structures and functions used in the ptp_thread_entry.c
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef PTP_EP_H_
#define PTP_EP_H_

#include <time.h>

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/* macros to print info, error and trap the error.*/
#define PRINT_INFO_STR(str)  (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_INFO_STR, sizeof(str), (str)))
#define PRINT_ERR_STR(str)   (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_STR, sizeof(str), (str)))
#define ERROR_TRAP(err)      (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_TRAP, sizeof(UINT *), &(err)))

/* Timeout options. */
#define LINK_ENABLE_WAIT_TIME      (1000U)
#define EVENT_WAIT_TIME            (0xFF)
#define TIMER_EXPIRATION_TICKS     (0xFFF)

/* PTP time sync event */
#define PTP_EVENT       (0x01)
/* To enable PTP in master mode.*/
#define PTP_MASTER_MODE            (0U)

/*MACROs to adjust month and year values */
#define MON_ADJUST_VALUE      (1)
#define YEAR_ADJUST_VALUE     (1900)

/* Time zone offset value can be changed as per required timezone.
 * to change the offset value, Refer -
 * https://www.epochconverter.com/timezones
 * All region time zone offset value are present in above URL.*/
/* Time zone offset value as per IST.*/
#define TIME_ZONE_OFFSET      (19800)

/* Maximum value of Port ID that the message was sent from.*/
#define PORT_ID_MAX_VAL       (0xFF)
/* clock id length. */
#define CLOCK_ID_LEN          (8U)
/* MAC Address length. */
#define MAC_LEN               (6U)
#define SHIFT_BIT             (32U)

/* Date and time structure */
typedef struct local_ptp_time
{
    struct tm local_time;
    uint32_t  tm_nsec;
}local_ptp_time_t;

#if PTP_MASTER_MODE
/* EP Banner Info. */
#define EP_INFO "\r\nThis project demonstrates the basic functionality of PTPC module along with NetX Duo DHCP client,\r\n"\
                "and Ethernet driver on RA MCUs using FSP NetX Duo stack with Azure RTOS.\r\n"\
                "The EP utilize DHCP client to get the IP address from the DHCP server for RA board IP assignment.\r\n"\
                "RA board's IP address obtained using DHCP is displayed on Jlink RTTViewer upon successful DHCP\r\n"\
                "client functionality execution.\r\n"\
                "Once PTP is initialized in E2E master mode, it will set the system clock value as local clock time stamp.\r\n"\
                "Status, Information and Error(if any occur) messages will be displayed on Jlink RTTViewer.\r\n\n\n"
#else
#define EP_INFO "\r\nThis project demonstrates the basic functionality of PTPC module along with NetX Duo DHCP client,\r\n"\
                "and Ethernet driver on RA MCUs using FSP NetX Duo stack with Azure RTOS.\r\n"\
                "The EP utilize DHCP client to get the IP address from the DHCP server for RA board IP assignment.\r\n"\
                "RA board's IP address obtained using DHCP is displayed on Jlink RTTViewer upon successful DHCP\r\n"\
                "client functionality execution."\
                "Once PTP is initialized in E2E slave mode, it will get the current time stamp from Raspberry pi\r\n"\
                "master clock and upon successful reception of time stamp,time stamp will be displayed on Jlink RTTViewer.\r\n"\
                "Status, Information and Error(if any occur) messages will be displayed on Jlink RTTViewer.\r\n\n\n"
#endif
#endif /* PTP_EP_H_ */
