/***********************************************************************************************************************
 * File Name    : wifi_ep.c
 * Description  : Contains data structures and functions used in wifi_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2024 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "wifi_ep.h"

/* Global variables */
WIFIScanResult_t g_scan_data[MAX_WIFI_SCAN_RESULTS];
WIFIIPConfiguration_t g_ip_config;
WIFINetworkParams_t g_net_params;

/* Private functions */
static WIFIReturnCode_t wifi_get_ip_info(void);
static fsp_err_t wifi_check_connect(void);

/*******************************************************************************************************************//**
 * @brief     This function initializes the Wi-Fi module.
 * @param[in] None.
 * @retval    eWiFiSuccess              Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
WIFIReturnCode_t wifi_init(void)
{
    /* Error status */
    WIFIReturnCode_t err = eWiFiSuccess;

    /* Initialize the Wi-Fi module */
    err = WIFI_On();

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function configures the Wi-Fi connection.
 * @param[in] None.
 * @retval    eWiFiSuccess              Configured successfully.
 * @retval    Any other error codes     Failed to configure.
 **********************************************************************************************************************/
WIFIReturnCode_t wifi_config(void)
{
    /* Error status */
    WIFIReturnCode_t err = eWiFiSuccess;

    /* Connect to the Access Point */
    err = WIFI_ConnectAP(&g_net_params);
    if (eWiFiSuccess != err)
    {
        return err;
    }

    /* Verify connection */
    if (FSP_SUCCESS != wifi_check_connect())
    {
        return eWiFiFailure;
    }

    /* Get the IP configuration */
    err = wifi_get_ip_info();

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function checks if the Wi-Fi Access Point is connected.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Connected successfully.
 * @retval    Any other error codes     Failed to connect.
 **********************************************************************************************************************/
static fsp_err_t wifi_check_connect(void)
{
    /* Connect to the Access Point */
    if (eWiFiSuccess != WIFI_IsConnected(&g_net_params))
    {
        return FSP_ERR_INTERNAL;
    }

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief     This function gets IP configuration.
 * @param[in] None.
 * @retval    eWiFiSuccess              Got successfully.
 * @retval    Any other error codes     Failed to get.
 **********************************************************************************************************************/
static WIFIReturnCode_t wifi_get_ip_info(void)
{
    /* Error status */
    WIFIReturnCode_t err = eWiFiSuccess;
    memset(&g_ip_config, RESET_VALUE, sizeof(WIFIIPConfiguration_t));

    /* Get IP configuration */
    err = WIFI_GetIPInfo(&g_ip_config);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function performs a Wi-Fi network scan.
 * @param[in] None.
 * @retval    eWiFiSuccess              Scanned successfully.
 * @retval    Any other error codes     Failed to scan.
 **********************************************************************************************************************/
WIFIReturnCode_t wifi_scan(void)
{
    /* Error status */
    WIFIReturnCode_t err = eWiFiSuccess;

    /* Scan Wi-Fi networks */
    memset(g_scan_data, RESET_VALUE, sizeof(g_scan_data));
    err = WIFI_Scan(&g_scan_data[0], MAX_WIFI_SCAN_RESULTS);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function de-initializes the Wi-Fi module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void wifi_deinit(void)
{
    /* Disconnect the connected Wi-Fi network */
    if (FSP_SUCCESS == wifi_check_connect())
    {
        WIFI_Disconnect();
    }

    /* Close the Wi-Fi module */
    WIFI_Off();
}

/*******************************************************************************************************************//**
 * @brief     This function sets SSID from scan result index.
 * @param[in] index                     Scan result index.
 * @retval    None.
 **********************************************************************************************************************/
void wifi_set_ap_ssid(uint32_t index)
{
    strcpy((char *)g_net_params.ucSSID, (char *)g_scan_data[index].ucSSID);
    g_net_params.ucSSIDLength = (uint8_t)strlen((char *)g_net_params.ucSSID);
    g_net_params.xSecurity = g_scan_data[index].xSecurity;
}

/*******************************************************************************************************************//**
 * @brief     This function sets SSID and security type from manual user input.
 * @param[in] ssid                      SSID type.
 * @param[in] security                  Security type.
 * @retval    None.
 **********************************************************************************************************************/
void wifi_set_manual_ssid(const char * ssid, WIFISecurity_t security)
{
    strncpy((char *)g_net_params.ucSSID, ssid, wificonfigMAX_SSID_LEN);
    g_net_params.ucSSIDLength = (uint8_t)strlen((char *)g_net_params.ucSSID);
    g_net_params.xSecurity = security;
}

/*******************************************************************************************************************//**
 * @brief     This function sets Wi-Fi Access Point password.
 * @param[in] password                  Char pointer pointing to password buffer.
 * @retval    None.
 **********************************************************************************************************************/
void wifi_set_password(const char * password)
{
    strncpy(g_net_params.xPassword.xWPA.cPassphrase, password, wificonfigMAX_PASSPHRASE_LEN);
    g_net_params.xPassword.xWPA.ucLength = sizeof(g_net_params.xPassword.xWPA.cPassphrase);
}

/*******************************************************************************************************************//**
 * @brief     This function returns a pointer to the stored SSID string.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
const char * wifi_get_ssid(void)
{
    return (const char *)g_net_params.ucSSID;
}

/*******************************************************************************************************************//**
 * @brief     This function returns the security type of stored Access Point configuration.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
WIFISecurity_t wifi_get_security(void)
{
    return g_net_params.xSecurity;
}
