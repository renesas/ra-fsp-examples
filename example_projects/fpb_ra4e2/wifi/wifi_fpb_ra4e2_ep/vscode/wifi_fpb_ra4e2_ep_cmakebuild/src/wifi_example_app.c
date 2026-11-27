/***********************************************************************************************************************
 * File Name    : wifi_example_app.c
 * Description  : Example application layer for the Wi-Fi EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - wifi_thread_entry.c calls a single function: wifi_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "tcp_socket.h"
#include "wifi_example_app.h"
#include "console/uart_console.h"

/* Private functions */
static void wifi_setup(void);
static void wifi_scan_ap(void);
static void wifi_manual_ap(void);
static fsp_err_t connect_tcp_server(void);
static fsp_err_t ping_target(void);
static void user_input(char * buf, int len);
static void handle_error(fsp_err_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the Wi-Fi example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void wifi_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};

    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        return;     /* UART failed to open — cannot print anything */
    }

    /* Get FLEX version information */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the terminal */
    printf(BANNER_INFO, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    printf(EP_INFO);

    /* Setup Access Point connection parameters */
    wifi_setup();
    printf("!!! Wi-Fi connected successfully !!!\r\n\r\n");

    /* Turn OFF the on-board LED */
    set_led(LED_OFF);

    /* Ping demonstration */
    err = ping_target();
    handle_error(err, "ping_target failed\r\n");
    printf("!!! Ping successfully !!!\r\n\r\n");

    /* Create TCP socket in client mode */
    printf("\r\nStart TCP server in PC and start listening.\r\n");
    err = socket_create();
    handle_error(err, "socket_create failed\r\n");
    printf("TCP Socket created in client mode on MCU.\r\n");
    printf("Note: Ensure TCP server is started before MCU tries to connect. Follow up guide\r\n"
            "on 'Sokit setup on PC as TCP server' section.\r\n");

    /* Connect to TCP socket */
    connect_tcp_server();
    handle_error(err, "connect_tcp_server failed\r\n");
    printf("\r\n!!! TCP socket connected successfully !!!\r\n\r\n");

    /* Send message to the server */
    err = tcp_send_data("Hello");
    printf("\r\nSending: %s", g_socket.send_buff);
    handle_error(err, EP_ERROR);
    printf(EP_GUIDE);

    while (FSP_SUCCESS == network_connectivity_check())
    {
        /* Receive message from the server */
        printf("\r\nWaiting for message from server...\r\n");
        err = tcp_recv_data();
        handle_error(err, EP_ERROR);
        if ('\0' == g_socket.recv_buff[0])
        {
            continue;
        }
        printf("\r\nReceived: %s", g_socket.recv_buff);

        /* Process message from the server */
        str_to_lowercase(g_socket.recv_buff);

        if (RESET_VALUE == strcmp((char *)g_socket.recv_buff, "on"))
        {
            printf("\r\nThe user LED ON.\r\n");
            set_led(LED_ON);
            err = tcp_send_data("Success");
            handle_error(err, EP_ERROR);
        }
        else if (RESET_VALUE == strcmp((char *)g_socket.recv_buff, "off"))
        {
            printf("\r\nThe user LED OFF.\r\n");
            set_led(LED_OFF);
            err = tcp_send_data("Success");
            handle_error(err, EP_ERROR);
        }
        else
        {
            printf("\r\nInvalid message. Send 'on' or 'off'\r\n");
            err = tcp_send_data("Invalid");
            handle_error(err, EP_ERROR);
        }
    }

    printf("\r\nWi-Fi example project exiting. Reset the MCU.\r\n");

    /* De-initialize WiFi module */
    wifi_deinit();

    while (1)
    {
        vTaskDelay(1);
    }
}

/***********************************************************************************************************************
 *  @brief     This function configures the Wi-Fi Access Point connection parameters.
 *  @param[in] None.
 *  @retval    None.
 **********************************************************************************************************************/
static void wifi_setup(void)
{
    fsp_err_t err = FSP_SUCCESS;
    char input_buff[BUFF_LEN] = {RESET_VALUE};
    uint8_t selection = RESET_VALUE;

    err = wifi_open();
    handle_error(err, "wifi_open failed\r\n");

    do
    {
        printf(EP_MENU);
        user_input(input_buff, (int)sizeof(input_buff));
        selection = (uint8_t)atoi((char *)input_buff);

        switch (selection)
        {
            case SCAN_AP_SELECTION:
            {
#if defined (BOARD_RA2E2_EK)
                printf(EP_NOTE);
#else
                wifi_scan_ap();
#endif /* BOARD_RA2E2_EK */
                break;
            }

            case ENTER_SSID_SELECTION:
            {
                wifi_manual_ap();
                break;
            }

            default:
            {
                printf("\r\nInvalid input.\r\n");
                break;
            }
        }
    } while ((SCAN_AP_SELECTION != selection) && (ENTER_SSID_SELECTION != selection));

    /* Password for non-open security */
    if (eWiFiSecurityOpen != g_wifi.security)
    {
        printf("\r\n\r\nEnter password for %s: ", g_wifi.ssid);
        user_input(g_wifi.pwd, (int)sizeof(g_wifi.pwd));
    }

    printf("\r\nConnecting to %s\r\n", g_wifi.ssid);
    err = wifi_config();
    handle_error(err, "wifi_config failed\r\n");

    printf("\r\nMCU IP address: %d.%d.%d.%d\r\n", g_wifi.ip_addr_device[0U], g_wifi.ip_addr_device[1U],
           g_wifi.ip_addr_device[2U], g_wifi.ip_addr_device[3U]);
}

/*******************************************************************************************************************//**
 * @brief     This function scans for available Wi-Fi Access Points and stores the selected network information.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void wifi_scan_ap(void)
{
    fsp_err_t err = FSP_SUCCESS;
    char input_buff[BUFF_LEN] = {RESET_VALUE};
    uint8_t index_ap = RESET_VALUE;
    uint8_t index = RESET_VALUE;

    do
    {
        /* Perform a Wi-Fi network scan */
        printf("\r\n\r\nScanning Wi-Fi Access Points...\r\n");
        err = wifi_scan();
        handle_error(err, "wifi_scan failed\r\n");

        /* Print result Wi-Fi Access Points scan */
        printf("\r\n------------List of Wi-Fi Access Points-----------\r\n");

        for (index = RESET_VALUE; index < MAX_WIFI_SCAN_RESULTS; index++)
        {
            if ('\0' == g_scan_data[index].ucSSID[0] || '\t' == g_scan_data[index].ucSSID[0])
            {
                continue;
            }

            const char * sec_str = "Unknown";
            switch (g_scan_data[index].xSecurity)
            {
                case eWiFiSecurityOpen:
                {
                    sec_str = "Open";
                    break;
                }

                case eWiFiSecurityWPA:
                {
                    sec_str = "WPA";
                    break;
                }

                case eWiFiSecurityWPA2:
                {
                    sec_str = "WPA2";
                    break;
                }

                default:
                {
                    break;
                }
            }
            printf("(%d) %s\r\nSecurity: %s, Signal: %d\r\n", index, g_scan_data[index].ucSSID,
                   sec_str, g_scan_data[index].cRSSI);
        }

        /* Perform the user enter Access Point index */
        printf("\r\nEnter Access Point index to connect, or non-digit to re-scan: ");
        memset(input_buff, RESET_VALUE, sizeof(input_buff));
        user_input(input_buff, (int)sizeof(input_buff));
        index_ap = (uint8_t)atoi(input_buff);

    } while (('0' > input_buff[0]) || ('9' < input_buff[0]) || (MAX_WIFI_SCAN_RESULTS <= index_ap));

    wifi_set_ap_ssid(index_ap);
}

/*******************************************************************************************************************//**
 * @brief     This function allows the user to manually enter the SSID and security type.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void wifi_manual_ap(void)
{
    char input_buff[BUFF_LEN] = {RESET_VALUE};
    uint32_t sec_val = RESET_VALUE;

    /* The user inputs the SSID of the WiFi AP */
    printf("\r\n\r\nEnter SSID of Wi-Fi Access Point: ");
    user_input(g_wifi.ssid, (int)sizeof(g_wifi.ssid));

    do
    {
        printf(SUB_MENU2);
        user_input(input_buff, (int)sizeof(input_buff));
        sec_val = (uint32_t)atoi(input_buff);

    } while (('0' > input_buff[0]) || ('2' < input_buff[0]) || ('\0' != input_buff[1]));

    if (0 == sec_val)
    {
        g_wifi.security = eWiFiSecurityOpen;
    }
    else if (1 == sec_val)
    {
        g_wifi.security = eWiFiSecurityWPA;
    }
    else
    {
        g_wifi.security = eWiFiSecurityWPA2;
    }
}

/*******************************************************************************************************************//**
 * @brief     This function validates the target URL or IP address.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Validated successfully.
 * @retval    Any other error codes     Failed to validate.
 **********************************************************************************************************************/
static fsp_err_t ping_target(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t ip_addr[IP_BUFF_LEN] = {RESET_VALUE};
    char input_buff[BUFF_LEN] = {RESET_VALUE};
    uint32_t dns_retry_count = WIFI_MAX_TRY;
    uint32_t ping_retry_count = WIFI_MAX_TRY;
    do
    {
        printf("\r\nEnter the IP or URL to ping: ");
        user_input(input_buff, (int)sizeof(input_buff));

        printf("\r\nPinging. Please wait...\r\n");
        err = dns_query(input_buff, ip_addr);

        if (FSP_SUCCESS != err)
        {
            printf("\r\nDNS lookup failed. Check IP or URL entered.\r\n");
            if (RESET_VALUE >= (dns_retry_count--))
            {
                printf("\r\nFailed maximum retry count for DNS lookup. Reset the MCU.\r\n");
                return err;
            }
        }
        else
        {
            err = ping(ip_addr);
            if (FSP_SUCCESS != err)
            {
                printf("\r\nPing failed. Check IP or URL entered.\r\n");
                if (RESET_VALUE >= (ping_retry_count--))
                {
                    printf("\r\nFailed maximum retry count for ping operation. Reset the MCU.\r\n");
                    return err;
                }
                dns_retry_count = WIFI_MAX_TRY;
            }
        }
    } while (FSP_SUCCESS != err);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function connects to the TCP server.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Connected successfully.
 * @retval    Any other error codes     Failed to connect.
 **********************************************************************************************************************/
static fsp_err_t connect_tcp_server(void)
{
    fsp_err_t err = FSP_SUCCESS;
    char input_buff[BUFF_LEN] = {RESET_VALUE};
    uint32_t dns_retry_count  = WIFI_MAX_TRY;
    uint32_t ping_retry_count = WIFI_MAX_TRY;

    do
    {
        printf("\r\nEnter TCP server IP address to connect: ");
        user_input(input_buff, (int)sizeof(input_buff));

        printf("\r\nValidating TCP server IP address. Please wait...\r\n");
        err = dns_query(input_buff, (uint8_t *)&g_socket.ip_addr_server);

        if (FSP_SUCCESS != err)
        {
            printf("\r\nDNS lookup failed. Check TCP server IP address entered.\r\n");

            if (RESET_VALUE >= (dns_retry_count--))
            {
                printf("\r\nFailed maximum retry count for DNS lookup. Reset the MCU.\r\n");
                return err;
            }
        }
        else
        {
            err = ping((uint8_t *)&g_socket.ip_addr_server);
            if (FSP_SUCCESS != err)
            {
                printf("\r\nPing failed. Check TCP server IP address.\r\n");

                if (RESET_VALUE >= (ping_retry_count--))
                {
                    printf("\r\nFailed maximum retry count for ping operation. Reset the MCU.\r\n");
                    return err;
                }
                dns_retry_count = WIFI_MAX_TRY;
            }
        }
    } while (FSP_SUCCESS != err);

    printf("\r\nEnter TCP server port number to connect (greater than 10,000): ");
    user_input(input_buff, (int)sizeof(input_buff));
    g_socket.port = (uint32_t)atoi(input_buff);

    /* Connect to the TCP server */
    err = tcp_connect(g_socket.ip_addr_server, g_socket.port);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief         This function reads a line of user input from stdin and removes trailing newline characters.
 * @param[in,out] buf                   Pointer to buffer where the input string is stored.
 * @param[in]     len                   Maximum number of characters to read (buffer size).
 * @retval        None.
 **********************************************************************************************************************/
static void user_input(char * buf, int len)
{
    fgets(buf, len, stdin);
    buf[strcspn(buf, "\r\n")] = '\0';
}

/*******************************************************************************************************************//**
 * @brief     This function handles an error by printing error message and code, de-initializing the related modules,
 *            and entering a trap state.
 * @param[in] err                       Error code.
 * @param[in] err_str                   Error message.
 * @retval    None.
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err, const char * err_str)
{
    if (FSP_SUCCESS != err)
    {
        printf("\r\n[ERR] %s", err_str);
        printf("Returned Error Code: 0x%x\r\n", (unsigned int)err);
        uart_console_flush();
        wifi_deinit();
        socket_deinit();
        __BKPT(0);
    }
}
