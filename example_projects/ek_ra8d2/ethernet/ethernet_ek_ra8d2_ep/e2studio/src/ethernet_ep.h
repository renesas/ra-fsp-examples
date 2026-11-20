/***********************************************************************************************************************
 * File Name    : ethernet_ep.h
 * Description  : Contains macros, data structures and functions used in ethernet_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef ETHERNET_EP_H_
#define ETHERNET_EP_H_

#include <stdbool.h>
#include <stdlib.h>
#include "hal_data.h"
#if (ipconfigUSE_DHCP != 0)
#include "FreeRTOS_DHCP.h"
#endif /* ipconfigUSE_DHCP */

/* Test domain name, ping target IP address and ping count */
#define USR_TEST_DOMAIN_NAME        "www.freertos.org"
#define USR_TEST_PING_IP            "8.8.8.8"
#define USR_PING_COUNT              (100U)

/* Print-ability flags for the example application */
#define SUCCESS                     (0U)
#define PRINT_UP_MSG_DISABLE        (0x01)
#define PRINT_DOWN_MSG_DISABLE      (0x02)
#define PRINT_NWK_USR_MSG_DISABLE   (0x04)

/* Ethernet link and IP link status flags */
#define ETHERNET_LINK_DOWN          (0x01)
#define ETHERNET_LINK_UP            (0x00)
#define IP_LINK_DOWN                (0x02)
#define IP_LINK_UP                  (0x00)

/* Macro for the reset value */
#define RESET_VALUE                 (0x00)

/* Macro for the name kit */
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
#elif defined (BOARD_RA8T1_MCK)
#define KIT_NAME                    "MCK-RA8T1"
#elif defined (BOARD_RA8P1_EK)
#define KIT_NAME                    "EK-RA8P1"
#elif defined (BOARD_RA8T2_MCK)
#define KIT_NAME                    "MCK-RA8T2"
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
    uint32_t sent;     /* Ping request */
    uint32_t received; /* Ping response */
    uint32_t lost;     /* Ping failure */
} ping_data_t;

/* External variables */
extern bool g_dhcp_in_use;
extern ping_data_t g_ping_data;
#if (ipconfigUSE_DHCP != 0)
extern IPV4Parameters_t g_xnd;
#endif

/* Function declarations */
#if (ipconfigUSE_DHCP != 0)
void update_dhcp_info(void);
eDHCPCallbackAnswer_t xApplicationDHCPHook(eDHCPCallbackPhase_t eDHCPPhase, uint32_t ulIPAddress);
#endif /* ipconfigUSE_DHCP */

#if (ipconfigDHCP_REGISTER_HOSTNAME == 1)
const char * pcApplicationHostnameHook(void);
#endif /* ipconfigDHCP_REGISTER_HOSTNAME */

BaseType_t ethernet_init (uint8_t const * p_ip_address, uint8_t const * p_netmask, uint8_t const * p_gateway_address,
                        uint8_t const * p_dns_server_address, uint8_t const * p_mac_address);
uint32_t is_network_up(void);
BaseType_t send_ping_request(const char * p_ip_address);
uint32_t get_host_by_name(const char * pcHostName);
void inet_ntoa(uint32_t ulIPAddress, char * pcBuffer);
uint32_t ulApplicationGetNextSequenceNumber(uint32_t ulSourceAddress, uint16_t usSourcePort,
                                            uint32_t ulDestinationAddress, uint16_t usDestinationPort);

#endif /* ETHERNET_EP_H_ */
