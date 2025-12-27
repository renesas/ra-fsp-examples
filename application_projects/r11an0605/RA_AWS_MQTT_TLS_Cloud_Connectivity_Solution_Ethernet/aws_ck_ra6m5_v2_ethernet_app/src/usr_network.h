/***********************************************************************************************************************
 * File Name    : usr_network.h
 * Description  : Contains macros, data structures and functions used in the Application
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/ 
#ifndef USR_NETWORK_H_
#define USR_NETWORK_H_

#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_Sockets.h"
#if( ipconfigUSE_DHCP != 0 )
#include "FreeRTOS_DHCP.h"
#endif

#define USR_TEST_PING_IP "8.8.8.8"
#define USR_PING_COUNT (100)

#define ETHERNET_LINK_DOWN (0x01)
#define ETHERNET_LINK_UP (0x00)
#define IP_LINK_DOWN (0x02)
#define IP_LINK_UP (0x00)

#if( ipconfigUSE_DHCP != 0 )
void updateDhcpResponseToUsr(void);
#endif

typedef struct st_ping_data
{
    uint32_t sent;     // Ping Request
    uint32_t received; // Ping Response
    uint32_t lost;     // Ping failure
} ping_data_t;

BaseType_t network_ip_init(void);
void print_ipconfig(void);
uint32_t isNetworkUp(void);

#endif /* USR_NETWORK_H_ */
