/***********************************************************************************************************************
 * File Name    : wifi_ep.h
 * Description  : Contains declarations of data structures and functions used in wifi_ep.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2022 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef WIFI_EP_H_
#define WIFI_EP_H_

#include "wifi_thread.h"
#include "ctype.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

#define EP_INFO             "\r\nThe example project demonstrates the functionalities of WiFi_OnChip_Silex"\
                            "\r\nmodule running on Renesas RA MCUs using Renesas Silex UART Pmod hardware."\
                            "\r\nOptions are provided to the user to either connect to a known Wi-Fi network"\
                            "\r\nmanually or by scanning available networks on the J-Link RTT Viewer."\
                            "\r\nA ping operation on a user-specified URL or IP address is performed to check the"\
                            "\r\ninternet connectivity. On successful validation, the user is now allowed to"\
                            "\r\nconnect to the UDP server by providing the UDP server's IP address and port"\
                            "\r\nnumber as inputs. A ping operation is performed to validate the connectivity"\
                            "\r\nto the server. On successful validation, EP creates a UDP socket, establishes"\
                            "\r\na connection, and sends a \"Hello\" message to the UDP server. The EP now"\
                            "\r\ncontinuously waits for any incoming messages from the UDP server. The incoming"\
                            "\r\nmessage is displayed on the J-Link RTT Viewer.\r\n"

/* Macros and variables for RTT operations */
#define BUFF_LEN                    (32U)
#define BYTES_RECEIVED_ZERO         (0U)

/* Macros for supported WiFi security types */
#define WIFI_SECURITY_OPEN          (0U)
#define WIFI_SECURITY_WPA           (1U)
#define WIFI_SECURITY_WPA2          (2U)

/* Macros and variables for network operations */
#define WIFI_MAX_TRY                (5U)
#define MAX_WIFI_SCAN_RESULTS       (20U)
#define PING_COUNT                  (4U)
#define PINT_INTERVAL_MS            (100U)
#define SOCKET_UDP                  (2U)
#define IPV4                        (4U)
#define DEFAULT_SSID_LEN            (17U)
#define DEFAULT_PASSWORD_LEN        (8U)
#define DEFAULT_SSID                "access_point_ssid"
#define DEFAULT_PASSPHRASE          "password"

/* Macros for WiFi operation Timeout */
#define WIFI_ONCHIP_SILEX_TIMEOUT_400MS                     (400U)
#define WIFI_ONCHIP_SILEX_TIMEOUT_15SEC                     (15000U)
#define WIFI_ONCHIP_SILEX_TIMEOUT_20SEC                     (20000U)

/* Macro for WiFi error return */
#define WIFI_ONCHIP_SILEX_ERR_ERROR                         (-1)

/* Converting from MCU byte-order to network byte-order */
#define SOCKETS_HTONL( ulIn )    ( ( uint32_t ) ( ( ( (ulIn) & 0xFF ) << 24 ) | ( ( (ulIn) & 0xFF00 ) << 8 )\
		                         | ( ( (ulIn) & 0xFF0000 ) >> 8 ) | ( ( (ulIn) & 0xFF000000 ) >> 24 ) ) )

/* Macros for Menu Options */
#define SCAN_AP                 (1U)
#define ENTER_SSID              (2U)
#define EXIT                    (3U)

/* Macros for array indexing */
#define INDEX_ZERO              (0U)
#define INDEX_ONE               (1U)
#define INDEX_TWO               (2U)
#define INDEX_THREE             (3U)
#define IP_BUFF_LEN             (4U)
#define SHIFT_BY_8              (8U)
#define SHIFT_BY_16             (16U)
#define SHIFT_BY_24             (24U)

#define UDP_PORT_MIN            (4096U)
#define UDP_PORT_MAX            (65535U)

#define MENU_OPTIONS            "\r\nMenu:"                                                 \
                                "\r\nPress 1 to scan for Wi-Fi Access Points"               \
								"\r\nPress 2 to enter Wi-Fi Access Point's credentials"     \
								"\r\nUser Input: \r\n"

#define SECURITY_MENU_OPTIONS   "\r\nSelect security type for the WiFi Access Point:"       \
                                "\r\nWiFi Security types:"                                  \
                                "\r\nPress 0 for Open - No Security"                        \
                                "\r\nPress 1 for WPA Security"                              \
                                "\r\nPress 2 for WPA2 Security"                             \
                                "\r\nUser Input: \r\n"

#define CR                      ('\r')
#define LF                      ('\n')
#define NULL_CHAR               ('\0')
#define MIN_ZERO                ('0')
#define MAX_TWO                 ('2')
#define MAX_NINE                ('9')
#define ONE_SEC_DELAY           (1)

/* Structure for socket parameters */
typedef struct
{
    uint32_t id;
    uint32_t ip_addr_server;
    uint32_t port;
    uint8_t send_buff[BUFF_LEN];
    uint8_t recv_buff[BUFF_LEN];
    uint32_t status;
}SocketPrameters_t;

/***********************************************************************************************************************
 * User-defined APIs
 **********************************************************************************************************************/

/* Function declarations */
void user_input(char * user_buff);
WIFIReturnCode_t scan_and_select(void);
WIFIReturnCode_t manual_connect(void);
WIFIReturnCode_t dns_query(char * dns, uint8_t * ip_addr);
WIFIReturnCode_t ping(uint8_t * ip_addr);
fsp_err_t udp_socket_create();
fsp_err_t udp_socket_connect(uint32_t ip_addr, uint32_t port);
int32_t udp_send_data(uint8_t * p_data);
int32_t udp_recv_data(uint8_t * p_data);
fsp_err_t network_connectivity_check(void);
void wifi_deinit(void);

#endif /* WIFI_EP_H_ */
