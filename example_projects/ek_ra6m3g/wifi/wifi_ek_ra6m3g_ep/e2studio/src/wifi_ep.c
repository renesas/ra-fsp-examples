/***********************************************************************************************************************
 * File Name    : wifi_ep.c
 * Description  : Contains data structures and functions used in wifi_ep.c
 **********************************************************************************************************************/
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
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#include "common_utils.h"
#include "wifi_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup wifi_ep
 * @{
 **********************************************************************************************************************/

WiFiParameters_t g_wifi =
{
 .ssid = {'\0'},
 .pwd = {'\0'},
 .security = RESET_VALUE,
 .ip_addr_device = {RESET_VALUE},
 .status = RESET_VALUE
};

SocketPrameters_t g_socket =
{
 .id = RESET_VALUE,
 .ip_addr_server = RESET_VALUE,
 .port = RESET_VALUE,
 .recv_buff = {RESET_VALUE},
 .send_buff = {RESET_VALUE},
 .status = RESET_VALUE
};

static sx_ulpgn_security_t convertSecurity(WIFISecurity_t xSecurity);

/*******************************************************************************************************************//**
 * @brief       This functions takes input from user.
 * @param[IN]   char pointer pointing to user input buffer.
 * @retval      None.
 * @retval      None.
 **********************************************************************************************************************/
void user_input(char * user_buff)
{
    char rByte[BUFF_LEN];           // RTT buffer to read data
    uint32_t num_bytes;             // Number of bytes read by RTT

    /* Resetting user_buff */
    memset(user_buff, RESET_VALUE, BUFF_LEN);

    num_bytes = RESET_VALUE;
    while (BYTES_RECEIVED_ZERO == num_bytes)
    {
        if (APP_CHECK_DATA)
        {
            num_bytes = (uint32_t) APP_READ(rByte);
            if(BYTES_RECEIVED_ZERO == num_bytes)
            {
                APP_PRINT("\r\nNo Input\r\n");
            }
        }
    }

    switch(rByte[num_bytes - INDEX_ONE])
    {
        case '\r':
        {
            rByte[num_bytes - INDEX_ONE] = '\0';
            memcpy(user_buff, rByte, (size_t) num_bytes);
        }
        break;
        case '\n':
        {
            if('\r' == rByte[num_bytes - INDEX_TWO])
            {
                rByte[num_bytes - INDEX_TWO] = '\0';
                rByte[num_bytes - INDEX_ONE] = '\0';
                memcpy(user_buff, rByte, (size_t) (num_bytes - INDEX_ONE));
            }
            else
            {
                rByte[num_bytes - INDEX_ONE] = '\0';
                memcpy(user_buff, rByte, (size_t) num_bytes);
            }
        }
        break;
        default:
        {
            memcpy(user_buff, rByte, (size_t) num_bytes);
        }
        break;
    }
}

/*******************************************************************************************************************//**
 * @brief       This functions scans for available WiFi APs and prints them in a list.
 *              Gives user the option to select the AP from that list and enter the
 *              credentials for the same to connect.
 * @param[IN]   None.
 * @retval      FSP_SUCCESS                 Upon successful completion of the function.
 * @retval      Any Other Error code        If WiFi AP scan fails.
 **********************************************************************************************************************/
fsp_err_t scan_and_select(void)
{
    fsp_err_t err = FSP_SUCCESS;
    char input_buff[BUFF_LEN] = {RESET_VALUE};                              // Buffer for storing user input
    uint8_t index_wifi_ap_list = RESET_VALUE;                               // Numerical conversion of RTT input string
    WIFIScanResult_t scan_data[MAX_WIFI_SCAN_RESULTS] = {RESET_VALUE};      // Buffer for storing WiFi AP scan result

    do
    {
        memset(scan_data, RESET_VALUE, sizeof(scan_data));
        APP_PRINT("\r\nScanning WiFi Access Points\r\n");

        /* Scanning for WiFi APs */
        err = rm_wifi_onchip_silex_scan(scan_data, MAX_WIFI_SCAN_RESULTS);
        if(FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\n** rm_wifi_onchip_silex_scan API for rm_wifi_onchip_silex failed **\r\n");
            return err;
        }

        /* Printing SSID of WiFi APs */
        APP_PRINT("\r\n***** List of WiFi Access Points *****")
        for(uint8_t index = RESET_VALUE ; index < MAX_WIFI_SCAN_RESULTS; index++)
        {
            char temp = scan_data[index].cSSID[INDEX_ZERO];
            if('\0' != temp)
            {
                switch(scan_data[index].xSecurity)
                {
                    case eWiFiSecurityOpen:
                    {
                        APP_PRINT("\r\n(%d) %s: \r\n    Security: Open, Signal strength: %d, Channel: %d",
                                  index, scan_data[index].cSSID, scan_data[index].cRSSI, scan_data[index].cChannel);
                    }
                    break;
                    case eWiFiSecurityWEP:
                    {
                        APP_PRINT("\r\n(%d) %s: \r\n    Security: WEP, Signal strength: %d, Channel: %d",
                                  index, scan_data[index].cSSID, scan_data[index].cRSSI, scan_data[index].cChannel);
                        APP_PRINT("\r\nNote: Currently WiFi APs with WEP security is not supported. Please do not select.");
                    }
                    break;
                    case eWiFiSecurityWPA:
                    {
                        APP_PRINT("\r\n(%d) %s: \r\n    Security: WPA, Signal strength: %d, Channel: %d",
                                  index, scan_data[index].cSSID, scan_data[index].cRSSI, scan_data[index].cChannel);
                    }
                    break;
                    case eWiFiSecurityWPA2:
                    {
                        APP_PRINT("\r\n(%d) %s: \r\n    Security: WPA2, Signal strength: %d, Channel: %d",
                                  index, scan_data[index].cSSID, scan_data[index].cRSSI, scan_data[index].cChannel);
                    }
                    break;
                    case eWiFiSecurityWPA2_ent:
                        /* Do nothing */
                        break;
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
    }while(('0' > input_buff[INDEX_ZERO]) || ('9' < input_buff[INDEX_ZERO]) || (MAX_WIFI_SCAN_RESULTS <= index_wifi_ap_list));

    /* Storing SSID  and security type of WiFi AP */
    strcpy(g_wifi.ssid, scan_data[index_wifi_ap_list].cSSID);
    g_wifi.security = convertSecurity(scan_data[index_wifi_ap_list].xSecurity);

    /* WiFi AP password input for non-open security type */
    if(WIFI_ONCHIP_SILEX_SECURITY_OPEN != g_wifi.security)
    {
        /* User input for password of WiFi AP */
        APP_PRINT("\r\nEnter password for " RTT_CTRL_TEXT_BRIGHT_CYAN "%s." RTT_CTRL_RESET, g_wifi.ssid);
        user_input(g_wifi.pwd);
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief       This functions takes user input for WiFi AP's SSID, security type and
 *              credentials. And connect to it.
 * @param[IN]   None.
 * @retval      FSP_SUCCESS                 Upon successful completion of the function.
 * @retval      Any Other Error code        If WiFi AP scan fails.
 **********************************************************************************************************************/
fsp_err_t manual_connect(void)
{
    fsp_err_t err = FSP_SUCCESS;
    char input_buff[BUFF_LEN] = {RESET_VALUE};      // Buffer for storing user input

    /* User input for SSID of WiFi AP  */
    APP_PRINT("\r\nEnter SSID of WiFi Access Point:");
    user_input(g_wifi.ssid);

    /* User selecting security type of WiFi AP */
    do
    {
        APP_PRINT("\r\nSelect security type for the WiFi Access Point:");
        /* Menu for WiFi Security types */
        APP_PRINT("\r\nWiFi Security types:");
        APP_PRINT("\r\nPress 0 for Open - No Security");
        APP_PRINT("\r\nPress 1 for WPA Security");
        APP_PRINT("\r\nPress 2 for WPA2 Security");
        APP_PRINT("\r\nUser Input: \r\n");
        user_input(input_buff);

        g_wifi.security = (uint32_t) atoi(input_buff);

        /* Checking for the Open security type */
        if(('0' == input_buff[INDEX_ZERO]) && ('\0' == input_buff[INDEX_ONE]))
        {
            break;
        }
        /* Loop iteration to discard any other value apart from the valid security type */
    }while(WIFI_ONCHIP_SILEX_SECURITY_WPA2 < (int32_t)g_wifi.security);

    /* WiFi AP password input for non-open security type */
    if(WIFI_ONCHIP_SILEX_SECURITY_OPEN != g_wifi.security)
    {
        /* User input for password of WiFi AP */
        APP_PRINT("\r\nEnter password for " RTT_CTRL_TEXT_BRIGHT_CYAN "%s" RTT_CTRL_RESET, g_wifi.ssid);
        user_input(g_wifi.pwd);
    }

    /* Connecting to user entered SSID */
    APP_PRINT("\r\nConnecting to " RTT_CTRL_TEXT_BRIGHT_CYAN "%s \r\n" RTT_CTRL_RESET, g_wifi.ssid);
    err = rm_wifi_onchip_silex_connect(g_wifi.ssid, (uint32_t) g_wifi.security, g_wifi.pwd);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** rm_wifi_onchip_silex_connect API for rm_wifi_onchip_silex failed ** \r\n");
        return err;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief       This functions resolves the DNS query or validates the IP address.
 * @param[IN]   p_dns                       Pointer to array holding URL or IP address to query from DNS.
 * @param[out]  p_ip_addr                   Pointer to IP address returned from look up.
 * @retval      FSP_SUCCESS                 Upon successful DNS resolving.
 * @retval      Any Other Error code        If DNS query fails.
 **********************************************************************************************************************/
fsp_err_t dns_query(char * p_dns, uint8_t * p_ip_addr)
{
    fsp_err_t err = FSP_SUCCESS;

    rm_wifi_onchip_silex_socket_connected(&g_wifi.status);
    if(FSP_SUCCESS != g_wifi.status)
    {
        APP_PRINT("\r\nWiFi not connected");
        return WIFI_ONCHIP_SILEX_ERR_ERROR;
    }

    err = rm_wifi_onchip_silex_dns_query(p_dns, p_ip_addr);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** rm_wifi_onchip_silex_dns_query API for rm_wifi_onchip_silex failed **\r\n");
        return err;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief       This functions ping the IP address.
 * @param[in]   p_ip_addr                   Pointer to IP address array.
 * @retval      FSP_SUCCESS                 Upon successful ping.
 * @retval      Any Other Error code        If ping fails.
 **********************************************************************************************************************/
fsp_err_t ping(uint8_t * p_ip_addr)
{
    fsp_err_t err = FSP_SUCCESS;

    rm_wifi_onchip_silex_socket_connected(&g_wifi.status);
    if(FSP_SUCCESS != g_wifi.status)
    {
        APP_PRINT("\r\nWiFi not connected");
        return WIFI_ONCHIP_SILEX_ERR_ERROR;
    }

    err = rm_wifi_onchip_silex_ping(p_ip_addr, PING_COUNT, PINT_INTERVAL_MS);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** rm_wifi_onchip_silex_ping API for rm_wifi_onchip_silex failed **\r\n");

        return err;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief       This functions creates a new TCP socket in client mode.
 * @param[in]   None.
 * @retval      FSP_SUCCESS                 Upon successful TCP socket creation.
 * @retval      Any Other Error code        If TCP socket creation fails.
 **********************************************************************************************************************/
fsp_err_t tcp_socket_create()
{
    fsp_err_t err = FSP_SUCCESS;

    rm_wifi_onchip_silex_socket_connected(&g_wifi.status);
    if(FSP_SUCCESS != g_wifi.status)
    {
        APP_PRINT("\r\nWiFi not connected");
        return WIFI_ONCHIP_SILEX_ERR_ERROR;
    }

    err = rm_wifi_onchip_silex_socket_create(g_socket.id, SOCKET_TCP, IPV4);
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** rm_wifi_onchip_silex_socket_create API for rm_wifi_onchip_silex failed **\r\n");
        return err;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief       This functions connects to the TCP server IP address.
 * @param[in]   ip_addr                     IP address of the server for socket connection.
 * @param[in]   port                        Port number of the server for socket connection.
 * @retval      FSP_SUCCESS                 Upon successfully connecting to the server.
 * @retval      Any Other Error code        If connection fails.
 **********************************************************************************************************************/
fsp_err_t tcp_socket_connect(uint32_t ip_addr, uint32_t port)
{
    fsp_err_t err = FSP_SUCCESS;

    rm_wifi_onchip_silex_socket_connected(&g_wifi.status);
    if(FSP_SUCCESS != g_wifi.status)
    {
        APP_PRINT("\r\nWiFi not connected");
        return WIFI_ONCHIP_SILEX_ERR_ERROR;
    }

    for(uint8_t i = RESET_VALUE; i <= WIFI_MAX_TRY; i++)
    {
        err = rm_wifi_onchip_silex_tcp_connect(g_socket.id, SOCKETS_htonl(ip_addr), port);
        if(FSP_SUCCESS == err)
        {
            return err;
        }
    }
    APP_ERR_PRINT("\r\n** rm_wifi_onchip_silex_tcp_connect API for rm_wifi_onchip_silex failed **\r\n");
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This functions sends data to the TCP server.
 * @param[in]   p_data                      Pointer to data to send.
 * @retval      bytes_sent                  Number of bytes sent to server.
 * @retval      Any Other Error code        If send fails.
 **********************************************************************************************************************/
fsp_err_t tcp_send_data(uint8_t * p_data)
{
    int32_t bytes_sent = RESET_VALUE;

    if(FSP_SUCCESS != network_connectivity_check())
    {
        return WIFI_ONCHIP_SILEX_ERR_ERROR;
    }
    else
    {
        bytes_sent = rm_wifi_onchip_silex_tcp_send(g_socket.id, p_data, strlen((char *) p_data), WIFI_ONCHIP_SILEX_TIMEOUT_400MS);
        if(RESET_VALUE >= bytes_sent)
        {
            APP_ERR_PRINT("\r\n** rm_wifi_onchip_silex_tcp_send API for rm_wifi_onchip_silex failed **\r\n");
            return bytes_sent;
        }
    }

    return bytes_sent;
}

/*******************************************************************************************************************//**
 * @brief       This functions receives data from the TCP server.
 * @param[out]  p_data                      Pointer to data received from socket.
 * @retval      bytes_received              Number of bytes received from server.
 * @retval      0                           If timeout occurs.
 * @retval      Any Other Error code        If receive fails.
 **********************************************************************************************************************/
fsp_err_t tcp_recv_data(uint8_t * p_data)
{
    int32_t bytes_received = RESET_VALUE;

    if(FSP_SUCCESS != network_connectivity_check())
    {
        return WIFI_ONCHIP_SILEX_ERR_ERROR;
    }
    else
    {
        bytes_received = rm_wifi_onchip_silex_tcp_recv(g_socket.id, p_data, BUFF_LEN, WIFI_ONCHIP_SILEX_TIMEOUT_15SEC);
        if(RESET_VALUE > bytes_received)
        {
            APP_ERR_PRINT("\r\n** rm_wifi_onchip_silex_tcp_recv API for rm_wifi_onchip_silex failed **\r\n");
            return bytes_received;
        }
    }
    return bytes_received;
}

/*******************************************************************************************************************//**
 * @brief       This functions checks both WiFi connectivity and TCP socket connectivity.
 * @param[in]   None
 * @retval      FSP_SUCCESS                         If both the connectivity checks are success.
 * @retval      WIFI_ONCHIP_SILEX_ERR_ERROR         If one of the connectivity check fails.
 **********************************************************************************************************************/
fsp_err_t network_connectivity_check(void)
{
    rm_wifi_onchip_silex_socket_connected(&g_wifi.status);
    if(FSP_SUCCESS != g_wifi.status)
    {
        APP_ERR_PRINT("\r\nWiFi not connected");
        return WIFI_ONCHIP_SILEX_ERR_ERROR;
    }

    rm_wifi_onchip_silex_socket_status_get(g_socket.id, &g_socket.status);
    if(WIFI_ONCHIP_SILEX_SOCKET_STATUS_CONNECTED != g_socket.status)
    {
        APP_ERR_PRINT("\r\nTCP socket disconnected");
        return WIFI_ONCHIP_SILEX_ERR_ERROR;
    }

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief       This functions disconnects the socket connection and closes the WiFi module.
 * @param[in]   None
 * @retval      None
 **********************************************************************************************************************/
void wifi_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Checking socket connection status */
    rm_wifi_onchip_silex_socket_status_get(g_socket.id, &g_socket.status);
    if(WIFI_ONCHIP_SILEX_SOCKET_STATUS_CONNECTED == g_socket.status)
    {
        /* If socket connected, then disconnecting */
        err = rm_wifi_onchip_silex_disconnect(g_socket.id);
        /* Handle error */
        if(FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\n** rm_wifi_onchip_silex_disconnect API for rm_wifi_onchip_silex failed **\r\n");
        }
    }

    /* Checking WiFi connectivity */
    rm_wifi_onchip_silex_socket_connected(&g_wifi.status);
    if(FSP_SUCCESS == g_wifi.status)
    {
        /* If WiFi connected, closing WiFi module. Thus disconnecting from connected AP */
        err = rm_wifi_onchip_silex_close();
        /* Handle error */
        if(FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("\r\n** rm_wifi_onchip_silex_close API for rm_wifi_onchip_silex failed **\r\n");
            APP_ERR_TRAP(err);
        }
    }
}

/*******************************************************************************************************************//**
 * @brief       This functions converts the security type used by WIFIScanResult_t to SilexAT supported security type.
 * @param[in]   xSecurity                   Security type used by WIFIScanResult_t structure.
 * @retval      sx_ulpgn_security_t         Security type supported by SilexAT.
 **********************************************************************************************************************/
static sx_ulpgn_security_t convertSecurity(WIFISecurity_t xSecurity)
{
    if(eWiFiSecurityOpen == xSecurity)
    {
        return WIFI_ONCHIP_SILEX_SECURITY_OPEN;
    }
    else if(eWiFiSecurityWEP == xSecurity)
    {
        return WIFI_ONCHIP_SILEX_SECURITY_WEP;
    }
    else if (eWiFiSecurityWPA == xSecurity)
    {
        return WIFI_ONCHIP_SILEX_SECURITY_WPA;
    }
    else if (eWiFiSecurityWPA2 == xSecurity)
    {
        return WIFI_ONCHIP_SILEX_SECURITY_WPA2;
    }
    else
    {
        return WIFI_ONCHIP_SILEX_SECURITY_UNDEFINED;
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup wifi_ep)
 **********************************************************************************************************************/
