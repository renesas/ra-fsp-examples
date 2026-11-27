/***********************************************************************************************************************
 * File Name    : tcp_socket.h
 * Description  : Contains macros, variables, data structures, and functions used in tcp_socket.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef TCP_SOCKET_H_
#define TCP_SOCKET_H_

#include "ctype.h"
#include "wifi_ep.h"

/* Macros for network operations */
#define PING_COUNT                          (4U)
#define PING_INTERVAL_MS                    (100U)
#define SOCKET_TCP                          (0U)
#define IPV4                                (4U)

/* Macros for Wi-Fi operation timeout */
#define TIMEOUT_400MS                       (400U)
#define TIMEOUT_15SEC                       (15000U)

/* Macro for Wi-Fi error return */
#define WIFI_ONCHIP_SILEX_ERR_ERROR         (-1)

/* Convert from MCU byte-order to network byte-order */
#define SOCKETS_HTONL(ulIn)                 ((uint32_t) ((((ulIn) & 0xFF) << 24 ) | ( ((ulIn) & 0xFF00) << 8) |\
                                             (((ulIn) & 0xFF0000) >> 8 ) | (((ulIn) & 0xFF000000) >> 24 )))

/* Structure for socket parameters */
typedef struct
{
    uint32_t id;
    uint32_t ip_addr_server;
    uint32_t port;
    uint8_t send_buff[BUFF_LEN];
    uint8_t recv_buff[BUFF_LEN];
    uint32_t status;
} SocketParameters_t;

/* Extern variable - Defined in tcp_socket.c */
extern SocketParameters_t g_socket;

/* Function declarations */
fsp_err_t dns_query(char * p_dns, uint8_t * p_ip_addr);
fsp_err_t ping(uint8_t * p_ip_addr);
fsp_err_t socket_create(void);
fsp_err_t tcp_connect(uint32_t ip_addr, uint32_t port);
fsp_err_t network_connectivity_check(void);
fsp_err_t tcp_send_data(const char * mess);
fsp_err_t tcp_recv_data(void);
void socket_deinit(void);

#endif /* TCP_SOCKET_H_ */
