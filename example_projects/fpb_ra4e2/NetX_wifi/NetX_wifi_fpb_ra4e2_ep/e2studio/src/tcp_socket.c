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
 .id = RESET_VALUE,
 .ip_addr_server = RESET_VALUE,
 .port = RESET_VALUE,
 .recv_buff = {RESET_VALUE},
 .send_buff = {RESET_VALUE},
 .status = RESET_VALUE
};

NX_PACKET * gp_nx_packet = NX_NULL;
NX_TCP_SOCKET  g_tcp_client_socket;

/* IP instance */
NX_IP g_ip0;

/* Stack memory for g_ip0 */
uint8_t g_ip0_stack_memory[G_IP0_TASK_STACK_SIZE] BSP_PLACE_IN_SECTION(".stack.g_ip0")\
        BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);

/* ARP cache memory for g_ip0 */
uint8_t g_ip0_arp_cache_memory[G_IP0_ARP_CACHE_SIZE] BSP_ALIGN_VARIABLE(4);

/* Packet pool instance (If this is a Trustzone part, the memory must be placed in Non-secure memory) */
NX_PACKET_POOL g_packet_pool0;

#if defined(ETHER_BUFFER_PLACE_IN_SECTION)
uint8_t g_packet_pool0_pool_memory[G_PACKET_POOL0_PACKET_NUM * (G_PACKET_POOL0_PACKET_SIZE +\
        sizeof(NX_PACKET))] BSP_ALIGN_VARIABLE(4) ETHER_BUFFER_PLACE_IN_SECTION;
#elif defined(WIFI_BUFFER_PLACE_IN_SECTION)
uint8_t g_packet_pool0_pool_memory[G_PACKET_POOL0_PACKET_NUM * (G_PACKET_POOL0_PACKET_SIZE +\
        sizeof(NX_PACKET))] BSP_ALIGN_VARIABLE(4) WIFI_BUFFER_PLACE_IN_SECTION;
#endif /* ETHER_BUFFER_PLACE_IN_SECTION || WIFI_BUFFER_PLACE_IN_SECTION */

static UINT ip0_init(void);
static UINT packet_pool0_init(void);

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

    /* Check the status of Wi-Fi Connection */
    err = socket_connected();
    if (FSP_SUCCESS != err)
    {
        return (fsp_err_t) WIFI_ONCHIP_SILEX_ERR_ERROR;
    }

    /* Initiate a DNS lookup for a given URL */
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

    /* Check status of Wi-Fi Connection */
    err = socket_connected();
    if (FSP_SUCCESS != err)
    {
        return (fsp_err_t) WIFI_ONCHIP_SILEX_ERR_ERROR;
    }

    /* Ping an IP address on the network */
    err = rm_wifi_onchip_silex_ping(p_ip_addr, PING_COUNT, PING_INTERVAL_MS);
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

/*******************************************************************************************************************//**
 * @brief     This function creates a TCP client socket.
 * @param[in] name                      Name of new TCP socket.
 * @retval    NX_SUCCESS                Created successfully.
 * @retval    Any other error codes     Failed to create.
 **********************************************************************************************************************/
UINT nx_tcp_create(char * name)
{
    UINT err = NX_SUCCESS;

    err = nx_tcp_socket_create(&g_ip0, &g_tcp_client_socket, name, NX_IP_NORMAL, NX_FRAGMENT_OKAY,
                               NX_IP_TIME_TO_LIVE, NETXDUO_TESTS_TCP_WINDOW_SIZE, NX_NULL, NX_NULL);
    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function binds the TCP socket to the configured local port.
 * @param[in] None.
 * @retval    NX_SUCCESS                Binded successfully.
 * @retval    Any other error codes     Failed to bind.
 **********************************************************************************************************************/
UINT nx_tcp_bind(void)
{
    UINT err = NX_SUCCESS;

    err = nx_tcp_client_socket_bind(&g_tcp_client_socket, PORT_BIND, NETXDUO_TESTS_SOCKET_TIMEOUT);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function establishes a TCP connection to the remote server.
 * @param[in] None.
 * @retval    NX_SUCCESS                Established successfully.
 * @retval    Any other error codes     Failed to establish.
 **********************************************************************************************************************/
UINT nx_tcp_connect(void)
{
    UINT err = NX_SUCCESS;

    err = nx_tcp_client_socket_connect(&g_tcp_client_socket, SOCKETS_HTONL(g_socket.ip_addr_server),
                                       g_socket.port, NETXDUO_TESTS_SOCKET_TIMEOUT);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function sends data to the connected TCP server.
 * @param[in] mess                      Message is sent to TCP.
 * @retval    NX_SUCCESS                Sent successfully.
 * @retval    Any other error codes     Failed to send.
 **********************************************************************************************************************/
UINT nx_tcp_send(char * mess)
{
    UINT err = NX_SUCCESS;

    /* Allocate a fresh packet for this send - do not reuse a packet still owned by a previous receive */
    err = nx_packet_allocate(&g_packet_pool0, &gp_nx_packet, NX_TCP_PACKET, NETXDUO_TESTS_SOCKET_TIMEOUT);
    if (NX_SUCCESS != err)
    {
        return err;
    }

    strcpy((char *)g_socket.send_buff, mess);

    /* Append data */
    err = nx_packet_data_append(gp_nx_packet, g_socket.send_buff, sizeof(g_socket.send_buff), &g_packet_pool0,
                                NETXDUO_TESTS_SOCKET_TIMEOUT);
    if (NX_SUCCESS != err)
    {
        return err;
    }

    /* Send the packet */
    err = nx_tcp_socket_send(&g_tcp_client_socket, gp_nx_packet, NETXDUO_TESTS_SOCKET_TIMEOUT);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function receives a packet from the connected TCP server.
 * @param[in] None.
 * @retval    NX_SUCCESS                Received successfully.
 * @retval    Any other error codes     Failed to receive.
 **********************************************************************************************************************/
UINT nx_tcp_recv(void)
{
    UINT err = NX_SUCCESS;

    memset(g_socket.recv_buff, RESET_VALUE, sizeof(g_socket.recv_buff));

    /* Receive the packet */
    err = nx_tcp_socket_receive(&g_tcp_client_socket, &gp_nx_packet, NETXDUO_TESTS_SOCKET_TIMEOUT);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function retrieves pay load data from the received packet.
 * @param[in] None.
 * @retval    NX_SUCCESS                Retrieved successfully.
 * @retval    Any other error codes     Failed to retrieve.
 **********************************************************************************************************************/
UINT nx_tcp_retrieve(void)
{
    UINT err = NX_SUCCESS;
    uint32_t bytes_copied = RESET_VALUE;

    memset(g_socket.recv_buff, RESET_VALUE, sizeof(g_socket.recv_buff));

    /* Retrieve the packet */
    err = nx_packet_data_retrieve(gp_nx_packet, g_socket.recv_buff, (ULONG *)&bytes_copied);
    if (NX_SUCCESS != err)
    {
        return err;
    }

    /* Release the packet back to the pool */
    err = nx_packet_release(gp_nx_packet);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function initializes the NetX system and Packet pool and IP initializations.
 * @param[in] None.
 * @retval    NX_SUCCESS                Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
UINT network_init(void)
{
    UINT err = NX_SUCCESS;

    nx_system_initialize();

    err = packet_pool0_init();
    if (NX_SUCCESS != err)
    {
        return err;
    }
    err = ip0_init();

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function initializes packet pool.
 * @param[in] None.
 * @retval    NX_SUCCESS                Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
static UINT packet_pool0_init(void)
{
    UINT err = NX_SUCCESS;

    /* Create the packet pool */
    err = nx_packet_pool_create(&g_packet_pool0, PACKET_POOL_NAME, G_PACKET_POOL0_PACKET_SIZE,
                                &g_packet_pool0_pool_memory[0],
                                (G_PACKET_POOL0_PACKET_NUM * (G_PACKET_POOL0_PACKET_SIZE + sizeof(NX_PACKET))));

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function initializes IP0.
 * @param[in] None.
 * @retval    NX_SUCCESS                Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
static UINT ip0_init(void)
{
    UINT err = NX_SUCCESS;
    ULONG current_state;

    /* Create the IP instance */
    err = nx_ip_create(&g_ip0, IP_NAME, IP_ADDRESS(g_wifi.ip_addr_device[0U], g_wifi.ip_addr_device[1U],
                       g_wifi.ip_addr_device[2U], g_wifi.ip_addr_device[3U]), G_IP0_SUBNET_MASK, &g_packet_pool0,
                       G_IP0_NETWORK_DRIVER, &g_ip0_stack_memory[0], G_IP0_TASK_STACK_SIZE, G_IP0_TASK_PRIORITY);
    if (NX_SUCCESS != err)
    {
        return err;
    }

    /* Enable the ARP */
    err = nx_arp_enable(&g_ip0, &g_ip0_arp_cache_memory[0], G_IP0_ARP_CACHE_SIZE);
    if (NX_SUCCESS != err)
    {
        return err;
    }

    /* Enable the TCP */
    err = nx_tcp_enable(&g_ip0);
    if (NX_SUCCESS != err)
    {
        return err;
    }

    /* Enable the ping using Internet Control Message Protocol */
    err = nx_icmp_enable(&g_ip0);
    if (NX_SUCCESS != err)
    {
        return err;
    }

    /* This function enables the IGMP management component for the specified IP instance */
    err = nx_igmp_enable(&g_ip0);
    if (NX_SUCCESS != err)
    {
        return err;
    }

    /* Check the connection status */
    err = nx_ip_status_check(&g_ip0, NX_IP_LINK_ENABLED, &current_state, NX_IP_STATUS_CHECK_TIMEOUT);
    if (NX_IP_LINK_ENABLED != current_state)
    {
        return NX_ARP_TIMER_ERROR;
    }

    return err;
}
