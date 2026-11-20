/***********************************************************************************************************************
 * File Name    : ethernet_ep.h
 * Description  : Contains macros, data structures and functions used in ethernet_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 ***********************************************************************************************************************/

#ifndef FREERTOS_NETWORK_EP_H_
#define FREERTOS_NETWORK_EP_H_

#include <stdbool.h>
#include <stdint.h>
#if (ipconfigUSE_DHCP != 0)
#include "FreeRTOS_DHCP.h"
#endif /* ipconfigUSE_DHCP */
#include "hal_data.h"
#include "FreeRTOS.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_TCP_server.h"
#include "FreeRTOS_HTTP_commands.h"
#include "FreeRTOS_server_private.h"

/* Macro for the reset value */
#define RESET_VALUE                 (0x00)

/* Macro for the null char */
#define NULL_CHAR                   ('\0')

/* Macros for network status */
#define IP_LINK_UP                  (1U << 0)
#define IP_LINK_DOWN                (1U << 1)
#define ETHERNET_LINK_UP            (1U << 2)
#define ETHERNET_LINK_DOWN          (1U << 3)

/* Macros for user input */
#define KEY_INDEX_SIZE              (3U)
#define REMOTE_INFO_SIZE            (65U)
#define USER_INPUT_SIZE             (32U)

/* Macros for ping request */
#define PING_TIMEOUT_MS             (2000U)
#define PING_DATA_SIZE              (32U)
#define PING_QUEUE_WAIT_TIME_MS     (2000U)

#if defined (BOARD_RA6M3_EK) || defined (BOARD_RA6M3G_EK)
#define KIT_NAME                    "EK-RA6M3"
#elif defined (BOARD_RA6M4_EK)  
#define KIT_NAME                    "EK-RA6M4"
#elif defined (BOARD_RA6M5_EK)  
#define KIT_NAME                    "EK-RA6M5"
#elif defined (BOARD_RA8M1_EK)  
#define KIT_NAME                    "EK-RA8M1"
#elif defined (BOARD_RA8D1_EK)  
#define KIT_NAME                    "EK-RA8D1"
#elif defined (BOARD_RA8P1_EK)  
#define KIT_NAME                    "EK-RA8P1"
#elif defined (BOARD_RA8D2_EK)  
#define KIT_NAME                    "EK-RA8D2"
#elif defined (BOARD_RA8M2_EK)  
#define KIT_NAME                    "EK-RA8M2"
#elif defined (BOARD_RA8T2_EK)  
#define KIT_NAME                    "EK-RA8T2"
#elif defined (BOARD_RA8P1_EK_V2)
#define KIT_NAME                    "EK-RA8P1 V2"
#endif /* Specific board */

typedef struct st_ping_data
{
    int8_t sent;     /* Ping request */
    int8_t received; /* Ping response */
    int8_t lost;     /* Ping failure */
} ping_data_t;

typedef enum
{
    PING_SUCCESS = 0,
    PING_TIMEOUT,
    PING_ERROR
} ping_status_t;

/* External variables */
extern uint8_t g_mac_address[6];
extern uint8_t g_ip_address[4];
extern uint8_t g_netmask[4];
extern uint8_t g_gateway_address[4];
extern uint8_t g_dns_server_address[4];
extern ping_data_t g_ping_data;
extern bool g_dhcp_in_use;
#if (ipconfigUSE_DHCP != 0)
extern IPV4Parameters_t g_xnd;
#endif /* ipconfigUSE_DHCP != 0 */

/* Function declarations */
BaseType_t ethernet_init(void);
uint32_t is_network_up(void);
ping_status_t send_ping_request(const char * p_ip_address);
uint32_t get_host_by_name(const char * pcHostName);
void inet_ntoa(uint32_t ulIPAddress, char * pcBuffer);
uint32_t ulApplicationGetNextSequenceNumber(uint32_t ulSourceAddress, uint16_t usSourcePort,
                                            uint32_t ulDestinationAddress, uint16_t usDestinationPort);
void vApplicationPingReplyHook(ePingReplyStatus_t eStatus, uint16_t usIdentifier);
#if (ipconfigUSE_DHCP != 0)
eDHCPCallbackAnswer_t xApplicationDHCPHook(eDHCPCallbackPhase_t eDHCPPhase, uint32_t ulIPAddress);
void update_dhcp_info(void);
#endif /* ipconfigUSE_DHCP != 0 */

#if (ipconfigDHCP_REGISTER_HOSTNAME == 1)
const char * pcApplicationHostnameHook(void);
#endif /* ipconfigDHCP_REGISTER_HOSTNAME == 1 */

#endif /* FREERTOS_NETWORK_EP_H_ */
