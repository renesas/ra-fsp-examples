/***********************************************************************************************************************
* File Name    : net_app.h
* Description  : Contains macros, data structures, and common functions used in the EP
***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef NET_APP_H_
#define NET_APP_H_

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "common_app.h"

/***********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/
extern NX_IP g_ip0;
extern net_info_t g_network_info;

extern TX_EVENT_FLAGS_GROUP g_led_event_flags;
extern volatile bsp_io_level_t g_led_1_status;
extern volatile bsp_io_level_t g_led_2_status;

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#if defined(BOARD_RA8D1_EK) || defined(BOARD_RA8M1_EK)
  #define FLASH_RS_PIN      (BSP_IO_PORT_01_PIN_06)
  #define FLASH_GPIO_CFG    (IOPORT_CFG_PORT_DIRECTION_OUTPUT | IOPORT_CFG_PORT_OUTPUT_HIGH)
  #define FLASH_XSPI_CFG    (IOPORT_CFG_DRIVE_HIGH | IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_OSPI)
#else
  #define FLASH_RS_PIN      (BSP_IO_PORT_03_PIN_10)
  #define FLASH_GPIO_CFG    (IOPORT_CFG_PORT_DIRECTION_OUTPUT | IOPORT_CFG_PORT_OUTPUT_HIGH)
  #define FLASH_XSPI_CFG    (IOPORT_CFG_DRIVE_HIGH | IOPORT_CFG_PERIPHERAL_PIN | IOPORT_PERIPHERAL_QSPI)
#endif

#define FLASH_RESET_TIME_US                     (1000U)

#define SELECT_TCP                              (1U)
#define SELECT_UDP                              (2U)
#define SELECT_DNS                              (3U)
#define SELECT_WEB                              (4U)

#define CHAR_ZERO                               ('0')
#define CHAR_NINE                               ('9')
#define CHAR_DOT                                ('.')
#define CHAR_COLON                              (':')
#define CHAR_NULL                               ('\0')

#define INPUT_MAX_SIZE                          (64U)
#define APP_BUFF_SIZE                           (1024U)

#define NET_APPLICATION_THREAD_SLEEP_TICK       (1U)

#define NET_TCP_CLIENT_WAIT                     (20 * NX_IP_PERIODIC_RATE)
#define NET_TCP_CLIENT_SEND_DATA                "This is a request sent from the RA board as a TCP client."

#define NET_UDP_CLIENT_QUEUE_SIZE               (5U)
#define NET_UDP_CLIENT_WAIT                     (20 * NX_IP_PERIODIC_RATE)
#define NET_UDP_CLIENT_SEND_DATA                "This is a request sent from the RA board as a UDP client."

#define NET_DNS_CLIENT_WAIT                     (20 * NX_IP_PERIODIC_RATE)
#define NET_DNS_SERVER_ADDR_1                   (IP_ADDRESS(8, 8, 8, 8))
#define NET_DNS_SERVER_ADDR_2                   (IP_ADDRESS(8, 8, 4, 4))
#define NET_DNS_NAME_MAX_LEN                    (64U)

#define HTTP_RESOURCE_INDEX                     "/index.html"
#define HTTP_RESOURCE_NETWORK                   "/network"
#define HTTP_RESOURCE_LED                       "/led"
#define HTTP_RESOURCE_SWITCH1                   "/switch1"
#define HTTP_RESOURCE_SWITCH2                   "/switch2"

#define HTTP_TYPE_MAX_LEN                       (30U)
#define HTTP_BODY_MAX_LEN                       (1024U)

#define JSON_NET_INFO                           \
"{\n"\
"    \"physical_address\": \"%x-%x-%x-%x-%x-%x\",\n"\
"    \"ipv4_address\": \"%d.%d.%d.%d\",\n"\
"    \"subnet_mask\": \"%d.%d.%d.%d\",\n"\
"    \"default_gateway\": \"%d.%d.%d.%d\"\n"\
"}"

#define JSON_LED_STATUS                         \
"{\n"\
"    \"led1\": %s,\n"\
"    \"led2\": %s\n"\
"}"

/***********************************************************************************************************************
 * Public function prototypes
 **********************************************************************************************************************/

#endif /* NET_APP_H_ */
