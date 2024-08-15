/***********************************************************************************************************************
 * File Name    : usr_app.h
 * Description  : Contains macros, data structures and functions used  in the Application
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef USR_APP_H_
#define USR_APP_H_

#if( ipconfigUSE_DHCP != 0 )
    #include "FreeRTOS_DHCP.h"
#endif

#define USR_TEST_DOMAIN_NAME "www.renesas.com"
#define USR_TEST_PING_IP "172.217.160.174"
#define USR_PING_COUNT (4)

#define SUCCESS (0)
#define PRINT_UP_MSG_DISABLE (0x01)
#define PRINT_DOWN_MSG_DISABLE (0x02)
#define PRINT_NWK_USR_MSG_DISABLE (0x04)


#define ETHERNET_LINK_DOWN (0x01)
#define ETHERNET_LINK_UP (0x00)
#define IP_LINK_DOWN (0x02)
#define IP_LINK_UP (0x00)

#if (ipconfigUSE_DHCP != 0)
/**********************************************************************************************************************
 * Function Name: updateDhcpResponseToUsr
 * Description  : .
 * Arguments    : None
 * Return Value : None.
 *********************************************************************************************************************/
void updateDhcpResponseToUsr (void);
#endif

#if (ipconfigUSE_DHCP != 0)
/**********************************************************************************************************************
 * Function Name: xApplicationDHCPHook
 * Description  : .
 * Arguments    : eDHCPPhase
 *              : ulIPAddress
 * Return Value : .
 *********************************************************************************************************************/
eDHCPCallbackAnswer_t xApplicationDHCPHook (eDHCPCallbackPhase_t eDHCPPhase, uint32_t ulIPAddress);
#endif

#if (ipconfigDHCP_REGISTER_HOSTNAME == 1)
    /* DHCP has an option for clients to register their hostname.  It doesn't
    have much use, except that a device can be found in a router along with its
    name. If this option is used the callback below must be provided by the
    application writer to return a const string, denoting the device's name. */
/**********************************************************************************************************************
 * Function Name: pcApplicationHostnameHook
 * Description  : .
 * Arguments    : None
 * Return Value : .
 *********************************************************************************************************************/
const char * pcApplicationHostnameHook (void);
#endif /* ipconfigDHCP_REGISTER_HOSTNAME */

/**********************************************************************************************************************
 * Function Name: ulApplicationGetNextSequenceNumber
 * Description  : .
 * Arguments    : ulSourceAddress
 *              : usSourcePort
 *              : ulDestinationAddress
 *              : usDestinationPort
 * Return Value : .
 *********************************************************************************************************************/
uint32_t ulApplicationGetNextSequenceNumber(uint32_t ulSourceAddress,
                                                uint16_t usSourcePort,
                                                uint32_t ulDestinationAddress,
                                                uint16_t usDestinationPort );
/**********************************************************************************************************************
 * Function Name: ulRand
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
uint32_t ulRand ();
/**********************************************************************************************************************
 * Function Name: isNetworkUp
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
uint32_t isNetworkUp (void);
/**********************************************************************************************************************
 * Function Name: vSendPing
 * Description  : .
 * Argument     : pcIPAddress
 * Return Value : .
 *********************************************************************************************************************/
BaseType_t vSendPing (const char *pcIPAddress);
/**********************************************************************************************************************
 * Function Name: print_ipconfig
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void print_ipconfig (void);
/**********************************************************************************************************************
 * Function Name: print_pingResult
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void print_pingResult (void);
/**********************************************************************************************************************
 * Function Name: dnsQuerryFunc
 * Description  : .
 * Argument     : domain
 * Return Value : .
 *********************************************************************************************************************/
void dnsQuerryFunc (char *domain);

typedef struct st_ping_data
{
    uint32_t sent;     // Ping Request
    uint32_t received; // Ping Response
    uint32_t lost;     // Ping failure
} ping_data_t;

#endif /* USR_APP_H_ */
