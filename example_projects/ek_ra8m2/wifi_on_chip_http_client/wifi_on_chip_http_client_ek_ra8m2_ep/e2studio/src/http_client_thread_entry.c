/***********************************************************************************************************************
 * File Name    : http_client_thread_entry.c
 * Description  : This program demonstrates the usage of an HTTP client on a chip with DA16200.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "http_client_thread.h"
#include "wifi.h"
#include "common_utils.h"

/***********************************************************************************************************************
 * Static globals variables
 **********************************************************************************************************************/
static char g_request_buffer[HTTP_ONCHIP_DA16XXX_BUFFER_SIZE];
static char g_response_buffer[HTTP_ONCHIP_DA16XXX_BUFFER_SIZE];
static http_onchip_da16xxx_request_t g_http_request;
static http_onchip_da16xxx_buffer_t g_user_buf =
{
 .p_request_buffer = g_request_buffer,
 .req_length = sizeof(g_request_buffer),
 .p_response_buffer = g_response_buffer,
 .resp_length = sizeof(g_response_buffer),
};
static http_onchip_da16xxx_instance_ctrl_t g_rm_http_onchip_da16xxx_instance;

/* pvParameters contains TaskHandle_t */
void http_client_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);
    WIFIReturnCode_t wifi_err = eWiFiSuccess;
    fsp_err_t http_err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};
    char rByte[BUFFER_SIZE_DOWN] = {RESET_VALUE};
    uint8_t rtt_input = RESET_VALUE;

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the Console */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major,\
              version.version_id_b.minor, version.version_id_b.patch);
    APP_PRINT(EP_INFO);

    /* Turns on Wi-Fi */
    wifi_err = WIFI_On();
    wifi_handle_error(wifi_err,"\r\nWi-Fi module failed to turn on\r\n");
    APP_PRINT("\r\n!!! Wi-Fi initialized successfully !!!\r\n");

    /* Setup Access Point connection parameters */
    wifi_err = wifi_config();
    wifi_handle_error(wifi_err, "\r\nWi-Fi module failed to connect\r\n");
    APP_PRINT("!!! Wi-Fi connected successfully !!!\r\n");

    /* Initialize the DA16XXX on-chip HTTP Client service */
    http_err = RM_HTTP_DA16XXX_Open(&g_rm_http_onchip_da16xxx_instance, &g_http_onchip_da16xxx_cfg);
    if (FSP_SUCCESS != http_err)
    {
        APP_PRINT("\r\n**RM_HTTP_DA16XXX_Open API failed**\r\n");
        APP_ERR_TRAP(http_err);
    }

    APP_PRINT(PRINT_HTTP_MENU);

    while (true)
    {
        memset(&g_http_request, RESET_VALUE, sizeof(http_onchip_da16xxx_request_t));
        user_input(rByte);
        rtt_input = (uint8_t)atoi((char*) rByte);
        switch (rtt_input)
        {
            case HTTP_GET_SELECTION:
            {
                APP_PRINT("\r\nHTTP GET request...\r\n");
                g_http_request.p_http_endpoint = HTTP_GET_ENDPOINT;
                g_http_request.p_request_body = NULL;
                g_http_request.method = HTTP_ONCHIP_DA16XXX_GET;
            }
            break;
            case HTTP_PUT_SELECTION:
            {
                APP_PRINT("\r\nHTTP PUT request...\r\n");
                g_http_request.p_http_endpoint = HTTP_PUT_ENDPOINT;
                g_http_request.p_request_body = HTTP_PUT_REQUEST_BODY_SAMPLE;
                g_http_request.method = HTTP_ONCHIP_DA16XXX_PUT;
            }
            break;
            case HTTP_POST_SELECTION:
            {
                APP_PRINT("\r\nHTTP POST request...\r\n");
                g_http_request.p_http_endpoint = HTTP_POST_ENDPOINT;
                g_http_request.p_request_body = HTTP_POST_REQUEST_BODY_SAMPLE;
                g_http_request.method = HTTP_ONCHIP_DA16XXX_POST;
            }
            break;
            default:
                APP_PRINT("\r\nInvalid Input.\r\n");
                APP_PRINT(PRINT_HTTP_MENU);
        }
        if (g_http_request.p_http_endpoint)
        {
            /* Send the HTTP request with the configured buffers */
            http_err = RM_HTTP_DA16XXX_Send(&g_rm_http_onchip_da16xxx_instance, &g_http_request, &g_user_buf);
            if (FSP_SUCCESS != http_err)
            {
                APP_PRINT("\r\n**RM_HTTP_DA16XXX_Send API failed**\r\n");
                if (FSP_SUCCESS != RM_HTTP_DA16XXX_Close(&g_rm_http_onchip_da16xxx_instance))
                {
                    APP_PRINT("\r\n**RM_HTTP_DA16XXX_Close API failed**\r\n");
                }
                APP_ERR_TRAP(http_err);
            }
            else
            {
                APP_PRINT("HTTP request successful!\n");
                APP_PRINT("HTTP response...\n");
                APP_PRINT("------------------------------------------------------------------------------");
                APP_PRINT("\r\n%s", g_user_buf.p_response_buffer);
                APP_PRINT("------------------------------------------------------------------------------\r\n");
            }
            APP_PRINT(PRINT_HTTP_MENU);
        }

        vTaskDelay(1);
    }
}
