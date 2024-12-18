/***********************************************************************************************************************
 * File Name    : usr_network.c
 * Description  : Contains Ethernet and network related data structures and functions used in the application
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2015 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "application_thread.h"
#include "usr_network.h"
#include "common_utils.h"

#if( ipconfigUSE_DHCP != 0 )
    extern NetworkAddressingParameters_t xNetworkAddressing;
    NetworkAddressingParameters_t xNd = {RESET_VALUE};

   /* DHCP populates these IP address, Sub net mask and Gateway Address. So start with this is zeroed out values
    * The MAC address is Test MAC address.
    */
    static  uint8_t ucMACAddress[ 6 ]       = {0x00, 0x11, 0x22, 0x33, 0x44, 0x75};
    static  uint8_t ucIPAddress[ 4 ]        = {RESET_VALUE};
    static  uint8_t ucNetMask[ 4 ]          = {RESET_VALUE};
    static  uint8_t ucGatewayAddress[ 4 ]   = {RESET_VALUE};
    static  uint8_t ucDNSServerAddress[ 4 ] = {RESET_VALUE};
#else
    /* Static IP configuration, when DHCP mode is not used for the Example Project.
     * This needs to be populated by the user according to the Network Settings of your LAN.
     * This sample address taken from the LAN where it is tested. This is different for different LAN.
     * get the Address using the PC IPconfig details.
     */
    static  uint8_t ucMACAddress[ 6 ]       = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
    static  uint8_t ucIPAddress[ 4 ]        = {192, 168, 0, 52};
    static  uint8_t ucNetMask[ 4 ]          = {255, 255, 255, 0};
    static  uint8_t ucGatewayAddress[ 4 ]   = {192, 168, 0, 3};
    static  uint8_t ucDNSServerAddress[ 4 ] = {10, 60, 1, 2};
#endif

uint32_t  dhcp_in_use   = RESET_VALUE;
ping_data_t ping_data   = {RESET_VALUE, RESET_VALUE, RESET_VALUE};

/*******************************************************************************************************************//**
* @brief      This is the User Hook for the DHCP Response. xApplicationDHCPHook() is called by DHCP Client Code when DHCP
*             handshake messages are exchanged from the Server.
* @param[in]  Different Phases of DHCP Phases and the Offered IP Address
* @retval     Returns DHCP Answers.
**********************************************************************************************************************/
#if( ipconfigUSE_DHCP != 0 )
eDHCPCallbackAnswer_t xApplicationDHCPHook( eDHCPCallbackPhase_t eDHCPPhase, uint32_t lulIPAddress )
{
    eDHCPCallbackAnswer_t eReturn = eDHCPContinue;
    /*
     * This hook is called in a couple of places during the DHCP process, as identified by the eDHCPPhase parameter.
     */
    switch( eDHCPPhase )
    {
        case eDHCPPhasePreDiscover  :
        /*
        *  A DHCP discovery is about to be sent out.  eDHCPContinue is returned to allow the discovery to go out.
        *  If eDHCPUseDefaults had been returned instead then the DHCP process would be stopped and the statically
        *  configured IP address would be used.
        *  If eDHCPStopNoChanges had been returned instead then the DHCP process would be stopped and whatever the
        *  current network configuration was would continue to be used.
        */
        break;

        case eDHCPPhasePreRequest  :
        /* An offer has been received from the DHCP server, and the offered IP address is passed in the lulIPAddress
        * parameter.
        */
        /*
        * The sub-domains don’t match, so continue with the DHCP process so the offered IP address is used.
        */
        /* Update the Structure, the DHCP state Machine is not updating this */
            xNetworkAddressing.ulDefaultIPAddress = lulIPAddress;
            dhcp_in_use = 1;
            updateDhcpResponseToUsr();
        break;

        default :
        /*
        * Cannot be reached, but set eReturn to prevent compiler warnings where compilers are disposed to generating one.
        */
        break;
    }

    return eReturn;
}
#endif

/*******************************************************************************************************************//**
 * @brief      Update the DHCP info to the User data structure.
 * @param[in]  None
 * @retval     None
 **********************************************************************************************************************/
#if( ipconfigUSE_DHCP != 0 )
void updateDhcpResponseToUsr(void)
{
    if(dhcp_in_use)
    {
        memcpy(&xNd, &xNetworkAddressing, sizeof(xNd));
    }
}
#endif

/*******************************************************************************************************************//**
 * @brief      DHCP Hook function to populate the user defined Host name for the Kit.
 * @param[in]  None
 * @retval     Hostname
 **********************************************************************************************************************/
#if( ipconfigDHCP_REGISTER_HOSTNAME == 1 )
const char *pcApplicationHostnameHook(void)
{
    return KIT_NAME;
}
#endif

/*******************************************************************************************************************//**
 * @brief      Network event callback. Indicates the Network event. Added here to avoid the build errors
 * @param[in]  None
 * @retval     Hostname
 **********************************************************************************************************************/
#if ( ipconfigUSE_NETWORK_EVENT_HOOK == 1 )
void vApplicationIPNetworkEventHook (eIPCallbackEvent_t eNetworkEvent)
{
    if(eNetworkUp == eNetworkEvent)
    {
        uint32_t lulIPAddress, lulNetMask, lulGatewayAddress, lulDNSServerAddress;
        int8_t lcBuffer[ 16 ];

        /* Signal application the network is UP */
        xTaskNotifyFromISR(application_thread, eNetworkUp, eSetBits, NULL);

        /* The network is up and configured.  Print out the configuration
        obtained from the DHCP server. */
        FreeRTOS_GetAddressConfiguration( &lulIPAddress,
                                          &lulNetMask,
                                          &lulGatewayAddress,
                                          &lulDNSServerAddress );

        /* Convert the IP address to a string then print it out. */
        FreeRTOS_inet_ntoa( lulIPAddress, (char *)lcBuffer );

        /* Convert the net mask to a string then print it out. */
        FreeRTOS_inet_ntoa( lulNetMask, (char *)lcBuffer );

        /* Convert the IP address of the gateway to a string then print it out. */
        FreeRTOS_inet_ntoa( lulGatewayAddress, (char *)lcBuffer );

        /* Convert the IP address of the DNS server to a string then print it out. */
        FreeRTOS_inet_ntoa( lulDNSServerAddress, (char *)lcBuffer );
    }
}
#endif

/*******************************************************************************************************************//**
* @brief      User Hook for the Ping Reply. vApplicationPingReplyHook() is called by the TCP/IP
*             stack when the stack receives a ping reply.
* @param[in]  Ping reply status and Identifier
* @retval     None
**********************************************************************************************************************/
#if ( ipconfigSUPPORT_OUTGOING_PINGS == 1 )
void vApplicationPingReplyHook( ePingReplyStatus_t eStatus, uint16_t usIdentifier )
{
    (void)  usIdentifier;

    switch( eStatus )
    {
        /* A valid ping reply has been received */
        case eSuccess    :
            ping_data.received++;
            break;
            /* A reply was received but it was not valid. */
        case eInvalidData :
        default:
            ping_data.lost++;
            break;
    }
}
#endif

/*******************************************************************************************************************//**
 * @brief      Initialize IP through FreeRTOS_IPInit
 * @param[in]  void
 * @retval     None
 **********************************************************************************************************************/
BaseType_t network_ip_init(void)
{
    BaseType_t status = pdFALSE;
    /* FreeRTOS IP Initialization: This init initializes the IP stack  */
    status = FreeRTOS_IPInit(ucIPAddress, ucNetMask, ucGatewayAddress, ucDNSServerAddress, ucMACAddress);
    if(pdFALSE == status)
    {
        LogError(("FreeRTOS_IPInit Failed"));
    }

    return status;
}

/*******************************************************************************************************************//**
 * @brief      Creates and prints the the IP configuration to display on the RTT console
 * @param[in]  void
 * @retval     None
 **********************************************************************************************************************/
void print_ipconfig(void)
{
#if( ipconfigUSE_DHCP != 0 )
    if(dhcp_in_use)
    {
        ucNetMask[3] = (uint8_t)((xNd.ulNetMask & 0xFF000000) >> 24);
        ucNetMask[2] = (uint8_t)((xNd.ulNetMask & 0x00FF0000) >> 16);
        ucNetMask[1] = (uint8_t)((xNd.ulNetMask & 0x0000FF00) >> 8);
        ucNetMask[0] = (uint8_t)(xNd.ulNetMask & 0x000000FF);

        ucGatewayAddress[3] = (uint8_t)((xNd.ulGatewayAddress & 0xFF000000)>> 24);;
        ucGatewayAddress[2] = (uint8_t)((xNd.ulGatewayAddress & 0x00FF0000)>> 16);
        ucGatewayAddress[1] = (uint8_t)((xNd.ulGatewayAddress & 0x0000FF00)>> 8);
        ucGatewayAddress[0] = (uint8_t)(xNd.ulGatewayAddress & 0x000000FF);

        ucDNSServerAddress[3] = (uint8_t)((xNd.ulDNSServerAddress & 0xFF000000)>> 24);
        ucDNSServerAddress[2] = (uint8_t)((xNd.ulDNSServerAddress & 0x00FF0000)>> 16);
        ucDNSServerAddress[1] = (uint8_t)((xNd.ulDNSServerAddress & 0x0000FF00)>> 8);
        ucDNSServerAddress[0] = (uint8_t)(xNd.ulDNSServerAddress & 0x000000FF);

        ucIPAddress[3] = (uint8_t)((xNd.ulDefaultIPAddress & 0xFF000000) >> 24);
        ucIPAddress[2] = (uint8_t)((xNd.ulDefaultIPAddress & 0x00FF0000) >> 16);
        ucIPAddress[1] = (uint8_t)((xNd.ulDefaultIPAddress & 0x0000FF00) >> 8);
        ucIPAddress[0] = (uint8_t)(xNd.ulDefaultIPAddress & 0x000000FF);
    }
#endif
    APP_PRINT("\r\nEthernet adapter for Renesas "KIT_NAME":\r\n");

    APP_PRINT("\tDescription . . . . . . . . . . . : Renesas "KIT_NAME" Ethernet\r\n");
    APP_PRINT("\tPhysical Address. . . . . . . . . : %02x-%02x-%02x-%02x-%02x-%02x\r\n",
            ucMACAddress[0], ucMACAddress[1], ucMACAddress[2],
            ucMACAddress[3], ucMACAddress[4], ucMACAddress[5]);
    APP_PRINT("\tDHCP Enabled. . . . . . . . . . . : %s\r\n",dhcp_in_use?"Yes":"No");
    APP_PRINT("\tIPv4 Address. . . . . . . . . . . : %d.%d.%d.%d\r\n",
            ucIPAddress[0], ucIPAddress[1], ucIPAddress[2], ucIPAddress[3]);
    APP_PRINT("\tSubnet Mask . . . . . . . . . . . : %d.%d.%d.%d\r\n",
            ucNetMask[0], ucNetMask[1], ucNetMask[2], ucNetMask[3]);
    APP_PRINT("\tDefault Gateway . . . . . . . . . : %d.%d.%d.%d\r\n",
            ucGatewayAddress[0], ucGatewayAddress[1], ucGatewayAddress[2], ucGatewayAddress[3]);
    APP_PRINT("\tDNS Servers . . . . . . . . . . . : %d.%d.%d.%d\r\n",
            ucDNSServerAddress[0], ucDNSServerAddress[1], ucDNSServerAddress[2], ucDNSServerAddress[3]);
}

/*******************************************************************************************************************//**
 * @brief      This Function checks the Network status (Both Ethernet and IP Layer). If the Network is down
 *             the Application will not send any data on the network.
 * @param[in]  None
 * @retval     Network Status
 **********************************************************************************************************************/
uint32_t isNetworkUp(void)
{
    fsp_err_t  eth_link_status = FSP_ERR_NOT_OPEN;
    BaseType_t networkUp = pdFALSE;
    uint32_t network_status = (IP_LINK_UP | ETHERNET_LINK_UP);

    networkUp = FreeRTOS_IsNetworkUp();
    eth_link_status = R_ETHER_LinkProcess(g_ether0.p_ctrl);

    if((FSP_SUCCESS == eth_link_status) && (pdTRUE == networkUp))
    {
        return network_status;
    }
    else
    {
        if(FSP_SUCCESS != eth_link_status)
        {
            network_status |= ETHERNET_LINK_DOWN;
        }
        else if(FSP_SUCCESS == eth_link_status)
        {
            network_status |= ETHERNET_LINK_UP;
        }

        if(pdTRUE != networkUp)
        {
             network_status |= IP_LINK_DOWN;
        }
        else if(pdTRUE == networkUp)
        {
             network_status |= IP_LINK_UP;
        }

        return network_status;
    }
}
