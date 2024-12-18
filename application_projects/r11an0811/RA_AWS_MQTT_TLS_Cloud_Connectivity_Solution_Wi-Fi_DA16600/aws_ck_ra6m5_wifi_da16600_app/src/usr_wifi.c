/***********************************************************************************************************************
 * File Name    : usr_wifi.c
 * Description  : Contains data structures and functions used in Wi-Fi Configs for Cloud Application Project.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright [2015-2023] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
 *
 *
 * The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
 * and/or its licensors ("Renesas") and subject to statutory and contractual protections.
 *
 * This file is subject to a Renesas FSP license agreement. Unless otherwise agreed in an FSP license agreement with
 * Renesas: 1) you may not use, copy, modify, distribute, display, or perform the contents; 2) you may not use any name
 * or mark of Renesas for advertising or publicity purposes or in connection with your use of the contents; 3) RENESAS
 * MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED
 * "AS IS" WITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, AND NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR
 * CONSEQUENTIAL DAMAGES, INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF
 * CONTRACT OR TORT, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents
 * included in this file may be subject to different terms.
 **********************************************************************************************************************/
#include "app_thread.h"
#include "common_utils.h"
#include "mqtt_demo_helpers.h"
#include "usr_config.h"
#include "usr_wifi.h"
#include "usr_hal.h"


#define BOLD_GREEN_TEXT_START               "\033[1;32m"
#define BOLD_BLUE_TEXT_START                "\033[1;34m"
#define BOLD_RED_TEXT_START                 "\033[1;31m"
#define STANDARD_GREEN_TEXT_START           "\033[0;32m"
#define STANDARD_BLUE_TEXT_START            "\033[0;34m"
#define STANDARD_RED_TEXT_START             "\033[0;31m"
#define BOLD_COLOR_TEXT_END                 "\033[0m"


wifi_nwk_param_t g_wifi_nwk;

static fsp_err_t wifi_preinit(void);

static WIFINetworkParams_t pNwkParam = {RESET_VALUE};

extern char g_ssid[32];
extern char g_pass[32];
extern char g_secure[64];

/***********************************************************************************************************************
 * @brief       This function checks both WiFi connectivity and TCP socket connectivity.
 * @param[in]   None
 * @retval      FSP_SUCCESS             If both the connectivity checks are success.
 * @retval      Any other error         If one of the connectivity check fails.
 **********************************************************************************************************************/
fsp_err_t network_connectivity_check(void)
{
    fsp_err_t err = FSP_SUCCESS;

    if (eWiFiSuccess != WIFI_IsConnected((const WIFINetworkParams_t *) &pNwkParam))
    {
        APP_ERR_PRINT("** Wi-Fi is not connected \r\n");
        err = FSP_ERR_INTERNAL;
    }

    g_wifi_nwk.status = err;
    return err;
}

/***********************************************************************************************************************
 * @brief       This function gets device IP information.
 * @param[in]   None
 * @retval      FSP_SUCCESS             Get device IP information successfully.
 * @retval      Any other error         Get device IP information unsuccessfully.
 **********************************************************************************************************************/
fsp_err_t device_ip_info_check(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Get and print assigned IP address */
    /* Handle error */
    if (eWiFiSuccess != WIFI_GetIPInfo (&g_wifi_nwk.device_ip_config))
    {
        APP_ERR_PRINT("** Device IP information get failed **\r\n");
        err = FSP_ERR_WIFI_FAILED;

        if (FSP_SUCCESS != wifi_deinit ())
        {
            APP_ERR_PRINT("** Wi-Fi deinit failed.**\r\n");
        }
    }

    g_wifi_nwk.status = FSP_SUCCESS;

    if(g_wifi_nwk.device_ip_config.xIPAddress.xType == eWiFiIPAddressTypeV4)
    {
        APP_PRINT("\r\nDevice IP address: %d.%d.%d.%d\r\n", (uint8_t)g_wifi_nwk.device_ip_config.xIPAddress.ulAddress[ZERO],
                  (uint8_t)(g_wifi_nwk.device_ip_config.xIPAddress.ulAddress[ZERO] >> 8 & 0xFF),
                  (uint8_t)(g_wifi_nwk.device_ip_config.xIPAddress.ulAddress[ZERO] >> 16 & 0xFF),
                  (uint8_t)(g_wifi_nwk.device_ip_config.xIPAddress.ulAddress[ZERO] >> 24 & 0xFF) );
        APP_PRINT("Device network mask: %d.%d.%d.%d\r\n", (uint8_t)g_wifi_nwk.device_ip_config.xNetMask.ulAddress[ZERO],
                (uint8_t)(g_wifi_nwk.device_ip_config.xNetMask.ulAddress[ZERO] >> 8 & 0xFF),
                (uint8_t)(g_wifi_nwk.device_ip_config.xNetMask.ulAddress[ZERO] >> 16 & 0xFF),
                (uint8_t)(g_wifi_nwk.device_ip_config.xNetMask.ulAddress[ZERO] >> 24 & 0xFF) );
        APP_PRINT("Device gateway address: %d.%d.%d.%d\r\n", (uint8_t)g_wifi_nwk.device_ip_config.xGateway.ulAddress[ZERO],
                (uint8_t)(g_wifi_nwk.device_ip_config.xGateway.ulAddress[ZERO] >> 8 & 0xFF),
                (uint8_t)(g_wifi_nwk.device_ip_config.xGateway.ulAddress[ZERO] >> 16 & 0xFF),
                (uint8_t)(g_wifi_nwk.device_ip_config.xGateway.ulAddress[ZERO] >> 24 & 0xFF) );
    }

    return err;
}

/***********************************************************************************************************************
 * @brief       This function resolves the DNS query for the domain name and gets Endpoint IP address.
 * @param[IN]   p_dns                       Pointer to array holding URL or IP address to query from DNS.
 * @param[out]  p_ip_addr                   Pointer to IP address returned from look up.
 * @retval      FSP_SUCCESS                 Upon successful DNS resolving.
 * @retval      Any Other Error code        If DNS query fails.
 **********************************************************************************************************************/
fsp_err_t dns_query(char *p_dns, uint8_t *p_ip_addr)
{
    fsp_err_t err = FSP_SUCCESS;

	err = network_connectivity_check();

    if (FSP_SUCCESS == err)
    {
        if (eWiFiSuccess != WIFI_GetHostIP (p_dns, p_ip_addr))
        {
            err = FSP_ERR_INTERNAL;
        }
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function validates and Initializes the WiFi user credential settings.
 * @param[in]   None
 * @retval      error code: FSP_SUCCESS or FSP_ERR_UNSUPPORTED
 **********************************************************************************************************************/
static fsp_err_t wifi_preinit(void)
{
    g_wifi_nwk.status = !(FSP_SUCCESS);
    g_wifi_nwk.server_ip = 0;
    memset(&g_wifi_nwk.ssid, 0x00, BUFF_LEN);
    memset(&g_wifi_nwk.device_ip_config, 0x00, sizeof(WIFIIPConfiguration_t));
    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief       This function Initializes the WiFi module.
 * @param[in]   None
 * @retval      FSP_SUCCESS, FSP_ERR_ASSERTION, FSP_ERR_OUT_OF_MEMORY, FSP_ERR_WIFI_FAILED, FSP_ERR_ALREADY_OPEN
 **********************************************************************************************************************/
fsp_err_t wifi_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = wifi_preinit ();
    /* Handle error */
    if (FSP_ERR_NOT_INITIALIZED == err)
    {
        APP_ERR_PRINT("** Wi-Fi Preinit failed  **\r\n");
        return err;
    }

    /* Opening WiFi module and Handle the Error*/
    if (eWiFiSuccess == WIFI_On())
    {
        APP_PRINT("!!! Wi-Fi Init Successful !!!**\r\n");
    }
    else
    {   
        err = FSP_ERR_NOT_INITIALIZED;
        APP_ERR_PRINT("** Wi-Fi Init Failure **\r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This function disconnects the socket connection and closes the WiFi module.
 * @param[in]   None
 * @retval      fsp_err_t: FSP_SUCCESS, FSP_ERR_WIFI_INIT_FAILED, FSP_ERR_WIFI_FAILED
 **********************************************************************************************************************/
fsp_err_t wifi_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Checking socket connection status */
    err = network_connectivity_check ();
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("** Wi-Fi is not connected **\r\n");
        return err;
    }

    if (FSP_SUCCESS == g_wifi_nwk.status)
    {
        /* If socket connected, then disconnecting */
        if(eWiFiSuccess != WIFI_Disconnect ())
        {
            APP_ERR_PRINT("** Wi-Fi Disconnect Failed **\r\n");
            return FSP_ERR_WIFI_FAILED;
        }
    }
    else
    {
        APP_ERR_PRINT("** SOCKET STATUS IS NOT CONNECTED **\r\n");
        return FSP_ERR_WIFI_INIT_FAILED;
    }


    /* Closing WiFi module. */
    /* Handle error */
    if (eWiFiSuccess != WIFI_Off())
    {
        APP_PRINT("** Closing the Wi-Fi Module failed **\r\n");
        return FSP_ERR_WIFI_FAILED;
    }
    g_wifi_nwk.status = !(FSP_SUCCESS);
    return err;
}

/*********************************************************************************************************************
 * @brief       connect_wifi : This function initializes and calls the WIFI connect.
 * @param[in]   None
 * @retval      FSP_SUCCESS             If both the connectivity checks are success.
 * @retval      Any other error         If one of the connectivity check fails.
 *********************************************************************************************************************/
fsp_err_t connect_wifi(void)
{
    fsp_err_t ret = FSP_SUCCESS;
    ret = wifi_init ();
    if (ret != FSP_SUCCESS)
    {
        // FAILURE_INDICATION
        configASSERT( ret == FSP_SUCCESS );
    }

    memcpy(pNwkParam.ucSSID, g_ssid, strlen(g_ssid));
    pNwkParam.ucSSIDLength = (uint8_t)strlen((char *)pNwkParam.ucSSID);
    APP_PRINT("\r\n SSID: %s",pNwkParam.ucSSID);


    if( strcmp(g_secure, "None") == 0 )
    {
        pNwkParam.xSecurity = eWiFiSecurityOpen;
    }
    else
    {
        if( strcmp(g_secure, "WPA") == 0 )
        {
            pNwkParam.xSecurity=eWiFiSecurityWPA;
        }
        else
        {
            pNwkParam.xSecurity=eWiFiSecurityWPA2;
        }
        /* WiFi AP password input for non-open security type */
        memcpy(pNwkParam.xPassword.xWPA.cPassphrase,g_pass,strlen(g_pass));
        pNwkParam.xPassword.xWPA.ucLength = (uint8_t)strlen(pNwkParam.xPassword.xWPA.cPassphrase);
        APP_PRINT("\r\nPassword: %s",pNwkParam.xPassword.xWPA.cPassphrase);
    }
    /* Connecting to user entered SSID */
    APP_PRINT("\r\nConnecting to %s \r\n", pNwkParam.ucSSID);
    /* Connecting to AP via user entered SSID */
    if (eWiFiSuccess != WIFI_ConnectAP(&pNwkParam))
    {
        APP_PRINT("[Error]** Connecting to Wi-Fi AP failed ** \r\n");
        configASSERT( ret == FSP_SUCCESS );
    }
    ret = network_connectivity_check();
    if (FSP_SUCCESS != ret)
    {
        // FAILURE_INDICATION
        APP_PRINT("[Error] Wi-Fi Channel not Open.\r\n");
        configASSERT( ret == FSP_SUCCESS );
    }
    APP_PRINT("\r\nWi-Fi connected to SSID %s.\r\n",g_wifi_nwk.ssid);

    ret = device_ip_info_check();
    if (FSP_SUCCESS != ret)
    {
        // FAILURE_INDICATION
        APP_PRINT("[Error]Get device IP information fail.\r\n");
        configASSERT( ret == FSP_SUCCESS );
    }
    return ret;
}
