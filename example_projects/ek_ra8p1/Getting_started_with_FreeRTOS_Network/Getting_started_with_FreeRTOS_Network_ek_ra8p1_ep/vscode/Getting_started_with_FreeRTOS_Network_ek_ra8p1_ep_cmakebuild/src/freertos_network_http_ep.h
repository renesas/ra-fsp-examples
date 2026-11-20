/***********************************************************************************************************************
 * File Name    : freertos_network_http_ep.h
 * Description  : Contains macros, data structures and functions used in freertos_network_http_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef FREERTOS_NETWORK_HTTP_EP_H_
#define FREERTOS_NETWORK_HTTP_EP_H_

#include "ethernet_ep.h"

/* Macros for messages data transmission */
#define SEND_MESS_TCP                   "[TCP message]Hello from RA board!"
#define SEND_MESS_UDP                   "[UDP message]Hello from RA board!"

/* Macros for network operation configuration */
#define USR_PING_COUNT                  (10U)
#define SOCKET_RECEIVE_TIMEOUT          (60000U)
#define SOCKET_SEND_TIMEOUT             (5000U)

/* Macro for LED configuration */
#define NUM_LEDS                        (2U)

/* Macro for operation status */
#define SUCCESS                         (0U)

/* Macro for network status message control */
#define PRINT_UP_MSG_DISABLE            (0x01)
#define PRINT_DOWN_MSG_DISABLE          (0x02)
#define PRINT_NWK_USR_MSG_DISABLE       (0x04)

/* Macro for ping retry configuration */
#define MAX_PING_RETRIES                (10U)

/* Macros for user-selectable network operations */
#define PING_OPERATION                  ('1')
#define TCP_CLIENT_OPERATION            ('2')
#define UDP_CLIENT_OPERATION            ('3')
#define DNS_OPERATION                   ('4')
#define HTTP_SERVER_OPERATION           ('5')

/* Macros for task delay and HTTP server configuration */
#define DELAY_10_TICKS                  (10U)
#define SERVER_PORT                     (80U)
#define SERVER_BACKLOG                  (12U)

/* Macros for HTTP server file paths */
#define HTTP_ROOT                       ("")
#define LED_STATUS_FILE_NAME            ("/led")
#define NETWORK_INFO_FILE_NAME          ("/network")

/* Macros for HTTP GET commands */
#define GET_LED_COMMAND                 ("GET /led")
#define GET_LED_COMMAND_LENGTH          (8U)
#define GET_SWITCH1_COMMAND             ("GET /switch1")
#define GET_SWITCH1_COMMAND_LENGTH      (12U)
#define GET_SWITCH2_COMMAND             ("GET /switch2")
#define GET_SWITCH2_COMMAND_LENGTH      (12U)

/* Macro for file access modes */
#define WRITE_MODE                      ("w")

/* Macros for buffer sizes */
#define BUFFER_SIZE                     (2048U)
#define RECV_BUFFER_SIZE                (128U)
#define NETWORK_BUFFER_SIZE             (1024U)
#define LED_BUFFER_SIZE                 (512U)

/* Macros for LED status display */
#define LED_COLOR_ON                    ("btn-danger")
#define LED_COLOR_OFF                   ("btn-secondary")

#define INNER_HTML_NET_ID               \
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

#define INNER_HTML_LED_ID               \
                                        "\n"\
                                        "<div class=\"col-2\"><span class=\"btn %s rounded-circle\" style=\"\
                                        ""width: 40px; height: 40px;\"></span ></div>\n"\
                                        "<div class=\"col-4 d-flex align-items-center\">LED1 %s</div>\n"\
                                        "<div class=\"col-2\"><span class=\"btn %s rounded-circle\" style=\"\
                                        ""width: 40px; height: 40px;\"></span ></div>\n"\
                                        "<div class=\"col-4 d-flex align-items-center\">LED2 %s</div>\n"

typedef enum
{
    STATUS_SUCCESS = 0,    /* Indicates successful execution */
    STATUS_ERROR,          /* Indicates an error occurred */
} status_code_t;

/* External variables */
extern bsp_leds_t g_bsp_leds;

/* Function declarations */
void remote_address_configure(int * p_ipv4_address, int port, struct freertos_sockaddr * p_remote_address);
status_code_t socket_create(BaseType_t socket_type, BaseType_t protocol);
ping_status_t ping_server(char * remote_ip_address);
status_code_t tcp_server_connect(struct freertos_sockaddr * p_remote_address);
void tcp_socket_shutdown(void);
void socket_close(void);
BaseType_t tcp_send(void);
BaseType_t tcp_receive(char * p_buffer, size_t buffer_size);
int32_t udp_send(struct freertos_sockaddr const * p_remote_address);
int32_t udp_receive(struct freertos_sockaddr * p_remote_address, char * p_receive_buffer, size_t receive_buffer_size);
fsp_err_t freertos_fat_init(void);
status_code_t file_system_mount(void);
status_code_t http_server_create(TCPServer_t ** pp_tcp_server);
void http_server_process(TCPServer_t * p_tcp_server);
void http_server_close(TCPServer_t * p_tcp_server);
void toggleLED(bsp_io_port_pin_t ledNumber);
status_code_t update_status_leds(void);
void freertos_fat_deinit(void);
status_code_t write_file(const char *fileName, const char *content);

#endif /* FREERTOS_NETWORK_HTTP_EP_H_ */
