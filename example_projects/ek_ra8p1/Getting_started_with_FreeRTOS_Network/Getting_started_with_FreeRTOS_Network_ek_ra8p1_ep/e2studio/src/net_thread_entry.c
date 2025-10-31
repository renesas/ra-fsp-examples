/***********************************************************************************************************************
 * File Name    : net_thread_entry.c
 * Description  : This file contains the User Application code for the Ethernet + TCP/IP.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "net_thread.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_Sockets.h"
#include "common_utils.h"
#include "usr_app.h"

static void process_user_input(void);
static void print_ipconfig(void);
static void dnsQuerryFunc(char *domain);
static uint32_t ulRand();
static uint32_t isNetworkUp(void);
static void ping_ip_address(char *destination);

/* IP address of the PC or any Device on the LAN/WAN where the Ping request is sent
 * Note: The user needs to change this according to the LAN settings of your Test PC or device
 * when running this project */
uint8_t *gp_remote_ip_address = (uint8_t *)USR_TEST_PING_IP;
uint8_t *gp_domain_name       = (uint8_t *)USR_TEST_DOMAIN_NAME;

#if (ipconfigUSE_DHCP != 0)
   /* DHCP populates these IP address, Sub net mask and Gateway Address. So start with this is zeroed out values
    * The MAC address is Test MAC address */
    uint8_t g_mac_address[MAC_ADDR_SIZE]         = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
    uint8_t g_ip_address[IP_ADDR_SIZE]           = {RESET_VALUE};
    uint8_t g_netmask[NETMASK_SIZE]              = {255, 255, 255, 128};
    uint8_t g_gateway_address[GATEWAY_ADDR_SIZE] = {132, 158, 124, 1};
    uint8_t g_dns_server_address[DNS_ADDR_SIZE]  = {RESET_VALUE};
#else
    /* Static IP configuration, when DHCP mode is not used for the example project
     * This needs to be populated by the user according to the Network Settings of your LAN
     * This sample address taken from the LAN where it is tested. This is different for different LAN
     * get the Address using the PC IPconfig details */
    static uint8_t g_mac_address[MAC_ADDR_SIZE]         = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
    static uint8_t g_ip_address[IP_ADDR_SIZE]           = {192, 168, 0, 52};
    static uint8_t g_netmask[NETMASK_SIZE]              = {255, 255, 255, 0};
    static uint8_t g_gateway_address[GATEWAY_ADDR_SIZE] = {192, 168, 0, 3};
    Static uint8_t g_dns_server_address[DNS_ADDR_SIZE]  = {10, 60, 1, 2};
#endif /* ipconfigUSE_DHCP != 0 */

#if (ipconfigUSE_DHCP != 0)
    IPV4Parameters_t xNd = {RESET_VALUE, RESET_VALUE, RESET_VALUE,
                            {RESET_VALUE, RESET_VALUE}, RESET_VALUE, RESET_VALUE};
#endif /* ipconfigUSE_DHCP != 0 */

static uint32_t    g_dhcp_in_use = RESET_VALUE;
static ping_data_t g_ping_data   = {RESET_VALUE, RESET_VALUE, RESET_VALUE};

/*******************************************************************************************************************//**
 * @brief      Generates 32 bit random number.
 * @param[in]  None.
 * @retval     Random number.
 **********************************************************************************************************************/
static uint32_t ulRand()
{
    /* Example of a 32-bit random number generator.
     * Here rand() returns a 15-bit number. So create 32 bit Random number using 15 bit rand() */
    uint32_t ulResult = ((((uint32_t) rand()) & 0x7fffuL)) |
                        ((((uint32_t) rand()) & 0x7fffuL) << 15 ) |
                        ((((uint32_t) rand()) & 0x0003uL) << 30 );
    return ulResult;
}

/*******************************************************************************************************************//**
* @brief      Generates 32 sequence number.
* @param[in]  ulSourceAddress       Source IP.
*             usSourcePort          Source Port.
*             ulDestinationAddress  Destination IP.
*             usDestinationPort     Destination Port.
* @retval     Sequence number.
***********************************************************************************************************************/
uint32_t ulApplicationGetNextSequenceNumber(uint32_t ulSourceAddress, uint16_t usSourcePort,
                                            uint32_t ulDestinationAddress,
                                            uint16_t usDestinationPort)
{
    /* Here we need to get random number for the sequence number
     * This is just for testing purpose, so software rand() is okay
     * This can also be tied to the TRNG */
     return((ulSourceAddress + ulDestinationAddress + usSourcePort + usDestinationPort) && ulRand());
}

/*******************************************************************************************************************//**
* @brief      Send ICMP Ping request based on the user input IP address.
* @param[in]  pcIPAddress       IP address to ping.
* @retval     PING_SUCCESS if ping to the IP address is successful, otherwise returns PING_TIMEOUT.
***********************************************************************************************************************/
ping_status_t SendPing(const char *pcIPAddress)
{
    uint32_t ulIPAddress;
    uint16_t usReplySequenceNumber = 0;
    BaseType_t usRequestSequenceNumber = pdFAIL;
    ping_status_t result = PING_ERROR;

    /* The pcIPAddress parameter holds the destination IP address as a string in
     * decimal dot notation (for example, "192.168.0.200"). Convert the string into
     * the required 32-bit format */
    ulIPAddress = FreeRTOS_inet_addr(pcIPAddress);

    /* Send a ping request to the specified IP address */
    usRequestSequenceNumber = FreeRTOS_SendPingRequest(ulIPAddress, PING_DATA_SIZE, PING_TIMEOUT_MS);
    if (usRequestSequenceNumber == pdFAIL)
    {
        result = PING_ERROR;
    }
    else
    {
        /* Wait for a reply or timeout */
        if (xQueueReceive(xPingReplyQueue, &usReplySequenceNumber,
                          PING_QUEUE_WAIT_TIME_MS/portTICK_PERIOD_MS) == pdPASS)
        {
            result = PING_SUCCESS;

        }
        else
        {
            result = PING_TIMEOUT;
        }
    }

    return result;
}

/*******************************************************************************************************************//**
* @brief      The user hooks for the ping reply. vApplicationPingReplyHook() is called by the TCP/IP
*             stack when the stack receives a ping reply.
* @param[in]  Ping reply status and identifier.
* @retval     None.
***********************************************************************************************************************/
void vApplicationPingReplyHook(ePingReplyStatus_t eStatus, uint16_t usIdentifier)
{
    (void) usIdentifier;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    switch(eStatus)
    {
        /* A valid ping reply has been received */
        case eSuccess:
            xQueueSendFromISR(xPingReplyQueue, &usIdentifier, &xHigherPriorityTaskWoken);
            break;
            /* A reply was received but it was not valid */
        case eInvalidData:
        default:
            g_ping_data.lost++;
            break;
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/*******************************************************************************************************************//**
* @brief      This is the user thread for the EP.
* @param[in]  Thread specific parameters.
* @retval     None.
***********************************************************************************************************************/
void net_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);
    BaseType_t status            = pdFALSE;
    fsp_pack_version_t version   = {RESET_VALUE};
    uint32_t g_usr_print_ability = RESET_VALUE;

    TERM_INIT();

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the RTT */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor,
              version.version_id_b.patch);

    /* Prints the Ethernet configuration prior to the IP initialization */
    APP_PRINT(ETH_PREINIT);
    print_ipconfig();

    /* FreeRTOS IP initialization: This function initializes the IP stack */
    status = FreeRTOS_IPInit(g_ip_address, g_netmask, g_gateway_address, g_dns_server_address, g_mac_address);

    if(pdFALSE == status)
    {
        APP_ERR_PRINT("FreeRTOS_IPInit Failed");
        APP_ERR_TRAP(status);
    }

    APP_PRINT(ETH_POSTINIT);

    while (true)
    {
        /* Check if both the Ethernet Link and IP link are UP */
        if ((IP_LINK_UP | ETHERNET_LINK_UP) == (isNetworkUp() & (IP_LINK_UP | ETHERNET_LINK_UP)))
        {
            /* usr_print_ability is added to avoid multiple UP messages or DOWN messages repeating */
            if (!(PRINT_UP_MSG_DISABLE & g_usr_print_ability))
            {
                APP_PRINT("\r\nNetwork is Up");
                g_usr_print_ability |= PRINT_UP_MSG_DISABLE;
            }

            if (!(PRINT_NWK_USR_MSG_DISABLE & g_usr_print_ability))
            {
#if (ipconfigUSE_DHCP != 0)
                /* Display the new IP credentials obtained from the DHCP server */
                updateDhcpResponseToUsr();
#endif /* ipconfigUSE_DHCP != 0 */
                /* Update IP credentials on to the RTT console */
                print_ipconfig();
                g_usr_print_ability |= PRINT_NWK_USR_MSG_DISABLE;
            }

            /* Handler for user input */
            process_user_input();
        }
        else
        {
            if (!(PRINT_DOWN_MSG_DISABLE & g_usr_print_ability))
            {
                APP_PRINT("\r\nNetwork is Down");
                g_usr_print_ability |= PRINT_DOWN_MSG_DISABLE;
            }
            else
            {
                APP_PRINT(".");
            }
        }
        vTaskDelay(DELAY_100_TICKS);
    }
}

/******************************************************************************************************************//**
 * @brief       Processes the user input.
 * @param[IN]   None.
 * @retval      None.
 *********************************************************************************************************************/
static void process_user_input(void)
{
    char key_index[KEY_INDEX_SIZE]     = {RESET_VALUE};
    char remote_info[REMOTE_INFO_SIZE] = {RESET_VALUE};
    int err                            = STATUS_SUCCESS;
    char user_input[USER_INPUT_SIZE]   = {NULL_CHAR};

    /* Print the main menu */
    APP_PRINT(MAIN_MENU);

    /* Wait for user input */
    while (!APP_CHECK_DATA)
    {
        vTaskDelay(DELAY_10_TICKS);
    }

    APP_READ(&user_input, sizeof(user_input));

    /* Parse user input */
    sscanf((char *) &user_input, "%2s %64s", key_index, remote_info);

    /* Process the user input */
    switch (key_index[ZERO_INDEX])
    {
        case PING_OPERATION:
            ping_ip_address((char *) remote_info);
            break;

        case TCP_CLIENT_OPERATION:
            err = tcp_client_handle((char *) remote_info);
            if (STATUS_SUCCESS != err)
            {
                APP_PRINT("\r\nTCP client handle operation failed. Please try again.\r\n");
            }
            break;

        case UDP_CLIENT_OPERATION:
            err = udp_client_handle((char *) remote_info);
            if (STATUS_SUCCESS != err)
            {
                APP_PRINT("\r\nUDP client handle operation failed. Please try again.\r\n");
            }
            break;

        case DNS_OPERATION:
            dnsQuerryFunc((char *) &remote_info);
            break;

        case HTTP_SERVER:
            err = http_server_handle();
            if (STATUS_SUCCESS != err)
            {
                /* De-initialize FreeRTOS+FAT on error */
                freertos_fat_deinit();
                APP_PRINT("\r\nhttp_server_handle operation failed\r\n");
                APP_ERR_TRAP(err);
            }
            break;

        default:
            APP_PRINT("\r\nInvalid input !\r\n");
            break;
    }

    /* Reset the key index after processing */
    key_index[ZERO_INDEX] = NULL_CHAR;
}

/*******************************************************************************************************************//**
* @brief      This is the user hook for the DHCP response. xApplicationDHCPHook() is called by DHCP Client Code when
*             DHCP handshake messages are exchanged from the Server.
* @param[in]  eDHCPPhase    Different phases of DHCP phases.
*             ulIPAddress   The offered IP address.
* @retval     Returns DHCP answers.
***********************************************************************************************************************/
#if (ipconfigUSE_DHCP != 0)
eDHCPCallbackAnswer_t xApplicationDHCPHook(eDHCPCallbackPhase_t eDHCPPhase, uint32_t ulIPAddress)
{
    eDHCPCallbackAnswer_t eReturn = eDHCPContinue;
    /* This hook is called in a couple of places during the DHCP process, as identified by the eDHCPPhase parameter */
    switch (eDHCPPhase)
    {
        case eDHCPPhasePreDiscover:
            /* A DHCP discovery is about to be sent out. eDHCPContinue is returned to allow the discovery to go out
             * If eDHCPUseDefaults had been returned instead then the DHCP process would be stopped and the statically
             * configured IP address would be used
             * If eDHCPStopNoChanges had been returned instead then the DHCP process would be stopped and whatever the
             * current network configuration was would continue to be used */
            break;

        case eDHCPPhasePreRequest:
            /* An offer has been received from the DHCP server, and the offered IP address is passed in the ulIPAddress
             * parameter */

            /* The sub-domains don’t match, so continue with the DHCP process so the offered IP address is used */
            /* Update the structure, the DHCP state machine is not updating this */
            xNd.ulIPAddress = ulIPAddress;
            g_dhcp_in_use = 1;
            break;

        default:
            /* Cannot be reached, but set eReturn to prevent compiler warnings
             * where compilers are disposed to generating one */
            break;
    }

  return eReturn;
}
#endif /* ipconfigUSE_DHCP != 0 */

/*******************************************************************************************************************//**
 * @brief      Creates and prints the the IP configuration to display on the console.
 * @param[in]  None.
 * @retval     None.
 **********************************************************************************************************************/
static void print_ipconfig(void)
{
#if (ipconfigUSE_DHCP != 0)
    if (g_dhcp_in_use)
    {
        g_netmask[3] = (uint8_t)((xNd.ulNetMask & 0xFF000000) >> 24);
        g_netmask[2] = (uint8_t)((xNd.ulNetMask & 0x00FF0000) >> 16);
        g_netmask[1] = (uint8_t)((xNd.ulNetMask & 0x0000FF00) >> 8);
        g_netmask[0] = (uint8_t)(xNd.ulNetMask & 0x000000FF);

        g_gateway_address[3] = (uint8_t)((xNd.ulGatewayAddress & 0xFF000000) >> 24);
        g_gateway_address[2] = (uint8_t)((xNd.ulGatewayAddress & 0x00FF0000) >> 16);
        g_gateway_address[1] = (uint8_t)((xNd.ulGatewayAddress & 0x0000FF00) >> 8);
        g_gateway_address[0] = (uint8_t)(xNd.ulGatewayAddress & 0x000000FF);

        g_dns_server_address[3] = (uint8_t)((xNd.ulDNSServerAddresses[0] & 0xFF000000) >> 24);
        g_dns_server_address[2] = (uint8_t)((xNd.ulDNSServerAddresses[0] & 0x00FF0000) >> 16);
        g_dns_server_address[1] = (uint8_t)((xNd.ulDNSServerAddresses[0] & 0x0000FF00) >> 8);
        g_dns_server_address[0] = (uint8_t)(xNd.ulDNSServerAddresses[0] & 0x000000FF);

        g_ip_address[3] = (uint8_t)((xNd.ulIPAddress & 0xFF000000) >> 24);
        g_ip_address[2] = (uint8_t)((xNd.ulIPAddress & 0x00FF0000) >> 16);
        g_ip_address[1] = (uint8_t)((xNd.ulIPAddress & 0x0000FF00) >> 8);
        g_ip_address[0] = (uint8_t)(xNd.ulIPAddress & 0x000000FF);
    }
#endif /*ipconfigUSE_DHCP != 0 */
    APP_PRINT("\r\nEthernet adapter for Renesas "KIT_NAME":\r\n");
    APP_PRINT("\tDescription . . . . . . . . . . . : Renesas "KIT_NAME" Ethernet\r\n");
    APP_PRINT("\tPhysical Address. . . . . . . . . : %02x-%02x-%02x-%02x-%02x-%02x\r\n",
              g_mac_address[0], g_mac_address[1], g_mac_address[2], g_mac_address[3], g_mac_address[4],
              g_mac_address[5]);
    APP_PRINT("\tDHCP Enabled. . . . . . . . . . . : %s\r\n", g_dhcp_in_use ? "Yes" : "No");
    APP_PRINT("\tIPv4 Address. . . . . . . . . . . : %d.%d.%d.%d\r\n", g_ip_address[0], g_ip_address[1],
              g_ip_address[2], g_ip_address[3]);
    APP_PRINT("\tSubnet Mask . . . . . . . . . . . : %d.%d.%d.%d\r\n", g_netmask[0], g_netmask[1],
              g_netmask[2], g_netmask[3]);
    APP_PRINT("\tDefault Gateway . . . . . . . . . : %d.%d.%d.%d\r\n", g_gateway_address[0], g_gateway_address[1],
              g_gateway_address[2], g_gateway_address[3]);
    APP_PRINT("\tDNS Servers . . . . . . . . . . . : %d.%d.%d.%d\r\n", g_dns_server_address[0], g_dns_server_address[1],
              g_dns_server_address[2], g_dns_server_address[3]);
}

/*******************************************************************************************************************//**
 * @brief      DNS query for the requested domain name. Uses the FreeRTOS client API FreeRTOS_gethostbyname
 *             to get the IP address for the domain name.
 * @param[in]  domain       Domain name.
 * @retval     None.
 **********************************************************************************************************************/
static void dnsQuerryFunc(char *domain)
{
    uint32_t ulIPAddress        = RESET_VALUE;
    int8_t cBuffer[DOMAIN_SIZE] = {RESET_VALUE};

    /* Lookup the IP address of the FreeRTOS.org website */
    ulIPAddress = FreeRTOS_gethostbyname((char*) domain);

    if (ulIPAddress != 0)
    {
        /* Convert the IP address to a string */
        FreeRTOS_inet_ntoa(ulIPAddress, (char *) cBuffer);

        /* Print out the IP address obtained from the DNS lookup */
        APP_PRINT("\r\nDNS Lookup for \"%s\" is: %s\r\n", domain, cBuffer);
    }
    else
    {
        APP_PRINT("\r\nDNS Lookup failed for \"%s\"\r\n", domain);
    }
}

/*******************************************************************************************************************//**
 * @brief      Send a ping request to the specified IP address.
 * @param[in]  destination  Specify the IP address to ping.
 * @retval     None.
 **********************************************************************************************************************/
static void ping_ip_address(char *destination)
{
    uint32_t pingcount        = RESET_VALUE;
    ping_status_t ping_status = PING_ERROR;
    g_ping_data.sent          = RESET_VALUE;
    g_ping_data.received      = RESET_VALUE;
    g_ping_data.lost          = RESET_VALUE;

    APP_PRINT("\r\nPinging to %s:\r\n\r\n", (char *) destination);

    while (pingcount < USR_PING_COUNT)
    {
        /* Send a ICMP Ping request to the requested IP address
         * USR_PING_COUNT (100) is used in this example project
         * For Continuous testing the count can be increased to bigger number */
        ping_status = SendPing((char*) destination);
        if (PING_ERROR != ping_status)
        {
            APP_PRINT("!");
            g_ping_data.sent++;
            if (PING_SUCCESS == ping_status)
            {
                g_ping_data.received++;
            }
            else if (PING_TIMEOUT == ping_status)
            {
                g_ping_data.lost++;
            }
        }
        else
        {
            g_ping_data.lost++;
            APP_PRINT(".");
        }
        pingcount++;
        /* Add some delay between Pings */
        vTaskDelay(DELAY_100_TICKS);
    }

    /* Print the Ping response on to the console */
    APP_PRINT("\r\n \r\nPing Statistics for %s :\r\n", (char *)destination);
    APP_PRINT("\r\nPackets: Sent  = %d, Received = %d, Lost = %d \r\n", g_ping_data.sent,
              g_ping_data.received, g_ping_data.lost);

}

/*******************************************************************************************************************//**
 * @brief      This function checks the network status (both Ethernet and IP layer). If the network is down
 *             the application will not send any data on the network.
 * @param[in]  None.
 * @retval     Network status.
 **********************************************************************************************************************/
static uint32_t isNetworkUp(void)
{
    fsp_err_t eth_link_status = FSP_ERR_NOT_OPEN;      /* Default status for Ethernet link */
    BaseType_t networkUp      = pdFALSE;               /* Default status for IP link */
    uint32_t network_status   = RESET_VALUE;           /* Combined status of both links, initially no flags set */

#if (ipconfigUSE_DHCP != 0)
    if (!g_dhcp_in_use)
    {
        return IP_LINK_DOWN;
    }
#endif /* ipconfigUSE_DHCP != 0 */

    /* Check Ethernet link status */
#if (BSP_PERIPHERAL_ESWM_PRESENT)
    eth_link_status = R_RMAC_LinkProcess(g_ether0.p_ctrl);
#else
    eth_link_status = R_ETHER_LinkProcess(g_ether0.p_ctrl);
#endif /* BSP_PERIPHERAL_ESWM_PRESENT */
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

#if (ipconfigUSE_DHCP != 0)
/*******************************************************************************************************************//**
 * @brief      Update the DHCP info to the user data structure.
 * @param[in]  None.
 * @retval     None.
 **********************************************************************************************************************/
void updateDhcpResponseToUsr(void)
{
    if (g_dhcp_in_use)
    {
        xNd.ulNetMask = FreeRTOS_GetNetmask();
        xNd.ulGatewayAddress = FreeRTOS_GetGatewayAddress();
        xNd.ulDNSServerAddresses[0] = FreeRTOS_GetDNSServerAddress();
    }
}
#endif /* ipconfigUSE_DHCP != 0 */

#if (ipconfigDHCP_REGISTER_HOSTNAME == 1)
/*******************************************************************************************************************//**
 * @brief      DHCP Hook function to populate the user defined host name for the Kit.
 * @param[in]  None.
 * @retval     Host name.
 **********************************************************************************************************************/
const char *pcApplicationHostnameHook(void)
{
    return KIT_NAME;
}
#endif /* ipconfigDHCP_REGISTER_HOSTNAME == 1 */
