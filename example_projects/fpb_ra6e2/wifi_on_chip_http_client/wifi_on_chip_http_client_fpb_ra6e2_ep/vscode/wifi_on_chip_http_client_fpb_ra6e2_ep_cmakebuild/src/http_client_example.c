/***********************************************************************************************************************
 * File Name    : http_client_example.c
 * Description  : Example application layer for the Wi-Fi On Chip HTTP Client EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: http_client_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "http_client_example.h"
#include "wifi_ep.h"
#include "console/uart_console.h"

/* Global variables */
char g_request_buffer[HTTP_ONCHIP_DA16XXX_BUFFER_SIZE];
char g_response_buffer[HTTP_ONCHIP_DA16XXX_BUFFER_SIZE];
http_onchip_da16xxx_request_t g_http_request;
http_onchip_da16xxx_buffer_t  g_user_buf =
{
    .p_request_buffer = g_request_buffer,
    .req_length = sizeof(g_request_buffer),
    .p_response_buffer = g_response_buffer,
    .resp_length = sizeof(g_response_buffer),
};
http_onchip_da16xxx_instance_ctrl_t g_rm_http_instance;

/* Private functions */
static void wifi_setup(void);
static void wifi_scan_ap(void);
static void wifi_manual_ap(void);
static void user_input(char * buf, int len);
static void wifi_handle_error(WIFIReturnCode_t wifi_err, const char * wifi_err_str);
static void handle_error(fsp_err_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the Wi-Fi On Chip HTTP Client example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void http_client_example_run(void)
{
    WIFIReturnCode_t wifi_err = eWiFiSuccess;
    fsp_err_t http_err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};
    char input_buff[BUFFER_SIZE] = {NULL_CHAR};
    uint32_t selection = RESET_VALUE;

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

    /* Initialize Wi-Fi module */
    wifi_err = wifi_init();
    wifi_handle_error(wifi_err, "wifi_init failed\r\n");

    /* Setup Access Point connection parameters */
    wifi_setup();
    printf("!!! Wi-Fi connected successfully !!!\r\n");
    vTaskDelay(1);

    /* Initialize the DA16XXX Onchip HTTP Client service */
    http_err = http_client_init(&g_rm_http_instance);
    handle_error(http_err, "http_client_init failed\r\n");

    printf(SUB_MENU1);

    while (true)
    {
        memset(&g_http_request, RESET_VALUE, sizeof(http_onchip_da16xxx_request_t));
        memset(g_response_buffer, RESET_VALUE, sizeof(g_response_buffer));
        memset(input_buff, NULL_CHAR, BUFFER_SIZE);
        user_input(input_buff, (int)sizeof(input_buff));
        selection = (uint32_t)atoi(input_buff);

        switch (selection)
        {
            case HTTP_GET_SELECTION:
            {
                printf("\r\n\r\nHTTP GET request...\r\n");
                g_http_request.p_http_endpoint = HTTP_GET_ENDPOINT;
                g_http_request.p_request_body = NULL;
                g_http_request.method = HTTP_ONCHIP_DA16XXX_GET;
                break;
            }

            case HTTP_PUT_SELECTION:
            {
                printf("\r\n\r\nHTTP PUT request...\r\n");
                g_http_request.p_http_endpoint = HTTP_PUT_ENDPOINT;
                g_http_request.p_request_body = HTTP_PUT_REQUEST_BODY_SAMPLE;
                g_http_request.method = HTTP_ONCHIP_DA16XXX_PUT;
                break;
            }

            case HTTP_POST_SELECTION:
            {
                printf("\r\n\r\nHTTP POST request...\r\n");
                g_http_request.p_http_endpoint = HTTP_POST_ENDPOINT;
                g_http_request.p_request_body = HTTP_POST_REQUEST_BODY_SAMPLE;
                g_http_request.method = HTTP_ONCHIP_DA16XXX_POST;
                break;
            }

            default:
            {
                printf("\r\nInvalid input\r\n");
                printf(SUB_MENU1);
                continue;
            }
        }

         /* Send the HTTP request with the configured buffers */
        http_err = http_client_send(&g_rm_http_instance, &g_http_request, &g_user_buf);
        handle_error(http_err, "http_client_send failed\r\n");

        printf("!!! HTTP request successful !!!\r\n");
        printf("HTTP response:\r\n");
        printf("------------------------------------------------------------------------------\r\n");
        printf("%s\r\n", g_user_buf.p_response_buffer);
        printf("------------------------------------------------------------------------------\r\n\r\n");
        printf(SUB_MENU1);

        vTaskDelay(1);
    }
}

/***********************************************************************************************************************
 * @brief     This function configures, establishes the Wi-Fi connection and displays the assigned IP configuration.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void wifi_setup(void)
{
    WIFIReturnCode_t wifi_err = eWiFiSuccess;
    char password[wificonfigMAX_PASSPHRASE_LEN] = {NULL_CHAR};
    char input_buff[BUFFER_SIZE] = {NULL_CHAR};
    uint32_t selection = RESET_VALUE;

    do
    {
        printf(EP_MENU);
        memset(input_buff, NULL_CHAR, BUFFER_SIZE);
        user_input(input_buff, (int)sizeof(input_buff));
        selection = (uint32_t)atoi(input_buff);

        switch (selection)
        {
            case SCAN_AP_SELECTION:
            {
                wifi_scan_ap();
                break;
            }

            case ENTER_SSID_SELECTION:
            {
                wifi_manual_ap();
                break;
            }

            default:
            {
                printf("\r\nInvalid input\r\n");
                break;
            }
        }

    } while ((SCAN_AP_SELECTION != selection) && (ENTER_SSID_SELECTION != selection));

    /* Password for non-open security */
    if (eWiFiSecurityOpen != wifi_get_security())
    {
        printf("\r\n\r\nEnter password for %s: ", wifi_get_ssid());
        user_input(password, (int)sizeof(password));
        wifi_set_password(password);
    }

    /* Connect to the Access Point */
    printf("\r\nConnecting to %s...\r\n", wifi_get_ssid());
    wifi_err = wifi_config();
    wifi_handle_error(wifi_err, "wifi_config failed\r\n");

    /* Print IP info */
    if (eWiFiIPAddressTypeV4 == g_ip_config.xIPAddress.xType)
    {
        printf("\r\nIP     : %d.%d.%d.%d\r\n",
               (uint8_t) g_ip_config.xIPAddress.ulAddress[0],
               (uint8_t)(g_ip_config.xIPAddress.ulAddress[0] >>  8 & 0xFF),
               (uint8_t)(g_ip_config.xIPAddress.ulAddress[0] >> 16 & 0xFF),
               (uint8_t)(g_ip_config.xIPAddress.ulAddress[0] >> 24 & 0xFF));
        printf("Netmask: %d.%d.%d.%d\r\n",
               (uint8_t) g_ip_config.xNetMask.ulAddress[0],
               (uint8_t)(g_ip_config.xNetMask.ulAddress[0] >>  8 & 0xFF),
               (uint8_t)(g_ip_config.xNetMask.ulAddress[0] >> 16 & 0xFF),
               (uint8_t)(g_ip_config.xNetMask.ulAddress[0] >> 24 & 0xFF));
        printf("Gateway: %d.%d.%d.%d\r\n",
               (uint8_t) g_ip_config.xGateway.ulAddress[0],
               (uint8_t)(g_ip_config.xGateway.ulAddress[0] >>  8 & 0xFF),
               (uint8_t)(g_ip_config.xGateway.ulAddress[0] >> 16 & 0xFF),
               (uint8_t)(g_ip_config.xGateway.ulAddress[0] >> 24 & 0xFF));
    }
}

/***********************************************************************************************************************
 * @brief     This function scans for available Wi-Fi Access Points and stores the selected network information.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void wifi_scan_ap(void)
{
    WIFIReturnCode_t wifi_err = eWiFiSuccess;
    char ap_buf[BUFFER_SIZE] = {NULL_CHAR};
    uint32_t ap_index = RESET_VALUE;
    uint32_t index = RESET_VALUE;

    do
    {
        /* Perform a Wi-Fi network scan */
        printf("\r\n\r\nScanning Wi-Fi Access Points...\r\n");
        wifi_err = wifi_scan();
        wifi_handle_error(wifi_err, "wifi_scan failed\r\n");

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
        memset(ap_buf, NULL_CHAR, sizeof(ap_buf));
        user_input(ap_buf, (int)sizeof(ap_buf));
        ap_index = (uint32_t)atoi(ap_buf);
    }
    while (('0' > ap_buf[0]) || ('9' < ap_buf[0]) || (MAX_WIFI_SCAN_RESULTS <= ap_index));

    wifi_set_ap_ssid(ap_index);
}

/***********************************************************************************************************************
 * @brief     This function allows the user to manually enter the SSID and security type.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void wifi_manual_ap(void)
{
    char ssid[wificonfigMAX_SSID_LEN] = {NULL_CHAR};
    char ap_buf[BUFFER_SIZE] = {NULL_CHAR};
    uint32_t sec_val = RESET_VALUE;
    WIFISecurity_t security = eWiFiSecurityNotSupported;

    printf("\r\n\r\nEnter SSID of Wi-Fi Access Point: ");
    user_input(ssid, (int)sizeof(ssid));

    do
    {
        printf(SUB_MENU2);
        user_input(ap_buf, (int)sizeof(ap_buf));
        sec_val = (uint32_t)atoi(ap_buf);
    }
    while (ap_buf[0] < '0' || ap_buf[0] > '2' || ap_buf[1] != '\0');

    if (0 == sec_val)
    {
        security = eWiFiSecurityOpen;
    }
    else if (1 == sec_val)
    {
        security = eWiFiSecurityWPA;
    }
    else
    {
        security = eWiFiSecurityWPA2;
    }

    wifi_set_manual_ssid(ssid, security);
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
 * @param[in] wifi_err                  Error code.
 * @param[in] wifi_err_str              Error message.
 * @retval    None.
 **********************************************************************************************************************/
static void wifi_handle_error(WIFIReturnCode_t wifi_err, const char * wifi_err_str)
{
    if (eWiFiSuccess != wifi_err)
    {
        printf("\r\n[ERR] %s", wifi_err_str);
        printf("Returned Error Code: 0x%x\r\n", (unsigned int)wifi_err);
        uart_console_flush();
        wifi_deinit();
        __BKPT(0);
    }
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
        http_client_deinit(&g_rm_http_instance);
        __BKPT(0);
    }
}
