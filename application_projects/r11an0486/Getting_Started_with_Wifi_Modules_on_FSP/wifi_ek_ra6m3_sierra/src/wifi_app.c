/***********************************************************************************************************************
 * File Name    : wifi_app.c
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

WiFiParameters_t g_wifi =
{
    .ssid           = {'\0'       },
    .pwd            = {'\0'       },
    .security       = RESET_VALUE,
    .device_ip_addr = {RESET_VALUE},
    .status         = RESET_VALUE
};

extern bsp_leds_t g_bsp_leds;

fsp_err_t wifi_preinit (void)
{
    fsp_err_t err = FSP_SUCCESS;

    strcpy(g_wifi.ssid, WIFI_SSID);
    strcpy(g_wifi.pwd, WIFI_PW);
    g_wifi.security = user_wifi_security(SECURITY);

    /* Handle error */
    if ((0 == strlen(g_wifi.ssid)) || (eWiFiSecurityNotSupported == g_wifi.security))
    {
        APP_ERR_PRINT("** Wi-Fi Credentials are not initialized \r\n");
        err = FSP_ERR_NOT_INITIALIZED;
    }

    return err;
}

fsp_err_t wifi_init (void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Opening WiFi module and Handle the Error*/
    if (eWiFiSuccess == WIFI_On())
    {
        APP_PRINT("**!!! Wi-Fi Init Successful !!!**\r\n");
    }
    else
    {
        err = FSP_ERR_NOT_INITIALIZED;
        APP_ERR_PRINT("** Wi-Fi Init Failure **\r\n");
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief       This functions disconnects the socket connection and closes the WiFi module.
 * @param[in]   None
 * @retval      fsp_err_t: FSP_SUCCESS, FSP_ERR_WIFI_INIT_FAILED, FSP_ERR_WIFI_FAILED
 **********************************************************************************************************************/
fsp_err_t wifi_deinit (void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Closing WiFi module. */
    /* Handle error */
    if (eWiFiSuccess != WIFI_Off())
    {
        APP_ERR_PRINT("** Closing the Wi-Fi Module failed **\r\n");

        return FSP_ERR_WIFI_FAILED;
    }

    g_wifi.status = !(FSP_SUCCESS);

    return err;
}

/*******************************************************************************************************************//**
 * @brief       This functions takes user  WiFi AP's SSID, Password and security type
 *              credentials. And connects to the SSID.
 * @param[IN]   None.
 * @retval      FSP_SUCCESS                 Upon successful completion of the function.
 * @retval      Any Other Error code        If WiFi AP connect fails.
 **********************************************************************************************************************/
fsp_err_t wifi_connect (void)
{
    fsp_err_t           err       = FSP_SUCCESS;
    WIFINetworkParams_t pNwkParam = {RESET_VALUE};
    memcpy(pNwkParam.ucSSID, g_wifi.ssid, strlen(g_wifi.ssid));
    memcpy(pNwkParam.xPassword.xWPA.cPassphrase, g_wifi.pwd, strlen(g_wifi.pwd));
    pNwkParam.ucSSIDLength            = (uint8_t) strlen(g_wifi.ssid);
    pNwkParam.xPassword.xWPA.ucLength = (uint8_t) strlen(g_wifi.pwd);
    pNwkParam.xSecurity               = g_wifi.security;

    /* Connecting to AP via user entered SSID */
    if (eWiFiSuccess != WIFI_ConnectAP(&pNwkParam))
    {
        APP_ERR_PRINT("** Connecting to Wi-Fi AP failed ** \r\n");
        err = FSP_ERR_WIFI_FAILED;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief       This functions ping the IP address.
 * @param[in]   p_ip_addr                   Pointer to IP address array.
 * @retval      FSP_SUCCESS                 Upon successful ping.
 * @retval      Any Other Error code        If ping fails.
 **********************************************************************************************************************/
fsp_err_t ping (uint8_t * p_ip_addr)
{
    fsp_err_t err = FSP_SUCCESS;

    rm_wifi_onchip_sierra_socket_connected(&g_wifi.status);
    if (FSP_SUCCESS != g_wifi.status)
    {
        APP_PRINT("\r\nWiFi not connected");

        return WIFI_ONCHIP_SIERRA_ERR_ERROR;
    }

    err = rm_wifi_onchip_sierra_ping(p_ip_addr, PING_COUNT, PING_INTERVAL_MS);
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\n** rm_wifi_onchip_sierra_ping API failed **\r\n");

        return err;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief       This functions converts the security type selected by the user to supported security type IOT SDK.
 * @param[in]   value                   Security type configured by the user.
 * @retval      WIFISecurity_t          Security type supported by AWS Wi-Fi for IOT SDK (WIFISecurity_t).
 **********************************************************************************************************************/
WIFISecurity_t user_wifi_security (char * value)
{
    if (0 == strcmp(value, "Open"))
    {
        return eWiFiSecurityOpen;
    }
    else if (0 == strcmp(value, "WEP"))
    {
        return eWiFiSecurityWEP;
    }
    else if (0 == strcmp(value, "WPA"))
    {
        return eWiFiSecurityWPA;
    }
    else if (0 == strcmp(value, "WPA2"))
    {
        return eWiFiSecurityWPA2;
    }
    else if (0 == strcmp(value, "WPA2_ent"))
    {
        return eWiFiSecurityWPA2_ent;
    }
    else
    {
        return eWiFiSecurityNotSupported;
    }
}

/***********************************************************************************************************************
 * @brief       This function turns ON/OFF the LED based on the type(Colored LED) and its requested action.
 *
 * @param[IN]   e_led_type    Type of LED, (GREEN, BLUE, RED).
 * @param[IN]   lled_state    Requested Action for the LED, (LED_OFF, LED_ON).
 * @retval      void
 **********************************************************************************************************************/
void led_on_off (e_led_type_t ltype, e_led_state_t lled_state)
{
    /* Get pin to toggle */
    uint32_t pin = RESET_VALUE;

    /* Holds level to set for pins */
    bsp_io_level_t pin_level = (bsp_io_level_t) LED_OFF;

    /* LED type structure */
    bsp_leds_t leds = g_bsp_leds;

    R_BSP_PinAccessEnable();

    if (((LED_ON == lled_state) || (LED_OFF == lled_state)) &&
        ((LED_GREEN == ltype) || (LED_BLUE == ltype) || (LED_RED == ltype)))
    {
        pin_level = (bsp_io_level_t) lled_state;
        pin       = leds.p_leds[ltype];

        /* Write to this pin */
        R_BSP_PinWrite((bsp_io_port_pin_t) pin, pin_level);
    }

    R_BSP_PinAccessDisable();
}

/***********************************************************************************************************************
 * @brief       This function toggles(Turns ON if it was OFF or vice versa) the LED, based on the type of LED(Colored LED).
 * The periodic toggle is based on Application code handling or any other mechanism(such as Timer).
 *
 * @param[IN]   e_led_type    Type of LED, (GREEN, BLUE, RED).
 * @retval      void
 **********************************************************************************************************************/
void led_toggle (e_led_type_t ltype)
{
    /* Get pin to toggle */
    uint32_t pin = RESET_VALUE;

    /* Holds level to set for pins */
    static bsp_io_level_t pin_level[3] = {(bsp_io_level_t) LED_OFF};

    /* LED type structure */
    bsp_leds_t leds = g_bsp_leds;

    R_BSP_PinAccessEnable();

    if (((LED_GREEN == ltype) || (LED_BLUE == ltype) || (LED_RED == ltype)))
    {
        pin = leds.p_leds[ltype];

        /* Write to this pin */
        R_BSP_PinWrite((bsp_io_port_pin_t) pin, pin_level[ltype]);

        if (LED_OFF == (e_led_state_t) pin_level[ltype])
        {
            pin_level[ltype] = (bsp_io_level_t) LED_ON;
        }
        else
        {
            pin_level[ltype] = (bsp_io_level_t) LED_OFF;
        }
    }

    R_BSP_PinAccessDisable();
}

/*******************************************************************************************************************//**
 * @} (end addtogroup wifi_ep)
 **********************************************************************************************************************/
