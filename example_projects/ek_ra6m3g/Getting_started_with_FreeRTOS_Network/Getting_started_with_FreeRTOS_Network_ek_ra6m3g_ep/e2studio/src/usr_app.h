/***********************************************************************************************************************
 * File Name    : usr_app.h
 * Description  : Contains macros, data structures and functions used  in the Application
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef USR_APP_H_
#define USR_APP_H_

#if( ipconfigUSE_DHCP != 0 )
    #include "FreeRTOS_DHCP.h"
#endif

#define USR_TEST_PING_IP                        ("8.8.8.8")
#define USR_PING_COUNT                          (10)
#define SOCKET_RECEIVE_TIMEOUT                  (60000)
#define SOCKET_SEND_TIMEOUT                     (5000)
#define MAC_ADDR_SIZE                           (6)
#define IP_ADDR_SIZE                            (4)
#define NETMASK_SIZE                            (4)
#define GATEWAY_ADDR_SIZE                       (4)
#define DNS_ADDR_SIZE                           (4)
#define DOMAIN_SIZE                             (32)

#define SUCCESS                                 (0)
#define PRINT_UP_MSG_DISABLE                    (0x01)
#define PRINT_DOWN_MSG_DISABLE                  (0x02)
#define PRINT_NWK_USR_MSG_DISABLE               (0x04)
#define MAX_PING_RETRIES                        (10)

#define PING_OPERATION                          ('1')
#define TCP_CLIENT_OPERATION                    ('2')
#define UDP_CLIENT_OPERATION                    ('3')
#define DNS_OPERATION                           ('4')
#define HTTP_SERVER                             ('5')
#define NUM_LEDS                                (2)

#define KEY_INDEX_SIZE                          (3)
#define REMOTE_INFO_SIZE                        (65)
#define USER_INPUT_SIZE                         (32)
#define DELAY_100_TICKS                         (100)
#define DELAY_10_TICKS                          (10)
#define SERVER_PORT                             (80)
#define SERVER_BACKLOG                          (12)

#define IP_LINK_UP                              (1U << 0)
#define IP_LINK_DOWN                            (1U << 1)
#define ETHERNET_LINK_UP                        (1U << 2)
#define ETHERNET_LINK_DOWN                      (1U << 3)

#define PING_TIMEOUT_MS                         (2000U)
#define PING_DATA_SIZE                          (32U)
#define PING_QUEUE_WAIT_TIME_MS                 (2000U)
#define PING_DELAY_MS                           (1000U)
#define USR_TEST_DOMAIN_NAME                    ("www.freertos.org")


#define HTTP_ROOT                               ("")
#define WEB_PAGE_FILE_NAME                      ("/index.html")
#define LOGO_FILE_NAME                          ("/logo.png")
#define LED_STATUS_FILE_NAME                    ("/led")
#define NETWORK_INFO_FILE_NAME                  ("/network")
#define LED1_STATUS_FILE_NAME                   ("/switch1")
#define LED2_STATUS_FILE_NAME                   ("/switch2")
#define GET_LED_COMMAND                         ("GET /led")
#define GET_LED_COMMAND_LENGTH                  (8)
#define GET_SWITCH1_COMMAND                     ("GET /switch1")
#define GET_SWITCH1_COMMAND_LENGTH              (12)
#define GET_SWITCH2_COMMAND                     ("GET /switch2")
#define GET_SWITCH2_COMMAND_LENGTH              (12)
#define WRITE_MODE                              ("w")
#define READ_MODE                               ("r")
#define BUFFER_SIZE                             (2048)
#define RECV_BUFFER_SIZE                        (128)
#define NETWORK_BUFFER_SIZE                     (1024)
#define LED_BUFFER_SIZE                         (512)
#define ZERO_INDEX                              (0)
#define LED_COLOR_ON                            ("btn-danger")
#define LED_COLOR_OFF                           ("btn-secondary")
#define LED_STATUS_ON                           ("ON")
#define LED_STATUS_OFF                          ("OFF")

#define EP_VERSION                              ("1.0")

#define BANNER_INFO             "\r\n********************************************************************************"\
                                "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module         *"\
                                "\r\n*   Example Project Version %s                                                *"\
                                "\r\n*   Flex Software Pack Version  %d.%d.%d                                          *"\
                                "\r\n********************************************************************************"\
                                "\r\nRefer to readme.txt file for more details on Example Project and              " \
                                "\r\nFSP User's Manual for more information about "MODULE_NAME"                    "\
                                "\r\n********************************************************************************\r\n"

#if defined (BOARD_RA6M3_EK) || defined (BOARD_RA6M3G_EK)
#define KIT_NAME                "EK-RA6M3"
#elif defined (BOARD_RA6M5_EK)
#define KIT_NAME                "EK-RA6M5"
#elif defined (BOARD_RA8M1_EK)
#define KIT_NAME                "EK-RA8M1"
#elif defined (BOARD_RA8D1_EK)
#define KIT_NAME                "EK-RA8D1"
#else
#define KIT_NAME                "EK-RA6M4"
#endif

#define MODULE_NAME             "Ethernet using FreeRTOS+TCP"

#define ETH_PREINIT             "\r\n \r\n--------------------------------------------------------------------------------"\
                                "\r\nEthernet adapter Configuration for Renesas "KIT_NAME": Pre IP Init       "\
                                "\r\n--------------------------------------------------------------------------------\r\n\r\n"

#define ETH_POSTINIT             "\r\n \r\n--------------------------------------------------------------------------------"\
                                "\r\nEthernet adapter Configuration for Renesas "KIT_NAME": Post IP Init       "\
                                "\r\n--------------------------------------------------------------------------------\r\n\r\n"


/* USB Drive HMSC Menu options */
#define MAIN_MENU               "\r\nSelect an option followed by the appropriate parameter:" \
                                "\r\n 1. Ping Request:   Specify the IP address to ping (e.g., 1 192.168.123.4)" \
                                "\r\n 2. TCP Client:     <Server_IP>:<Server_Port> (e.g., 2 192.168.123.4:1234)" \
                                "\r\n 3. UDP Client:     <Server_IP>:<Server_Port> (e.g., 3 192.168.123.4:1234)" \
                                "\r\n 4. DNS Resolution: <Domain_Name> (e.g., 4 www.example.com)" \
                                "\r\n 5. Start HTTP Server and launch a web browser to access the user interface"\
                                "\r\n\r\nPlease provide your selection: "

#define INNER_HTML_NET_ID       \
                                "\n"\
                                "<div class=\"col-6\">\n"\
                                "    <div class=\"mb-2 fw-semibold\">Physical Address</div>\n"\
                                "    <div class=\"mb-2 fw-semibold\">IPv4 Address</div>\n"\
                                "    <div class=\"mb-2 fw-semibold\">Subnet Mask</div>\n"\
                                "    <div class=\"mb-2 fw-semibold\">Default Gateway</div>\n"\
                                "</div>\n"\
                                "<div class=\"col-6\">\n"\
                                "    <div class=\"mb-2 fw-semibold\">%.2x-%.2x-%.2x-%.2x-%.2x-%.2x</div>\n"\
                                "    <div class=\"mb-2 fw-semibold\">%d.%d.%d.%d</div>\n"\
                                "    <div class=\"mb-2 fw-semibold\">%d.%d.%d.%d</div>\n"\
                                "    <div class=\"mb-2 fw-semibold\">%d.%d.%d.%d</div>\n"\
                                "</div>\n"

#define INNER_HTML_LED_ID       \
                                "\n"\
                                "<div class=\"col-2\"><span class=\"btn %s rounded-circle\" style=\"width: 40px; height: 40px;\"></span ></div>\n" \
                                "<div class=\"col-4 d-flex align-items-center\">LED1 %s</div>\n" \
                                "<div class=\"col-2\"><span class=\"btn %s rounded-circle\" style=\"width: 40px; height: 40px;\"></span ></div>\n" \
                                "<div class=\"col-4 d-flex align-items-center\">LED2 %s</div>\n"

#define INNER_HTML_LED1_ID      \
                                "\n"\
                                "<div class=\"col-2\"><span class=\"btn %s rounded-circle\" style=\"width: 40px; height: 40px;\"></span ></div>\n" \
                                "<div class=\"col-4 d-flex align-items-center\">LED1 %s</div>\n"

#define INNER_HTML_LED2_ID      \
                                "\n"\
                                "<div class=\"col-2\"><span class=\"btn %s rounded-circle\" style=\"width: 40px; height: 40px;\"></span ></div>\n" \
                                "<div class=\"col-4 d-flex align-items-center\">LED2 %s</div>\n"

/* Define return codes */
typedef enum
{
    PING_SUCCESS = 0,
    PING_TIMEOUT,
    PING_ERROR
} ping_status_t;

typedef enum
{
    STATUS_SUCCESS = 0,   // Indicates successful execution
    STATUS_ERROR,          // Indicates an error occurred
} status_code_t;

#if( ipconfigUSE_DHCP != 0 )
    void updateDhcpResponseToUsr(void);
#endif

#if( ipconfigUSE_DHCP != 0 )
    eDHCPCallbackAnswer_t xApplicationDHCPHook( eDHCPCallbackPhase_t eDHCPPhase, uint32_t ulIPAddress );
#endif

#if( ipconfigDHCP_REGISTER_HOSTNAME == 1 )
    /* DHCP has an option for clients to register their hostname.  It doesn't
    have much use, except that a device can be found in a router along with its
    name. If this option is used the callback below must be provided by the
    application writer to return a const string, denoting the device's name. */
    const char *pcApplicationHostnameHook( void );
#endif /* ipconfigDHCP_REGISTER_HOSTNAME */

uint32_t ulApplicationGetNextSequenceNumber( uint32_t ulSourceAddress,
                                             uint16_t usSourcePort,
                                             uint32_t ulDestinationAddress,
                                             uint16_t usDestinationPort );
status_code_t http_server_handle(void);
status_code_t udp_client_handle(char *destination);
status_code_t tcp_client_handle(char *destination);
ping_status_t SendPing( const char *pcIPAddress );
void freertos_fat_deinit(void);

typedef struct st_ping_data
{
    int8_t sent;     // Ping Request
    int8_t received; // Ping Response
    int8_t lost;     // Ping failure
} ping_data_t;

#endif /* USR_APP_H_ */
