/***********************************************************************************************************************
 * File Name    : usr_network.h
 * Description  : Contains macros, data structures and functions used  in the Application
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
#ifndef USR_NETWORK_H_
#define USR_NETWORK_H_

#if( ipconfigUSE_DHCP != 0 )
    #include "FreeRTOS_DHCP.h"
#endif

#define USR_TEST_PING_IP "8.8.8.8"
#define USR_PING_COUNT 100


#define ETHERNET_LINK_DOWN 0x01
#define ETHERNET_LINK_UP 0x00
#define IP_LINK_DOWN 0x02
#define IP_LINK_UP 0x00

#if( ipconfigUSE_DHCP != 0 )
    void updateDhcpResponseToUsr(void);
#endif

#if( ipconfigUSE_DHCP != 0 )
extern    eDHCPCallbackAnswer_t xApplicationDHCPHook( eDHCPCallbackPhase_t eDHCPPhase, uint32_t ulIPAddress );
#endif

#if( ipconfigDHCP_REGISTER_HOSTNAME == 1 )
    /* DHCP has an option for clients to register their hostname.  It doesn't
    have much use, except that a device can be found in a router along with its
    name. If this option is used the callback below must be provided by the
    application writer to return a const string, denoting the device's name. */
    const char *pcApplicationHostnameHook( void );
#endif /* ipconfigDHCP_REGISTER_HOSTNAME */

void print_ipconfig(void);
BaseType_t usr_network_init();
uint32_t isNetworkUp(void);

extern TaskHandle_t application_thread;

#endif /* USR_NETWORK_H_ */
