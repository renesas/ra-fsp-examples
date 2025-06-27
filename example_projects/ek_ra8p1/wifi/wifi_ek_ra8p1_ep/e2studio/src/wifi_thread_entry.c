/***********************************************************************************************************************
 * File Name    : wifi_thread_entry.c
 * Description  : Contains data structures and functions used in wifi_thread_entry.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
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
    int32_t transfer_err = RESET_VALUE;
    fsp_pack_version_t version = {RESET_VALUE};

    char input_buff[BUFF_LEN] = {RESET_VALUE};          /* Buffer for storing the user input */
    uint8_t index_menu_option = RESET_VALUE;            /* Index to store menu option selected */

    uint8_t ip_addr[IP_BUFF_LEN] = {RESET_VALUE};       /* IP address for ping operation */
    int8_t dns_retry_count = WIFI_MAX_TRY;              /* Max retry count for DNS lookup */
    int8_t ping_retry_count = WIFI_MAX_TRY;             /* Max retry count for ping operation */

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the Console */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor,\
              version.version_id_b.patch);
    APP_PRINT(EP_INFO);

    /* Open WiFi module */
    err = rm_wifi_onchip_silex_open(&g_wifi_onchip_silex_cfg);
    /* Handle error */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** rm_wifi_onchip_silex_open API failed **\r\n");
        APP_ERR_TRAP(err);
    }

    /* Reset WiFi status flag */
    g_wifi.status = INT32_MAX;

    /* Connect to WiFi AP */
    do
    {
        /* Menu for the user selection */
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
                APP_PRINT("\r\nWiFi Access Point scanning is not available for this board.");
                APP_PRINT("\r\nPlease press 2 for entering WiFi Access Point's credentials manually.\r\n");
#else
                /* Scan for APs and connect to the user chosen one */
                err = scan_and_select();
                /* Handle error */
                if(FSP_SUCCESS != err)
                {
                    APP_PRINT("\r\nWiFi Access Point scanning failed. Retry.");
                    APP_PRINT("\r\nIf multiple failures, please enter WiFi AP credentials.\r\n");
                }
                else
                {
                    /* Connect to the user selected WiFi AP */
                    APP_PRINT("\r\nConnecting to %s", g_wifi.ssid);
                    err = rm_wifi_onchip_silex_connect(g_wifi.ssid, g_wifi.security, g_wifi.pwd);
                    /* Handle error */
                    if(FSP_SUCCESS != err)
                    {
                        APP_ERR_PRINT("\r\n** rm_wifi_onchip_silex_connect API failed **\r\n");
                        APP_ERR_PRINT("\r\nIncorrect Access Point credentials entered. Try again.\r\n");
                    }
                }
#endif
                break;
            case ENTER_SSID:
                /* The user manually enters WiFi AP credentials to connect */
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
        APP_ERR_PRINT("\r\n** rm_wifi_onchip_silex_ip_addr_get API failed **\r\n");
        wifi_deinit();
        APP_ERR_TRAP(err);
    }
    APP_PRINT("\r\nMCU IP address: %d.%d.%d.%d\r\n", g_wifi.ip_addr_device[INDEX_ZERO],\
            g_wifi.ip_addr_device[INDEX_ONE],\
            g_wifi.ip_addr_device[INDEX_TWO], g_wifi.ip_addr_device[INDEX_THREE]);

    /* Ping demonstration */
    do
    {
        /* The user inputs the IP address or the URL to ping */
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
        /* Only if DNS lookup is successful, ping would be performed */
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
                APP_PRINT("\r\nNetwork device may be off-line or invalid URL/IP address. "\
                          "Try again or enter different URL/IP.\r\n");
                /* Reset DNS lookup retry count for next iteration */
                dns_retry_count = WIFI_MAX_TRY;
            }
        }
    }while(FSP_SUCCESS != err);
    APP_PRINT("\r\nPing Successful to %s[%d.%d.%d.%d]\r\n", input_buff, ip_addr[INDEX_ZERO], ip_addr[INDEX_ONE],
              ip_addr[INDEX_TWO], ip_addr[INDEX_THREE]);

    /* Start TCP client-server communication */
    APP_PRINT("\r\nStart TCP server in PC and start listening.");
    APP_PRINT("\r\nSuggestion: TCP server in PC should listen at a port number greater than 10000.");
    APP_PRINT(RTT_CTRL_TEXT_BRIGHT_YELLOW "\r\nNote: If TCP server not started before MCU tries to connect, "\
              "then application would fail.\r\n" RTT_CTRL_RESET);

    /* Create TCP socket in client mode */
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

    /* Reset DNS and ping retry_count */
    dns_retry_count = WIFI_MAX_TRY;
    ping_retry_count = WIFI_MAX_TRY;
    /* The user inputs the TCP server IP address and its validation */
    do
    {
        /* The user inputs the TCP server IP address */
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
        /* Only if DNS lookup is successful, ping would be performed */
        else
        {
            APP_PRINT("\r\nPinging TCP server IP address to validate. Please wait...\r\n");
            /* Ping IP address to validate */
            err = ping((uint8_t *) &g_socket.ip_addr_server);
            /* Handle error */
            if(FSP_SUCCESS != err)
            {
                APP_PRINT("\r\nPing Unsuccessful.\r\n");
                /* If ping fails maximum times, then EP fails */
                if(RESET_VALUE >= (ping_retry_count--))
                {
                    APP_ERR_PRINT("\r\nFailed maximum retry count for pinging to \"server IP address: %s\" . "\
                                  "Reset MCU.\r\n", input_buff);
                    wifi_deinit();
                    APP_ERR_TRAP(err);
                }
                APP_PRINT("\r\nTCP server may be off-line or invalid server IP address. Please check!!!\r\n");
                /* Reset DNS lookup retry count for next iteration */
                dns_retry_count = WIFI_MAX_TRY;
            }
        }
    }while(FSP_SUCCESS != err);

    /* The user inputs the TCP server port number */
    APP_PRINT("\r\nEnter TCP server port number to connect:");
    APP_PRINT("\r\nNote: Please give free port number, otherwise TCP connect would fail and application "\
              "would also fail.\r\n");
    user_input(input_buff);
    g_socket.port = (uint32_t) atoi(input_buff);

    /* Connect to the TCP server */
    err = tcp_socket_connect(g_socket.ip_addr_server, g_socket.port);
    /* Handle error */
    if(FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nConnection failed. \r\nCheck TCP server settings and network connectivity.\r\n");
        wifi_deinit();
        APP_ERR_TRAP(err);
    }

    APP_PRINT("\r\nTCP socket connected.\r\n");

    /* Send message to the server */
    strcpy((char *) g_socket.send_buff, "Hello");
    APP_PRINT("\r\nSending: %s", g_socket.send_buff);

    /* Capture number of bytes */
    transfer_err = tcp_send_data(g_socket.send_buff);
    /* Handle error */
    if(RESET_VALUE >= transfer_err)
    {
        APP_ERR_PRINT("\r\nCheck TCP server settings and network connectivity.");
        wifi_deinit();
        APP_ERR_TRAP(FSP_ERR_WIFI_FAILED);
    }

    APP_PRINT("\r\nSend 'on' or 'off' message from server.\r\n");
    while(FSP_SUCCESS == network_connectivity_check())
    {
        /* Receive message from the server */
        memset(g_socket.recv_buff, RESET_VALUE, sizeof(g_socket.recv_buff));
        APP_PRINT("\r\nWaiting for message from server...\r\n");

        /* Capture number of bytes */
        transfer_err = tcp_recv_data(g_socket.recv_buff);
        if(RESET_VALUE >= transfer_err)
        {
            /* Handle error */
            if(RESET_VALUE > transfer_err)
            {
                APP_ERR_PRINT("\r\nConnection failure.\r\nReset MCU.\r\n");
                wifi_deinit();
                APP_ERR_TRAP(FSP_ERR_WIFI_FAILED);
            }
        }
        else
        {
            /* Process message from the server */
            /* Convert received data to lower case for string comparison */
            xstrlwr(g_socket.recv_buff);
            if(RESET_VALUE == strcmp((char *) g_socket.recv_buff, "on"))
            {
                APP_PRINT("\r\nReceived: %s", g_socket.recv_buff);
                APP_PRINT("\r\nThe user LED ON.\r\n");
#if defined (BOARD_RA6T2_MCK) || defined (BOARD_RA8T1_MCK)
                R_IOPORT_PinWrite(&g_ioport_ctrl, USER_LED, BSP_IO_LEVEL_LOW);
#else
                R_IOPORT_PinWrite(&g_ioport_ctrl, USER_LED, BSP_IO_LEVEL_HIGH);
#endif

                /* Send acknowledgment to the server */
                strcpy((char *) g_socket.send_buff, "Success");

                /* Capture number of bytes */
                transfer_err = tcp_send_data(g_socket.send_buff);
                /* Handle error */
                if(RESET_VALUE >= transfer_err)
                {
                    APP_ERR_PRINT("\r\nConnection failure.\r\nReset MCU.\r\n");
                    wifi_deinit();
                    APP_ERR_TRAP(FSP_ERR_WIFI_FAILED);
                }
            }
            else if(RESET_VALUE == strcmp((char *) g_socket.recv_buff, "off"))
            {
                APP_PRINT("\r\nReceived: %s", g_socket.recv_buff);
                APP_PRINT("\r\nThe user LED OFF.\r\n");
#if defined (BOARD_RA6T2_MCK) || defined (BOARD_RA8T1_MCK)
                R_IOPORT_PinWrite(&g_ioport_ctrl, USER_LED, BSP_IO_LEVEL_HIGH);
#else
                R_IOPORT_PinWrite(&g_ioport_ctrl, USER_LED, BSP_IO_LEVEL_LOW);
#endif

                /* Sending acknowledgment to the server */
                strcpy((char *) g_socket.send_buff, "Success");

                /* Capture number of bytes */
                transfer_err = tcp_send_data(g_socket.send_buff);
                /* Handle error */
                if(RESET_VALUE >= transfer_err)
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

                /* Send error message to the server */
                strcpy((char *) g_socket.send_buff, "Invalid");

                /* Capture number of bytes */
                transfer_err = tcp_send_data(g_socket.send_buff);
                /* Handle error */
                if(RESET_VALUE >= transfer_err)
                {
                    APP_ERR_PRINT("\r\nConnection failure.\r\nReset MCU.\r\n");
                    wifi_deinit();
                    APP_ERR_TRAP(FSP_ERR_WIFI_FAILED);
                }
            }
        }
    }

    APP_PRINT("\r\nWiFi example project exiting. Reset the MCU.\r\n");
    /* De-initialize WiFi module */
    wifi_deinit();

    while (1)
    {
        vTaskDelay (1);
    }
}

/*******************************************************************************************************************//**
 * @brief       This function converts the input string pointed by the pointer to lower case.
 * @param[in]   p_str       Pointer to string of uint8_t data-type.
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
