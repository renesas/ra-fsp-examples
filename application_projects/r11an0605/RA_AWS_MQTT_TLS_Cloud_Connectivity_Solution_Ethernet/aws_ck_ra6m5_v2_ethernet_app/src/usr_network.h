/***********************************************************************************************************************
 * File Name    : usr_network.h
 * Description  : Contains macros, data structures and functions used in the Application
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
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
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
