/***********************************************************************************************************************
 * File Name    : usr_wifi.c
 * Description  : Contains data structures and functions used in Wi-Fi Configs for Cloud Application Project.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright [2015-2020] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
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

#include "common_utils.h"
#include "usr_wifi.h"
#include "usr_config.h"
#include <rm_wifi_onchip_silex.h>
#include "mqtt_interface.h"

static fsp_err_t wifi_preinit(void);
static WIFISecurity_t user_wifi_security(char *value);

wifi_param_t g_wifi = { .ssid = WIFI_SSID, .pwd = WIFI_PW, .security = RESET_VALUE,
	                    .device_ip_addr = { RESET_VALUE }, .status = PRE_INIT_VALUE
	                  };

nwk_param_t g_network = { .id = RESET_VALUE, .server_ip = RESET_VALUE, .status = PRE_INIT_VALUE };

/***********************************************************************************************************************
 * @brief       This functions checks both WiFi connectivity and TCP socket connectivity flags.
 * @param[in]   None
 * @retval      FSP_SUCCESS             If both the connectivity checks are success.
 * @retval      FSP_ERR_ABORTED         If one of the connectivity check fails.
 **********************************************************************************************************************/
fsp_err_t usr_network_connectivity_check(void)
{
    fsp_err_t status = FSP_SUCCESS;

    if ((FSP_SUCCESS != g_wifi.status) && (FSP_SUCCESS != g_network.status))
    {
        status = FSP_ERR_ABORTED;
    }
    return status;
}

/***********************************************************************************************************************
 * @brief       This functions checks both WiFi connectivity and TCP socket connectivity.
 * @param[in]   None
 * @retval      FSP_SUCCESS             If both the connectivity checks are success.
 * @retval      Any other error         If one of the connectivity check fails.
 **********************************************************************************************************************/
fsp_err_t network_connectivity_check(void)
{
    fsp_err_t err = FSP_SUCCESS;

    if (pdTRUE != WIFI_IsConnected())
    {
        IotLogError("** Wi-Fi is not connected \r\n");
        err = FSP_ERR_INTERNAL;
    }
    return err;
}

/***********************************************************************************************************************
 * @brief       This functions resolves the DNS query for the domain name and gets Endpoint IP address.
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
            IotLogError("** Host IP Lookup failed **\r\n");
            err = FSP_ERR_INTERNAL;
        }
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This functions takes user  WiFi AP's SSID, Password and security type 
 *              credentials. And connects to the SSID.
 * @param[IN]   None.
 * @retval      FSP_SUCCESS                 Upon successful completion of the function.
 * @retval      Any Other Error code        If WiFi AP connect fails.
 **********************************************************************************************************************/
fsp_err_t wifi_connect(void)
{
    fsp_err_t err = FSP_SUCCESS;
	WIFINetworkParams_t pNwkParam = {RESET_VALUE};
    pNwkParam.pcSSID = (char *)&g_wifi.ssid;
    pNwkParam.pcPassword = (char *)&g_wifi.pwd;
	pNwkParam.ucSSIDLength = (uint8_t)strlen(g_wifi.ssid);
    pNwkParam.ucPasswordLength = (uint8_t)strlen(g_wifi.pwd);
	pNwkParam.xSecurity = g_wifi.security;


    /* Connecting to AP via user entered SSID */
    if (eWiFiSuccess != WIFI_ConnectAP(&pNwkParam))
    {
        IotLogError("** Connecting to Wi-Fi AP failed ** \r\n");
		err = FSP_ERR_WIFI_FAILED;
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This functions validates and Iniitalizes the WiFi user credential settings.
 * @param[in]   None
 * @retval      error code: FSP_SUCCESS or FSP_ERR_UNSUPPORTED
 **********************************************************************************************************************/
static fsp_err_t wifi_preinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    strcpy (g_wifi.ssid, WIFI_SSID);
    strcpy (g_wifi.pwd, WIFI_PW);
    g_wifi.security = user_wifi_security (SECURITY);

    /* Handle error */
    if ((0 == strlen (g_wifi.ssid)) || (eWiFiSecurityNotSupported == g_wifi.security))
    {
        IotLogError("** Wi-Fi Credentials are not initialized \r\n");
        err = FSP_ERR_NOT_INITIALIZED;
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This functions Iniitalizes the WiFi module.
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
        IotLogError("** Wi-Fi Preinit failed  **\r\n");
        return err;
    }

    /* Opening WiFi module and Handle the Error*/
    if (eWiFiSuccess == WIFI_On())
    {
        IotLog("!!! Wi-Fi Init Successful !!!**\r\n");
    }
    else
    {   
        err = FSP_ERR_NOT_INITIALIZED;
        IotLogError("** Wi-Fi Init Failure **\r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This functions disconnects the socket connection and closes the WiFi module.
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
        IotLogError("** Wi-Fi is not connected **\r\n");
        return err;
    }

    if (FSP_SUCCESS == g_network.status)
    {
        /* If socket connected, then disconnecting */
        if(eWiFiSuccess != WIFI_Disconnect ())
        {
            IotLogError("** Wi-Fi Disconnect Failed **\r\n");
            return FSP_ERR_WIFI_FAILED;
        }
    }
    else
    {
        IotLogError("** SOCKET STATUS IS NOT CONNECTED **\r\n");
        return FSP_ERR_WIFI_INIT_FAILED;
    }


    /* Closing WiFi module. */
    /* Handle error */
    if (eWiFiSuccess != WIFI_Off())
    {
        IotLogError("** Closing the Wi-Fi Module failed **\r\n");
        return FSP_ERR_WIFI_FAILED;
    }
    g_network.status = !(FSP_SUCCESS);
    g_wifi.status = !(FSP_SUCCESS);
    return err;
}

/*******************************************************************************************************************//**
 * @brief       This functions converts the security type selected by the user to supported security type IOT SDK.
 * @param[in]   value                   Security type configured by the user.
 * @retval      WIFISecurity_t          Security type supported by AWS Wi-Fi for IOT SDK (WIFISecurity_t).
 **********************************************************************************************************************/
static WIFISecurity_t user_wifi_security(char *value)
{
    if (0 == strcmp (value, "Open"))
    {
        return eWiFiSecurityOpen;
    }
    else if (0 == strcmp (value, "WEP"))
    {
        return eWiFiSecurityWEP;
    }
    else if (0 == strcmp (value, "WPA"))
    {
        return eWiFiSecurityWPA;
    }
    else if (0 == strcmp (value, "WPA2"))
    {
        return eWiFiSecurityWPA2;
    }
    else if (0 == strcmp (value, "WPA2_ent"))
    {
        return eWiFiSecurityWPA2_ent;
    }
    else
    {
        return eWiFiSecurityNotSupported;
    }
}

