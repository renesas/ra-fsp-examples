/***********************************************************************************************************************
 * File Name    : user_app_thread_entry.c
 * Description  : This file contains the User Application code for the AWS HTTPS Client
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
 * Copyright (C) 2022 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_Sockets.h"
#include "iot_init.h"
#include "iot_system_init.h"
#include "board_cfg.h"
#include "adc_app.h"
#include "littlefs_app.h"
#include "https_agent.h"
#include "common_utils.h"
#include "user_app_thread.h"
#include "user_app.h"

/*******************************************************************************************************************//**
 * @addtogroup aws_https_client_ep
 * @{
 **********************************************************************************************************************/

/******************************************************************************
 Macro definitions
 ******************************************************************************/

/******************************************************************************
 Exported global functions (to be accessed by other files)
 ******************************************************************************/
extern NetworkAddressingParameters_t xNetworkAddressing;
extern TaskHandle_t user_app_thread;

/******************************************************************************
 Exported global variables
 ******************************************************************************/
ping_data_t ping_data = { RESET_VALUE, RESET_VALUE, RESET_VALUE };
NetworkAddressingParameters_t xNd = { RESET_VALUE };

uint32_t dhcp_in_use = RESET_VALUE;
/* To store die temperature into this variable */
float mcu_die_temp  = RESET_VALUE;

/* Domain for the DNS Host lookup is used in this Example Project.
 * The project can be built with different *domain_name to validate the DNS client
 */
char *domain_name = IOT_DEMO_HOST_ADDRESS;

/* IP address of the PC or any Device on the LAN/WAN where the Ping request is sent.
 * Note: Users needs to change this according to the LAN settings of your Test PC or device
 * when running this project.
 */
char remote_ip_address[] = IOT_DEMO_TEST_PING_IP;

/* DHCP populates these IP address, Sub net mask and Gateway Address. So start with this is zeroed out values
 * The MAC address is Test MAC address.
 */
uint8_t ucMACAddress[6] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55 };
uint8_t ucIPAddress[4] = { RESET_VALUE };
uint8_t ucNetMask[4] = { RESET_VALUE };
uint8_t ucGatewayAddress[4] = { RESET_VALUE };
uint8_t ucDNSServerAddress[4] = { RESET_VALUE };


/******************************************************************************
 Private global variables and functions
 ******************************************************************************/



/*******************************************************************************************************************//**
* @brief      This is the User Thread for the EP.
* @param[in]  Thread specific parameters
* @retval     None
**********************************************************************************************************************/
void user_app_thread_entry(void *pvParameters)
{
    BaseType_t status = pdFALSE;
    fsp_pack_version_t version  = { RESET_VALUE };
    Userinput_t user_input = RESET_VALUE;
    unsigned char rByte[BUFFER_SIZE_DOWN] =  { RESET_VALUE };
    fsp_err_t err = FSP_SUCCESS;
    int ierr = RESET_VALUE;
    BaseType_t bt_status = pdFALSE;
    uint32_t ip_status = RESET_VALUE;
    uint32_t usrPingCount = RESET_VALUE;
    /* HTTPS Client library return status. */
    IotHttpsReturnCode_t httpsClientStatus = IOT_HTTPS_OK;

    FSP_PARAMETER_NOT_USED(pvParameters);

    /* Version get API for FLEX Pack version */
    R_FSP_VersionGet (&version);

    /* Print banner */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.major, version.minor, version.patch);
    /* Print Project info */
    APP_PRINT(EP_INFO);

    /* Initialize Renesas littlefs port */
    err = hal_littlefs_init ();
    /* Error Handler */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("** Failed in hsl_littlefs_init() function ** \r\n");
        APP_ERR_TRAP(err);
    }

    /* Initialize ADC in continuous mode */
    err = hal_adc_init ();
    /* Error Handler */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("** Failed in hal_adc_init() function** \r\n");
        hal_littlefs_deinit ();
        APP_ERR_TRAP(err);
    }

    /* Initialize IOT FreeRTOS Libraries */
    bt_status = SYSTEM_Init ();
    if (pdPASS != bt_status)
    {
        APP_ERR_PRINT("** Socket Init failed **\r\n");
        hal_littlefs_deinit ();
        hal_adc_deinit ();
        APP_ERR_TRAP(err);
    }

    /* Initialize the crypto hardware acceleration. */
    /* Initialize mbedtls3. */
    ierr = mbedtls_platform_setup (NULL);
    /* Error Handler */
    if (FSP_SUCCESS != ierr)
    {
        APP_ERR_PRINT("** Failed in mbedtls_platform_setup() function ** \r\n");
        hal_littlefs_deinit ();
        hal_adc_deinit ();
        APP_ERR_TRAP(err);
    }
    else
    {
        APP_PRINT("\r\n mbedtls_platform setup successful");
    }

    /* Print the pre init Ethernet IP configuration */
    APP_PRINT(ETH_PREINIT);
    print_ipconfig ();

    /* FreeRTOS IP initializes the IP stack  */
    status = FreeRTOS_IPInit (ucIPAddress, ucNetMask, ucGatewayAddress, ucDNSServerAddress, ucMACAddress);
    /* Error Handler */
    if (pdFALSE == status)
    {
        APP_ERR_PRINT("FreeRTOS_IPInit Failed");
        hal_littlefs_deinit ();
        hal_adc_deinit ();
        mbedtls_platform_teardown (NULL);
        APP_ERR_TRAP(status);
    }

    APP_PRINT("Waiting for network link up...");
    bt_status = xTaskNotifyWait(pdFALSE, pdFALSE, &ip_status, portMAX_DELAY);
    if (pdTRUE != bt_status)
    {
        APP_ERR_PRINT("xTaskNotifyWait Failed");
        hal_littlefs_deinit ();
        hal_adc_deinit ();
        mbedtls_platform_teardown (NULL);
        APP_ERR_TRAP(bt_status);
    }

    APP_PRINT(ETH_POSTINIT);
    print_ipconfig ();
    /*DNS lookup for the Domain name requested. This is Synchronous Activity */
    dnsQuerryFunc (domain_name);

    APP_PRINT("\r\nPinging %s:\r\n\r\n", (char* )remote_ip_address);

    while (usrPingCount < USR_PING_COUNT)
    {
        /* Send a ICMP Ping request to the requested IP address
         * USR_PING_COUNT (100) is used in this Example Project
         * For Continuous testing the count can be increased to bigger number
         */
        status = vSendPing ((char*) remote_ip_address);
        if (pdFAIL != status)
        {
            ping_data.sent++;
        }
        else
        {
            ping_data.lost++;
        }
        usrPingCount++;
        /* Add some delay between pings */
        vTaskDelay (PING_DELAY);
    }
    print_pingResult ();

    /* Perform device provisioning using the specified TLS client credentials */
    status = provision_alt_key ();
    /* Error Handler */
    if (pdPASS != status)
    {
        APP_ERR_PRINT("\r\n Failed in network_init() function ");
        hal_littlefs_deinit ();
        hal_adc_deinit ();
        mbedtls_platform_teardown (NULL);
        APP_ERR_TRAP(status);
    }

    /* Initialize HTTPS client with presigned URL */
    httpsClientStatus = initialize_https_client (IOT_DEMO_HTTPS_PRESIGNED_GET_URL);
    /* Handle_error */
    if (IOT_HTTPS_OK != httpsClientStatus)
    {
        APP_ERR_PRINT("\r\nFailed in HTTPS client Initialization function");
        hal_littlefs_deinit ();
        hal_adc_deinit ();
        mbedtls_platform_teardown (NULL);
        APP_ERR_TRAP(httpsClientStatus);
    }

    /* Create connection establishment to the server */
    httpsClientStatus = connect_aws_https_client (&IotNetworkAfr);
    /* Handle_error */
    if (IOT_HTTPS_OK != httpsClientStatus)
    {
        APP_ERR_PRINT("\r\nFailed in server connection establishment");
        hal_littlefs_deinit ();
        hal_adc_deinit ();
        mbedtls_platform_teardown (NULL);
        APP_ERR_TRAP(httpsClientStatus);
    }

    /* Menu for user selection */
    APP_PRINT(PRINT_MENU);

    /* Read the RTT input */
    while (true)
    {
        if (APP_CHECK_DATA)
        {
            APP_READ(rByte);
            user_input = (uint8_t) atoi ((char*) rByte);
            /* Read the internal mcu die temperature to upload the value using PUT/POST request */
            mcu_die_temp = adc_data_read ();

            switch (user_input)
            {
                case GET:
                {
                    APP_PRINT("\r\nProcessing Get Request\r\n");
                    httpsClientStatus = Process_GETRequest ();
                    if (IOT_HTTPS_OK != httpsClientStatus)
                    {
                        APP_ERR_PRINT("** Failed in GET Request ** \r\n");
                        APP_ERR_TRAP(httpsClientStatus);
                    }
                }
                break;

                case PUT:
                {
                    APP_PRINT("\r\nProcessing PUT Request\r\n");
                    httpsClientStatus = Process_PUTRequest (mcu_die_temp);
                    if (IOT_HTTPS_OK != httpsClientStatus)
                    {
                        APP_ERR_PRINT("** Failed in PUT Request ** \r\n");
                        APP_ERR_TRAP(httpsClientStatus);
                    }
                }
                break;
                case POST:
                {
                    APP_PRINT("\r\nProcessing POST Request\r\n");
                    httpsClientStatus = Process_POSTRequest (mcu_die_temp);
                    if (IOT_HTTPS_OK != httpsClientStatus)
                    {
                        APP_ERR_PRINT("** Failed in POST Request ** \r\n");
                        APP_ERR_TRAP(httpsClientStatus);
                    }
                }
                break;
                default:
                break;
            }
            /* Repeat the menu to display for user selection */
            APP_PRINT(PRINT_MENU);
        }
    }
    vTaskDelay (TASK_DELAY);

}

/*******************************************************************************************************************//**
 * @brief      This is the User Hook for the DHCP Response. xApplicationDHCPHook() is called by DHCP Client Code when DHCP
 *             handshake messages are exchanged from the Server.
 * @param[in]  Different Phases of DHCP Phases and the Offered IP Address
 * @retval     Returns DHCP Answers.
 **********************************************************************************************************************/
eDHCPCallbackAnswer_t xApplicationDHCPHook(eDHCPCallbackPhase_t eDHCPPhase, uint32_t lulIPAddress)
{
    eDHCPCallbackAnswer_t eReturn = eDHCPContinue;
    /*
     * This hook is called in a couple of places during the DHCP process, as identified by the eDHCPPhase parameter.
     */
    switch (eDHCPPhase)
    {
        case eDHCPPhasePreDiscover:
            /*
             *  A DHCP discovery is about to be sent out.  eDHCPContinue is returned to allow the discovery to go out.
             *  If eDHCPUseDefaults had been returned instead then the DHCP process would be stopped and the statically
             *  configured IP address would be used.
             *  If eDHCPStopNoChanges had been returned instead then the DHCP process would be stopped and whatever the
             *  current network configuration was would continue to be used.
             */
        break;

        case eDHCPPhasePreRequest:
            /* An offer has been received from the DHCP server, and the offered IP address is passed in the lulIPAddress
             * parameter.
             */

            /*
             * The sub-domains don't match, so continue with the DHCP process so the offered IP address is used.
             */
            /* Update the Structure, the DHCP state Machine is not updating this */
            xNetworkAddressing.ulDefaultIPAddress = lulIPAddress;
            dhcp_in_use = 1;
            updateDhcpResponseToUsr ();
        break;

        default:
            /*
             * Cannot be reached, but set eReturn to prevent compiler warnings where compilers are disposed to generating one.
             */
        break;
    }

    return eReturn;
}

/*******************************************************************************************************************//**
 * @brief      Send ICMP Ping request  based on the user input IP Address.
 * @param[in]  IP address to Ping
 * @retval     Sequence Number
 **********************************************************************************************************************/
BaseType_t vSendPing(const char *pcIPAddress)
{
    uint32_t ulIPAddress = RESET_VALUE;
    /*
     * The pcIPAddress parameter holds the destination IP address as a string in
     * decimal dot notation (for example, “192.168.0.200”). Convert the string into
     * the required 32-bit format.
     */
    ulIPAddress = FreeRTOS_inet_addr (pcIPAddress);

    /*
     * Send a ping request containing 8 data bytes.  Wait (in the Blocked state) a
     * maximum of 100ms for a network buffer into which the generated ping request
     * can be written and sent.
     */
    return (FreeRTOS_SendPingRequest (ulIPAddress, 8, 100 / portTICK_PERIOD_MS));
}

/*******************************************************************************************************************//**
 * @brief      User Hook for the Ping Reply. vApplicationPingReplyHook() is called by the TCP/IP
 *             stack when the stack receives a ping reply.
 * @param[in]  Ping reply status and Identifier
 * @retval     None
 **********************************************************************************************************************/
void vApplicationPingReplyHook(ePingReplyStatus_t eStatus, uint16_t usIdentifier)
{
    (void) usIdentifier;

    switch (eStatus)
    {
        /* A valid ping reply has been received */
        case eSuccess:
            ping_data.received++;
        break;
            /* A reply was received but it was not valid. */
        case eInvalidData:
        default:
            ping_data.lost++;
        break;
    }
}

/*******************************************************************************************************************//**
 * @brief      Prints the Ping response on to the RTT console
 * @param[in]  void
 * @retval     None
 **********************************************************************************************************************/
void print_pingResult(void)
{
    APP_PRINT("\r\nPing Statistics for %s :\r\n", (char* )remote_ip_address);
    APP_PRINT("\r\nPackets: Sent  = %02d, Received = %02d, Lost = %02d \r\n", ping_data.sent, ping_data.received, ping_data.lost);
}

/*******************************************************************************************************************//**
 * @brief      Creates and prints the the IP configuration to display on the RTT console
 * @param[in]  void
 * @retval     None
 **********************************************************************************************************************/
void print_ipconfig(void)
{
    if (dhcp_in_use)
    {
        ucNetMask[3] = (uint8_t) ((xNd.ulNetMask & 0xFF000000) >> 24);
        ucNetMask[2] = (uint8_t) ((xNd.ulNetMask & 0x00FF0000) >> 16);
        ucNetMask[1] = (uint8_t) ((xNd.ulNetMask & 0x0000FF00) >> 8);
        ucNetMask[0] = (uint8_t) (xNd.ulNetMask & 0x000000FF);

        ucGatewayAddress[3] = (uint8_t) ((xNd.ulGatewayAddress & 0xFF000000) >> 24);
        ucGatewayAddress[2] = (uint8_t) ((xNd.ulGatewayAddress & 0x00FF0000) >> 16);
        ucGatewayAddress[1] = (uint8_t) ((xNd.ulGatewayAddress & 0x0000FF00) >> 8);
        ucGatewayAddress[0] = (uint8_t) (xNd.ulGatewayAddress & 0x000000FF);

        ucDNSServerAddress[3] = (uint8_t) ((xNd.ulDNSServerAddress & 0xFF000000) >> 24);
        ucDNSServerAddress[2] = (uint8_t) ((xNd.ulDNSServerAddress & 0x00FF0000) >> 16);
        ucDNSServerAddress[1] = (uint8_t) ((xNd.ulDNSServerAddress & 0x0000FF00) >> 8);
        ucDNSServerAddress[0] = (uint8_t) (xNd.ulDNSServerAddress & 0x000000FF);

        ucIPAddress[3] = (uint8_t) ((xNd.ulDefaultIPAddress & 0xFF000000) >> 24);
        ucIPAddress[2] = (uint8_t) ((xNd.ulDefaultIPAddress & 0x00FF0000) >> 16);
        ucIPAddress[1] = (uint8_t) ((xNd.ulDefaultIPAddress & 0x0000FF00) >> 8);
        ucIPAddress[0] = (uint8_t) (xNd.ulDefaultIPAddress & 0x000000FF);
    }
    APP_PRINT("\r\nEthernet adapter for Renesas "KIT_NAME":\r\n");
    APP_PRINT("\tDescription                       : Renesas "KIT_NAME" Ethernet\r\n");
    APP_PRINT("\tPhysical Address                  : %02x-%02x-%02x-%02x-%02x-%02x\r\n",
            ucMACAddress[0], ucMACAddress[1], ucMACAddress[2], ucMACAddress[3], ucMACAddress[4], ucMACAddress[5]);
    APP_PRINT("\tDHCP Enabled                      : %s\r\n", dhcp_in_use ? "Yes" : "No")
    APP_PRINT("\tIPv4 Address                      : %d.%d.%d.%d\r\n", ucIPAddress[0], ucIPAddress[1], ucIPAddress[2],
              ucIPAddress[3]);
    APP_PRINT("\tSubnet Mask                       : %d.%d.%d.%d\r\n", ucNetMask[0], ucNetMask[1], ucNetMask[2],
              ucNetMask[3]);
    APP_PRINT("\tDefault Gateway                   : %d.%d.%d.%d\r\n", ucGatewayAddress[0], ucGatewayAddress[1],
              ucGatewayAddress[2], ucGatewayAddress[3]);
    APP_PRINT("\tDNS Servers                       : %d.%d.%d.%d\r\n", ucDNSServerAddress[0], ucDNSServerAddress[1],
              ucDNSServerAddress[2], ucDNSServerAddress[3]);
}

/*******************************************************************************************************************//**
 * @brief      DNS Query for the requested Domain name.  Uses the FreeRTOS Client API  FreeRTOS_gethostbyname
 *             to get the IP address for the domain name
 * @param[in]  Domain name
 * @retval     None
 **********************************************************************************************************************/
void dnsQuerryFunc(char *domain)
{
    uint32_t ulIPAddress = RESET_VALUE;
    int8_t cBuffer[16] = { RESET_VALUE };

    /* Lookup the IP address of the FreeRTOS.org website. */
    ulIPAddress = FreeRTOS_gethostbyname ((char*) domain);
    if (ulIPAddress != 0)
    {
        /* Convert the IP address to a string. */
        FreeRTOS_inet_ntoa (ulIPAddress, (char*) cBuffer);

        /* Print out the IP address obtained from the DNS lookup. */
        APP_PRINT("\r\nDNS Lookup for %s is      : %s  \r\n",domain, cBuffer);
    }
    else
    {
        APP_ERR_PRINT("\r\nDNS Lookup failed for %s \r\n",domain);
    }
}


/*******************************************************************************************************************//**
 * @brief      Update the DHCP info to the User data structure.
 * @param[in]  None
 * @retval     None
 **********************************************************************************************************************/
void updateDhcpResponseToUsr(void)
{
    if (dhcp_in_use)
    {
        memcpy (&xNd, &xNetworkAddressing, sizeof(xNd));
    }
}

/********************************************************************************************************************//**
 * @brief      DHCP Hook function to populate the user defined Host name for the Kit.
 * @param[in]  None
 * @retval     Hostname
 **********************************************************************************************************************/
#if( ipconfigDHCP_REGISTER_HOSTNAME == true )
const char* pcApplicationHostnameHook(void)
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
        int8_t lcBuffer[BUFF_SIZE];

        /* Signal application the network is UP */
        xTaskNotifyFromISR(user_app_thread, eNetworkUp, eSetBits, NULL);

        /* The network is up and configured.  Print out the configuration
        obtained from the DHCP server. */
        FreeRTOS_GetAddressConfiguration(&lulIPAddress,
                                         &lulNetMask,
                                         &lulGatewayAddress,
                                         &lulDNSServerAddress);

        /* Convert the IP address to a string then print it out. */
        FreeRTOS_inet_ntoa(lulIPAddress, (char *)lcBuffer);

        /* Convert the net mask to a string then print it out. */
        FreeRTOS_inet_ntoa(lulNetMask, (char *)lcBuffer);

        /* Convert the IP address of the gateway to a string then print it out. */
        FreeRTOS_inet_ntoa(lulGatewayAddress, (char *)lcBuffer);

        /* Convert the IP address of the DNS server to a string then print it out. */
        FreeRTOS_inet_ntoa(lulDNSServerAddress, (char *)lcBuffer);
    }
}
#endif

/********************************************************************************************************************//**
 * @brief      provision_alt_key function provides the device with client certificate and client key .
 * @param[in]  None
 * @retval     pdPASS                   Upon successful provision of client certificate and key
 * @return     Any other Error Code     Upon unsuccessful provision of client certificate and key
 **********************************************************************************************************************/
BaseType_t provision_alt_key(void)
{
    BaseType_t status = pdPASS;
    ProvisioningParams_t params = {RESET_VALUE};
    CK_RV xResult = CKR_OK;
    /* Provision the device. */
    params.pucClientPrivateKey       = (uint8_t *) CLIENT_KEY_PEM;
    params.pucClientCertificate      = (uint8_t *) CLIENT_CERTIFICATE_PEM;
    params.ulClientPrivateKeyLength  = 1 + strlen((const char *) params.pucClientPrivateKey);
    params.ulClientCertificateLength = 1 + strlen((const char *) params.pucClientCertificate);
    params.pucJITPCertificate        = NULL;
    params.ulJITPCertificateLength   = RESET_VALUE;


    xResult = vAlternateKeyProvisioning(&params);
    if (CKR_OK != xResult)
    {
        APP_ERR_PRINT("\r\n Failed in vAlternateKeyProvisioning() function ");
        return (BaseType_t) xResult;
    }

    APP_PRINT("\r\n Successfully provisioned the device with client certificate and client key ");
    return status;
}


/*******************************************************************************************************************//**
 * @} (end defgroup aws_https_client_ep)
 **********************************************************************************************************************/
