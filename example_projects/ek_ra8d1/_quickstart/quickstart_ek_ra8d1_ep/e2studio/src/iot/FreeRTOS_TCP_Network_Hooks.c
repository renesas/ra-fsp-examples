/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/**********************************************************************************************************************
 * File Name    : FreeRTOS_TCP_Network_Hooks.c
 * Version      : .
 * Description  : .
 *********************************************************************************************************************/

#include "network_thread.h"

#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_DHCP.h"

#include "common_init.h"

#define BUFF_SIZE    (16)

extern TaskHandle_t network_thread;

extern volatile uint32_t             dhcp_in_use;
extern IPV4Parameters_t xNd;

void updateDhcpResponseToUsr(void);

#if (ipconfigDHCP_REGISTER_HOSTNAME == true)

/**********************************************************************************************************************
 * Function Name: pcApplicationHostnameHook
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
const char * pcApplicationHostnameHook (void)
{

    // TODO Add ~unique UUID portion (short)~ example:
    return KIT_NAME;
}

/**********************************************************************************************************************
 * End of function pcApplicationHostnameHook
 *********************************************************************************************************************/
#endif

#if (ipconfigUSE_NETWORK_EVENT_HOOK == 1)
 #if (ipconfigIPv4_BACKWARD_COMPATIBLE == 1)
void vApplicationIPNetworkEventHook (eIPCallbackEvent_t eNetworkEvent)
 #else

/**********************************************************************************************************************
 * Function Name: vApplicationIPNetworkEventHook_Multi
 * Description  : .
 * Arguments    : eNetworkEvent
 *              : endif
 * Return Value : .
 *********************************************************************************************************************/
void vApplicationIPNetworkEventHook_Multi (eIPCallbackEvent_t eNetworkEvent, struct xNetworkEndPoint * pxEndPoint)
 #endif
{
    if (eNetworkUp == eNetworkEvent)
    {
        int8_t lcBuffer[BUFF_SIZE];

        /* Signal application the network is UP */
        xTaskNotifyFromISR(network_thread, eNetworkUp, eSetBits, NULL);

        /* The network is up and configured.  Print out the configuration
         * obtained from the DHCP server. */
 #if (ipconfigIPv4_BACKWARD_COMPATIBLE == 1)
        uint32_t lulIPAddress, lulNetMask, lulGatewayAddress, lulDNSServerAddress;

        FreeRTOS_GetAddressConfiguration(&lulIPAddress, &lulNetMask, &lulGatewayAddress, &lulDNSServerAddress);

        /* Convert the IP address to a string then print it out. */
        FreeRTOS_inet_ntoa(lulIPAddress, (char *) lcBuffer);

        /* Convert the net mask to a string then print it out. */
        FreeRTOS_inet_ntoa(lulNetMask, (char *) lcBuffer);

        /* Convert the IP address of the gateway to a string then print it out. */
        FreeRTOS_inet_ntoa(lulGatewayAddress, (char *) lcBuffer);

        /* Convert the IP address of the DNS server to a string then print it out. */
        FreeRTOS_inet_ntoa(lulDNSServerAddress, (char *) lcBuffer);
 #else
        FreeRTOS_inet_ntoa(pxEndPoint->ipv4_settings.ulIPAddress, (char *) lcBuffer);

        /* Convert the net mask to a string then print it out. */
        FreeRTOS_inet_ntoa(pxEndPoint->ipv4_settings.ulNetMask, (char *) lcBuffer);

        /* Convert the IP address of the gateway to a string then print it out. */
        FreeRTOS_inet_ntoa(pxEndPoint->ipv4_settings.ulGatewayAddress, (char *) lcBuffer);

        /* Convert the IP address of the DNS server to a string then print it out. */
        FreeRTOS_inet_ntoa(pxEndPoint->ipv4_settings.ulDNSServerAddresses[0], (char *) lcBuffer);
 #endif
    }
}

/**********************************************************************************************************************
 * End of function vApplicationIPNetworkEventHook_Multi
 *********************************************************************************************************************/
#endif

#if (ipconfigUSE_DHCP_HOOK == 1)
 #if (ipconfigIPv4_BACKWARD_COMPATIBLE == 1)
eDHCPCallbackAnswer_t xApplicationDHCPHook (eDHCPCallbackPhase_t eDHCPPhase, uint32_t lulIPAddress)
 #else

/**********************************************************************************************************************
 * Function Name: xApplicationDHCPHook_Multi
 * Description  : .
 * Arguments    : eDHCPPhase
 *              : pxEndPoint
 *              : endif
 * Return Value : .
 *********************************************************************************************************************/
eDHCPCallbackAnswer_t xApplicationDHCPHook_Multi (eDHCPCallbackPhase_t      eDHCPPhase,
                                                  struct xNetworkEndPoint * pxEndPoint,
                                                  IP_Address_t            * pxIPAddress)
 #endif
{
    eDHCPCallbackAnswer_t eReturn = eDHCPContinue;
 #if (ipconfigIPv4_BACKWARD_COMPATIBLE == 0)
    FSP_PARAMETER_NOT_USED(pxEndPoint);
 #endif

    /*
     * This hook is called in a couple of places during the DHCP process, as identified by the eDHCPPhase parameter.
     */
    switch (eDHCPPhase)
    {
        case eDHCPPhasePreDiscover:
        {
            /*
             *  A DHCP discovery is about to be sent out.  eDHCPContinue is returned to allow the discovery to go out.
             *  If eDHCPUseDefaults had been returned instead then the DHCP process would be stopped and the statically
             *  configured IP address would be used.
             *  If eDHCPStopNoChanges had been returned instead then the DHCP process would be stopped and whatever the
             *  current network configuration was would continue to be used.
             */
            break;
        }

        case eDHCPPhasePreRequest:
        {
            /* An offer has been received from the DHCP server, and the offered IP address is passed in the lulIPAddress
             * parameter.
             */

            /*
             * The sub-domains don't match, so continue with the DHCP process so the offered IP address is used.
             */

            /* Update the Structure, the DHCP state Machine is not updating this */
 #if (ipconfigIPv4_BACKWARD_COMPATIBLE == 1)
        	xNd.ulIPAddress = lulIPAddress;
 #else
        	xNd.ulIPAddress = pxIPAddress->ulIP_IPv4;
 #endif
            dhcp_in_use = 1;
            updateDhcpResponseToUsr();
            break;
        }

        default:
        {
            /*
             * Cannot be reached, but set eReturn to prevent compiler warnings where compilers are disposed to generating one.
             */
            break;
        }
    }

    return eReturn;
}

/**********************************************************************************************************************
 * End of function xApplicationDHCPHook_Multi
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: updateDhcpResponseToUsr
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void updateDhcpResponseToUsr (void)
 {
	if (dhcp_in_use)
	{
#if ( ipconfigIPv4_BACKWARD_COMPATIBLE == 1 ) && ( ipconfigUSE_IPv4 != 0 )

        xNd.ulNetMask = FreeRTOS_GetNetmask();
        xNd.ulGatewayAddress = FreeRTOS_GetGatewayAddress();
        xNd.ulDNSServerAddresses[0] = FreeRTOS_GetDNSServerAddress();

#endif
	}
}

/**********************************************************************************************************************
 * End of function updateDhcpResponseToUsr
 *********************************************************************************************************************/
#endif
