/***********************************************************************************************************************
 * File Name    : wifi_app.h
 * Description  : Contains declarations of data structures and functions used in wifi_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef WIFI_APP_H_
#define WIFI_APP_H_

#include "wifi_thread.h"
#include "ctype.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

#define APP_INFO                                                                       \
    "\r\nThe Application project demonstrates the basic functionalities of the user "  \
    "\r\npack, created for Sierra Wi-Fi module running on Renesas RA MCUs.   "         \
    "\r\nThe App project connects to the Wi-Fi Access Point and gets the IP address  " \
    "\r\nfrom the Access Point. The acquired IP address is displayed on the console. " \
    "\r\nAlso User can ping to the Device address from PC to validate the connectivity. \r\n \r\n"

/* Macros and variables for RTT operations */
#define BUFF_LEN                        (32U)

/* Macro for WiFi error return */
#define WIFI_ONCHIP_SIERRA_ERR_ERROR    (-1)

/* Macros for array indexing */
#define INDEX_ZERO                      (0U)
#define INDEX_ONE                       (1U)
#define INDEX_TWO                       (2U)
#define INDEX_THREE                     (3U)
#define IP_BUFF_LEN                     (4U)

#define PING_COUNT                      (4U)
#define PING_INTERVAL_MS                (10U)

#define WIFI_SSID                       "Renesas"
#define WIFI_PW                         "@Renesas123"
#define SECURITY                        "WPA2"

#define FAILURE_INDICATION              led_on_off(LED_RED, LED_ON);
#define FAILURE_RECOVERED_INDICATION    led_on_off(LED_RED, LED_OFF);
#define NETWORK_CONNECT_INDICATION      led_on_off(LED_BLUE, LED_ON);
#define NETWORK_ACTIVITY_INDICATION     led_toggle(LED_BLUE);
#define APP_ACTIVITY_INDICATION         led_toggle(LED_GREEN);

/* Structure for WiFi parameters */
typedef struct
{
    char      ssid[BUFF_LEN];
    char      pwd[BUFF_LEN];
    uint32_t  security;
    uint32_t  device_ip_addr[4];
    fsp_err_t status;
} WiFiParameters_t;

typedef enum e_led_type
{
    LED_BLUE = 0, LED_GREEN = 1, LED_RED = 2,
} e_led_type_t;

/* LED Number to identify the LED as per BSP support for EK-RA6M3/G Board */
typedef enum e_led_state
{
    LED_OFF = 0, LED_ON = 1,
} e_led_state_t;

/***********************************************************************************************************************
 * User-defined APIs
 **********************************************************************************************************************/

/*
 * function declarations
 */
fsp_err_t      wifi_init(void);
fsp_err_t      wifi_preinit(void);
fsp_err_t      wifi_deinit(void);
fsp_err_t      wifi_connect(void);
fsp_err_t      ping(uint8_t * ip_addr);
fsp_err_t      network_connectivity_check(void);
WIFISecurity_t user_wifi_security(char * value);
void           led_on_off(e_led_type_t ltype, e_led_state_t lled_state);
void           led_toggle(e_led_type_t ltype);

#endif                                 /* WIFI_APP_H_ */
