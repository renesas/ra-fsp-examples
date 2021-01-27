/***********************************************************************************************************************
 * File Name    : usr_network.c
 * Description  : Contains Ethernet and network related data structures and functions used in the application
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright [2015-2020] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
 *
 * The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
 * and/or its licensors ("Renesas") and subject to statutory and contractual protections.
 *
 * This file is subject to a Renesas FSP license agreement. Unless otherwise agreed in an FSP license agreement with
 * Renesas: 1) you may not use, copy, modify, distribute, display, or perform the contents; 2) you may not use any name
 * or mark of Renesas for advertising or publicity purposes or in connection with your use of the contents; 3) RENESAS
 * MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED
 * "AS IS" WITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, AND NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR
 * CONSEQUENTIAL DAMAGES, INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF
 * CONTRACT OR TORT, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents
 * included in this file may be subject to different terms.
 **********************************************************************************************************************/

#include "application_thread.h"
#include "common_utils.h"
#include "FreeRTOS.h"
#include "FreeRTOSIPConfig.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_TCP_IP.h"
#include "usr_config.h"
#include "usr_network.h"
#include "usr_hal.h"

#if( ipconfigUSE_DHCP != 0 )
   /* DHCP populates these IP address, Sub net mask and Gateway Address. So start with this is zeroed out values
    * The MAC address is Test MAC address.
    */
    static  uint8_t ucMACAddress[ 6 ]       = {0x00, 0x11, 0x22, 0x33, 0x44, 0x75};
    static  uint8_t ucIPAddress[ 4 ]        = {RESET_VALUE};
    static  uint8_t ucNetMask[ 4 ]          = {RESET_VALUE};
    static  uint8_t ucGatewayAddress[ 4 ]   = {RESET_VALUE};
    static  uint8_t ucDNSServerAddress[ 4 ] = {RESET_VALUE};
#else /* STATIC IP MODE */
#if ATWORK
    /* Static IP configuration, when DHCP mode is not used for the Application Project.
     * This needs to be populated by the user according to the Network Settings of your LAN.
     * This sample address taken from the LAN where it is tested. This is different for different LAN.
     * get the Address using the PC IPconfig details.
     */
    static  uint8_t ucMACAddress[ 6 ]       = {0x00, 0x11, 0x22, 0x33, 0x44, 0x95};
    static  uint8_t ucIPAddress[ 4 ]        = {132, 158, 145, 200};
    static  uint8_t ucNetMask[ 4 ]          = {255, 255, 254, 0};
    static  uint8_t ucGatewayAddress[ 4 ]   = {132, 158, 145, 254};
    static  uint8_t ucDNSServerAddress[ 4 ] = {132, 158, 118, 225};
#elif ATHOME
    /* Static IP configuration, when DHCP mode is not used for the Application Project.
     * This needs to be populated by the user according to the Network Settings of your LAN.
     * This sample address taken from the LAN where it is tested. This is different for different LAN.
     * get the Address using the PC IPconfig details.
     */
    static  uint8_t ucMACAddress[ 6 ]       = {0x00, 0x11, 0x22, 0x33, 0x44, 0x95};
    static  uint8_t ucIPAddress[ 4 ]        = {10, 0, 0, 20};
    static  uint8_t ucNetMask[ 4 ]          = {255, 255, 255, 0};
    static  uint8_t ucGatewayAddress[ 4 ]   = {10, 0, 0, 1};
    static  uint8_t ucDNSServerAddress[ 4 ] = {75, 75, 75, 75};
#else
    /* Static IP configuration, when DHCP mode is not used for the Application Project.
     * This needs to be populated by the user according to the Network Settings of your LAN.
     * This sample address taken from the LAN where it is tested. This is different for different LAN.
     * get the Address using the PC IPconfig details.
     */
    static  uint8_t ucMACAddress[ 6 ]       = {0x00, 0x11, 0x22, 0x33, 0x44, 0x95};
    static  uint8_t ucIPAddress[ 4 ]        = {192, 168, 0, 20};
    static  uint8_t ucNetMask[ 4 ]          = {255, 255, 255, 0};
    static  uint8_t ucGatewayAddress[ 4 ]   = {192, 168, 0, 254};
    static  uint8_t ucDNSServerAddress[ 4 ] = {75, 75, 75, 75};

#endif
#endif
#if( ipconfigUSE_DHCP != 0 )
    extern NetworkAddressingParameters_t xNetworkAddressing;
    NetworkAddressingParameters_t xNd = {RESET_VALUE};
#else
    NetworkAddressingParameters_t xNd = {RESET_VALUE};
#endif

uint32_t  dhcp_in_use     = RESET_VALUE;
uint32_t  socket_in_use   = RESET_VALUE;
uint32_t  usrPingCount    = RESET_VALUE;


/*******************************************************************************************************************//**
* @brief      User application network initialization.
*               
* @param[in]  None
* @retval     None
**********************************************************************************************************************/

BaseType_t usr_network_init()
{
    BaseType_t status = RESET_VALUE;
    /* FreeRTOS IP Initialization: This initializes the IP stack  */
    status = FreeRTOS_IPInit(ucIPAddress, ucNetMask, ucGatewayAddress, ucDNSServerAddress, ucMACAddress);

    if(pdFALSE == status)
    {
        IotLogError("FreeRTOS_IPInit Failed");
    }
    return status;
}


/*******************************************************************************************************************//**
 * @brief      Creates and prints the the IP configuration to display on to the console
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
    IotLog("\r\nEthernet adapter for Renesas "KIT_NAME":\r\n");

    IotLog("\tDescription . . . . . . . . . . . : Renesas "KIT_NAME" Ethernet\r\n");
    IotLog("\tPhysical Address. . . . . . . . . : %02x-%02x-%02x-%02x-%02x-%02x\r\n",ucMACAddress[0],ucMACAddress[1],ucMACAddress[2],ucMACAddress[3],ucMACAddress[4],ucMACAddress[5]);
    IotLog("\tDHCP Enabled. . . . . . . . . . . : %s\r\n",dhcp_in_use?"Yes":"No");
    IotLog("\tIPv4 Address. . . . . . . . . . . : %d.%d.%d.%d\r\n",ucIPAddress[0],ucIPAddress[1],ucIPAddress[2],ucIPAddress[3]);
    IotLog("\tSubnet Mask . . . . . . . . . . . : %d.%d.%d.%d\r\n",ucNetMask[0],ucNetMask[1],ucNetMask[2],ucNetMask[3]);
    IotLog("\tDefault Gateway . . . . . . . . . : %d.%d.%d.%d\r\n",ucGatewayAddress[0],ucGatewayAddress[1],ucGatewayAddress[2],ucGatewayAddress[3]);
    IotLog("\tDNS Servers . . . . . . . . . . . : %d.%d.%d.%d\r\n",ucDNSServerAddress[0],ucDNSServerAddress[1],ucDNSServerAddress[2],ucDNSServerAddress[3]);
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

/*******************************************************************************************************************//**
* @brief      This is the User Hook for the DHCP Response. xApplicationDHCPHook() is called by DHCP Client Code when DHCP
*             handshake messages are exchanged from the Server.
* @param[in]  Different Phases of DHCP Phases and the Offered IP Address
* @retval     Returns DHCP Answers.
**********************************************************************************************************************/
#if( ipconfigUSE_DHCP != 0 )
eDHCPCallbackAnswer_t xApplicationDHCPHook( eDHCPCallbackPhase_t eDHCPPhase,
                                            uint32_t lulIPAddress )
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
 * @brief      Network event callback. Indicates the Network event. Added here to avoid the build errors
 * @param[in]  None
 * @retval     Hostname
 **********************************************************************************************************************/
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
        FreeRTOS_inet_ntoa( lulIPAddress, lcBuffer );

        /* Convert the net mask to a string then print it out. */
        FreeRTOS_inet_ntoa( lulNetMask, lcBuffer );

        /* Convert the IP address of the gateway to a string then print it out. */
        FreeRTOS_inet_ntoa( lulGatewayAddress, lcBuffer );

        /* Convert the IP address of the DNS server to a string then print it out. */
        FreeRTOS_inet_ntoa( lulDNSServerAddress, lcBuffer );
    }
}

/*******************************************************************************************************************//**
* @brief      User Hook for the Ping Reply. vApplicationPingReplyHook() is called by the TCP/IP
*             stack when the stack receives a ping reply. This is added here to avoid the build error
* @param[in]  Ping reply status and Identifier
* @retval     None
**********************************************************************************************************************/
void vApplicationPingReplyHook( ePingReplyStatus_t eStatus, uint16_t usIdentifier )
{
    (void)  usIdentifier;
    (void)  eStatus;
}

/*******************************************************************************************************************//**
* @brief      DHCP Hook function to populate the user defined Host name for the Kit.
* @param[in]  None
* @retval     None
**********************************************************************************************************************/
const char * pcApplicationHostnameHook (void)
{
    return "RA6M3";
}
