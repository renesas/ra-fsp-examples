/***********************************************************************************************************************
 * File Name    : usr_wifi.h
 * Description  : Contains declarations of data structures and functions used in usr_wifi.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright [2015-2020] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
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

#ifndef USR_WIFI_H_
#define USR_WIFI_H_

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

/* Macros used in the usr_wifi Data Structures */
#define BUFF_LEN                    (32U)
#define IP_V4                       (4U)

/* Macros and variables for network operations */
/* Macro for WiFi error return */
#define WIFI_ONCHIP_SILEX_ERR_ERROR (-1)

/* Macros for array indexing */
#define ZERO      (0U)
#define ONE       (1U)
#define TWO       (2U)
#define THREE     (3U)

/* Macro for Integer to Ascii conversion of IP address */
#define FreeRTOS_inet_ntoa( ulIPAddress, pucBuffer )                                      \
                             sprintf(( char * ) ( pucBuffer ), "%u.%u.%u.%u",             \
                                    (( unsigned ) (( ulIPAddress ) & 0xffUL )),           \
                                    (( unsigned ) ((( ulIPAddress ) >> 8 ) & 0xffUL )),   \
                                    (( unsigned ) ((( ulIPAddress ) >> 16 ) & 0xffUL )),  \
                                    (( unsigned ) (( ulIPAddress ) >> 24 )))

/*********************************************************************************************************************
 *Structure for WiFi parameters Such as SSID, Password, Security Type and storing the IP address obtained from DHCP  *
 *and Wi-Fi Status                                                                                                   *
 *********************************************************************************************************************/
typedef struct
{
    char ssid[BUFF_LEN];
    char pwd[BUFF_LEN];
    uint32_t security;
    uint8_t device_ip_addr[IP_V4];
    fsp_err_t status;
} wifi_param_t;

/**********************************************************************************************************************
 * Structure for network parameters Network ID, Server IP and connection status                                       *
 **********************************************************************************************************************/
typedef struct
{
    uint32_t id;
    uint32_t server_ip;
    uint32_t status;
} nwk_param_t;

extern wifi_param_t g_wifi;
extern nwk_param_t g_network;

/***********************************************************************************************************************
 * User-defined Functions Declaration                                                                                  *
 ***********************************************************************************************************************/
extern fsp_err_t wifi_init(void);
extern fsp_err_t wifi_connect(void);
extern fsp_err_t dns_query(char *dns, uint8_t *ip_addr);
extern fsp_err_t wifi_deinit(void);
extern fsp_err_t network_connectivity_check(void);
extern fsp_err_t usr_network_connectivity_check(void);

#endif /* USR_WIFI_H_ */
