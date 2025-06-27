/***********************************************************************************************************************
 * File Name    : wifi_ep.c
 * Description  : Contains data structures and functions used in wifi_ep.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2022 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "wifi_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup wifi_on_chip_udp_freertos_ep
 * @{
 **********************************************************************************************************************/

/* Setup Access Point connection parameters */
WIFINetworkParams_t g_net_params =
{
 .ucChannel                  = RESET_VALUE,
 .xPassword.xWPA.cPassphrase = DEFAULT_PASSPHRASE,
 .ucSSID                     = DEFAULT_SSID,
 .xPassword.xWPA.ucLength    = DEFAULT_PASSWORD_LEN,
 .ucSSIDLength               = DEFAULT_SSID_LEN,
 .xSecurity                  = eWiFiSecurityWPA2,
};

/* Setup socket parameters */
SocketPrameters_t g_socket =
{
 .id                        = RESET_VALUE,
 .ip_addr_server            = RESET_VALUE,
 .port                      = RESET_VALUE,
 .recv_buff                 = {RESET_VALUE},
 .send_buff                 = {RESET_VALUE},
 .status                    = RESET_VALUE
};

/*******************************************************************************************************************//**
 * @brief       This function takes input from user.
 * @param[IN]   user_buff     Char pointer pointing to user input buffer.
 * @retval      None.
 * @retval      None.
 **********************************************************************************************************************/
void user_input(char * user_buff)
{
    uint8_t rByte[BUFF_LEN] = {RESET_VALUE};      /* RTT buffer to read data */
    uint32_t num_bytes = RESET_VALUE;             /* Number of bytes read by RTT */

    /* Resetting user_buff */
    memset(user_buff, RESET_VALUE, BUFF_LEN);
    num_bytes = RESET_VALUE;

    while(BYTES_RECEIVED_ZERO == num_bytes)
    {
        if(APP_CHECK_DATA)
        {
            num_bytes = (uint32_t) APP_READ(rByte);
            if(BYTES_RECEIVED_ZERO == num_bytes)
            {
                APP_PRINT("\r\nNo input\r\n");
            }
        }
    }

    switch(rByte[num_bytes - INDEX_ONE])
    {
        /* If the last character is carriage return */
        case CR:
        {
            rByte[num_bytes - INDEX_ONE] = NULL_CHAR;
            memcpy(user_buff, rByte, (size_t) num_bytes);
        }
        break;
        /* If the last character is line feed */
        case LF:
        {
            if(CR == rByte[num_bytes - INDEX_TWO])
            {
                rByte[num_bytes - INDEX_TWO] = NULL_CHAR;
                rByte[num_bytes - INDEX_ONE] = NULL_CHAR;
                memcpy(user_buff, rByte, (size_t) (num_bytes - INDEX_ONE));
            }
            else
            {
                rByte[num_bytes - INDEX_ONE] = NULL_CHAR;
                memcpy(user_buff, rByte, (size_t) num_bytes);
            }
        }
        break;
        /* Copy the string to user buffer */
        default:
        {
            memcpy(user_buff, rByte, (size_t) num_bytes);
        }
        break;
    }
}

/*******************************************************************************************************************//**
 * @brief       This function scans for available Wi-Fi Access Points (AP) and prints them in a list.
 *              Gives user the option to select the AP from that list and enter the
 *              credentials for the same to connect.
 * @param[IN]   None.
 * @retval      eWiFiSuccess                Upon successful completion of the function.
 * @retval      Any other error code        If Wi-Fi AP scan fails.
 **********************************************************************************************************************/
WIFIReturnCode_t scan_and_select(void)
{
    WIFIReturnCode_t wifi_err = eWiFiSuccess;
    char input_buff[BUFF_LEN] = {RESET_VALUE};              /* Buffer for storing user input */
    uint8_t index_wifi_ap_list = RESET_VALUE;               /* Numerical conversion of RTT input string */
    WIFIScanResult_t scan_data[MAX_WIFI_SCAN_RESULTS] =
    {
        {
            .ucSSID       = {RESET_VALUE},
            .ucSSIDLength = RESET_VALUE,
            .ucBSSID      = {RESET_VALUE},
            .xSecurity    = RESET_VALUE,
            .cRSSI        = RESET_VALUE,
            .ucChannel    = RESET_VALUE
        }
    }; /* Buffer for storing WiFi AP scan result */

    do
    {
        memset(scan_data, RESET_VALUE, sizeof(scan_data));
        APP_PRINT("\r\nScanning WiFi Access Points\r\n");

        /* Scanning for WiFi APs */
        wifi_err = WIFI_Scan(&scan_data[INDEX_ZERO], MAX_WIFI_SCAN_RESULTS);
        if(eWiFiSuccess != wifi_err)
        {
            APP_ERR_PRINT("\r\n** rm_wifi_onchip_silex_scan API for rm_wifi_onchip_silex failed **\r\n");
            return wifi_err;
        }

        /* Printing SSID of WiFi APs */
        APP_PRINT("\r\n***** List of WiFi Access Points *****");
        for(uint8_t index = RESET_VALUE; index < MAX_WIFI_SCAN_RESULTS; index++)
        {
            uint8_t temp = scan_data[index].ucSSID[INDEX_ZERO];
            if(NULL_CHAR != temp)
            {
                switch(scan_data[index].xSecurity)
                {
                    /* Open network */
                    case eWiFiSecurityOpen:
                    {
                        APP_PRINT("\r\n(%d) %s: \r\n    Security: Open, Signal strength: %d, Channel: %d",\
                                  index, scan_data[index].ucSSID, scan_data[index].cRSSI, scan_data[index].ucChannel);
                    }
                    break;
                    /* Security type - WEP */
                    case eWiFiSecurityWEP:
                    {
                        APP_PRINT("\r\n(%d) %s: \r\n    Security: WEP, Signal strength: %d, Channel: %d",\
                                  index, scan_data[index].ucSSID, scan_data[index].cRSSI, scan_data[index].ucChannel);
                        APP_PRINT("\r\nNote: Currently WiFi APs with WEP security is not supported. "\
                                  "Please do not select.");
                    }
                    break;
                    /* Security type - WPA */
                    case eWiFiSecurityWPA:
                    {
                        APP_PRINT("\r\n(%d) %s: \r\n    Security: WPA, Signal strength: %d, Channel: %d",\
                                  index, scan_data[index].ucSSID, scan_data[index].cRSSI, scan_data[index].ucChannel);
                    }
                    break;
                    /* Security type - WPA2 */
                    case eWiFiSecurityWPA2:
                    {
                        APP_PRINT("\r\n(%d) %s: \r\n    Security: WPA2, Signal strength: %d, Channel: %d",\
                                  index, scan_data[index].ucSSID, scan_data[index].cRSSI, scan_data[index].ucChannel);
                    }
                    break;
                    /* Security type - WPA ENT */
                    case eWiFiSecurityWPA2_ent:
                        /* Do nothing */
                        break;
                    /* Security type - WPA3 */
                    case eWiFiSecurityWPA3:
                        /* Do nothing */
                        break;
                    /* Type not supported */
                    case eWiFiSecurityNotSupported:
                        /* Do nothing */
                        break;
                }
            }
        }

        /* User selecting SSID from WiFi APs list */
        APP_PRINT("\r\nEnter WiFi AP's SSID index to be connected.");
        APP_PRINT("\r\nOR Enter any other character to Re-scan\r\n");
        user_input(input_buff);

        index_wifi_ap_list = (uint8_t) atoi(input_buff);
    }while((MIN_ZERO > input_buff[INDEX_ZERO]) || (MAX_NINE < input_buff[INDEX_ZERO]) || \
            (MAX_WIFI_SCAN_RESULTS <= index_wifi_ap_list));

    /* Storing SSID and security type of WiFi AP */
    strcpy((char*) g_net_params.ucSSID, (char*) scan_data[index_wifi_ap_list].ucSSID);
    g_net_params.xSecurity = scan_data[index_wifi_ap_list].xSecurity;

    /* WiFi AP password input for non-open security type */
    if(eWiFiSecurityOpen != g_net_params.xSecurity)
    {
        /* User input for password of WiFi AP */
        APP_PRINT("\r\nEnter password for " RTT_CTRL_TEXT_BRIGHT_CYAN "%s." RTT_CTRL_RESET, g_net_params.ucSSID);
        user_input(g_net_params.xPassword.xWPA.cPassphrase);
    }

    return wifi_err;
}

/*******************************************************************************************************************//**
 * @brief       This function takes user input for WiFi AP's SSID, security type and credentials, and connect to it.
 * @param[IN]   None.
 * @retval      eWiFiSuccess                Upon successful completion of the function.
 * @retval      Any other error code        If WiFi AP scan fails.
 **********************************************************************************************************************/
WIFIReturnCode_t manual_connect(void)
{
    WIFIReturnCode_t wifi_err = eWiFiSuccess;
    WIFIReturnCode_t connect_status = eWiFiFailure;
    char input_buff[BUFF_LEN] = {RESET_VALUE};      /* Buffer for storing user input */

    /* User input for SSID of WiFi AP */
    APP_PRINT("\r\nEnter SSID of WiFi Access Point:");
    user_input((char *)g_net_params.ucSSID);

    /* User selecting security type of WiFi AP */
    do
    {
        /* Menu for WiFi Security types */
        APP_PRINT(SECURITY_MENU_OPTIONS);
        user_input(input_buff);

        /* Loop iteration to discard any other value apart from the valid security type */
    }while((MIN_ZERO > input_buff[INDEX_ZERO]) || (MAX_TWO < input_buff[INDEX_ZERO]) || \
            (NULL_CHAR != input_buff[INDEX_ONE]));

    /* Checking for the Open security type */
    if(WIFI_SECURITY_OPEN == (uint32_t)atoi(input_buff))
    {
        g_net_params.xSecurity = eWiFiSecurityOpen;
    }
    else
    {
        g_net_params.xSecurity = ((uint32_t)atoi(input_buff) == \
                                  WIFI_SECURITY_WPA ? eWiFiSecurityWPA : eWiFiSecurityWPA2);
        /* WiFi AP password input for non-open security type */
        APP_PRINT("\r\nEnter password for " RTT_CTRL_TEXT_BRIGHT_CYAN "%s" RTT_CTRL_RESET, g_net_params.ucSSID);
        user_input(g_net_params.xPassword.xWPA.cPassphrase);
    }

    /* Connecting to user entered SSID */
    APP_PRINT("\r\nConnecting to " RTT_CTRL_TEXT_BRIGHT_CYAN "%s \r\n" RTT_CTRL_RESET, g_net_params.ucSSID);
    wifi_err = WIFI_ConnectAP(&g_net_params);
    if(eWiFiSuccess != wifi_err)
    {
        APP_ERR_PRINT("\r\n** WIFI_ConnectAP API failed **\r\n");
        return wifi_err;
    }
    else
    {
        /* Status check of WiFi connectivity */
        connect_status = WIFI_IsConnected(NULL);
        if(eWiFiSuccess != connect_status)
        {
            APP_PRINT("\r\nWiFi not connected.");
        }
    }

    return wifi_err;
}

/*******************************************************************************************************************//**
 * @brief       This function resolves the DNS query or validates the IP address.
 * @param[IN]   p_dns                       Pointer to array holding URL or IP address to query from DNS.
 * @param[out]  p_ip_addr                   Pointer to IP address returned from look up.
 * @retval      eWiFiSuccess                Upon successful DNS resolving.
 * @retval      Any other error code        If DNS query fails.
 **********************************************************************************************************************/
WIFIReturnCode_t dns_query(char * p_dns, uint8_t * p_ip_addr)
{
    WIFIReturnCode_t wifi_err = eWiFiSuccess;
    WIFIReturnCode_t connect_status = eWiFiFailure;

    /* Status check of WiFi connectivity */
    connect_status = WIFI_IsConnected(NULL);
    if(eWiFiSuccess != connect_status)
    {
        APP_PRINT("\r\nWiFi not connected");
        return eWiFiFailure;
    }

    /* DNS query to retrieve the Host IP */
    wifi_err = WIFI_GetHostIP(p_dns, p_ip_addr);
    if(eWiFiSuccess != wifi_err)
    {
        APP_ERR_PRINT("\r\n** WIFI_GetHostIP API Failed **\r\n");
        return wifi_err;
    }

    return wifi_err;
}

/*******************************************************************************************************************//**
 * @brief       This function pings the IP address.
 * @param[in]   p_ip_addr                   Pointer to IP address array.
 * @retval      eWiFiSuccess                Upon successful ping.
 * @retval      Any other error code        If ping fails.
 **********************************************************************************************************************/
WIFIReturnCode_t ping(uint8_t * p_ip_addr)
{
    WIFIReturnCode_t wifi_err = eWiFiSuccess;
    WIFIReturnCode_t connect_status = eWiFiFailure;

    /* Status check of WiFi connectivity */
    connect_status = WIFI_IsConnected(NULL);
    if(eWiFiSuccess != connect_status)
    {
        APP_PRINT("\r\nWiFi not connected");
        return eWiFiFailure;
    }

    /* Ping IP address */
    wifi_err = WIFI_Ping(p_ip_addr, PING_COUNT, PINT_INTERVAL_MS);
    if(eWiFiSuccess != wifi_err)
    {
        APP_ERR_PRINT("\r\n** WIFI_Ping API failed **\r\n");
        return wifi_err;
    }

    return wifi_err;
}

/*******************************************************************************************************************//**
 * @brief       This function creates a new UDP socket in client mode.
 * @param[in]   None.
 * @retval      FSP_SUCCESS                 Upon successful UDP socket creation.
 * @retval      Any other error code        If UDP socket creation fails.
 **********************************************************************************************************************/
fsp_err_t udp_socket_create()
{
    fsp_err_t err = FSP_SUCCESS;
    WIFIReturnCode_t connect_status = eWiFiFailure;

    /* Status check of WiFi connectivity */
    connect_status = WIFI_IsConnected(NULL);
    if(eWiFiSuccess != connect_status)
    {
        APP_PRINT("\r\nWiFi not connected");
        return FSP_ERR_WIFI_FAILED;
    }

    /* Creating UDP socket */
    err = rm_wifi_onchip_silex_socket_create(g_socket.id, SOCKET_UDP, IPV4);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** rm_wifi_onchip_silex_socket_create API failed **\r\n");
        return err;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function connects to the UDP server IP address.
 * @param[in]   ip_addr                     IP address of the server for socket connection.
 * @param[in]   port                        Port number of the server for socket connection.
 * @retval      FSP_SUCCESS                 Upon successfully connecting to the server.
 * @retval      Any other error code        If connection fails.
 **********************************************************************************************************************/
fsp_err_t udp_socket_connect(uint32_t ip_addr, uint32_t port)
{
    fsp_err_t err = FSP_SUCCESS;
    WIFIReturnCode_t connect_status = eWiFiFailure;

    /* Status check of WiFi connectivity */
    connect_status = WIFI_IsConnected(NULL);
    if(eWiFiSuccess != connect_status)
    {
        APP_PRINT("\r\nWiFi not connected");
        return FSP_ERR_WIFI_FAILED;
    }

    for(uint8_t iteration = RESET_VALUE; iteration <= WIFI_MAX_TRY; iteration++)
    {
        /* Connecting to UDP socket */
        err = rm_wifi_onchip_silex_udp_connect(g_socket.id, SOCKETS_HTONL(ip_addr), port, 0);
        if(FSP_SUCCESS == err)
        {
            return err;
        }
    }
    APP_ERR_PRINT("\r\n** rm_wifi_onchip_silex_udp_connect API failed **\r\n");
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function sends data to the UDP server.
 * @param[in]   p_data                      Pointer to data to be send.
 * @retval      bytes_sent                  Number of bytes sent to server.
 * @retval      Any other error code        If send fails.
 **********************************************************************************************************************/
int32_t udp_send_data(uint8_t * p_data)
{
    int32_t bytes_sent = RESET_VALUE;

    /* Check for Internet connectivity */
    if(FSP_SUCCESS != network_connectivity_check())
    {
        return WIFI_ONCHIP_SILEX_ERR_ERROR;
    }
    else
    {
        /* Send data to UDP server */
        bytes_sent = rm_wifi_onchip_silex_send(g_socket.id, p_data, strlen((char *) p_data), \
                                               WIFI_ONCHIP_SILEX_TIMEOUT_400MS);
        if(RESET_VALUE >= bytes_sent)
        {
            APP_ERR_PRINT("\r\n** rm_wifi_onchip_silex_send API failed **\r\n");
            return bytes_sent;
        }
    }

    return bytes_sent;
}

/*******************************************************************************************************************//**
 * @brief       This function receives data from the UDP server.
 * @param[out]  p_data                      Pointer to data received from socket.
 * @retval      bytes_received              Number of bytes received from server.
 * @retval      Any other error code        If timeout occurs.or receive fails.
 **********************************************************************************************************************/
int32_t udp_recv_data(uint8_t * p_data)
{
    int32_t bytes_received = RESET_VALUE;

    /* Check for Internet connectivity */
    if(FSP_SUCCESS != network_connectivity_check())
    {
        return WIFI_ONCHIP_SILEX_ERR_ERROR;
    }
    else
    {
        /* Receive data from UDP server */
        bytes_received = rm_wifi_onchip_silex_recv(g_socket.id, p_data, BUFF_LEN, WIFI_ONCHIP_SILEX_TIMEOUT_15SEC);
        if(RESET_VALUE > bytes_received)
        {
            APP_ERR_PRINT("\r\n** rm_wifi_onchip_silex_recv API failed **\r\n");
            return bytes_received;
        }
    }
    return bytes_received;
}

/*******************************************************************************************************************//**
 * @brief       This function checks both WiFi connectivity and UDP socket connectivity.
 * @param[in]   None
 * @retval      FSP_SUCCESS                         If both the connectivity checks are success.
 * @retval      FSP_ERR_WIFI_FAILED                 If one of the connectivity check fails.
 **********************************************************************************************************************/
fsp_err_t network_connectivity_check(void)
{
    WIFIReturnCode_t connect_status = eWiFiFailure;

    /* Checking WiFi connectivity */
    connect_status = WIFI_IsConnected(NULL);
    if(eWiFiSuccess != connect_status)
    {
        APP_PRINT("\r\nWiFi not connected");
        return FSP_ERR_WIFI_FAILED;
    }

    /* Checking socket connection status */
    rm_wifi_onchip_silex_socket_status_get(g_socket.id, &g_socket.status);
    if(WIFI_ONCHIP_SILEX_SOCKET_STATUS_CONNECTED != g_socket.status)
    {
        APP_ERR_PRINT("\r\nUDP socket disconnected");
        return FSP_ERR_WIFI_FAILED;
    }

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief       This function disconnects the socket connection and closes the WiFi module.
 * @param[in]   None
 * @retval      None
 **********************************************************************************************************************/
void wifi_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;
    WIFIReturnCode_t connect_status = eWiFiFailure;
    WIFIReturnCode_t wifi_err = eWiFiSuccess;

    /* Checking socket connection status */
    rm_wifi_onchip_silex_socket_status_get(g_socket.id, &g_socket.status);
    if(WIFI_ONCHIP_SILEX_SOCKET_STATUS_CONNECTED == g_socket.status)
    {
        /* If socket connected, then disconnecting */
        err = rm_wifi_onchip_silex_disconnect();
        /* Handle error */
        if(FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\n** rm_wifi_onchip_silex_disconnect API failed **\r\n");
        }
    }

    /* Checking WiFi connectivity */
    connect_status = WIFI_IsConnected(NULL);
    if(eWiFiSuccess != connect_status)
    {
        /* If WiFi connected, closing WiFi module. Thus disconnecting from connected AP */
        wifi_err = WIFI_Off();
        /* Handle error */
        if(eWiFiSuccess != wifi_err)
        {
            APP_ERR_PRINT("\r\n** WIFI_Off API failed **\r\n");
            APP_ERR_TRAP(wifi_err);
        }
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup wifi_on_chip_udp_freertos_ep)
 **********************************************************************************************************************/
