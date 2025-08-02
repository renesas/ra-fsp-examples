/***********************************************************************************************************************
 * File Name    : wifi_ep.c
 * Description  : Contains functions related to managing WiFi functionality.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "wifi.h"

/***********************************************************************************************************************
 * Static Globals Variables
 **********************************************************************************************************************/
static WIFINetworkParams_t g_net_params;

/***********************************************************************************************************************
 * Local function prototypes
 **********************************************************************************************************************/
static WIFIReturnCode_t scan_wifi(void);
static fsp_err_t is_network_connected(void);
static WIFIReturnCode_t network_info(void);
static void manual_connect(void);
static fsp_err_t wifi_deinit(void);

/*******************************************************************************************************************//**
 * @brief       This function scans for available WiFi APs and prints them in a list.
 *              Gives user the option to select the AP from that list to connect.
 * @param[IN]   None.
 * @retval      eWiFiSuccess                Upon successful completion of the function.
 * @retval      Any Other Error code        If WiFi AP scan fails.
 **********************************************************************************************************************/
static WIFIReturnCode_t scan_wifi(void)
{
    WIFIScanResult_t scan_data[MAX_WIFI_SCAN_RESULTS] = {
                                                         {.ucSSIDLength = RESET_VALUE}, {.ucSSID = {RESET_VALUE}},\
                                                         {.ucBSSID = {RESET_VALUE}}, {.xSecurity = RESET_VALUE},\
                                                         {.cRSSI = RESET_VALUE}, {.ucChannel = RESET_VALUE}
                                                        };
    WIFIReturnCode_t wifi_err = eWiFiSuccess;
    char input_buff[BUFFER_SIZE_DOWN] = {RESET_VALUE};
    uint8_t index_wifi_ap_list = RESET_VALUE;

    do
    {
        /* Reset the scan_data buffer */
        memset(scan_data, RESET_VALUE, sizeof(scan_data));
        APP_PRINT("\r\nScanning WiFi Access Points\r\n");

        /* Perform a Wi-Fi network Scan */
        wifi_err = WIFI_Scan(&scan_data[0], MAX_WIFI_SCAN_RESULTS);
        APP_ERR_RETURN(wifi_err, "\r\n**Wi-Fi networks was unsuccessful**");

        /* Printing SSID of WiFi APs */
        APP_PRINT("\r\n------------List of WiFi Access Points-----------");
        for(uint8_t index = RESET_VALUE; index < MAX_WIFI_SCAN_RESULTS; index++)
        {
            uint8_t temp = scan_data[index].ucSSID[INDEX_ZERO];
            if(NULL_CHARACTER != temp && TAB_CHARACTER != temp)
            {
                switch(scan_data[index].xSecurity)
                {
                    case eWiFiSecurityOpen:
                    {
                        APP_PRINT("\r\n(%d) %s: \r\n    Security: Open, Signal strength: %d", index,\
                                  scan_data[index].ucSSID, scan_data[index].cRSSI);
                    }
                    break;
                    case eWiFiSecurityWPA:
                    {
                        APP_PRINT("\r\n(%d) %s: \r\n    Security: WPA, Signal strength: %d", index,\
                                  scan_data[index].ucSSID, scan_data[index].cRSSI);
                    }
                    break;
                    case eWiFiSecurityWPA2:
                    {
                        APP_PRINT("\r\n(%d) %s: \r\n    Security: WPA2, Signal strength: %d", index,\
                                  scan_data[index].ucSSID, scan_data[index].cRSSI);
                    }
                    break;
                    case eWiFiSecurityWEP:
                        /* Do nothing */
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
                    default:
                        /* Do nothing */;
                }
            }
        }

        /* The user selecting SSID from WiFi APs list */
        APP_PRINT("\r\n\r\nEnter WiFi AP's SSID index to be connected.");
        APP_PRINT("\r\nOR Enter any other character to Re-scan\r\n");

        /* Take input from the user */
        user_input(input_buff);

        /* Convert a string to an integer */
        index_wifi_ap_list = (uint8_t)atoi(input_buff);
    }
    while((ZERO_ASCII > input_buff[INDEX_ZERO]) || (NINE_ASCII < input_buff[INDEX_ZERO])\
           || (MAX_WIFI_SCAN_RESULTS <= index_wifi_ap_list));

    /* Store SSID and security type of WiFi AP */
    strcpy((char*) g_net_params.ucSSID, (char*) scan_data[index_wifi_ap_list].ucSSID);
    g_net_params.xSecurity = scan_data[index_wifi_ap_list].xSecurity;

    return wifi_err;
}

/*******************************************************************************************************************//**
 * @brief       This function configures the WiFi connection. Displays a menu with options to either scan
 *              for WiFi access points or manually enter credentials and connect to the Access Point.
 *              Finally print outs the obtained IP configuration.
 * @param[IN]   None.
 * @retval      eWiFiSuccess                Upon successful completion of the function.
 * @retval      Any Other Error code        If WiFi AP scan fails.
 **********************************************************************************************************************/
WIFIReturnCode_t wifi_config(void)
{
    WIFIReturnCode_t wifi_err = eWiFiSuccess;
    uint8_t index_menu_option = RESET_VALUE;
    char input_buff[BUFFER_SIZE_DOWN] ={ RESET_VALUE };
    uint8_t wifi_length = RESET_VALUE;

    do
    {
        /* Menu for user selection */
        APP_PRINT(PRINT_WIFI_MENU);
        user_input(input_buff);
        index_menu_option = (uint8_t)atoi((char*) input_buff);

        switch(index_menu_option)
        {
            case SCAN_AP:
                /* Scan WiFi APs for user to choose one */
                wifi_err = scan_wifi();
                APP_ERR_RETURN(wifi_err, "\r\n**WiFi Access Point scanning failed. Retry.**");
                break;
            case ENTER_SSID:
                /* User manually enters WiFi AP credentials to connect */
                manual_connect();
                break;
            default:
                APP_PRINT("\r\nInvalid Input.\r\n");
        }

        /* Get the length of the SSID */
        wifi_length = (uint8_t)strlen((char*) g_net_params.ucSSID);
        if(RESET_VALUE != wifi_length)
        {
            /* Set the length of the SSID */
            g_net_params.ucSSIDLength = wifi_length;
            /* WiFi AP password input for non-open security type */
            if(eWiFiSecurityOpen != g_net_params.xSecurity)
            {
                /* User input for password of WiFi AP */
                APP_PRINT("\r\nEnter password for " RTT_CTRL_TEXT_BRIGHT_CYAN "%s:" RTT_CTRL_RESET,\
                          g_net_params.ucSSID);
                user_input(g_net_params.xPassword.xWPA.cPassphrase);
                g_net_params.xPassword.xWPA.ucLength = sizeof(g_net_params.xPassword.xWPA.cPassphrase);
            }

            /* Connecting to user selected WiFi AP */
            APP_PRINT("\r\nConnecting to " RTT_CTRL_TEXT_BRIGHT_CYAN "%s \r\n" RTT_CTRL_RESET, g_net_params.ucSSID);

            /* Connect to the Access Point */
            wifi_err = WIFI_ConnectAP(&g_net_params);
            APP_ERR_RETURN(wifi_err, "\r\n**WIFI AP connection failed.**");
        }
        /* Status check of WiFi connectivity */
    }
    while((RESET_VALUE == wifi_length) || (FSP_SUCCESS != is_network_connected()));

    /* Get IP configuration */
    wifi_err = network_info();
    APP_ERR_RETURN(wifi_err, "\r\n**network_info failed.**");

    return wifi_err;
}

/*******************************************************************************************************************//**
 * @brief       This function takes input from the user.
 * @param[IN]   user_buff: char pointer pointing to user input buffer.
 * @retval      None.
 * @retval      None.
 **********************************************************************************************************************/
void user_input(char *user_buff)
{
    /* RTT buffer to read data */
    char rByte[BUFFER_SIZE_DOWN];
    /* Number of bytes read by RTT */
    uint32_t num_bytes = BYTES_RECEIVED_ZERO;

    /* Reset user_buff */
    memset(user_buff, RESET_VALUE, BUFFER_SIZE_DOWN);

    num_bytes = RESET_VALUE;
    while(BYTES_RECEIVED_ZERO == num_bytes)
    {
        /* Check if there is data available to read */
        if(APP_CHECK_DATA)
        {
            num_bytes = (uint32_t)APP_READ(rByte);
            /* If no bytes are received, print message */
            if(BYTES_RECEIVED_ZERO == num_bytes)
            {
                APP_PRINT("\r\nNo Input\r\n");
            }
        }
    }

    /* Check the last character of received data */
    switch(rByte[num_bytes - INDEX_ONE])
    {
        case CARRIAGE_RETURN_CHARACTER:
        {
            /* Replace carriage return with null terminator */
            rByte[num_bytes - INDEX_ONE] = NULL_CHARACTER;
            memcpy(user_buff, rByte, (size_t)num_bytes);
        }
        break;
        case NEWLINE_CHARACTER:
        {
            if (num_bytes > INDEX_ONE && CARRIAGE_RETURN_CHARACTER == rByte[num_bytes - INDEX_TWO])
            {
                /* Replace both carriage return and newline with null terminators */
                rByte[num_bytes - INDEX_TWO] = NULL_CHARACTER;
                rByte[num_bytes - INDEX_ONE] = NULL_CHARACTER;
                memcpy(user_buff, rByte, (size_t) (num_bytes - INDEX_ONE));
            }
            else
            {
                /* Replace newline with null terminator */
                rByte[num_bytes - INDEX_ONE] = NULL_CHARACTER;
                memcpy(user_buff, rByte, (size_t) num_bytes);
            }
        }
        break;
        default:
        {
            memcpy(user_buff, rByte, (size_t) num_bytes);
        }
    }
}

/*******************************************************************************************************************//**
 * @brief       This function takes user input for WiFi AP's SSID, and security type.
 * @param[IN]   None.
 * @retval      None.
 **********************************************************************************************************************/
static void manual_connect(void)
{
    char input_buff[BUFF_LEN] =
    { RESET_VALUE };      /* Buffer for storing user input */

    /* User input for SSID of WiFi AP */
    APP_PRINT("\r\nEnter SSID of WiFi Access Point:");
    user_input((char*) g_net_params.ucSSID);

    /* The user selecting security type of WiFi AP */
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

        /* Loop iteration to discard any other value apart from the valid security type */
    }
    while((ZERO_ASCII > input_buff[INDEX_ZERO]) || (TWO_ASCII < input_buff[INDEX_ZERO])\
           || (NULL_CHARACTER != input_buff[INDEX_ONE]));

    /* Checking for the Open security type */
    if(WIFI_SECURITY_OPEN == (uint32_t)atoi(input_buff))
    {
        g_net_params.xSecurity = eWiFiSecurityOpen;
    }
    else
    {
        /* WiFi AP password input for non-open security type */
        g_net_params.xSecurity =
                ((uint32_t)atoi(input_buff) == WIFI_SECURITY_WPA ? eWiFiSecurityWPA : eWiFiSecurityWPA2);
    }
}

/***********************************************************************************************************************
 * @brief       This function checks both WiFi connectivity and TCP socket connectivity.
 * @param[in]   None
 * @retval      FSP_SUCCESS             If both the connectivity checks are success.
 * @retval      Any other error         If one of the connectivity check fails.
 **********************************************************************************************************************/
static fsp_err_t is_network_connected(void)
{
    fsp_err_t result = FSP_SUCCESS;

    if(eWiFiSuccess != WIFI_IsConnected((const WIFINetworkParams_t*) &g_net_params))
    {
        APP_ERR_PRINT("** Wi-Fi is not connected **\r\n");
        result = FSP_ERR_INTERNAL;
    }

    return result;
}

/***********************************************************************************************************************
 * @brief       This function gets the IP configuration and print out the obtained IP configuration.
 * @param[in]   None
 * @retval      eWiFiSuccess            If successful and IP Address buffer has the interface's IP address.
 * @retval      Any other error         Failure code.
 **********************************************************************************************************************/
static WIFIReturnCode_t network_info(void)
{
    WIFIReturnCode_t wifi_err = eWiFiSuccess;
    WIFIIPConfiguration_t ipconfg;

    /* Get IP configuration (IP address, NetworkMask, Gateway */
    memset(&ipconfg, RESET_VALUE, sizeof(WIFIIPConfiguration_t));
    wifi_err = WIFI_GetIPInfo(&ipconfg);
    APP_ERR_RETURN(wifi_err, "\r\n**Device IP information get failed.**");

    /* Only eWiFiIPAddressTypeV4 is currently supported */
    if(ipconfg.xIPAddress.xType == eWiFiIPAddressTypeV4)
    {
        APP_PRINT("\r\nInfo: Device IP address: %d.%d.%d.%d\r\n", (uint8_t)ipconfg.xIPAddress.ulAddress[INDEX_ZERO],\
                  (uint8_t)(ipconfg.xIPAddress.ulAddress[INDEX_ZERO] >> 8 & 0xFF),\
                  (uint8_t)(ipconfg.xIPAddress.ulAddress[INDEX_ZERO] >> 16 & 0xFF),\
                  (uint8_t)(ipconfg.xIPAddress.ulAddress[INDEX_ZERO] >> 24 & 0xFF));
        APP_PRINT("Info: Device network mask: %d.%d.%d.%d\r\n", (uint8_t)ipconfg.xNetMask.ulAddress[INDEX_ZERO],\
                  (uint8_t)(ipconfg.xNetMask.ulAddress[INDEX_ZERO] >> 8 & 0xFF),\
                  (uint8_t)(ipconfg.xNetMask.ulAddress[INDEX_ZERO] >> 16 & 0xFF),\
                  (uint8_t)(ipconfg.xNetMask.ulAddress[INDEX_ZERO] >> 24 & 0xFF));
        APP_PRINT("Info: Device gateway address: %d.%d.%d.%d\r\n", (uint8_t)ipconfg.xGateway.ulAddress[INDEX_ZERO],\
                  (uint8_t)(ipconfg.xGateway.ulAddress[INDEX_ZERO] >> 8 & 0xFF),\
                  (uint8_t)(ipconfg.xGateway.ulAddress[INDEX_ZERO] >> 16 & 0xFF),\
                  (uint8_t)(ipconfg.xGateway.ulAddress[INDEX_ZERO] >> 24 & 0xFF));
    }

    return wifi_err;
}

/*******************************************************************************************************************//**
 * @brief       This function disconnects the socket connection and closes the WiFi module.
 * @param[in]   None
 * @retval      fsp_err_t: FSP_SUCCESS, FSP_ERR_WIFI_FAILED.
 **********************************************************************************************************************/
fsp_err_t wifi_deinit(void)
{

    /* Check socket connection status */
    if(FSP_SUCCESS == is_network_connected())
    {
        /* If socket is connected, then disconnect */
        if(eWiFiSuccess != WIFI_Disconnect())
        {
            APP_ERR_PRINT("** WIFI_Disconnect API failed **\r\n");
            return FSP_ERR_WIFI_FAILED;
        }
    }

    /* Close WiFi module */
    if(eWiFiSuccess != WIFI_Off())
    {
        APP_PRINT("** WIFI_Off API failed **\r\n");
        return FSP_ERR_WIFI_FAILED;
    }

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 *  @brief       This function handles error if error occurred, closes all opened modules, and prints and traps errors.
 *  @param[in]   err        Error value.
 *  @param[in]   err_str    Error string.
 *  @retval      None
 **********************************************************************************************************************/
void wifi_handle_error(WIFIReturnCode_t err,  const char * err_str)
{
    if(eWiFiSuccess != err)
    {
        /* Print the error */
        APP_PRINT(err_str);

        /* De-initializes wi-fi module */
        if(FSP_SUCCESS != wifi_deinit())
        {
            APP_ERR_PRINT("** De-initializes the Wi-Fi module failed **\r\n");
        }

        /* Trap the error */
        APP_ERR_TRAP(err);
    }
}
