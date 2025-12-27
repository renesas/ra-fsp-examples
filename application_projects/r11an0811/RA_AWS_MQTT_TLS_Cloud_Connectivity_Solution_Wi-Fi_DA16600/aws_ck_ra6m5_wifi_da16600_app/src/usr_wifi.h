/***********************************************************************************************************************
 * File Name    : usr_wifi.h
 * Description  : Contains declarations of data structures and functions used in usr_wifi.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2015 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef USR_WIFI_H_
#define USR_WIFI_H_

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/* Macros used in the usr_wifi Data Structures */
#define BUFF_LEN                    (32U)

/* Macros and variables for network operations */
#define BYTES_RECEIVED_ZERO         (0U)

/* Macros for array indexing */
#define ZERO      (0U)
#define ONE       (1U)
#define TWO       (2U)
#define THREE     (3U)

/* Macros and variables for network operations */
#define MAX_WIFI_SCAN_RESULTS       (10U)

/* Macros for supported WiFi security types */
#define WIFI_SECURITY_OPEN          (0U)
#define WIFI_SECURITY_WPA           (1U)
#define WIFI_SECURITY_WPA2          (2U)

/* Macro for Wi-Fi Connect Menu Options */
#define SCAN_AP                     (1U)
#define ENTER_SSID                  (2U)
#define RUN_PRE_CONFIG              (3U)
#define PROVISIONING_MODE           (4U)

/* Macro for Integer to Ascii conversion of IP address */
#define FREERTOS_INET_NTOA( ulIPAddress, pucBuffer )                                      \
                             (sprintf(( char * ) ( pucBuffer ), "%u.%u.%u.%u",             \
                                    (( unsigned ) (( ulIPAddress ) & 0xffUL )),           \
                                    (( unsigned ) ((( ulIPAddress ) >> 8 ) & 0xffUL )),   \
                                    (( unsigned ) ((( ulIPAddress ) >> 16 ) & 0xffUL )),  \
                                    (( unsigned ) (( ulIPAddress ) >> 24 ))))

/*********************************************************************************************************************
 *Structure for WiFi parameters Such as SSID, Password, Security Type and storing the IP address obtained from DHCP  *
 *and Wi-Fi Status                                                                                                   *
 *********************************************************************************************************************/
typedef struct
{
    char ssid[BUFF_LEN];
    WIFIIPConfiguration_t device_ip_config;
    uint32_t server_ip;
    fsp_err_t status;
} wifi_nwk_param_t;

extern wifi_nwk_param_t g_wifi_nwk;

/***********************************************************************************************************************
 * User-defined Functions Declaration                                                                                  *
 ***********************************************************************************************************************/
extern fsp_err_t wifi_init(void);
extern fsp_err_t dns_query(char *dns, uint8_t *ip_addr);
extern fsp_err_t wifi_deinit(void);
extern fsp_err_t network_connectivity_check(void);
extern fsp_err_t device_ip_info_check(void);
fsp_err_t connect_wifi(void);

#endif /* USR_WIFI_H_ */
