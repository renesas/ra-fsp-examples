/***********************************************************************************************************************
 * File Name    : wifi_ep.c
 * Description  : Contains data structures and functions used in wifi_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "wifi_ep.h"

/* Global variables */
WiFiParameters_t g_wifi =
{
    .ssid           = {'\0'},
    .pwd            = {'\0'},
    .security       = RESET_VALUE,
    .ip_addr_device = {RESET_VALUE},
    .status         = RESET_VALUE
};

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
 * @brief     This function opens and configures the Wi-Fi module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Opened and configured successfully.
 * @retval    Any other error codes     Failed to open or configure.
 **********************************************************************************************************************/
fsp_err_t wifi_open(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = rm_wifi_onchip_silex_open(&g_wifi_onchip_silex_cfg);
    if (FSP_SUCCESS == err)
    {
        /* Reset WiFi status flag */
        g_wifi.status = FSP_SUCCESS;
    }

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
 * @brief     This function sets the SSID and security type from a scan result.
 * @param[in] index                     Scan result index.
 * @retval    None.
 **********************************************************************************************************************/
void wifi_set_ap_ssid(uint32_t index)
{
    strcpy(g_wifi.ssid, (char*)g_scan_data[index].ucSSID);
    g_wifi.security = g_scan_data[index].xSecurity;
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
 * @brief     This function disconnects from the Wi-Fi network and closes the Wi-Fi module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void wifi_deinit(void)
{
    /* Close the connected Wi-Fi */
    if (FSP_SUCCESS == socket_connected())
    {
        rm_wifi_onchip_silex_close();
    }
}

/*******************************************************************************************************************//**
 * @brief     This function converts the input string to lowercase.
 * @param[in] p_str                     Pointer to string of uint8_t data-type.
 * @retval    None.
 **********************************************************************************************************************/
void str_to_lowercase(uint8_t * p_str)
{
    uint8_t i = RESET_VALUE;

    while ((p_str[i] != '\0') && (i < BUFF_LEN))
    {
        p_str[i] = (uint8_t)tolower(p_str[i]);
        i++;
    }
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
