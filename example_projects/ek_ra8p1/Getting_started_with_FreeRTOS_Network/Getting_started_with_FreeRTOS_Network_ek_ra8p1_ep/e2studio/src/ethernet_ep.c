/**********************************************************************************************************************
 * File Name    : ethernet_ep.c
 * Description  : Contains macros, data structures and functions used in ethernet_ep.c.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *********************************************************************************************************************/

#include "ethernet_ep.h"

/* Global variables */
bool g_dhcp_in_use = false;
ping_data_t g_ping_data = {RESET_VALUE, RESET_VALUE, RESET_VALUE};

#if (ipconfigUSE_DHCP != 0)
    IPV4Parameters_t g_xnd = {RESET_VALUE, RESET_VALUE, RESET_VALUE,
                            {RESET_VALUE, RESET_VALUE}, RESET_VALUE, RESET_VALUE};

   /* DHCP populates these IP address, Sub net mask and Gateway Address. So start with this is zeroed out values
    * The MAC address is Test MAC address */
    uint8_t g_mac_address[6]         = {0x74, 0x90, 0x50, 0x65, 0x81, 0x70};
    uint8_t g_ip_address[4]          = {RESET_VALUE};
    uint8_t g_netmask[4]             = {255, 255, 255, 128};
    uint8_t g_gateway_address[4]     = {132, 158, 124, 1};
    uint8_t g_dns_server_address[4]  = {RESET_VALUE};
#else
    /* Static IP configuration, when DHCP mode is not used for the example project
     * This needs to be populated by the user according to the Network Settings of your LAN
     * This sample address taken from the LAN where it is tested. This is different for different LAN
     * get the Address using the PC IPconfig details */
    uint8_t g_mac_address[6]         = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
    uint8_t g_ip_address[4]          = {192, 168, 0, 52};
    uint8_t g_netmask[4]             = {255, 255, 255, 0};
    uint8_t g_gateway_address[4]     = {192, 168, 0, 3};
    uint8_t g_dns_server_address[4]  = {10, 60, 1, 2};
#endif /* ipconfigUSE_DHCP != 0 */

/* Private function */
static uint32_t ulRand(void);

/*******************************************************************************************************************//**
 * @brief     This function initializes the FreeRTOS+TCP.
 * @param[in] None.
 * @retval    pdPASS                Initialized successfully.
 * @retval    pdFAIL                Failed to initialize.
 **********************************************************************************************************************/
BaseType_t ethernet_init(void)
{
    return FreeRTOS_IPInit(g_ip_address, g_netmask, g_gateway_address, g_dns_server_address, g_mac_address);
}

/*******************************************************************************************************************//**
 * @brief     This function checks the network status.
 * @param[in] None.
 * @retval    Network status flags indicating the Ethernet and IP link states.
 **********************************************************************************************************************/
uint32_t is_network_up(void)
{
    fsp_err_t ethernet_link_status  = FSP_ERR_NOT_OPEN;
    BaseType_t networkUp = pdFALSE;
    uint32_t network_status = RESET_VALUE;

#if (ipconfigUSE_DHCP != 0)
    if (!g_dhcp_in_use)
    {
        return IP_LINK_DOWN;
    }
#endif /* ipconfigUSE_DHCP != 0 */

#if (BSP_PERIPHERAL_ESWM_PRESENT)
    ethernet_link_status = R_RMAC_LinkProcess(g_ether0.p_ctrl);
#else
    ethernet_link_status = R_ETHER_LinkProcess(g_ether0.p_ctrl);
#endif /* BSP_PERIPHERAL_ESWM_PRESENT */
    network_status |= ((FSP_SUCCESS == ethernet_link_status) ? ETHERNET_LINK_UP : ETHERNET_LINK_DOWN);

    networkUp = FreeRTOS_IsNetworkUp();
    network_status |= ((pdTRUE == networkUp) ? IP_LINK_UP : IP_LINK_DOWN);

    return network_status;
}

/*******************************************************************************************************************//**
 * @brief     This function sends ICMP Ping request based on the user input IP address.
 * @param[in] p_ip_address               IP address to ping.
 * @retval    PING_SUCCESS               Sent successfully.
 * @retval    Any other error codes      Failed to send.
 **********************************************************************************************************************/
ping_status_t send_ping_request(const char * p_ip_address)
{
    uint32_t ulIPAddress;
    uint16_t usReplySequenceNumber = 0;
    BaseType_t usRequestSequenceNumber = pdFAIL;
    ping_status_t result = PING_ERROR;

    /* The p_ip_address parameter holds the destination IP address as a string in
     * decimal dot notation (for example, "192.168.0.200"). Convert the string into
     * the required 32-bit format */
    ulIPAddress = FreeRTOS_inet_addr(p_ip_address);

    /* Send a ping request to the specified IP address */
    usRequestSequenceNumber = FreeRTOS_SendPingRequest(ulIPAddress, PING_DATA_SIZE, PING_TIMEOUT_MS);
    if (pdFAIL == usRequestSequenceNumber)
    {
        result = PING_ERROR;
    }
    else
    {
        /* Wait for a reply or timeout */
        if (pdPASS == xQueueReceive(xPingReplyQueue, &usReplySequenceNumber,
                                    (PING_QUEUE_WAIT_TIME_MS / portTICK_PERIOD_MS)))
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
 * @brief     This function performs a DNS lookup for the specified host name.
 * @param[in] pcHostName                Pointer to the host name to resolve.
 * @retval    Resolved IPv4 address.
 **********************************************************************************************************************/
uint32_t get_host_by_name(const char * pcHostName)
{
    return FreeRTOS_gethostbyname(pcHostName);
}

/*******************************************************************************************************************//**
 * @brief      This function converts an IPv4 address to dotted-decimal string format.
 * @param[in]  ulIPAddress              IPv4 address.
 * @param[out] pcBuffer                 Pointer to the output buffer.
 * @retval     None.
 **********************************************************************************************************************/
void inet_ntoa(uint32_t ulIPAddress, char * pcBuffer)
{
    FreeRTOS_inet_ntoa(ulIPAddress, pcBuffer);
}

/*******************************************************************************************************************//**
 * @brief     This function generates a 32-bit pseudo-random number.
 * @param[in] None.
 * @retval    32-bit pseudo-random number.
 **********************************************************************************************************************/
static uint32_t ulRand(void)
{
    /* Example of a 32-bit random number generator.
     * Here rand() returns a 15-bit number. So create 32-bit random number using 15 bit rand() */
    uint32_t ulResult = ((((uint32_t) rand()) & 0x7fffuL)) | ((((uint32_t) rand()) & 0x7fffuL) << 15) |
                        ((((uint32_t) rand()) & 0x0003uL) << 30);

    return ulResult;
}

/*******************************************************************************************************************//**
 * @brief     This function generates an initial TCP sequence number.
 * @param[in] ulSourceAddress           Source IPv4 address.
 * @param[in] usSourcePort              Source port number.
 * @param[in] ulDestinationAddress      Destination IPv4 address.
 * @param[in] usDestinationPort         Destination port number.
 * @retval    Initial TCP sequence number.
***********************************************************************************************************************/
uint32_t ulApplicationGetNextSequenceNumber(uint32_t ulSourceAddress, uint16_t usSourcePort,
                                            uint32_t ulDestinationAddress, uint16_t usDestinationPort)
{
    /* Here we need to get random number for the sequence number.
     * This is just for testing purpose, so software rand() is okay.
     * This can also be tied to the TRNG. */
    uint32_t ulResult = ((ulSourceAddress + ulDestinationAddress + usSourcePort + usDestinationPort) & ulRand());

    return ulResult;
}

/*******************************************************************************************************************//**
 * @brief     This function processes an ICMP echo reply received by the TCP/IP stack.
 * @param[in] eStatus                   Ping reply status.
 * @param[in] usIdentifier              Ping request identifier.
 * @retval    None.
 **********************************************************************************************************************/
void vApplicationPingReplyHook(ePingReplyStatus_t eStatus, uint16_t usIdentifier)
{
    (void) usIdentifier;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    switch (eStatus)
    {
        /* A valid ping reply has been received */
        case eSuccess:
        {
            xQueueSendFromISR(xPingReplyQueue, &usIdentifier, &xHigherPriorityTaskWoken);
            break;
        }

        /* A reply was received but it was not valid */
        case eInvalidData:
        default:
        {
            g_ping_data.lost++;
            break;
        }
    }
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

#if (ipconfigUSE_DHCP != 0)
/*******************************************************************************************************************//**
 * @brief     This function handles DHCP callback events during the DHCP negotiation process.
 * @param[in] eDHCPPhase                DHCP negotiation phase.
 * @param[in] ulIPAddress               Offered IPv4 address from the DHCP server.
 * @retval    eReturn                   DHCP answers.
 **********************************************************************************************************************/
eDHCPCallbackAnswer_t xApplicationDHCPHook(eDHCPCallbackPhase_t eDHCPPhase, uint32_t ulIPAddress)
{
    eDHCPCallbackAnswer_t eReturn = eDHCPContinue;

    /* This hook is called in a couple of places during the DHCP process, as identified by the eDHCPPhase parameter */
    switch (eDHCPPhase)
    {
        case eDHCPPhasePreDiscover:
        {
            /* A DHCP discovery is about to be sent out. eDHCPContinue is returned to allow the discovery to go out
             * If eDHCPUseDefaults had been returned instead then the DHCP process would be stopped and the statically
             * configured IP address would be used
             * If eDHCPStopNoChanges had been returned instead then the DHCP process would be stopped and whatever the
             * current network configuration was would continue to be used */
            break;
        }

        case eDHCPPhasePreRequest:
        {
            /* An offer has been received from the DHCP server, and the offered IP address is passed in the ulIPAddress
             * parameter */

            /* The sub-domains do not match, so continue with the DHCP process so the offered IP address is used */
            /* Update the structure, the DHCP state machine is not updating this */
            g_xnd.ulIPAddress = ulIPAddress;
            g_dhcp_in_use = true;
            break;
        }

        default:
        {
            /* Cannot be reached, but set eReturn to prevent compiler warnings
             * where compilers are disposed to generating one */
            break;
        }
    }

    return eReturn;
}

/*******************************************************************************************************************//**
 * @brief      This function updates the DHCP info to the user data structure.
 * @param[in]  None.
 * @retval     None.
 **********************************************************************************************************************/
void update_dhcp_info(void)
{
    if (g_dhcp_in_use)
    {
        g_xnd.ulNetMask = FreeRTOS_GetNetmask();
        g_xnd.ulGatewayAddress = FreeRTOS_GetGatewayAddress();
        g_xnd.ulDNSServerAddresses[0] = FreeRTOS_GetDNSServerAddress();
    }
}
#endif /* ipconfigUSE_DHCP != 0 */

#if (ipconfigDHCP_REGISTER_HOSTNAME == 1)
/*******************************************************************************************************************//**
 * @brief     This function returns the host name used for the kit.
 * @param[in] None.
 * @retval    KIT_NAME                  Host name for the kit.
 **********************************************************************************************************************/
const char * pcApplicationHostnameHook(void)
{
    return KIT_NAME;
}
#endif /* ipconfigDHCP_REGISTER_HOSTNAME == 1 */
