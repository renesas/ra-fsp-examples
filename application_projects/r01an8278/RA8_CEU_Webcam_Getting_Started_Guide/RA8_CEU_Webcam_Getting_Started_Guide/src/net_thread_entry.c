/***********************************************************************************************************************
 * File Name    : net_thread_entry.c
 * Description  : The functionality of this file is obtain the IPv4 address
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#include "net_thread.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_Sockets.h"
#include "common_utils.h"
#include "usr_app.h"

static void print_ipconfig(void);
static uint32_t ulRand();
static uint32_t isNetworkUp(void);

#if( ipconfigUSE_DHCP != 0 )
   /* DHCP populates these IP address, Sub net mask and Gateway Address. So start with this is zeroed out values
    * The MAC address is Test MAC address.
    */
    uint8_t g_mac_address[MAC_ADDR_SIZE]            = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
    uint8_t g_ip_address[IP_ADDR_SIZE]              = {RESET_VALUE};
    uint8_t g_netmask[NETMASK_SIZE]                 = {255, 255, 255, 128};
    uint8_t g_gateway_address[GATEWAY_ADDR_SIZE]    = {132, 158, 124, 1};
    uint8_t g_dns_server_address[DNS_ADDR_SIZE]     = {RESET_VALUE};
#else
    /* Static IP configuration, when DHCP mode is not used for the Application Project.
     * This needs to be populated by the user according to the Network Settings of your LAN.
     * This sample address taken from the LAN where it is tested. This is different for different LAN.
     * get the Address using the PC IPconfig details.
     */
    static uint8_t g_mac_address[MAC_ADDR_SIZE]             = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
    static uint8_t g_ip_address[IP_ADDR_SIZE]               = {192, 168, 0, 52};
    static uint8_t g_netmask[NETMASK_SIZE]                  = {255, 255, 255, 0};
    static uint8_t g_gateway_address[GATEWAY_ADDR_SIZE]     = {192, 168, 0, 3};
    Static uint8_t g_dns_server_address[DNS_ADDR_SIZE]      = {10, 60, 1, 2};
#endif


#if( ipconfigUSE_DHCP != 0 )
    IPV4Parameters_t xNd = {RESET_VALUE, RESET_VALUE, RESET_VALUE, {RESET_VALUE, RESET_VALUE}, RESET_VALUE, RESET_VALUE};
#endif

static uint32_t  g_dhcp_in_use      = RESET_VALUE;

/**********************************************************************************************************************
 * @brief      Generates 32 bit Random number
 * @param[in]  void
 * @retval     Random Number
 **********************************************************************************************************************/
static uint32_t ulRand()
{
    /* example of a 32-bit random number generator.
     * Here rand() returns a 15-bit number. so create 32 bit Random number using 15 bit rand()
     */
    uint32_t ulResult =
        ((((uint32_t) rand()) & 0x7fffuL)       ) |
        ((((uint32_t) rand()) & 0x7fffuL) << 15 ) |
        ((((uint32_t) rand()) & 0x0003uL) << 30 );
    return ulResult;
}

/***********************************************************************************************************************
* @brief      Generates 32 sequence number
* @param[in]  Source IP, Source Port, Destination IP and Destination Port.
* @retval     Sequence Number
***********************************************************************************************************************/
uint32_t ulApplicationGetNextSequenceNumber(uint32_t ulSourceAddress, uint16_t usSourcePort, uint32_t ulDestinationAddress,
                                            uint16_t usDestinationPort)
{
    /* Here we need to get random number for the sequence number.
     * This is just for testing purpose, so software rand() is okay.
     * This can also be tied to the TRNG.
     */

     return((ulSourceAddress + ulDestinationAddress + usSourcePort + usDestinationPort) ^ ulRand());
}

/**********************************************************************************************************************
* @brief      This is the User Thread for the EP.
* @param[in]  Thread specific parameters
* @retval     None
**********************************************************************************************************************/
void net_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);
    BaseType_t status            = pdFALSE;

    TERM_INIT();

    /* Print the Application Banner and Informations */
    APP_PRINT(BANNER_INFO);
    APP_PRINT(AP_INFO);

    APP_PRINT("\r\nStarting the Network !!!\r\n");

    /* FreeRTOS IP Initialization: This init initializes the IP stack  */
    status = FreeRTOS_IPInit(g_ip_address, g_netmask, g_gateway_address, g_dns_server_address, g_mac_address);
    if(status == pdFALSE)
    {
        APP_ERR_PRINT("\r\nFreeRTOS_IPInit Failed\r\n");
        APP_ERR_TRAP(status);
    }

    /* Wait for the Network link up */
    while ((isNetworkUp() & (IP_LINK_UP | ETHERNET_LINK_UP))
            != (IP_LINK_UP | ETHERNET_LINK_UP))
    {
        APP_PRINT("!");
        vTaskDelay(pdMS_TO_TICKS(500));
    }

#if( ipconfigUSE_DHCP != 0 )
                /* Display the New IP credentials obtained from the DHCP server */
                updateDhcpResponseToUsr ();
#endif
    print_ipconfig();

    /* Network established announcement */
    APP_PRINT("\r\nNetwork Established !!!\r\n");

    /* Notify Camera Thread task for Encode method setup*/
    xEventGroupSetBits(g_system_event, EVT_NET_READY);

    /* Net task done */
    vTaskDelete(NULL);
}

/*******************************************************************************************************************//**
 * @brief      Creates and prints the the IP configuration to display on the console
 * @param[in]  void
 * @retval     None
 **********************************************************************************************************************/
static void print_ipconfig(void)
{
#if( ipconfigUSE_DHCP != 0 )
    if(g_dhcp_in_use)
    {
        g_netmask[3] = (uint8_t)((xNd.ulNetMask & 0xFF000000) >> 24);
        g_netmask[2] = (uint8_t)((xNd.ulNetMask & 0x00FF0000) >> 16);
        g_netmask[1] = (uint8_t)((xNd.ulNetMask & 0x0000FF00) >> 8);
        g_netmask[0] = (uint8_t)(xNd.ulNetMask & 0x000000FF);

        g_gateway_address[3] = (uint8_t)((xNd.ulGatewayAddress & 0xFF000000)>> 24);;
        g_gateway_address[2] = (uint8_t)((xNd.ulGatewayAddress & 0x00FF0000)>> 16);
        g_gateway_address[1] = (uint8_t)((xNd.ulGatewayAddress & 0x0000FF00)>> 8);
        g_gateway_address[0] = (uint8_t)(xNd.ulGatewayAddress & 0x000000FF);

        g_dns_server_address[3] = (uint8_t)((xNd.ulDNSServerAddresses[0] & 0xFF000000)>> 24);
        g_dns_server_address[2] = (uint8_t)((xNd.ulDNSServerAddresses[0] & 0x00FF0000)>> 16);
        g_dns_server_address[1] = (uint8_t)((xNd.ulDNSServerAddresses[0] & 0x0000FF00)>> 8);
        g_dns_server_address[0] = (uint8_t)(xNd.ulDNSServerAddresses[0] & 0x000000FF);

        g_ip_address[3] = (uint8_t)((xNd.ulIPAddress & 0xFF000000) >> 24);
        g_ip_address[2] = (uint8_t)((xNd.ulIPAddress & 0x00FF0000) >> 16);
        g_ip_address[1] = (uint8_t)((xNd.ulIPAddress & 0x0000FF00) >> 8);
        g_ip_address[0] = (uint8_t)(xNd.ulIPAddress & 0x000000FF);
    }
#endif

    APP_PRINT("\r\n\r\n\tPhysical Address. . . . : %02x-%02x-%02x-%02x-%02x-%02x\r\n",
            g_mac_address[0],g_mac_address[1],g_mac_address[2],g_mac_address[3],g_mac_address[4],g_mac_address[5]);
    APP_PRINT("\tIPv4 Address. . . . . . : %d.%d.%d.%d\r\n",g_ip_address[0],g_ip_address[1],g_ip_address[2],g_ip_address[3]);
    APP_PRINT("\tSubnet Mask . . . . . . : %d.%d.%d.%d\r\n",g_netmask[0],g_netmask[1],g_netmask[2],g_netmask[3]);
    APP_PRINT("\tDefault Gateway . . . . : %d.%d.%d.%d\r\n",g_gateway_address[0],g_gateway_address[1],g_gateway_address[2],g_gateway_address[3]);

    APP_PRINT("\r\nWebpage URL  %d.%d.%d.%d/index.html\n", g_ip_address[0],g_ip_address[1],g_ip_address[2],g_ip_address[3]);
}

/*******************************************************************************************************************//**
 * @brief      This Function checks the Network status (Both Ethernet and IP Layer). If the Network is down
 *             the Application will not send any data on the network.
 * @param[in]  None
 * @retval     Network Status
 **********************************************************************************************************************/
static uint32_t isNetworkUp(void)
{
    fsp_err_t eth_link_status = FSP_ERR_NOT_OPEN;      // Default status for Ethernet link
    BaseType_t networkUp      = pdFALSE;               // Default status for IP link
    uint32_t network_status   = RESET_VALUE;           // Combined status of both links, initially no flags set

#if (ipconfigUSE_DHCP != 0)
    if (!g_dhcp_in_use)
    {
        return IP_LINK_DOWN;
    }
#endif

    /* Check Ethernet link status */
    eth_link_status = R_ETHER_LinkProcess(g_ether0.p_ctrl);
    if (FSP_SUCCESS == eth_link_status)
    {
        network_status |= ETHERNET_LINK_UP;
    }
    else
    {
        network_status |= ETHERNET_LINK_DOWN;
    }

    /* Check IP link status using FreeRTOS network function */
    networkUp = FreeRTOS_IsNetworkUp();
    if (pdTRUE == networkUp)
    {
        network_status |= IP_LINK_UP;
    }
    else
    {
        network_status |= IP_LINK_DOWN;
    }

    return network_status;
}

/**********************************************************************************************************************
* @brief      This is the User Hook for the DHCP Response. xApplicationDHCPHook() is called by DHCP Client Code when DHCP
*             handshake messages are exchanged from the Server.
* @param[in]  Different Phases of DHCP Phases and the Offered IP Address
* @retval     Returns DHCP Answers.
**********************************************************************************************************************/
#if( ipconfigUSE_DHCP != 0 )
eDHCPCallbackAnswer_t xApplicationDHCPHook( eDHCPCallbackPhase_t eDHCPPhase,
                                            uint32_t ulIPAddress )
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
      /* An offer has been received from the DHCP server, and the offered IP address is passed in the ulIPAddress
       * parameter.
       */

        /*
         * The sub-domains don’t match, so continue with the DHCP process so the offered IP address is used.
         */
        /* Update the Structure, the DHCP state Machine is not updating this */
        xNd.ulIPAddress = ulIPAddress;
        g_dhcp_in_use = 1;
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

/**********************************************************************************************************************
 * @brief      Update the DHCP info to the User data structure.
 * @param[in]  None
 * @retval     None
 **********************************************************************************************************************/
#if( ipconfigUSE_DHCP != 0 )
void updateDhcpResponseToUsr(void)
{
    if(g_dhcp_in_use)
    {
        xNd.ulNetMask = FreeRTOS_GetNetmask();
        xNd.ulGatewayAddress = FreeRTOS_GetGatewayAddress();
        xNd.ulDNSServerAddresses[0] = FreeRTOS_GetDNSServerAddress();
    }
}
#endif
