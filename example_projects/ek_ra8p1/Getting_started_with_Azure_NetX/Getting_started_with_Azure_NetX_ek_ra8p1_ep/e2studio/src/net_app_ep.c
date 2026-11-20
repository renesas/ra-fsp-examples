/***********************************************************************************************************************
 * File Name    : net_app_ep.c
 * Description  : Contains data structures and functions used in net_app_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include "net_app_thread.h"
#include "net_app_ep.h"
#include "net_link_ep.h"
#include "led_control_ep.h"

/* Private variables */
static NX_TCP_SOCKET s_tcp_client_socket;
static NX_UDP_SOCKET s_udp_client_socket;
static NX_WEB_HTTP_SERVER s_web_http_server0;
static NX_DNS s_dns_client;
static FX_MEDIA s_fx_media0;
static uint8_t s_fx_media0_media_memory[G_FX_MEDIA0_MEDIA_MEMORY_SIZE];
static uint8_t s_web_http_server0_stack_memory[G_WEB_HTTP_SERVER0_STACK_SIZE] BSP_PLACE_IN_SECTION\
                                              (".stack.g_web_http_server0") BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);
static uint32_t s_fx_media0_event = RESET_VALUE;

/* Private functions */
static UINT file_system_create(void);
static UINT server_request_callback(NX_WEB_HTTP_SERVER * p_server, UINT request_type, CHAR * p_resource,
                                    NX_PACKET * p_packet);
static UINT server_callback_response_net_info(NX_WEB_HTTP_SERVER * p_server, CHAR * p_resource);
static UINT server_callback_response_led_status(NX_WEB_HTTP_SERVER * p_server, CHAR * p_resource);
static UINT server_callback_response_switch_event(NX_WEB_HTTP_SERVER * p_server, CHAR * p_resource);

/*******************************************************************************************************************//**
 * @brief     This function is triggered whenever a FileX block media event occurs.
 * @param[in] p_args                    Pointer to block media callback arguments.
 * @retval    None.
 **********************************************************************************************************************/
void g_rm_filex_block_media_0_callback(rm_filex_block_media_callback_args_t * p_args)
{
    s_fx_media0_event = p_args->event;
}

/*******************************************************************************************************************//**
 * @brief     This function creates the network application components.
 * @param[in] None.
 * @retval    NX_SUCCESS                Created successfully.
 * @retval    Any other error codes     Failed to create.
 **********************************************************************************************************************/
UINT net_app_create(void)
{
    UINT status = NX_SUCCESS;

    /* Create a TCP socket */
    status = nx_tcp_socket_create(&g_ip0, &s_tcp_client_socket, "TCP Client Socket",
                                  NX_IP_NORMAL, NX_FRAGMENT_OKAY, NX_IP_TIME_TO_LIVE, 200, NX_NULL, NX_NULL);
    if (NX_SUCCESS != status)
    {
        return status;
    }

    /* Bind the TCP client socket to an available port number */
    status = nx_tcp_client_socket_bind(&s_tcp_client_socket, NX_ANY_PORT, TX_TIMER_TICKS_PER_SECOND);
    if (NX_SUCCESS != status)
    {
        return status;
    }

    /* Create a UDP socket */
    status = nx_udp_socket_create(&g_ip0, &s_udp_client_socket, "UDP Client Socket",
                                  NX_IP_NORMAL, NX_FRAGMENT_OKAY, NX_IP_TIME_TO_LIVE, NET_UDP_CLIENT_QUEUE_SIZE);
    if (NX_SUCCESS != status)
    {
        return status;
    }

    /* Bind the UDP client socket to an available port number */
    status = nx_udp_socket_bind(&s_udp_client_socket, NX_ANY_PORT, TX_TIMER_TICKS_PER_SECOND);
    if (NX_SUCCESS != status)
    {
        return status;
    }

    /* Create a DNS instance for the DNS client */
    status = nx_dns_create(&s_dns_client, &g_ip0, (UCHAR *)"DNS Client Socket");
    if (NX_SUCCESS != status)
    {
        return status;
    }

    /* Set the packet pool for the DNS client instance */
    status = nx_dns_packet_pool_set(&s_dns_client, &g_packet_pool0);
    if (NX_SUCCESS != status)
    {
        return status;
    }

    /* Add the primary Google Public DNS server */
    status = nx_dns_server_add(&s_dns_client, NET_DNS_SERVER_ADDR_1);
    if (NX_SUCCESS != status)
    {
        return status;
    }

    /* Add the secondary Google Public DNS server */
    status = nx_dns_server_add(&s_dns_client, NET_DNS_SERVER_ADDR_2);
    if (NX_SUCCESS != status)
    {
        return status;
    }

    /* Create the file system for the web HTTP server */
    status = file_system_create();
    if (FX_SUCCESS != status)
    {
        return status;
    }

    /* Create the web HTTP server */
    status = nx_web_http_server_create(&s_web_http_server0,
                                       "WEB HTTP Server", &g_ip0,
                                       G_WEB_HTTP_SERVER0_PORT_NUMBER, &s_fx_media0,
                                       s_web_http_server0_stack_memory, G_WEB_HTTP_SERVER0_STACK_SIZE,
                                       &g_packet_pool0, NX_NULL, server_request_callback);

    return status;
}

/*******************************************************************************************************************//**
 * @brief     This function resets the external flash device, initializes the FileX block media driver, and opens
 *            the media for the file system operations.
 * @param[in] None.
 * @retval    FX_SUCCESS                Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
static UINT file_system_create(void)
{
    UINT status = FX_SUCCESS;

    /* Hardware reset the flash device by driving the RESET pin */
    R_IOPORT_PinCfg(&g_ioport_ctrl, FLASH_RS_PIN, FLASH_GPIO_CFG);
    R_IOPORT_PinWrite(&g_ioport_ctrl, FLASH_RS_PIN, BSP_IO_LEVEL_LOW);
    R_BSP_SoftwareDelay(FLASH_RESET_TIME_US, BSP_DELAY_UNITS_MICROSECONDS);
    R_IOPORT_PinWrite(&g_ioport_ctrl, FLASH_RS_PIN, BSP_IO_LEVEL_HIGH);
    R_IOPORT_PinCfg(&g_ioport_ctrl, FLASH_RS_PIN, FLASH_XSPI_CFG);

    /* Initialize the block media driver used by FileX */
    status = (UINT)RM_FILEX_BLOCK_MEDIA_Open(&g_rm_filex_block_media_0_ctrl, &g_rm_filex_block_media_0_cfg);
    if (FX_SUCCESS != status)
    {
        return status;
    }

    /* Open the media for file system operations */
    status = fx_media_open(&s_fx_media0,
                           G_FX_MEDIA0_VOLUME_NAME,
                           RM_FILEX_BLOCK_MEDIA_BlockDriver,
                           (void *)&g_rm_filex_block_media_0_instance,
                           s_fx_media0_media_memory,
                           G_FX_MEDIA0_MEDIA_MEMORY_SIZE);

    return status;
}

/*******************************************************************************************************************//**
 * @brief     This function connects the TCP client socket to the TCP server.
 * @param[in] ipv4                      IPv4 address of the TCP server.
 * @param[in] port                      Port number of the TCP server.
 * @retval    NX_SUCCESS                Connected successfully.
 * @retval    Any other error codes     Failed to connect.
 **********************************************************************************************************************/
UINT tcp_client_connect(ULONG ipv4, UINT port)
{
    return nx_tcp_client_socket_connect(&s_tcp_client_socket, ipv4, port, NET_TCP_CLIENT_WAIT);
}

/*******************************************************************************************************************//**
 * @brief     This function sends data to the TCP server.
 * @param[in] p_data                    Pointer to the data to be sent.
 * @param[in] size                      Size of the data to be sent.
 * @retval    NX_SUCCESS                Sent successfully.
 * @retval    Any other error codes     Failed to send.
 **********************************************************************************************************************/
UINT tcp_client_send(const CHAR * p_data, ULONG size)
{
    UINT status = NX_SUCCESS;
    NX_PACKET * p_packet = NX_NULL;

    /* Allocate a TCP packet */
    status = nx_packet_allocate(&g_packet_pool0, &p_packet, NX_TCP_PACKET, TX_TIMER_TICKS_PER_SECOND);
    if (NX_SUCCESS != status)
    {
        return status;
    }

    /* Write the data into the packet payload */
    status = nx_packet_data_append(p_packet, (VOID *)p_data, size, &g_packet_pool0, TX_TIMER_TICKS_PER_SECOND);
    if (NX_SUCCESS != status)
    {
        nx_packet_release(p_packet);
        return status;
    }

    /* Send the TCP packet */
    status = nx_tcp_socket_send(&s_tcp_client_socket, p_packet, NET_TCP_CLIENT_WAIT);
    if (NX_SUCCESS != status)
    {
        nx_packet_release(p_packet);
    }

    return status;
}

/*******************************************************************************************************************//**
 * @brief      This function receives data from the TCP server.
 * @param[out] p_buffer                 Pointer to the buffer to store the received data.
 * @retval     NX_SUCCESS               Received successfully.
 * @retval     Any other error codes    Failed to receive.
 **********************************************************************************************************************/
UINT tcp_client_receive(CHAR * const p_buffer)
{
    UINT status = NX_SUCCESS;
    ULONG len = RESET_VALUE;
    NX_PACKET * p_packet = NX_NULL;

    /* Receive a TCP packet from the TCP server */
    status = nx_tcp_socket_receive(&s_tcp_client_socket, &p_packet, NET_TCP_CLIENT_WAIT);
    if (NX_SUCCESS != status)
    {
        return status;
    }

    /* Retrieve data from the packet */
    status = nx_packet_data_retrieve(p_packet, p_buffer, &len);
    if (NX_SUCCESS == status)
    {
        /* Null-terminate the buffer string */
        p_buffer[len] = NULL_CHAR;
    }

    /* Release the TCP packet */
    nx_packet_release(p_packet);

    return status;
}

/*******************************************************************************************************************//**
 * @brief     This function disconnects the TCP client socket from the TCP server.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void tcp_client_disconnect(void)
{
    nx_tcp_socket_disconnect(&s_tcp_client_socket, NET_TCP_CLIENT_WAIT);
}

/*******************************************************************************************************************//**
 * @brief     This function sends data to the UDP server.
 * @param[in] ipv4                      IPv4 address of the UDP server.
 * @param[in] port                      Port number of the UDP server.
 * @param[in] p_data                    Pointer to the data to be sent.
 * @param[in] size                      Size of the data to be sent.
 * @retval    NX_SUCCESS                Sent successfully.
 * @retval    Any other error codes     Failed to send.
 **********************************************************************************************************************/
UINT udp_client_send(ULONG ipv4, UINT port, const CHAR * p_data, ULONG size)
{
    UINT status = NX_SUCCESS;
    NX_PACKET * p_packet = NX_NULL;

    /* Allocate a UDP packet */
    status = nx_packet_allocate(&g_packet_pool0, &p_packet, NX_UDP_PACKET, TX_TIMER_TICKS_PER_SECOND);
    if (NX_SUCCESS != status)
    {
        return status;
    }

    /* Write the data into the packet payload */
    status = nx_packet_data_append(p_packet, (VOID *)p_data, size, &g_packet_pool0, TX_TIMER_TICKS_PER_SECOND);
    if (NX_SUCCESS != status)
    {
        nx_packet_release(p_packet);
        return status;
    }

    /* Send the UDP packet */
    status = nx_udp_socket_send(&s_udp_client_socket, p_packet, ipv4, port);
    if (NX_SUCCESS != status)
    {
        nx_packet_release(p_packet);
    }

    return status;
}

/*******************************************************************************************************************//**
 * @brief      This function receives data from the UDP server.
 * @param[out] p_buffer                 Pointer to the buffer to store the received data.
 * @retval     NX_SUCCESS               Received successfully.
 * @retval     Any other error codes    Failed to receive.
 **********************************************************************************************************************/
UINT udp_client_receive(CHAR * const p_buffer)
{
    UINT status = NX_SUCCESS;
    ULONG len = RESET_VALUE;
    NX_PACKET * p_packet = NX_NULL;

    /* Receive a UDP packet from the UDP server */
    status = nx_udp_socket_receive(&s_udp_client_socket, &p_packet, NET_UDP_CLIENT_WAIT);
    if (NX_SUCCESS != status)
    {
        return status;
    }

    /* Retrieve data from the packet */
    status = nx_packet_data_retrieve(p_packet, p_buffer, &len);
    if (NX_SUCCESS == status)
    {
        /* Null-terminate the buffer string */
        p_buffer[len] = NULL_CHAR;
    }

    /* Release the UDP packet */
    nx_packet_release(p_packet);

    return status;
}

/*******************************************************************************************************************//**
 * @brief      This function resolves a host name into its corresponding IPv4 address.
 * @param[in]  p_name                   Pointer to the host name.
 * @param[out] p_ipv4                   Pointer to store the IPv4 address.
 * @retval     NX_SUCCESS               Resolved successfully.
 * @retval     Any other error codes    Failed to resolve.
 **********************************************************************************************************************/
UINT dns_host_by_name(const CHAR * p_name, ULONG * const p_ipv4)
{
    return nx_dns_host_by_name_get(&s_dns_client, (UCHAR *)p_name, p_ipv4, NET_DNS_CLIENT_WAIT);
}

/*******************************************************************************************************************//**
 * @brief      This function resolves an IPv4 address into its corresponding host name.
 * @param[in]  ipv4                     IPv4 address.
 * @param[out] p_name                   Pointer to the buffer to store the host name.
 * @param[in]  name_size                Size of the buffer.
 * @retval     NX_SUCCESS               Resolved successfully.
 * @retval     Any other error codes    Failed to resolve.
 **********************************************************************************************************************/
UINT dns_host_by_address(ULONG ipv4, CHAR * const p_name, ULONG name_size)
{
    return nx_dns_host_by_address_get(&s_dns_client, ipv4, (UCHAR *)p_name, name_size, NET_DNS_CLIENT_WAIT);
}

/*******************************************************************************************************************//**
 * @brief     This function starts the web server's internal thread.
 * @param[in] None.
 * @retval    NX_SUCCESS                Started successfully.
 * @retval    Any other error codes     Failed to start.
 **********************************************************************************************************************/
UINT web_server_start(void)
{
    return nx_web_http_server_start(&s_web_http_server0);
}

/*******************************************************************************************************************//**
 * @brief     This function stops the web server's internal thread.
 * @param[in] None.
 * @retval    NX_SUCCESS                Stopped successfully.
 * @retval    Any other error codes     Failed to stop.
 **********************************************************************************************************************/
UINT web_server_stop(void)
{
    return nx_web_http_server_stop(&s_web_http_server0);
}

/*******************************************************************************************************************//**
 * @brief     This function processes incoming HTTP GET requests and sends appropriate responses
 *            based on the requested resource.
 * @param[in] p_server                          Pointer to the web server instance.
 * @param[in] request_type                      Type of the HTTP request (GET, POST, etc.).
 * @param[in] p_resource                        Pointer to the requested resource string.
 * @param[in] p_packet                          Pointer to the packet associated with the request.
 * @retval    NX_WEB_HTTP_CALLBACK_COMPLETED    The request is successfully handled.
 * @retval    Any other error codes             Failed to handle the request.
 **********************************************************************************************************************/
static UINT server_request_callback(NX_WEB_HTTP_SERVER * p_server, UINT request_type, CHAR * p_resource,\
                                    NX_PACKET * p_packet)
{
    UINT status = NX_SUCCESS;

    (void)p_packet;

    if (NX_WEB_HTTP_SERVER_GET_REQUEST == request_type)
    {
        if (NX_NULL == strcmp(HTTP_RESOURCE_NETWORK, p_resource))
        {
            /* Send a response for the GET network configuration request */
            status = server_callback_response_net_info(p_server, p_resource);
            if (NX_SUCCESS == status)
            {
                return NX_WEB_HTTP_CALLBACK_COMPLETED;
            }
            else
            {
                return status;
            }
        }
        else if (NX_NULL == strcmp(HTTP_RESOURCE_LED, p_resource))
        {
            /* Send a response for the GET LED status request */
            status = server_callback_response_led_status(p_server, p_resource);
            if (NX_SUCCESS == status)
            {
                return NX_WEB_HTTP_CALLBACK_COMPLETED;
            }
            else
            {
                return status;
            }
        }
        else if (NX_NULL == strcmp(HTTP_RESOURCE_SWITCH1, p_resource))
        {
            /* Set the LED1 event flag */
            tx_event_flags_set(&g_led_event_flags, LED_1_TOGGLE_EVENT, TX_OR);

            /* Send a response for the GET switch 1 request */
            status = server_callback_response_switch_event(p_server, p_resource);
            if (NX_SUCCESS == status)
            {
                return NX_WEB_HTTP_CALLBACK_COMPLETED;
            }
            else
            {
                return status;
            }
        }
        else if (NX_NULL == strcmp(HTTP_RESOURCE_SWITCH2, p_resource))
        {
            /* Set the LED2 event flag */
            tx_event_flags_set(&g_led_event_flags, LED_2_TOGGLE_EVENT, TX_OR);

            /* Send a response to the switch 2 resource GET request */
            status = server_callback_response_switch_event(p_server, p_resource);
            if (NX_SUCCESS == status)
            {
                return NX_WEB_HTTP_CALLBACK_COMPLETED;
            }
            else
            {
                return status;
            }
        }
        else
        {
            /* Do nothing */
        }
    }

    return NX_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief     This function generates and sends an HTTP response containing the current network configuration,
 *            including physical address, IPv4 address, subnet mask, and default gateway.
 * @param[in] p_server                  Pointer to the web server instance.
 * @param[in] p_resource                Pointer to the requested resource string.
 * @retval    NX_SUCCESS                Sent successfully.
 * @retval    Any other error codes     Failed to send.
 **********************************************************************************************************************/
static UINT server_callback_response_net_info(NX_WEB_HTTP_SERVER * p_server, CHAR * p_resource)
{
    UINT status = NX_SUCCESS;
    UINT length = NX_NULL;
    CHAR content_type[HTTP_TYPE_MAX_LEN];
    CHAR content_body[HTTP_BODY_MAX_LEN];
    NX_PACKET * p_packet = NX_NULL;

    /* Determine the client request type from the resource string */
    status = nx_web_http_server_type_get(p_server, p_resource, content_type, &length);
    if (NX_SUCCESS != status && NX_WEB_HTTP_EXTENSION_MIME_DEFAULT != status)
    {
        return status;
    }

    /* Null-terminate the content type string */
    content_type[length] = NX_NULL;

    /* Generate an HTTP response header with status OK and no additional headers */
    status = nx_web_http_server_callback_generate_response_header(p_server, &p_packet, NX_WEB_HTTP_STATUS_OK,
                                                                  NX_NULL, content_type, NX_NULL);
    if (NX_SUCCESS != status)
    {
        return status;
    }

    /* Clean the HTTP content body buffer */
    memset(content_body, RESET_VALUE, sizeof(content_body));

    /* Format the current network configuration as a JSON string */
    length = (UINT)snprintf(content_body, sizeof(content_body), JSON_NET_INFO,
                            g_network_info.phy_addr.block.a, g_network_info.phy_addr.block.b,
                            g_network_info.phy_addr.block.c, g_network_info.phy_addr.block.d,
                            g_network_info.phy_addr.block.e, g_network_info.phy_addr.block.f,
                            g_network_info.ipv4_addr.block.a, g_network_info.ipv4_addr.block.b,
                            g_network_info.ipv4_addr.block.c, g_network_info.ipv4_addr.block.d,
                            g_network_info.subnet_mask.block.a, g_network_info.subnet_mask.block.b,
                            g_network_info.subnet_mask.block.c, g_network_info.subnet_mask.block.d,
                            g_network_info.default_gateway.block.a, g_network_info.default_gateway.block.b,
                            g_network_info.default_gateway.block.c, g_network_info.default_gateway.block.d);

    /* Append the content body to the response packet */
    status = nx_packet_data_append(p_packet, content_body, length,
                                   p_server->nx_web_http_server_packet_pool_ptr, NX_WAIT_FOREVER);
    if (NX_SUCCESS != status)
    {
        nx_packet_release(p_packet);
        return status;
    }

    /* Send the response packet to the client */
    status = nx_web_http_server_callback_packet_send(p_server, p_packet);
    if (NX_SUCCESS != status)
    {
        nx_packet_release(p_packet);
    }

    return status;
}

/*******************************************************************************************************************//**
 * @brief     This function processes the client request, updates the content body with the LED status,
 *            and generates an HTTP response that is sent back to the client.
 * @param[in] p_server                  Pointer to the HTTP server instance.
 * @param[in] p_resource                Pointer to the client request resource string.
 * @retval    NX_SUCCESS                Sent successfully.
 * @retval    Any other error codes     Failed to send.
 **********************************************************************************************************************/
static UINT server_callback_response_led_status(NX_WEB_HTTP_SERVER * p_server, CHAR * p_resource)
{
    UINT status = NX_SUCCESS;
    UINT length = NX_NULL;
    CHAR content_type[HTTP_TYPE_MAX_LEN];
    CHAR content_body[HTTP_BODY_MAX_LEN];
    NX_PACKET * p_packet = NX_NULL;

    /* Determine the client request type from the resource string */
    status = nx_web_http_server_type_get(p_server, p_resource, content_type, &length);
    if (NX_SUCCESS != status && NX_WEB_HTTP_EXTENSION_MIME_DEFAULT != status)
    {
        return status;
    }

    /* Null-terminate the content type string */
    content_type[length] = NX_NULL;

    /* Generate an HTTP response header with status OK and no additional headers */
    status = nx_web_http_server_callback_generate_response_header(p_server, &p_packet, NX_WEB_HTTP_STATUS_OK, NX_NULL,
                                                                  content_type, NX_NULL);
    if (NX_SUCCESS != status)
    {
        return status;
    }

    /* Clean the HTTP content body buffer */
    memset(content_body, RESET_VALUE, sizeof(content_body));

    /* Format the current LED status as a JSON string */
    length = (UINT)snprintf(content_body, sizeof(content_body), JSON_LED_STATUS,
                            (BSP_IO_LEVEL_HIGH == g_led_1_status) ? "true" : "false",
                            (BSP_IO_LEVEL_HIGH == g_led_2_status) ? "true" : "false");

    /* Append the content body to the response packet */
    status = nx_packet_data_append(p_packet, content_body, length,
                                   p_server->nx_web_http_server_packet_pool_ptr, NX_WAIT_FOREVER);
    if (NX_SUCCESS != status)
    {
        nx_packet_release(p_packet);
        return status;
    }

    /* Send the response packet to the client */
    status = nx_web_http_server_callback_packet_send(p_server, p_packet);
    if (NX_SUCCESS != status)
    {
        nx_packet_release(p_packet);
    }

    return status;
}

/*******************************************************************************************************************//**
 * @brief     This function generates and sends an HTTP response indicating the status of a switch request.
 * @param[in] p_server                  Pointer to the web server instance.
 * @param[in] p_resource                Pointer to the requested resource string.
 * @retval    NX_SUCCESS                Sent successfully.
 * @retval    Any other error codes     Failed to send.
 **********************************************************************************************************************/
static UINT server_callback_response_switch_event(NX_WEB_HTTP_SERVER * p_server, CHAR * p_resource)
{
    UINT status = NX_SUCCESS;
    UINT length = NX_NULL;
    CHAR content_type[HTTP_TYPE_MAX_LEN];
    NX_PACKET * p_packet = NX_NULL;

    /* Determine the client request type from the resource string */
    status = nx_web_http_server_type_get(p_server, p_resource, content_type, &length);
    if (NX_SUCCESS != status && NX_WEB_HTTP_EXTENSION_MIME_DEFAULT != status)
    {
        return status;
    }

    /* Null-terminate the content type string */
    content_type[length] = NX_NULL;

    /* Generate an HTTP response header with status OK and no additional headers */
    status = nx_web_http_server_callback_generate_response_header(p_server, &p_packet, NX_WEB_HTTP_STATUS_OK,
                                                                  NX_NULL, content_type, NX_NULL);
    if (NX_SUCCESS != status)
    {
        return status;
    }

    /* Send the response packet to the client */
    status = nx_web_http_server_callback_packet_send(p_server, p_packet);
    if (NX_SUCCESS != status)
    {
        nx_packet_release(p_packet);
    }

    return status;
}
