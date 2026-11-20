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
#endif /* ipconfigUSE_DHCP */

/* Private function */
static uint32_t ulRand(void);

/*******************************************************************************************************************//**
 * @brief     This function initializes the FreeRTOS+TCP network stack using the specified network configuration.
 * @param[in] p_ip_address          Pointer to the IPv4 address.
 * @param[in] p_netmask             Pointer to the subnet mask.
 * @param[in] p_gateway_address     Pointer to the gateway address.
 * @param[in] p_dns_server_address  Pointer to the DNS server address.
 * @param[in] p_mac_address         Pointer to the MAC address.
 * @retval    pdPASS                Initialized successfully.
 * @retval    pdFAIL                Failed to initialize.
 **********************************************************************************************************************/
BaseType_t ethernet_init(uint8_t const * p_ip_address, uint8_t const * p_netmask, uint8_t const * p_gateway_address,
                        uint8_t const * p_dns_server_address, uint8_t const * p_mac_address)
{
    return FreeRTOS_IPInit(p_ip_address, p_netmask, p_gateway_address, p_dns_server_address, p_mac_address);
}

/*******************************************************************************************************************//**
 * @brief     This function sends an ICMP echo request to the specified IPv4 address.
 * @param[in] p_ip_address          Pointer to the destination IPv4 address string.
 * @retval    A positive value      Sent successfully.
 * @retval    pdFAIL                Failed to send.
 **********************************************************************************************************************/
BaseType_t send_ping_request(const char * p_ip_address)
{
    uint32_t ulIPAddress = RESET_VALUE;

    /* The p_ip_address parameter holds the destination IP address as a string in
     * dotted-decimal notation (for example: 192.168.0.200). Convert the string into
     * the required 32-bit format */
    ulIPAddress = FreeRTOS_inet_addr(p_ip_address);

    /* Send a ping request containing 8 data bytes. Wait (in the Blocked state) a
     * maximum of 100ms for a network buffer into which the generated ping request
     * can be written and sent */
    return FreeRTOS_SendPingRequest(ulIPAddress, 8, (100 / portTICK_PERIOD_MS));
}

/*******************************************************************************************************************//**
 * @brief      This function checks the network status.
 * @param[in]  None.
 * @retval     IP_LINK_UP           Connected successfully.
 * @retval     ETHERNET_LINK_UP     Connected successfully.
 * @retval     ETHERNET_LINK_DOWN   Failed to connect.
 * @retval     IP_LINK_DOWN         Failed to connect.
 **********************************************************************************************************************/
uint32_t is_network_up(void)
{
    fsp_err_t eth_link_status = FSP_ERR_NOT_OPEN;
    BaseType_t networkUp = pdFALSE;
    uint32_t network_status = (IP_LINK_UP | ETHERNET_LINK_UP);

#if (ipconfigUSE_DHCP != 0)
    if (!g_dhcp_in_use)
    {
        return IP_LINK_DOWN;
    }
#endif /* ipconfigUSE_DHCP */

    networkUp = FreeRTOS_IsNetworkUp();
#if (BSP_PERIPHERAL_ESWM_PRESENT)
    eth_link_status = R_RMAC_LinkProcess(g_ether0.p_ctrl);
#else
    eth_link_status = R_ETHER_LinkProcess(g_ether0.p_ctrl);
#endif /* BSP_PERIPHERAL_ESWM_PRESENT */

    if ((FSP_SUCCESS == eth_link_status) && (pdTRUE == networkUp))
    {
        return network_status;
    }
    else
    {
        if (FSP_SUCCESS != eth_link_status)
        {
            network_status |= ETHERNET_LINK_DOWN;
        }
        else if (FSP_SUCCESS == eth_link_status)
        {
            network_status |= ETHERNET_LINK_UP;
        }
        else
        {
            /* Do nothing */
        }

        if (pdTRUE != networkUp)
        {
            network_status |= IP_LINK_DOWN;
        }
        else if (pdTRUE == networkUp)
        {
            network_status |= IP_LINK_UP;
        }
        else
        {
            /* Do nothing */
        }
        return network_status;
    }
}

/*******************************************************************************************************************//**
 * @brief     This function performs a DNS lookup for the specified host name.
 * @param[in] pcHostName                Host name to resolve.
 * @retval    Resolved IPv4 address.
 **********************************************************************************************************************/
uint32_t get_host_by_name(const char * pcHostName)
{
    return FreeRTOS_gethostbyname(pcHostName);
}

/*******************************************************************************************************************//**
 * @brief      This function converts an IPv4 address to dotted-decimal string format.
 * @param[in]  ulIPAddress              IPv4 address.
 * @param[out] pcBuffer                 Output buffer.
 * @retval     None.
 **********************************************************************************************************************/
void inet_ntoa(uint32_t ulIPAddress, char * pcBuffer)
{
    FreeRTOS_inet_ntoa(ulIPAddress, pcBuffer);
}

/*******************************************************************************************************************//**
 * @brief     This function generates an initial TCP sequence number.
 * @param[in] ulSourceAddress           Source IPv4 address.
 * @param[in] usSourcePort              Source port number.
 * @param[in] ulDestinationAddress      Destination IPv4 address.
 * @param[in] usDestinationPort         Destination port number.
 * @retval    ulResult                  TCP sequence number.
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
 * @brief     This function generates a 32-bit pseudo-random number.
 * @param[in] None.
 * @retval    ulResult                  Random number.
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
 * @brief     This function processes an ICMP echo reply received by the TCP/IP stack.
 * @param[in] eStatus                   Ping reply status.
 * @param[in] usIdentifier              Ping request identifier.
 * @retval    None.
 **********************************************************************************************************************/
void vApplicationPingReplyHook(ePingReplyStatus_t eStatus, uint16_t usIdentifier)
{
    (void) usIdentifier;

    switch (eStatus)
    {
        /* A valid ping reply has been received */
        case eSuccess:
        {
            g_ping_data.received++;
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
}

#if (ipconfigUSE_DHCP != 0)
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

            /* A DHCP discovery is about to be sent out. eDHCPContinue is returned to allow the discovery to go out
             * If eDHCPUseDefaults had been returned instead then the DHCP process would be stopped and the statically
             * configured IP address would be used
             * If eDHCPStopNoChanges had been returned instead then the DHCP process would be stopped and whatever the
             * current network configuration was would continue to be used */
            break;

        case eDHCPPhasePreRequest:

            /* An offer has been received from the DHCP server, and the offered IP address is passed in the ulIPAddress
             * parameter */

            /* The sub-domains do not match, so continue with the DHCP process so the offered IP address is used */
            /* Update the structure, the DHCP state machine is not updating this */
            g_xnd.ulIPAddress = ulIPAddress;
            g_dhcp_in_use = true;
            break;

        default:

            /* Cannot be reached, but set eReturn to prevent compiler warnings
             * where compilers are disposed to generating one */
            break;
  }

    return eReturn;
}
#endif /* ipconfigUSE_DHCP */

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
#endif /* ipconfigDHCP_REGISTER_HOSTNAME */
