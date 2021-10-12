/***********************************************************************************************************************
     * File Name    : wifi_thread_entry.c
 * Description  : Contains data structures and functions used in wifi_thread_entry.c
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

extern WiFiParameters_t g_wifi;
extern SocketPrameters_t g_socket;

static void xstrlwr(uint8_t * p_str);

/* WiFi Thread entry function */
/* pvParameters contains TaskHandle_t */
void wifi_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);

    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};

    char input_buff[BUFF_LEN] = {RESET_VALUE};          // Buffer for storing user input
    uint8_t index_menu_option = RESET_VALUE;           // Index to store menu option selected

    uint8_t ip_addr[IP_BUFF_LEN] = {RESET_VALUE};       // IP address for ping operation
    int8_t dns_retry_count = WIFI_MAX_TRY;              // Max retry count for DNS lookup
    int8_t ping_retry_count = WIFI_MAX_TRY;             // Max retry count for ping operation

    /* version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Example Project information printed on the Console */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.major, version.minor, version.patch);
    APP_PRINT(EP_INFO);

    /* Opening WiFi module */
    err = rm_wifi_onchip_silex_open(&g_wifi_onchip_silex_cfg);
    /* Handle error */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** rm_wifi_onchip_silex_open API for rm_wifi_onchip_silex failed **\r\n");
        APP_ERR_TRAP(err);
    }

    /* Reseting WiFi status flag */
    g_wifi.status = INT32_MAX;

    /* Connecting to WiFi AP */
    do
    {
        /* Menu for User Selection */
        APP_PRINT("\r\nMenu:");
        APP_PRINT("\r\nPress 1 to scan for WiFi Access Points");
        APP_PRINT("\r\nPress 2 to enter WiFi Access Point's credentials");
        APP_PRINT("\r\nUser Input: \r\n");
        user_input(input_buff);
        index_menu_option = (uint8_t)atoi((char *) input_buff);

        switch(index_menu_option)
        {
            case SCAN_AP:
#if defined (BOARD_RA2E2_EK)
                APP_PRINT("\r\nWiFi Access Point scanning feature is unsupported, refer readme.txt for additional details.");
                APP_PRINT("\r\nPlease press 2 for entering WiFi Access Point's credentials manually.\r\n");
#else
                /* Scan for APs and connect to user chosen one */
                err = scan_and_select();
                /* Handle error */
                if(FSP_SUCCESS != err)
                {
                    APP_PRINT("\r\nWiFi Access Point scanning failed. Retry.");
                    APP_PRINT("\r\nIf multiple failures, please enter WiFi AP credentials.\r\n");
                }
                else
                {
                    /* Connecting to user selected WiFi AP */
                    APP_PRINT("\r\nConnecting to %s", g_wifi.ssid);
                    err = rm_wifi_onchip_silex_connect(g_wifi.ssid, g_wifi.security, g_wifi.pwd);
                    /* Handle error */
                    if(FSP_SUCCESS != err)
                    {
                        APP_ERR_PRINT("\r\n** rm_wifi_onchip_silex_connect API for rm_wifi_onchip_silex failed ** \r\n");
                        APP_ERR_PRINT("\r\nIncorrect Access Point credentials entered. Try again.\r\n");
                    }
                }
#endif
                break;
            case ENTER_SSID:
                /* User manually enter WiFi AP credentials to connect */
                err = manual_connect();
                /* Handle error */
                if(FSP_SUCCESS != err)
                {
                    APP_ERR_PRINT("\r\nIncorrect Access Point credentials entered. Try again.\r\n");
                }
                break;
            default:
                APP_PRINT("\r\nInvalid Input.\r\n");
                break;
        }

        /* Status check of WiFi connectivity */
        rm_wifi_onchip_silex_socket_connected(&g_wifi.status);
        if(FSP_SUCCESS != g_wifi.status)
        {
            APP_PRINT("\r\nWiFi not connected.");
        }
    }while(FSP_SUCCESS != g_wifi.status);

    APP_PRINT("\r\nWiFi connected.\r\n");

    /* Get and print assigned IP address */
    err = rm_wifi_onchip_silex_ip_addr_get(g_wifi.ip_addr_device);
    /* Handle error */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** rm_wifi_onchip_silex_ip_addr_get API for rm_wifi_onchip_silex failed **\r\n");
        wifi_deinit();
        APP_ERR_TRAP(err);
    }
    APP_PRINT("\r\nMCU IP address: %d.%d.%d.%d\r\n", g_wifi.ip_addr_device[INDEX_ZERO], g_wifi.ip_addr_device[INDEX_ONE],
            g_wifi.ip_addr_device[INDEX_TWO], g_wifi.ip_addr_device[INDEX_THREE]);

    /* Ping demonstration */
    do
    {
        /* User input for IP address or URL to ping */
        APP_PRINT("\r\nEnter the IP or URL to ping:\r\n");
        user_input(input_buff);
        APP_PRINT("\r\nPinging. Please wait...\r\n");
        /* DNS lookup for URL or IP */
        err = dns_query(input_buff, ip_addr);
        /* Handle error */
        if(FSP_SUCCESS != err)
        {
            APP_PRINT("\r\nDNS lookup failed. Check IP/URL entered.\r\n");
            if(RESET_VALUE >= (dns_retry_count--))
            {
                APP_ERR_PRINT("\r\nFailed maximum retry count for DNS lookup. Reset MCU.\r\n");
                wifi_deinit();
                APP_ERR_TRAP(err);
            }
        }
        /* Only if DNS lookup is successful, ping would be performed. */
        else
        {
            /* Ping URL or IP address */
            err = ping(ip_addr);
            /* Handle error */
            if(FSP_SUCCESS != err)
            {
                APP_PRINT("\r\nPing Unsuccessful.\r\n");
                /* If ping fails maximum times, then EP fails */
                if(RESET_VALUE >= (ping_retry_count--))
                {
                    APP_ERR_PRINT("\r\nFailed maximum retry count for ping operation. Reset MCU.\r\n");
                    wifi_deinit();
                    APP_ERR_TRAP(err);
                }
                APP_PRINT("\r\nNetwork device may be off-line or invalid URL/IP address. Try again or enter different URL/IP.\r\n");
                /* Reseting DNS lookup retry count for next iteration */
                dns_retry_count = WIFI_MAX_TRY;
            }
        }
    }while(FSP_SUCCESS != err);
    APP_PRINT("\r\nPing Successful to %s[%d.%d.%d.%d]\r\n", input_buff, ip_addr[INDEX_ZERO], ip_addr[INDEX_ONE],
            ip_addr[INDEX_TWO], ip_addr[INDEX_THREE]);

    /* Starting TCP client-server communication */
    APP_PRINT("\r\nStart TCP server in PC and start listening.");
    APP_PRINT("\r\nSuggestion: TCP server in PC should listen at a port number greater than 10000.");
    APP_PRINT(RTT_CTRL_TEXT_BRIGHT_YELLOW "\r\nNote: If TCP server not started before MCU tries to connect, then application would fail.\r\n" RTT_CTRL_RESET);

    /* Creating TCP socket in client mode */
    err = tcp_socket_create();
    /* Handle error */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nTCP socket create in client mode failed.");
        APP_ERR_PRINT("\r\nCheck network connectivity.");
        wifi_deinit();
        APP_ERR_TRAP(err);
    }
    APP_PRINT("\r\nTCP Socket created in client mode on MCU.\r\n");

    /* Resetting DNS and ping retry_count */
    dns_retry_count = WIFI_MAX_TRY;
    ping_retry_count = WIFI_MAX_TRY;
    /* User input for TCP server IP address and its validation */
    do
    {
        /* User input for TCP server IP address */
        APP_PRINT("\r\nEnter TCP server IP address to connect:\r\n");
        user_input(input_buff);

        APP_PRINT("\r\nValidating TCP server IP address. Please wait...\r\n");
        /* DNS lookup for TCP server IP address */
        err = dns_query(input_buff, (uint8_t *) &g_socket.ip_addr_server);
        /* Handle error */
        if(FSP_SUCCESS != err)
        {
            APP_PRINT("\r\nDNS lookup failed. Check TCP server IP address entered.\r\n");
            if(RESET_VALUE >= (dns_retry_count--))
            {
                APP_ERR_PRINT("\r\nFailed maximum retry count for DNS lookup. Reset MCU.\r\n");
                wifi_deinit();
                APP_ERR_TRAP(err);
            }
        }
        /* Only if DNS lookup is successful, ping would be performed. */
        else
        {
            APP_PRINT("\r\nPinging TCP server IP address to validate. Please wait...\r\n");
            /* Pinging IP address to validate */
            err = ping((uint8_t *) &g_socket.ip_addr_server);
            /* Handle error */
            if(FSP_SUCCESS != err)
            {
                APP_PRINT("\r\nPing Unsuccessful.\r\n");
                /* If ping fails maximum times, then EP fails */
                if(RESET_VALUE >= (ping_retry_count--))
                {
                    APP_ERR_PRINT("\r\nFailed maximum retry count for pinging to \"server IP address: %s\" . Reset MCU.\r\n", input_buff);
                    wifi_deinit();
                    APP_ERR_TRAP(err);
                }
                APP_PRINT("\r\nTCP server may be off-line or invalid server IP address. Please check!!!\r\n");
                /* Reseting DNS lookup retry count for next iteration */
                dns_retry_count = WIFI_MAX_TRY;
            }
        }
    }while(FSP_SUCCESS != err);

    /* User input for TCP server port number */
    APP_PRINT("\r\nEnter TCP server port number to connect:");
    APP_PRINT("\r\nNote: Please give free port number, otherwise TCP connect would fail and application would also fail.\r\n");
    user_input(input_buff);
    g_socket.port = (uint32_t) atoi(input_buff);

    /* Connecting to TCP server */
    err = tcp_socket_connect(g_socket.ip_addr_server, g_socket.port);
    /* Handle error */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nConnection failed. \r\nCheck TCP server settings and network connectivity.\r\n");
        wifi_deinit();
        APP_ERR_TRAP(err);
    }

    APP_PRINT("\r\nTCP socket connected.\r\n");

    /* Sending message to server */
    strcpy((char *) g_socket.send_buff, "Hello");
    APP_PRINT("\r\nSending: %s", g_socket.send_buff);

    /* Resetting err to capture number of bytes */
    err = RESET_VALUE;
    err = tcp_send_data(g_socket.send_buff);
    /* Handle error */
    if(RESET_VALUE >= err)
    {
        APP_ERR_PRINT("\r\nCheck TCP server settings and network connectivity.");
        wifi_deinit();
        APP_ERR_TRAP(FSP_ERR_WIFI_FAILED);
    }

    APP_PRINT("\r\nSend 'on' or 'off' message from server.\r\n");
    while(FSP_SUCCESS == network_connectivity_check())
    {
        /* Receiving message from server */
        memset(g_socket.recv_buff, RESET_VALUE, sizeof(g_socket.recv_buff));
        APP_PRINT("\r\nWaiting for message from server...\r\n");

        /* Resetting err to capture number of bytes */
        err = RESET_VALUE;
        err = tcp_recv_data(g_socket.recv_buff);
        if(RESET_VALUE >= err)
        {
            /* Handle error */
            if(RESET_VALUE > err)
            {
                APP_ERR_PRINT("\r\nConnection failure.\r\nReset MCU.\r\n");
                wifi_deinit();
                APP_ERR_TRAP(FSP_ERR_WIFI_FAILED);
            }
        }
        else
        {
            /* Processing message from server */
            /* Converting received data to lower case for string comparison */
            xstrlwr(g_socket.recv_buff);
            if(RESET_VALUE == strcmp((char *) g_socket.recv_buff, "on"))
            {
                APP_PRINT("\r\nReceived: %s", g_socket.recv_buff);
                APP_PRINT("\r\nUser LED ON.\r\n");
                R_IOPORT_PinWrite(&g_ioport_ctrl, USER_LED, BSP_IO_LEVEL_HIGH);

                /* Sending acknowledgement to server */
                strcpy((char *) g_socket.send_buff, "Success");

                /* Resetting err to capture number of bytes */
                err = RESET_VALUE;
                err = tcp_send_data(g_socket.send_buff);
                /* Handle error */
                if(RESET_VALUE >= err)
                {
                    APP_ERR_PRINT("\r\nConnection failure.\r\nReset MCU.\r\n");
                    wifi_deinit();
                    APP_ERR_TRAP(FSP_ERR_WIFI_FAILED);
                }
            }
            else if(RESET_VALUE == strcmp((char *) g_socket.recv_buff, "off"))
            {
                APP_PRINT("\r\nReceived: %s", g_socket.recv_buff);
                APP_PRINT("\r\nUser LED OFF.\r\n");
                R_IOPORT_PinWrite(&g_ioport_ctrl, USER_LED, BSP_IO_LEVEL_LOW);

                /* Sending acknowledgement to server */
                strcpy((char *) g_socket.send_buff, "Success");

                /* Resetting err to capture number of bytes */
                err = RESET_VALUE;
                err = tcp_send_data(g_socket.send_buff);
                /* Handle error */
                if(RESET_VALUE >= err)
                {
                    APP_ERR_PRINT("\r\nConnection failure.\r\nReset MCU.\r\n");
                    wifi_deinit();
                    APP_ERR_TRAP(FSP_ERR_WIFI_FAILED);
                }
            }
            else
            {
                APP_PRINT("\r\nReceived: %s", g_socket.recv_buff);
                APP_PRINT("\r\nInvalid message. Send 'on' or 'off'\r\n");

                /* Sending error message to server */
                strcpy((char *) g_socket.send_buff, "Invalid");

                /* Resetting err to capture number of bytes */
                err = RESET_VALUE;
                err = tcp_send_data(g_socket.send_buff);
                /* Handle error */
                if(RESET_VALUE >= err)
                {
                    APP_ERR_PRINT("\r\nConnection failure.\r\nReset MCU.\r\n");
                    wifi_deinit();
                    APP_ERR_TRAP(FSP_ERR_WIFI_FAILED);
                }
            }
        }
    }

    APP_PRINT("\r\nWiFi example project exiting. Reset the MCU.\r\n")
    /* De-initializing WiFi module */
    wifi_deinit();

    while (1)
    {
        vTaskDelay (1);
    }
}

/*******************************************************************************************************************//**
 * @brief       This functions converts the input string pointed by the pointer to lower case.
 * @param[in]   p_str                       Pointer to string of uint8_t data-type.
 * @retval      None.
 **********************************************************************************************************************/
static void xstrlwr(uint8_t * p_str)
{
    uint8_t i = RESET_VALUE;

    while((p_str[i] != '\0') && (i < BUFF_LEN))
    {
        p_str[i] = (uint8_t) tolower(p_str[i]);
        i++;
    }
}
