/***********************************************************************************************************************
 * File Name    : tcp_socket.c
 * Description  : Contains data structures and functions used in tcp_socket.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "tcp_socket.h"

SocketParameters_t g_socket =
{
    .id             = RESET_VALUE,
    .ip_addr_server = RESET_VALUE,
    .port           = RESET_VALUE,
    .recv_buff      = {RESET_VALUE},
    .send_buff      = {RESET_VALUE},
    .status         = RESET_VALUE
};

/*******************************************************************************************************************//**
 * @brief      This function resolves the DNS query or validates the IP address.
 * @param[in]  p_dns                    Pointer to array holding URL or IP address to query from DNS.
 * @param[out] p_ip_addr                Pointer to IP address returned from look up.
 * @retval     FSP_SUCCESS              Resolved successfully.
 * @retval     Any other error codes    Failed to resolve.
 **********************************************************************************************************************/
fsp_err_t dns_query(char * p_dns, uint8_t * p_ip_addr)
{
    fsp_err_t err = FSP_SUCCESS;

    err = socket_connected();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = rm_wifi_onchip_silex_dns_query(p_dns, p_ip_addr);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function pings the IP address.
 * @param[in] p_ip_addr                 Pointer to IP address array.
 * @retval    FSP_SUCCESS               Pinged successfully.
 * @retval    Any other error codes     Failed to ping.
 **********************************************************************************************************************/
fsp_err_t ping(uint8_t * p_ip_addr)
{
    fsp_err_t err = FSP_SUCCESS;

    err = rm_wifi_onchip_silex_ping(p_ip_addr, PING_COUNT, PING_INTERVAL_MS);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function creates a TCP client socket.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Created successfully.
 * @retval    Any other error codes     Failed to create.
 **********************************************************************************************************************/
fsp_err_t socket_create(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = socket_connected();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    err = rm_wifi_onchip_silex_socket_create(g_socket.id, SOCKET_TCP, IPV4);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function establishes a TCP connection to the specified server.
 * @param[in] ip_addr                   IP address of the server for socket connection.
 * @param[in] port                      Port number of the server for socket connection.
 * @retval    FSP_SUCCESS               Established successfully.
 * @retval    Any other error codes     Failed to establish.
 **********************************************************************************************************************/
fsp_err_t tcp_connect(uint32_t ip_addr, uint32_t port)
{
    fsp_err_t err = FSP_SUCCESS;

    err = socket_connected();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    for (uint8_t i = RESET_VALUE; i < WIFI_MAX_TRY; i++)
    {
        err = rm_wifi_onchip_silex_tcp_connect(g_socket.id, SOCKETS_HTONL(ip_addr), port);
        if (FSP_SUCCESS == err)
        {
            return err;
        }
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function checks the socket status.
 * @param[in] None
 * @retval    FSP_SUCCESS               Connected successfully.
 * @retval    Any other error codes     Failed to connect.
 **********************************************************************************************************************/
fsp_err_t network_connectivity_check(void)
{
    fsp_err_t err = FSP_SUCCESS;

    err = socket_connected();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    rm_wifi_onchip_silex_socket_status_get(g_socket.id, &g_socket.status);
    if (WIFI_ONCHIP_SILEX_SOCKET_STATUS_CONNECTED != g_socket.status)
    {
        return FSP_ERR_WIFI_FAILED;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function receives data from the TCP server.
 * @param[in] mess                      Pointer to the message to be transmitted.
 * @retval    FSP_SUCCESS               Sent successfully.
 * @retval    Any other error codes     Failed to send.
 **********************************************************************************************************************/
fsp_err_t tcp_send_data(const char * mess)
{
    fsp_err_t err = FSP_SUCCESS;
    int32_t transfer= RESET_VALUE;

    /* Send acknowledgment to the server */
    strcpy((char *)g_socket.send_buff, mess);

    /* Capture number of bytes */
    transfer = rm_wifi_onchip_silex_send(g_socket.id, g_socket.send_buff,
                                         strlen((char *)g_socket.send_buff), TIMEOUT_400MS);
    if (RESET_VALUE >= transfer)
    {
        return FSP_ERR_WIFI_FAILED;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function receives data from the TCP server.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Data received successfully.
 * @retval    Any other error codes     Failed to receive data.
 **********************************************************************************************************************/
fsp_err_t tcp_recv_data(void)
{
    fsp_err_t err = FSP_SUCCESS;
    int32_t transfer = RESET_VALUE;

    memset(g_socket.recv_buff, RESET_VALUE, sizeof(g_socket.recv_buff));

    transfer = rm_wifi_onchip_silex_recv(g_socket.id, g_socket.recv_buff, BUFF_LEN, TIMEOUT_15SEC);
    if (RESET_VALUE > transfer)
    {
        err = FSP_ERR_WIFI_FAILED;
    }
    else
    {
        err = FSP_SUCCESS;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function disconnects from the Wi-Fi network.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void socket_deinit(void)
{
    rm_wifi_onchip_silex_socket_status_get(g_socket.id, &g_socket.status);
    if (WIFI_ONCHIP_SILEX_SOCKET_STATUS_CONNECTED == g_socket.status)
    {
        rm_wifi_onchip_silex_disconnect();
    }
}
