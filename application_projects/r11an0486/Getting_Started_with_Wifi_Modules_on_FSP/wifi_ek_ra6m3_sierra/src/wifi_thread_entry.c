/***********************************************************************************************************************
 * File Name    : wifi_thread_entry.c
 * Description  : Contains data structures and functions used in wifi_ek_ra6m3_sierra.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#include "common_utils.h"
#include "wifi_app.h"
#include "rm_wifi_onchip_sierra.h"

extern WiFiParameters_t g_wifi;

/* WiFi Thread entry function */
/* pvParameters contains TaskHandle_t */
void wifi_thread_entry (void * pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);

    fsp_err_t             err     = FSP_SUCCESS;
    fsp_pack_version_t    version = {RESET_VALUE};
    WIFIIPConfiguration_t locIPInfo;

    /* version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Example Project information printed on the Console */
    APP_PRINT(BANNER_INFO,
              EP_VERSION,
              version.version_id_b.major,
              version.version_id_b.minor,
              version.version_id_b.patch);
    APP_PRINT(APP_INFO);

    err = wifi_preinit();

    /* Handle error */
    if (FSP_ERR_NOT_INITIALIZED == err)
    {
        FAILURE_INDICATION APP_ERR_PRINT("** Wi-Fi Preinit failed  **\r\n");
    }

    err = wifi_init();

    /* Handle error */
    if (FSP_ERR_NOT_INITIALIZED == err)
    {
        FAILURE_INDICATION APP_ERR_PRINT("** Wi-Fi Init failed  **\r\n");
    }

    /* Connecting to WiFi AP/Router */
    err = wifi_connect();

    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        FAILURE_INDICATION APP_ERR_PRINT("** Wi-Fi Access Point Connect failed. **\r\n");

        if (FSP_SUCCESS == wifi_deinit())
        {
            APP_ERR_TRAP(err);
        }
        else
        {
            APP_ERR_PRINT("** Wi-Fi deinit failed.**\r\n");
            APP_ERR_TRAP(err);
        }
    }
    else
    {
        /* Connecting to user selected WiFi AP */
        APP_PRINT("\r\nConnecting to SSID %s\r\n", g_wifi.ssid);
    }

    /* Get and print assigned IP address */
    /* Handle error */
    if (eWiFiSuccess != WIFI_GetIPInfo(&locIPInfo))
    {
        FAILURE_INDICATION APP_ERR_PRINT("** IP Address get failed **\r\n");

        err = FSP_ERR_WIFI_FAILED;

        if (FSP_SUCCESS != wifi_deinit())
        {
            APP_ERR_PRINT("** Wi-Fi deinit failed.**\r\n");
        }

        APP_ERR_TRAP(err);
    }

    g_wifi.status = FSP_SUCCESS;
    for (uint32_t i = 0; i < 4; i++)
    {
        g_wifi.device_ip_addr[i] = locIPInfo.xIPAddress.ulAddress[i];
    }

    APP_PRINT("\r\nDevice IP address: %d.%d.%d.%d\r\n",
              g_wifi.device_ip_addr[0],
              g_wifi.device_ip_addr[1],
              g_wifi.device_ip_addr[2],
              g_wifi.device_ip_addr[3]);

    NETWORK_CONNECT_INDICATION

    /* Ping  IP address */
    while (1)
    {
        vTaskDelay(100);
        APP_ACTIVITY_INDICATION
    }
}
