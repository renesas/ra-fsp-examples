/***********************************************************************************************************************
 * File Name    : usr_network.h
 * Description  : Contains macros, data structures and functions used  in the Application
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
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
extern     const char *pcApplicationHostnameHook( void );
#endif /* ipconfigDHCP_REGISTER_HOSTNAME */

extern void print_ipconfig(void);
extern BaseType_t usr_network_init();
extern uint32_t isNetworkUp(void);

extern TaskHandle_t application_thread;

#endif /* USR_NETWORK_H_ */
