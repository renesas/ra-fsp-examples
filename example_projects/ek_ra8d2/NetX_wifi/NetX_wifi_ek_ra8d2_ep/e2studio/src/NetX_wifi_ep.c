/***********************************************************************************************************************
 * File Name    : NetX_wifi_ep.c
 * Description  : Contains data structures and functions used in the EP
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "NetX_wifi_ep.h"
#include "common_utils.h"

/*******************************************************************************************************************//**
 * @addtogroup NetX_wifi_ep
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

SocketParameters_t g_socket =
{
 .id = RESET_VALUE,
 .ip_addr_server = RESET_VALUE,
 .port = RESET_VALUE,
 .recv_buff = {RESET_VALUE},
 .send_buff = {RESET_VALUE},
 .status = RESET_VALUE
};

/*******************************************************************************************************************//**
 * @brief       This function takes input from the user.
 * @param[in]   user_buff   Pointer pointing to user input buffer.
 * @retval      FSP_SUCCESS
 **********************************************************************************************************************/
fsp_err_t get_user_input(char * user_buff)
{
    fsp_err_t err = FSP_SUCCESS;
    do{
        err = check_for_RTT_user_IP(user_buff);
    } while (err != TX_SUCCESS);

    /* Remove new line character from input password string */
    while (*user_buff)
    {
        if ((* user_buff == '\r') || (* user_buff == '\n'))
        {
            *user_buff = NULL_CHAR;
        }
        user_buff++;
    }
  return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief       This function scans for available WiFi APs and prints them in a list.
 *              Gives the user the option to select the AP from that list and enter the
 *              credentials for the same to connect.
 * @param[in]   None.
 * @retval      FSP_SUCCESS                 Upon successful completion of the function.
 * @retval      Any other error code        If WiFi AP scan fails.
 **********************************************************************************************************************/
fsp_err_t scan_and_select(void)
{
    fsp_err_t err = FSP_SUCCESS;
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
        PRINT_INFO_STR("\r\nScanning WiFi Access Points\r\n");

        /* Scanning for WiFi APs */
        err = rm_wifi_onchip_silex_scan(scan_data, MAX_WIFI_SCAN_RESULTS);
        if (FSP_SUCCESS != err)
        {
            PRINT_ERR_STR("** rm_wifi_onchip_silex_scan API for rm_wifi_onchip_silex failed **");
            return err;
        }

        /* Printing SSID of WiFi APs */
        PRINT_STR("***** List of WiFi Access Points *****");
        for (uint8_t index = RESET_VALUE ; index < MAX_WIFI_SCAN_RESULTS; index++)
        {
            uint8_t temp = scan_data[index].ucSSID[INDEX_ZERO];
            if (NULL_CHAR != temp)
            {
                switch (scan_data[index].xSecurity)
                {
                    case eWiFiSecurityOpen:
                    {
                        app_rtt_print_data(RTT_OUTPUT_PRINT_INDEX, sizeof(index), &index);
                        app_rtt_print_data(RTT_OUTPUT_MESSAGE_SECURITY, sizeof(scan_data[index]), &scan_data[index]);
                        PRINT_STR("Security: Open");
                    }
                    break;
                    case eWiFiSecurityWEP:
                    {
                        app_rtt_print_data(RTT_OUTPUT_PRINT_INDEX, sizeof(index), &index);
                        app_rtt_print_data(RTT_OUTPUT_MESSAGE_SECURITY, sizeof(scan_data[index]), &scan_data[index]);
                        PRINT_STR("Security: WEP");
                        PRINT_INFO_STR("Currently WiFi APs with WEP security is not supported. Please do not select");
                    }
                    break;
                    case eWiFiSecurityWPA:
                    {
                        app_rtt_print_data(RTT_OUTPUT_PRINT_INDEX, sizeof(index), &index);
                        app_rtt_print_data(RTT_OUTPUT_MESSAGE_SECURITY, sizeof(scan_data[index]), &scan_data[index]);
                        PRINT_STR("Security: WPA");
                    }
                    break;
                    case eWiFiSecurityWPA2:
                    {
                        app_rtt_print_data(RTT_OUTPUT_PRINT_INDEX, sizeof(index), &index);
                        app_rtt_print_data(RTT_OUTPUT_MESSAGE_SECURITY, sizeof(scan_data[index]), &scan_data[index]);
                        PRINT_STR("Security: WPA2");
                    }
                    break;
                    case eWiFiSecurityWPA2_ent:
                        /* Do nothing */
                        break;
                    case eWiFiSecurityWPA3:
                        /* Do nothing */
                        break;
                    case eWiFiSecurityNotSupported:
                        /* Do nothing */
                        break;
                }
            }
        }

        /* The user selecting SSID from WiFi APs list */
        PRINT_STR("Enter WiFi AP's SSID index to be connected.");
        PRINT_STR("OR Enter any other character to Re-scan");
        get_user_input(input_buff);
        index_wifi_ap_list = (uint8_t) atoi(input_buff);
    } while (('0' > input_buff[INDEX_ZERO]) || ('9' < input_buff[INDEX_ZERO]) || \
            (MAX_WIFI_SCAN_RESULTS <= index_wifi_ap_list));

    /* Storing SSID and security type of WiFi AP */
    memcpy(g_wifi.ssid, (char*) scan_data[index_wifi_ap_list].ucSSID, sizeof(g_wifi.ssid));
    g_wifi.security = scan_data[index_wifi_ap_list].xSecurity;

    /* WiFi AP password input for non-open security type */
    if (eWiFiSecurityOpen != g_wifi.security)
    {
        /* User input for password of WiFi AP */
        PRINT_STR("Enter password for");
        PRINT_STR(g_wifi.ssid);
        get_user_input(input_buff);
        memcpy(g_wifi.pwd, input_buff, sizeof(g_wifi.pwd));
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function takes user input for WiFi AP's SSID, security type and credentials. And connect to it.
 * @param[in]   None.
 * @retval      FSP_SUCCESS                 Upon successful completion of the function.
 * @retval      Any other error code        If WiFi AP scan fails.
 **********************************************************************************************************************/
fsp_err_t manual_connect(void)
{
    fsp_err_t err = FSP_SUCCESS;
    char input_buff[BUFF_LEN] = {RESET_VALUE};      /* Buffer for storing user input */

    /* User input for SSID of WiFi AP */
    PRINT_STR("Enter SSID of WiFi Access Point:");
    memset(input_buff, NULL_CHAR, sizeof(input_buff));
    err = get_user_input(input_buff);

    /* Copy user input to WiFi structures member */
    memcpy(g_wifi.ssid, input_buff, sizeof(g_wifi.ssid));

    /* The user selecting security type of WiFi AP */
    do
    {
        app_rtt_print_data(RTT_OUTPUT_PRINT_SECURITY_MENU, RESET_VALUE, NULL);
        get_user_input(input_buff);
        /* Loop iteration to discard any other value apart from the valid security type */
    } while(('0' > input_buff[INDEX_ZERO]) || ('2' < input_buff[INDEX_ZERO]));

    /* Checking for the Open security type */
    if (WIFI_SECURITY_OPEN == (uint32_t)atoi(input_buff))
    {
        g_wifi.security = eWiFiSecurityOpen;
    }
    else
    {
        g_wifi.security = ((uint32_t)atoi(input_buff) == WIFI_SECURITY_WPA ? eWiFiSecurityWPA : eWiFiSecurityWPA2);
        /* WiFi AP password input for non-open security type */
        PRINT_STR("Enter password for");
        PRINT_STR(g_wifi.ssid);
        memset(input_buff, NULL_CHAR, sizeof(input_buff));
        err = get_user_input(input_buff);
        if (FSP_SUCCESS != err)
        {
            PRINT_ERR_STR("Failed to receive User Input");
            return err;
        }
        memcpy(g_wifi.pwd, input_buff, sizeof(g_wifi.pwd));
    }

    /* Connecting to the user entered SSID */
    PRINT_STR("Connecting to ");
    PRINT_STR(g_wifi.ssid);
    err = rm_wifi_onchip_silex_connect(g_wifi.ssid, g_wifi.security, g_wifi.pwd);
    if (FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("** rm_wifi_onchip_silex_connect API for rm_wifi_onchip_silex failed **");
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function resolves the DNS query or validates the IP address.
 * @param[in]   p_dns                       Pointer to array holding URL or IP address to query from DNS.
 * @param[out]  p_ip_addr                   Pointer to IP address returned from look up.
 * @retval      FSP_SUCCESS                 Upon successful DNS resolving.
 * @retval      Any other error code        If DNS query fails.
 **********************************************************************************************************************/
fsp_err_t dns_query(char * p_dns, uint8_t * p_ip_addr)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Checking the status of WiFi Connection */
    err = rm_wifi_onchip_silex_socket_connected(&g_wifi.status);
    if (FSP_SUCCESS != g_wifi.status && FSP_SUCCESS!=err)
    {
        PRINT_ERR_STR("WiFi not connected");
        return WIFI_ONCHIP_SILEX_ERR_ERROR;
    }
    /* Initiate a DNS lookup for a given URL */
    err = rm_wifi_onchip_silex_dns_query(p_dns, p_ip_addr);
    if (FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("** rm_wifi_onchip_silex_dns_query API for rm_wifi_onchip_silex failed **");

    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function pings the IP address.
 * @param[in]   p_ip_addr                   Pointer to IP address array.
 * @retval      FSP_SUCCESS                 Upon successful ping.
 * @retval      Any other error code        If ping fails.
 **********************************************************************************************************************/
fsp_err_t ping(uint8_t * p_ip_addr)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Checking status of Wifi Connection */
    rm_wifi_onchip_silex_socket_connected(&g_wifi.status);
    if (FSP_SUCCESS != g_wifi.status)
    {
        PRINT_ERR_STR("WiFi not connected");
        return WIFI_ONCHIP_SILEX_ERR_ERROR;
    }
    /* Ping an IP address on the network */
    err = rm_wifi_onchip_silex_ping(p_ip_addr, PING_COUNT, PINT_INTERVAL_MS);
    if (FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("** rm_wifi_onchip_silex_ping API failed **");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function disconnects the socket connection and closes the WiFi module.
 * @param[in]   None
 * @retval      None
 **********************************************************************************************************************/
void wifi_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Checking socket connection status */
    rm_wifi_onchip_silex_socket_status_get(g_socket.id, &g_socket.status);
    if (WIFI_ONCHIP_SILEX_SOCKET_STATUS_CONNECTED == g_socket.status)
    {
        /* If socket connected, then disconnecting */
        err = rm_wifi_onchip_silex_disconnect();
        if (FSP_SUCCESS != err)
        {
            PRINT_ERR_STR("** rm_wifi_onchip_silex_disconnect API for rm_wifi_onchip_silex failed **");
        }
    }

    /* Checking WiFi connectivity */
    err = rm_wifi_onchip_silex_socket_connected(&g_wifi.status);
    if (FSP_SUCCESS == g_wifi.status)
    {
        /* If WiFi connected, closing WiFi module. Thus disconnecting from connected AP */
        err = rm_wifi_onchip_silex_close();
        if (FSP_SUCCESS != err)
        {
            PRINT_ERR_STR("** rm_wifi_onchip_silex_close API for rm_wifi_onchip_silex failed **");
            ERROR_TRAP(err);
        }
    }
    else
    {
        PRINT_ERR_STR("** rm_wifi_onchip_silex_close API for rm_wifi_onchip_silex failed **");
        ERROR_TRAP(err);
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup NetX_wifi_ep)
 **********************************************************************************************************************/
