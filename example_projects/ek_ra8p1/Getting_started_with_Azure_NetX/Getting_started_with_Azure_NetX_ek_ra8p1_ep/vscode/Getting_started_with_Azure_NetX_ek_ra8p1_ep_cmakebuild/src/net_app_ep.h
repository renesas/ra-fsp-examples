/***********************************************************************************************************************
 * File Name    : net_app_ep.h
 * Description  : Contains macros and functions used in the net_app_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef NET_APP_EP_H_
#define NET_APP_EP_H_

#include "nx_api.h"
#include "terminal.h"

/* Macro definitions */
/* Macros for the user selections */
#define SELECT_TCP                          (1U)
#define SELECT_UDP                          (2U)
#define SELECT_DNS                          (3U)
#define SELECT_WEB                          (4U)

/* Size of the buffer used to receive the TCP and UDP responses, including the NULL char */
#define APP_BUFF_SIZE                       (1024U)

#define NET_APPLICATION_THREAD_SLEEP_TICK   (1U)

/* Data sent by the TCP and UDP clients */
#define NET_TCP_CLIENT_SEND_DATA            "This is a request sent from the RA board as a TCP client."
#define NET_UDP_CLIENT_SEND_DATA            "This is a request sent from the RA board as a UDP client."

/* Macros for the TCP client, the UDP client and the DNS client */
#define NET_TCP_CLIENT_WAIT                 (20 * TX_TIMER_TICKS_PER_SECOND)
#define NET_UDP_CLIENT_QUEUE_SIZE           (5U)
#define NET_UDP_CLIENT_WAIT                 (20 * TX_TIMER_TICKS_PER_SECOND)
#define NET_DNS_CLIENT_WAIT                 (20 * TX_TIMER_TICKS_PER_SECOND)

/* Macros for the DNS client */
#define NET_DNS_SERVER_ADDR_1               (IP_ADDRESS(8, 8, 8, 8))
#define NET_DNS_SERVER_ADDR_2               (IP_ADDRESS(8, 8, 4, 4))
#define NET_DNS_NAME_MAX_LEN                (64U)

/* Macros for the web HTTP server */
#define HTTP_RESOURCE_INDEX                 "/index.html"
#define HTTP_RESOURCE_NETWORK               "/network"
#define HTTP_RESOURCE_LED                   "/led"
#define HTTP_RESOURCE_SWITCH1               "/switch1"
#define HTTP_RESOURCE_SWITCH2               "/switch2"

/* Message to guide the user to the web page, and the size of the buffer used to format it */
#define WEB_SERVER_INFO                     "Please open a browser and enter \"%d.%d.%d.%d"HTTP_RESOURCE_INDEX\
                                            "\" to go to the web page."
#define WEB_SERVER_INFO_SIZE                (128U)

#define HTTP_TYPE_MAX_LEN                   (30U)
#define HTTP_BODY_MAX_LEN                   (1024U)

#define JSON_NET_INFO                       "{\n"\
                                            "    \"physical_address\": \"%x-%x-%x-%x-%x-%x\",\n"\
                                            "    \"ipv4_address\": \"%d.%d.%d.%d\",\n"\
                                            "    \"subnet_mask\": \"%d.%d.%d.%d\",\n"\
                                            "    \"default_gateway\": \"%d.%d.%d.%d\"\n"\
                                            "}"

#define JSON_LED_STATUS                     "{\n"\
                                            "    \"led1\": %s,\n"\
                                            "    \"led2\": %s\n"\
                                            "}"

/* Macros for the external flash reset */
#if defined(BOARD_RA8D1_EK) || defined(BOARD_RA8M1_EK) || defined(BOARD_RA8P1_EK) || \
    defined(BOARD_RA8D2_EK) || defined(BOARD_RA8M2_EK)
#define FLASH_RS_PIN                        (BSP_IO_PORT_01_PIN_06)
#define FLASH_GPIO_CFG                      (IOPORT_CFG_PORT_DIRECTION_OUTPUT | IOPORT_CFG_PORT_OUTPUT_HIGH)
#define FLASH_XSPI_CFG                      (IOPORT_CFG_DRIVE_HIGH | IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_OSPI)
#elif defined(BOARD_RA8T2_EK)
#define FLASH_RS_PIN                        (BSP_IO_PORT_12_PIN_07)
#define FLASH_GPIO_CFG                      (IOPORT_CFG_PORT_DIRECTION_OUTPUT | IOPORT_CFG_PORT_OUTPUT_HIGH)
#define FLASH_XSPI_CFG                      (IOPORT_CFG_DRIVE_HIGH | IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_OSPI)
#else
#define FLASH_RS_PIN                        (BSP_IO_PORT_03_PIN_10)
#define FLASH_GPIO_CFG                      (IOPORT_CFG_PORT_DIRECTION_OUTPUT | IOPORT_CFG_PORT_OUTPUT_HIGH)
#define FLASH_XSPI_CFG                      (IOPORT_CFG_DRIVE_HIGH | IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_QSPI)
#endif /* Specific board */

/* Macro for the external flash reset time, in microseconds */
#define FLASH_RESET_TIME_US                 (1000U)

/* Function declarations */
UINT net_app_create(void);
UINT tcp_client_connect(ULONG ipv4, UINT port);
UINT tcp_client_send(const CHAR * p_data, ULONG size);
UINT tcp_client_receive(CHAR * const p_buffer);
void tcp_client_disconnect(void);
UINT udp_client_send(ULONG ipv4, UINT port, const CHAR * p_data, ULONG size);
UINT udp_client_receive(CHAR * const p_buffer);
UINT dns_host_by_name(const CHAR * p_name, ULONG * const p_ipv4);
UINT dns_host_by_address(ULONG ipv4, CHAR * const p_name, ULONG name_size);
UINT web_server_start(void);
UINT web_server_stop(void);

#endif /* NET_APP_EP_H_ */
