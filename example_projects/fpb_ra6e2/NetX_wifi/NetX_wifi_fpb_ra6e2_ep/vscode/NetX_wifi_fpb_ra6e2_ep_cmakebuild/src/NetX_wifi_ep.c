/***********************************************************************************************************************
 * File Name    : NetX_wifi_ep.c
 * Description  : Contains data structures and functions used in the EP
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "NetX_wifi_ep.h"

WiFiParameters_t g_wifi =
{
 .ssid = {NULL_CHAR},
 .pwd = {NULL_CHAR},
 .security = RESET_VALUE,
 .ip_addr_device = {RESET_VALUE},
 .status = RESET_VALUE
};

/* Buffer for storing Wi-Fi AP scan result */
WIFIScanResult_t g_scan_data[MAX_WIFI_SCAN_RESULTS] =
{
    {
        .ucSSID       = {RESET_VALUE},
        .ucSSIDLength = RESET_VALUE,
        .ucBSSID      = {RESET_VALUE},
        .xSecurity    = RESET_VALUE,
        .cRSSI        = RESET_VALUE,
        .ucChannel    = RESET_VALUE
    }
};

/* Private functions */
static fsp_err_t wifi_connect(void);
static fsp_err_t wifi_get_ip_addr(void);

/*******************************************************************************************************************//**
 * @brief     This function opens the Wi-Fi module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Opened successfully.
 * @retval    Any other error codes     Failed to open.
 **********************************************************************************************************************/
fsp_err_t wifi_open(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = rm_wifi_onchip_silex_open(&g_wifi_onchip_silex_cfg);

    g_wifi.status = err;

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function configures the Wi-Fi module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Configured successfully.
 * @retval    Any other error codes     Failed to configure.
 **********************************************************************************************************************/
fsp_err_t wifi_config(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = wifi_connect();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = socket_connected();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = wifi_get_ip_addr();

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function checks whether the Wi-Fi module is connected to an Access Point.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Connected successfully.
 * @retval    Any other error codes     Failed to connect.
 **********************************************************************************************************************/
fsp_err_t socket_connected(void)
{
    rm_wifi_onchip_silex_socket_connected(&g_wifi.status);
    return g_wifi.status;
}

/*******************************************************************************************************************//**
 * @brief     This function connects to the Wi-Fi Access Point.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Connected successfully.
 * @retval    Any other error codes     Failed to connect.
 **********************************************************************************************************************/
static fsp_err_t wifi_connect(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = rm_wifi_onchip_silex_connect(g_wifi.ssid, g_wifi.security, g_wifi.pwd);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function gets the assigned module IP address.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Got successfully.
 * @retval    Any other error codes     Failed to get.
 **********************************************************************************************************************/
static fsp_err_t wifi_get_ip_addr(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = rm_wifi_onchip_silex_ip_addr_get(g_wifi.ip_addr_device);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function scans for available Wi-Fi Access Point.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Scanned successfully.
 * @retval    Any other error codes     Failed to scan.
 **********************************************************************************************************************/
fsp_err_t wifi_scan(void)
{
    fsp_err_t err = FSP_SUCCESS;

    memset(g_scan_data, RESET_VALUE, sizeof(g_scan_data));
    err = rm_wifi_onchip_silex_scan(g_scan_data, MAX_WIFI_SCAN_RESULTS);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function sets the SSID and security type from a scan result.
 * @param[in] index                     Scan result index.
 * @retval    None.
 **********************************************************************************************************************/
void wifi_set_ap_ssid(uint32_t index)
{
    memcpy(g_wifi.ssid, (char *)g_scan_data[index].ucSSID, sizeof(g_wifi.ssid));
    g_wifi.security = g_scan_data[index].xSecurity;
}

/*******************************************************************************************************************//**
 * @brief     This function turns the on-board LED ON or OFF.
 * @param[in] b_value                   LED state.
 * @retval    None.
 **********************************************************************************************************************/
void set_led(bsp_io_level_t b_value)
{
    R_IOPORT_PinWrite(g_ioport.p_ctrl, USER_LED, b_value);
}

/*******************************************************************************************************************//**
 * @brief     This function converts the input string pointed by the pointer to lower case.
 * @param[in] p_str                    Pointer to string of uint8_t data-type.
 * @retval    None.
 **********************************************************************************************************************/
void convert_to_lowercase(uint8_t * p_str)
{
    uint8_t index = RESET_VALUE;
    if (NULL != p_str)
    {
        for (index = RESET_VALUE; (NULL_CHAR != p_str[index]); index++)
        {
            p_str[index] = (uint8_t)tolower(p_str[index]);
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function closes the connected Wi-Fi module.
 * @param[in] None
 * @retval    None
 **********************************************************************************************************************/
void wifi_deinit(void)
{
    /* Check Wi-Fi connectivity */
    if (FSP_SUCCESS == socket_connected())
    {
        rm_wifi_onchip_silex_close();
    }
}
