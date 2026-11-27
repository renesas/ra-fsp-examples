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

#include "NetX_wifi_ep.h"

/* Macros for TCP connectivity */
#define NETXDUO_TESTS_TCP_WINDOW_SIZE   (4U * 1024U)
#define NETXDUO_TESTS_SOCKET_TIMEOUT    (3000U)
#define PING_COUNT                      (4U)
#define PING_INTERVAL_MS                (100U)
#define PORT_BIND                       (3050U)
#define PACKET_POOL_NAME                "g_packet_pool0 Packet Pool"
#define IP_NAME                         "g_ip0 IP Instance"

/* Macro for Wi-Fi error return */
#define WIFI_ONCHIP_SILEX_ERR_ERROR     (-1)

/* Macro for time out */
#define NX_IP_STATUS_CHECK_TIMEOUT      (1000U)

/* Structure for socket parameters */
typedef struct
{
    uint32_t id;
    ULONG ip_addr_server;
    UINT  port;
    uint8_t send_buff[BUFF_LEN];
    uint8_t recv_buff[BUFF_LEN];
    uint32_t status;
} SocketParameters_t;

/* Extern variables - Defined in tcp_socket.c */
extern SocketParameters_t g_socket;
extern NX_PACKET * gp_nx_packet;

/* Function declarations */
fsp_err_t dns_query(char * p_dns, uint8_t * p_ip_addr);
fsp_err_t ping(uint8_t * p_ip_addr);
void socket_deinit(void);
UINT nx_tcp_create(char * name);
UINT nx_tcp_bind(void);
UINT nx_tcp_connect(void);
UINT nx_tcp_send(char * mess);
UINT nx_tcp_recv(void);
UINT nx_tcp_retrieve(void);
UINT network_init(void);

#endif /* TCP_SOCKET_H_ */
