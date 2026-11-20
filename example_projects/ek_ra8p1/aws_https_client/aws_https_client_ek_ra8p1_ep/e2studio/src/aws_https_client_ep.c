/***********************************************************************************************************************
 * File Name    : aws_https_client_ep.c
 * Description  : Contains data structures and functions used in aws_https_client_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include "aws_https_client_ep.h"
#include "FreeRTOS_IP_Private.h"
#include "adc_ep.h"
#include "littlefs_ep.h"

/* Global variables */
ping_data_t g_ping_data = {RESET_VALUE, RESET_VALUE, RESET_VALUE};
uint32_t g_dhcp_in_use = RESET_VALUE;

/* Details of the HTTP request sent and of the response received. Used to display them in debug mode */
HTTPRequestInfo_t g_request_info = {RESET_VALUE, RESET_VALUE, RESET_VALUE, RESET_VALUE, RESET_VALUE, RESET_VALUE,
                                    RESET_VALUE};
HTTPResponse_t g_response = {RESET_VALUE, RESET_VALUE, RESET_VALUE, RESET_VALUE, RESET_VALUE, RESET_VALUE,
                             RESET_VALUE, RESET_VALUE, RESET_VALUE, RESET_VALUE, RESET_VALUE, RESET_VALUE,
                             RESET_VALUE, RESET_VALUE};

/* Domain for the DNS Host lookup is used in this example project.
 * The project can be built with different *domain_name to validate the DNS client */
char * domain_name = HTTPS_HOST_ADDRESS;

/* IP address of the PC or any Device on the LAN/WAN where the Ping request is sent.
 * Note: Users need to change this according to the LAN settings of the test PC or device
 * when running this project. */
char remote_ip_address[] = HTTPS_TEST_PING_IP;

/* Active key generated from io.adafruit.com site, entered by the user */
char g_aio_key[SIZE_64] = {RESET_VALUE};

/* DHCP populates these IP address, subnet mask and gateway address, so they start with zeroed out values.
 * The MAC address is a test MAC address. */
uint8_t ucMACAddress[6] = {0x74, 0x90, 0x50, 0x65, 0x81, 0x70};
uint8_t ucIPAddress[4] = {RESET_VALUE};
uint8_t ucNetMask[4] = {255, 255, 255, 0};
uint8_t ucGatewayAddress[4] = {RESET_VALUE};
uint8_t ucDNSServerAddress[4] = {RESET_VALUE};

/* Private variables */
static IPV4Parameters_t s_nd = {RESET_VALUE, RESET_VALUE, RESET_VALUE, {RESET_VALUE, RESET_VALUE}, RESET_VALUE,
                                RESET_VALUE};
static NetworkContext_t s_network_context;
static TlsTransportParams_t s_tls_transport_params;
static TransportInterface_t s_transport_interface;
static HTTPRequestHeaders_t s_request_headers;
static uint8_t s_request_buffer[USER_BUFF] = {RESET_VALUE};
static uint8_t s_response_buffer[USER_BUFF] = {RESET_VALUE};

/* Flag to track whether the crypto platform is set up */
static bool s_crypto_ready = false;

/* Private functions */
static BaseType_t vSendPing(const char * pcIPAddress);
static void updateDhcpResponseToUsr(void);
static HTTPStatus_t add_header(HTTPRequestHeaders_t * pRequestHeaders);

/*******************************************************************************************************************//**
 * @brief     This function initializes the crypto hardware acceleration and mbedtls.
 * @param[in] None.
 * @retval    0                        Initialized successfully.
 * @retval    Any other error codes    Failed to initialize.
 **********************************************************************************************************************/
int32_t crypto_init(void)
{
    int ierr = mbedtls_platform_setup(NULL);
    if (0 == ierr)
    {
        s_crypto_ready = true;
    }

    return (int32_t) ierr;
}

/*******************************************************************************************************************//**
 * @brief     This function initializes the FreeRTOS+TCP stack.
 * @param[in] None.
 * @retval    FSP_SUCCESS              Initialized successfully.
 * @retval    Any other error codes    Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t ip_stack_init(void)
{
    BaseType_t status = FreeRTOS_IPInit(ucIPAddress, ucNetMask, ucGatewayAddress, ucDNSServerAddress, ucMACAddress);

    return (pdFALSE == status) ? FSP_ERR_INTERNAL : FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief     This function waits until the network is up. The notification is sent by the network event hook.
 * @param[in] None.
 * @retval    FSP_SUCCESS              Network is up.
 * @retval    Any other error codes    Failed to wait for the network.
 **********************************************************************************************************************/
fsp_err_t network_link_wait(void)
{
    uint32_t ip_status = RESET_VALUE;
    BaseType_t bt_status = xTaskNotifyWait(pdFALSE, pdFALSE, &ip_status, portMAX_DELAY);

    return (pdTRUE == bt_status) ? FSP_SUCCESS : FSP_ERR_TIMEOUT;
}

/*******************************************************************************************************************//**
 * @brief     This function updates the IP configuration with the values provided by the DHCP server, if used.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void ip_config_update(void)
{
    if (g_dhcp_in_use)
    {
        ucNetMask[3] = (uint8_t) ((s_nd.ulNetMask & 0xFF000000) >> 24);
        ucNetMask[2] = (uint8_t) ((s_nd.ulNetMask & 0x00FF0000) >> 16);
        ucNetMask[1] = (uint8_t) ((s_nd.ulNetMask & 0x0000FF00) >> 8);
        ucNetMask[0] = (uint8_t) (s_nd.ulNetMask & 0x000000FF);

        ucGatewayAddress[3] = (uint8_t) ((s_nd.ulGatewayAddress & 0xFF000000) >> 24);
        ucGatewayAddress[2] = (uint8_t) ((s_nd.ulGatewayAddress & 0x00FF0000) >> 16);
        ucGatewayAddress[1] = (uint8_t) ((s_nd.ulGatewayAddress & 0x0000FF00) >> 8);
        ucGatewayAddress[0] = (uint8_t) (s_nd.ulGatewayAddress & 0x000000FF);

        ucDNSServerAddress[3] = (uint8_t) ((s_nd.ulDNSServerAddresses[0] & 0xFF000000) >> 24);
        ucDNSServerAddress[2] = (uint8_t) ((s_nd.ulDNSServerAddresses[0] & 0x00FF0000) >> 16);
        ucDNSServerAddress[1] = (uint8_t) ((s_nd.ulDNSServerAddresses[0] & 0x0000FF00) >> 8);
        ucDNSServerAddress[0] = (uint8_t) (s_nd.ulDNSServerAddresses[0] & 0x000000FF);

        ucIPAddress[3] = (uint8_t) ((s_nd.ulIPAddress & 0xFF000000) >> 24);
        ucIPAddress[2] = (uint8_t) ((s_nd.ulIPAddress & 0x00FF0000) >> 16);
        ucIPAddress[1] = (uint8_t) ((s_nd.ulIPAddress & 0x0000FF00) >> 8);
        ucIPAddress[0] = (uint8_t) (s_nd.ulIPAddress & 0x000000FF);
    }
}

/*******************************************************************************************************************//**
 * @brief      This function resolves the IP address of the requested domain name.
 * @param[in]  p_domain                 Domain name.
 * @param[out] p_ip_str                 IP address string buffer.
 * @retval     FSP_SUCCESS              Resolved successfully.
 * @retval     FSP_ERR_NOT_FOUND        Failed to resolve.
 **********************************************************************************************************************/
fsp_err_t dns_lookup(char * p_domain, char * p_ip_str)
{
    uint32_t ulIPAddress = FreeRTOS_gethostbyname(p_domain);
    if (RESET_VALUE == ulIPAddress)
    {
        return FSP_ERR_NOT_FOUND;
    }

    /* Convert the IP address to a string */
    FreeRTOS_inet_ntoa(ulIPAddress, p_ip_str);

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief     This function sends one ICMP ping request to the remote IP address and updates the ping statistics.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void ping_request_send(void)
{
    if (pdFAIL != vSendPing(remote_ip_address))
    {
        g_ping_data.sent++;
    }
    else
    {
        g_ping_data.lost++;
    }
}

/*******************************************************************************************************************//**
 * @brief     This function provisions the device with the client certificate and the client key.
 * @param[in] None.
 * @retval    0                        Provisioned successfully.
 * @retval    Any other error codes    Failed to provision.
 **********************************************************************************************************************/
int32_t provision_alt_key(void)
{
    ProvisioningParams_t params = {RESET_VALUE, RESET_VALUE, RESET_VALUE, RESET_VALUE, RESET_VALUE, RESET_VALUE};

    /* Provision the device */
    params.pucClientPrivateKey = (uint8_t *) CLIENT_KEY_PEM;
    params.pucClientCertificate = (uint8_t *) CLIENT_CERTIFICATE_PEM;
    params.ulClientPrivateKeyLength = 1 + strlen((const char *) params.pucClientPrivateKey);
    params.ulClientCertificateLength = 1 + strlen((const char *) params.pucClientCertificate);
    params.pucJITPCertificate = NULL;
    params.ulJITPCertificateLength = RESET_VALUE;

    return (int32_t) vAlternateKeyProvisioning(&params);
}

/*******************************************************************************************************************//**
 * @brief     This function makes one attempt to connect to the server with all required connection configuration
 *            settings, and sets up the transport interface used by the HTTP requests.
 * @param[in] None.
 * @retval    TLS_TRANSPORT_SUCCESS    Connected successfully.
 * @retval    Any other error codes    Failed to connect.
 **********************************************************************************************************************/
TlsTransportStatus_t connect_aws_https_client(void)
{
    TlsTransportStatus_t tls_status = TLS_TRANSPORT_SUCCESS;
    NetworkCredentials_t connConfig = {RESET_VALUE, RESET_VALUE, RESET_VALUE, RESET_VALUE, RESET_VALUE, RESET_VALUE,
                                       RESET_VALUE, RESET_VALUE, RESET_VALUE, RESET_VALUE};

    memset(&s_tls_transport_params, RESET_VALUE, sizeof(TlsTransportParams_t));
    memset(&s_network_context, RESET_VALUE, sizeof(NetworkContext_t));
    s_network_context.pParams = &s_tls_transport_params;

    /* Set the connection configurations */
    connConfig.disableSni = pdFALSE;
    connConfig.pRootCa = (const unsigned char *) HTTPS_TRUSTED_ROOT_CA;
    connConfig.rootCaSize = sizeof(HTTPS_TRUSTED_ROOT_CA);
    connConfig.pUserName = NULL;
    connConfig.userNameSize = 0;
    connConfig.pPassword = NULL;
    connConfig.passwordSize = 0;
    connConfig.pClientCertLabel = pkcs11configLABEL_DEVICE_CERTIFICATE_FOR_TLS;
    connConfig.pPrivateKeyLabel = pkcs11configLABEL_DEVICE_PRIVATE_KEY_FOR_TLS;
    connConfig.pAlpnProtos = NULL;

    /* Connect to server */
    tls_status = TLS_FreeRTOS_Connect(&s_network_context, HTTPS_HOST_ADDRESS, HTTPS_PORT, &connConfig,\
                                      SOCKET_SEND_RECV_TIME_OUT_MS, SOCKET_SEND_RECV_TIME_OUT_MS);
    if (TLS_TRANSPORT_SUCCESS == tls_status)
    {
        s_transport_interface.pNetworkContext = &s_network_context;
        s_transport_interface.send = TLS_FreeRTOS_send;
        s_transport_interface.recv = TLS_FreeRTOS_recv;
    }

    return tls_status;
}

/*******************************************************************************************************************//**
 * @brief     This function converts the HTTP Client library status code to a string.
 * @param[in] status                    HTTP Client library status code.
 * @retval    Pointer to the status string.
 **********************************************************************************************************************/
const char * http_error_string_get(HTTPStatus_t status)
{
    return HTTPClient_strerror(status);
}

/*******************************************************************************************************************//**
 * @brief     This function tears down the crypto platform if it is set up.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void deinit_aws_https_client(void)
{
    if (s_crypto_ready)
    {
        mbedtls_platform_teardown(NULL);
        s_crypto_ready = false;
    }
}

/*******************************************************************************************************************//**
 * @brief     This function is the user hook for the DHCP response. xApplicationDHCPHook() is called by the DHCP
 *            client code when DHCP handshake messages are exchanged with the server.
 * @param[in] eDHCPPhase                DHCP phase.
 * @param[in] lulIPAddress              The offered IP address.
 * @retval    eDHCPContinue             DHCP process continues.
 **********************************************************************************************************************/
eDHCPCallbackAnswer_t xApplicationDHCPHook(eDHCPCallbackPhase_t eDHCPPhase, uint32_t lulIPAddress)
{
    eDHCPCallbackAnswer_t eReturn = eDHCPContinue;

    /* This hook is called in a couple of places during the DHCP process, as identified by the eDHCPPhase parameter */
    switch (eDHCPPhase)
    {
        case eDHCPPhasePreDiscover:
        {
            /* A DHCP discovery is about to be sent out.  eDHCPContinue is returned to allow the discovery to go out.
             * If eDHCPUseDefaults had been returned instead then the DHCP process would be stopped and the statically
             * configured IP address would be used.
             * If eDHCPStopNoChanges had been returned instead then the DHCP process would be stopped and whatever the
             * current network configuration was would continue to be used. */
            break;
        }

        case eDHCPPhasePreRequest:
        {
            /* An offer has been received from the DHCP server, and the offered IP address is passed in the
             * lulIPAddress parameter. */

            /* Update the structure, the DHCP state machine is not updating this */
            s_nd.ulIPAddress = lulIPAddress;
            g_dhcp_in_use = 1;
            updateDhcpResponseToUsr();
            break;
        }

        default:
        {
            /* Cannot be reached, kept to prevent compiler warnings where compilers are disposed to
             * generating one. */
            break;
        }
    }

    return eReturn;
}

/*******************************************************************************************************************//**
 * @brief     This function is the user hook for the ping reply.
 * @param[in] eStatus                   Ping reply status.
 * @param[in] usIdentifier              Ping reply identifier.
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

/*******************************************************************************************************************//**
 * @brief     This function is the DHCP hook that populates the user defined host name for the kit.
 * @param[in] None.
 * @retval    KIT_NAME                  Host name.
 **********************************************************************************************************************/
#if (ipconfigDHCP_REGISTER_HOSTNAME == 1)
const char * pcApplicationHostnameHook(void)
{
    return KIT_NAME;
}
#endif /* ipconfigDHCP_REGISTER_HOSTNAME */

/*******************************************************************************************************************//**
 * @brief     This function is the network event callback. It indicates the network event. When the network is up
 *            and configured by the DHCP server, the user thread is notified.
 * @param[in] eNetworkEvent             Network event.
 * @retval    None.
 **********************************************************************************************************************/
#if (ipconfigUSE_NETWORK_EVENT_HOOK == 1)
void vApplicationIPNetworkEventHook(eIPCallbackEvent_t eNetworkEvent)
{
    if ((eNetworkUp == eNetworkEvent) && (g_dhcp_in_use))
    {
        /* Signal application the network is UP */
        xTaskNotifyFromISR(user_app_thread, eNetworkUp, eSetBits, NULL);
    }
}
#endif /* ipconfigUSE_NETWORK_EVENT_HOOK */

/*******************************************************************************************************************//**
 * @brief     This function sends an ICMP ping request to the IP address specified by the user.
 * @param[in] pcIPAddress               IP address to ping.
 * @retval    Sequence number           Sent successfully.
 * @retval    pdFAIL                    Failed to send.
 **********************************************************************************************************************/
static BaseType_t vSendPing(const char * pcIPAddress)
{
    uint32_t ulIPAddress = RESET_VALUE;

    /* The pcIPAddress parameter holds the destination IP address as a string in
     * decimal dot notation (for example, "192.168.0.200"). Convert the string into
     * the required 32-bit format. */
    ulIPAddress = FreeRTOS_inet_addr(pcIPAddress);

    /* Send a ping request containing 8 data bytes.  Wait (in the Blocked state) a
     * maximum of 100ms for a network buffer into which the generated ping request
     * can be written and sent. */
    return FreeRTOS_SendPingRequest(ulIPAddress, 8, 100 / portTICK_PERIOD_MS);
}

/*******************************************************************************************************************//**
 * @brief     This function updates the DHCP info to the user data structure.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void updateDhcpResponseToUsr(void)
{
    if (g_dhcp_in_use)
    {
        s_nd.ulNetMask = FreeRTOS_GetNetmask();
        s_nd.ulGatewayAddress = FreeRTOS_GetGatewayAddress();
        s_nd.ulDNSServerAddresses[0] = FreeRTOS_GetDNSServerAddress();
    }
}

/*******************************************************************************************************************//**
 * @brief     This function adds the header for HTTPS request in JSON format.
 * @param[in] pRequestHeaders           Request handle for GET, PUT, POST methods.
 * @retval    HTTPSuccess               Header added successfully.
 * @retval    Any other error codes     Failed to add the header.
 **********************************************************************************************************************/
static HTTPStatus_t add_header(HTTPRequestHeaders_t * pRequestHeaders)
{
    HTTPStatus_t status = HTTPSuccess;

    configASSERT(NULL != pRequestHeaders);

    status = HTTPClient_AddHeader(pRequestHeaders, "Content-Type", strlen("Content-Type"),
                                  "application/json", strlen("application/json"));
    if (HTTPSuccess == status)
    {
        status = HTTPClient_AddHeader(pRequestHeaders, "X-AIO-Key", strlen("X-AIO-Key"), g_aio_key,
                                      strlen(g_aio_key));
    }

    return status;
}

/*******************************************************************************************************************//**
 * @brief     This function builds the HTTP request and sends it over the established TLS connection.
 * @param[in] p_method                  HTTP method of the request.
 * @param[in] p_path                    Path of the request.
 * @param[in] temp_c                    MCU die temperature in degrees Celsius to upload.
 * @retval    HTTPSuccess               Request sent and response received successfully.
 * @retval    Any other error codes     Failed to send the request.
 **********************************************************************************************************************/
HTTPStatus_t http_request_send(const char * p_method, const char * p_path, float temp_c)
{
    HTTPStatus_t status = HTTPSuccess;
    char upload_str[SIZE_64] = {RESET_VALUE};
    size_t upload_len = RESET_VALUE;

    /* Initialize all HTTP Client library API structs to 0 */
    memset(&g_request_info, 0, sizeof(g_request_info));
    memset(&g_response, 0, sizeof(g_response));
    memset(&s_request_headers, 0, sizeof(s_request_headers));

    /* Initialize the request object */
    g_request_info.pPath = p_path;
    g_request_info.pathLen = strlen(p_path);
    g_request_info.pHost = HTTPS_HOST_ADDRESS;
    g_request_info.hostLen = strlen(HTTPS_HOST_ADDRESS);
    g_request_info.pMethod = p_method;
    g_request_info.methodLen = strlen(p_method);

    /* Set "Connection" HTTP header to "keep-alive" so that multiple requests
     * can be sent over the same established TCP connection */
    g_request_info.reqFlags = HTTP_REQUEST_KEEP_ALIVE_FLAG;

    /* Set the buffer used for storing request headers */
    s_request_headers.pBuffer = s_request_buffer;
    s_request_headers.bufferLen = sizeof(s_request_buffer);
    memset(s_request_headers.pBuffer, 0, s_request_headers.bufferLen);

    status = HTTPClient_InitializeRequestHeaders(&s_request_headers, &g_request_info);
    if (HTTPSuccess == status)
    {
        status = add_header(&s_request_headers);
    }

    /* Set the buffer used for storing the response */
    g_response.pBuffer = s_response_buffer;
    g_response.bufferLen = sizeof(s_response_buffer);
    memset(g_response.pBuffer, 0, g_response.bufferLen);

    /* Format the temperature into string to send in JSON format, the GET request has no body */
    if (0 != strcmp(p_method, HTTP_METHOD_GET))
    {
        snprintf(upload_str, sizeof(upload_str), "{\"datum\":{\"value\":\"%.02f\"}}", temp_c);
        upload_len = strlen(upload_str);
    }

    if (HTTPSuccess == status)
    {
        status = HTTPClient_Send(&s_transport_interface, &s_request_headers,
                                 (0 != upload_len) ? (const uint8_t *) upload_str : NULL, upload_len, &g_response, 0);
    }

    return status;
}
