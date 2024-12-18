/***********************************************************************************************************************
* File Name    : net_app_thread_entry.c
* Description  : Contains data structures and functions used in the network application thread
**********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include <net_app_thread.h>
#include "net_app.h"
#include "net_link.h"
#include "common_app.h"

/***********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/
static UINT net_app_create(void);
static UINT file_system_create(void);

static UINT tcp_client_operation(CHAR * p_input);
static UINT udp_client_operation(CHAR * p_input);
static UINT dns_client_operation(CHAR * p_input);
static UINT web_server_operation(void);

static UINT server_request_callback(NX_WEB_HTTP_SERVER *p_server, UINT request_type, CHAR * p_resource, NX_PACKET * p_packet);
static UINT server_callback_response_net_info (NX_WEB_HTTP_SERVER * p_server, CHAR * p_resource);
static UINT server_callback_response_led_status (NX_WEB_HTTP_SERVER * p_server, CHAR * p_resource);
static UINT server_callback_response_switch_event (NX_WEB_HTTP_SERVER * p_server, CHAR * p_resource);

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/
static NX_TCP_SOCKET g_tcp_client_socket;
static NX_UDP_SOCKET g_udp_client_socket;
static NX_WEB_HTTP_SERVER g_web_http_server0;
static NX_DNS g_dns_client;
static FX_MEDIA g_fx_media0;
static uint8_t g_fx_media0_media_memory[G_FX_MEDIA0_MEDIA_MEMORY_SIZE];
static uint8_t g_web_http_server0_stack_memory[G_WEB_HTTP_SERVER0_STACK_SIZE] BSP_PLACE_IN_SECTION(".stack.g_web_http_server0") BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);
static uint32_t g_fx_media0_event = RESET_VALUE;

/***********************************************************************************************************************
 * Global Variables
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Functions
 **********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief Callback function for FileX block media event handling.
 *
 * This function is triggered whenever a FileX block media event occurs.
 * It sets the `g_fx_media0_event` flag based on the event.
 *
 * @param[in] p_args Pointer to block media callback arguments.
 **********************************************************************************************************************/
void g_rm_filex_block_media_0_callback(rm_filex_block_media_callback_args_t *p_args)
{
    g_fx_media0_event = p_args->event;
}

/*******************************************************************************************************************//**
 * @brief Entry point for the network application thread.
 *
 * This function runs indefinitely, waiting for user input from the terminal,
 * and performs the corresponding network operation (TCP client, UDP client, DNS client, Web server).
 **********************************************************************************************************************/
void net_app_thread_entry(void)
{
    UINT status = TX_SUCCESS;
    UINT select = RESET_VALUE;
    UINT len = RESET_VALUE;
    CHAR input [INPUT_MAX_SIZE];
    CHAR argv [INPUT_MAX_SIZE];

    /* Create the necessary resources for the operations of all network operations. */
    status = net_app_create();
    ERROR_TRAP(NX_SUCCESS != status, status, "net_app_create failed");

    while (true)
    {
        /* Try to get the user input queue. */
        while (true)
        {
            /* Send an output queue to request print the menu options. */
            PRINT_EP_MENU;

            /* Clean the buffer. Reset input length to zero. */
            len = RESET_VALUE;
            memset(input, RESET_VALUE, sizeof(input));
            memset(argv, RESET_VALUE, sizeof(argv));

            /* Wait for user input from the input queue. */
            status = terminal_get_input_queue(input, (uint32_t *)&len, TX_WAIT_FOREVER);
            if (TX_SUCCESS == status)
            {
                break;
            }
            else
            {
                PRINT_ERR_STR("terminal_get_input_queue failed");
            }
        }

        /* Parse the user's selection and retrieve the network operation argument in string format. */
        select = RESET_VALUE;
        sscanf(input, "%d %64s", &select, argv);

        /* Perform the selected operation based on the user's input. */
        switch (select)
        {
            case SELECT_TCP:
                PRINT_INFO_STR("The TCP client operation begins");
                status = tcp_client_operation(argv);
                if (NX_SUCCESS != status)
                {
                    PRINT_ERR_STR("The TCP client operation failed\r\n");
                }
                else
                {
                    PRINT_INFO_STR("The TCP client operation was successful\r\n");
                }
                break;

            case SELECT_UDP:
                PRINT_INFO_STR("The UDP client operation begins");
                status = udp_client_operation(argv);
                if (NX_SUCCESS != status)
                {
                    PRINT_ERR_STR("The UDP client operation failed\r\n");
                }
                else
                {
                    PRINT_INFO_STR("The UDP client operation was successful\r\n");
                }
                break;

            case SELECT_DNS:
                PRINT_INFO_STR("The DNS client operation begins");
                status = dns_client_operation(argv);
                if (NX_SUCCESS != status)
                {
                    PRINT_ERR_STR("The DNS client operation failed\r\n");
                }
                else
                {
                    PRINT_INFO_STR("The DNS client operation was successful\r\n");
                }
                break;

            case SELECT_WEB:
                PRINT_INFO_STR("The WEB HTTP server operation begins");
                status = web_server_operation();
                if (NX_SUCCESS != status)
                {
                    PRINT_ERR_STR("The WEB HTTP server operation failed\r\n");
                }
                else
                {
                    PRINT_INFO_STR("The WEB HTTP server operation was successful\r\n");
                }
                break;

            default:
                PRINT_INFO_STR("User selection is invalid\r\n");
                break;
        }
        tx_thread_sleep (NET_APPLICATION_THREAD_SLEEP_TICK);
    }
}

/*******************************************************************************************************************//**
 * @brief Creates network application components, including TCP and UDP sockets, DNS instance, and a web HTTP server.
 *
 * @retval NX_SUCCESS on successful operation, other error codes otherwise.
 **********************************************************************************************************************/
static UINT net_app_create(void)
{
    UINT status = NX_SUCCESS;

    /* Create a TCP socket. */
    status = nx_tcp_socket_create(&g_ip0, &g_tcp_client_socket, "TCP Client Socket",
                                  NX_IP_NORMAL, NX_FRAGMENT_OKAY, NX_IP_TIME_TO_LIVE, 200, NX_NULL, NX_NULL);
    ERROR_RET(NX_SUCCESS != status, status, "nx_tcp_socket_create failed");

    /* Bind the TCP client socket to an available port number.  */
    status = nx_tcp_client_socket_bind(&g_tcp_client_socket, NX_ANY_PORT, NX_IP_PERIODIC_RATE);
    ERROR_RET(NX_SUCCESS != status, status, "nx_tcp_client_socket_bind failed");

    /* Create a UDP socket. */
    status = nx_udp_socket_create(&g_ip0, &g_udp_client_socket, "UDP Client Socket",
                                  NX_IP_NORMAL, NX_FRAGMENT_OKAY, NX_IP_TIME_TO_LIVE, NET_UDP_CLIENT_QUEUE_SIZE);
    ERROR_RET(NX_SUCCESS != status, status, "nx_udp_socket_create failed");

    /* Bind the UDP client socket to an available port number. */
    status = nx_udp_socket_bind(&g_udp_client_socket, NX_ANY_PORT, NX_IP_PERIODIC_RATE);
    ERROR_RET(NX_SUCCESS != status, status, "nx_udp_socket_bind failed");

    /* Create a DNS instance for the DNS client. */
    status = nx_dns_create(&g_dns_client, &g_ip0, (UCHAR *)"DNS Client Socket");
    ERROR_RET(NX_SUCCESS != status, status, "nx_dns_create failed");

    /* Set the packet poll for the DNS client instance. */
    status = nx_dns_packet_pool_set(&g_dns_client, &g_packet_pool0);
    ERROR_RET(NX_SUCCESS != status, status, "nx_dns_packet_pool_set failed");

    /* Add the primary Google Public DNS server. */
    status = nx_dns_server_add(&g_dns_client, NET_DNS_SERVER_ADDR_1);
    ERROR_RET(NX_SUCCESS != status, status, "nx_dns_server_add failed");

    /* Add the secondary Google Public DNS server. */
    status = nx_dns_server_add(&g_dns_client, NET_DNS_SERVER_ADDR_2);
    ERROR_RET(NX_SUCCESS != status, status, "nx_dns_server_add failed");

    /* Create the file system for the web HTTP server. */
    status = file_system_create();
    ERROR_RET(NX_SUCCESS != status, status, "web_http_server_file_system_create failed");

    /* Create the Web HTTP Server. */
    status = nx_web_http_server_create(&g_web_http_server0,
                                        "WEB HTTP Server", &g_ip0,
                                        G_WEB_HTTP_SERVER0_PORT_NUMBER, &g_fx_media0,
                                        g_web_http_server0_stack_memory, G_WEB_HTTP_SERVER0_STACK_SIZE,
                                        &g_packet_pool0, NX_NULL, server_request_callback);
    ERROR_RET(NX_SUCCESS != status, status, "nx_web_http_server_create failed");

    return status;
}

/*******************************************************************************************************************//**
 * @brief Create and open the file system using FileX.
 *
 * This function initializes the FileX block media driver and opens the media
 * for file system operations. It returns the status of these operations.
 *
 * @retval FX_SUCCESS on successful operation, other error codes otherwise.
 **********************************************************************************************************************/
static UINT file_system_create(void)
{
    UINT status = FX_SUCCESS;

    /* Hardware reset the flash device by driving RESET pin. */
    R_IOPORT_PinCfg(&g_ioport_ctrl, FLASH_RS_PIN, FLASH_GPIO_CFG);
    R_IOPORT_PinWrite(&g_ioport_ctrl, FLASH_RS_PIN, BSP_IO_LEVEL_LOW);
    R_BSP_SoftwareDelay(FLASH_RESET_TIME_US, BSP_DELAY_UNITS_MICROSECONDS);
    R_IOPORT_PinWrite(&g_ioport_ctrl, FLASH_RS_PIN, BSP_IO_LEVEL_HIGH);
    R_IOPORT_PinCfg(&g_ioport_ctrl, FLASH_RS_PIN, FLASH_XSPI_CFG);

    /* Initializes the block media driver used by FileX. */
    status = (UINT)RM_FILEX_BLOCK_MEDIA_Open(&g_rm_filex_block_media_0_ctrl, &g_rm_filex_block_media_0_cfg);
    ERROR_RET(FX_SUCCESS != status, status, "RM_FILEX_BLOCK_MEDIA_Open failed");

    /* Open the media for file system operations. */
    status = fx_media_open(&g_fx_media0,
                          G_FX_MEDIA0_VOLUME_NAME,
                          RM_FILEX_BLOCK_MEDIA_BlockDriver,
                          (void *) &g_rm_filex_block_media_0_instance,
                          g_fx_media0_media_memory,
                          G_FX_MEDIA0_MEDIA_MEMORY_SIZE);
    ERROR_RET(FX_SUCCESS != status, status, "fx_media_open failed");

    return status;
}

/*******************************************************************************************************************//**
 * @brief Performs TCP client operations including connecting to a server, sending data, and receiving a response.
 *
 * @param[in] p_input A string containing the server IP address and port in the format "x.x.x.x:port".
 *
 * @retval NX_SUCCESS on successful operation, other error codes otherwise.
 **********************************************************************************************************************/
static UINT tcp_client_operation(CHAR * p_input)
{
    UINT status = NX_SUCCESS;
    UINT ipv4_a = NX_NULL;
    UINT ipv4_b = NX_NULL;
    UINT ipv4_c = NX_NULL;
    UINT ipv4_d = NX_NULL;
    UINT port = NX_NULL;
    ULONG ipv4 = NX_NULL;
    ULONG len = NX_NULL;
    CHAR buffer [APP_BUFF_SIZE];
    NX_PACKET * p_packet = NX_NULL;

    /* Check length of the input string. */
    INFO_RET(NX_NULL == strlen(p_input), "The input arguments are invalid");

    /* Parse TCP client operation parameters. */
    sscanf(p_input, "%d.%d.%d.%d:%d", &ipv4_a, &ipv4_b, &ipv4_c, &ipv4_d, &port);
    ipv4 = IP_ADDRESS(ipv4_a, ipv4_b, ipv4_c, ipv4_d);

    /* Ping the TCP server to verify the connection. */
    status = net_system_ping(ipv4);
    INFO_RET(NX_SUCCESS != status, "Ping to the TCP server failed");

    /* Connect to the TCP server. */
    status = nx_tcp_client_socket_connect(&g_tcp_client_socket, ipv4, port, NET_TCP_CLIENT_WAIT);
    INFO_RET(NX_SUCCESS != status, "Connection to the TCP server failed");

    /* Allocate a TCP packet. */
    status = nx_packet_allocate(&g_packet_pool0, &p_packet, NX_TCP_PACKET, NX_IP_PERIODIC_RATE);
    if (NX_SUCCESS != status)
    {
        /* Disconnect from the TCP server. */
        nx_tcp_socket_disconnect(&g_tcp_client_socket, NET_TCP_CLIENT_WAIT);
        /* Print the error information */
        INFO_RET(NX_SUCCESS != status, "Allocating a TCP packet failed");
    }

    /* Write fixed data into the packet payload. */
    status = nx_packet_data_append(p_packet, NET_TCP_CLIENT_SEND_DATA, sizeof(NET_TCP_CLIENT_SEND_DATA), &g_packet_pool0, NX_IP_PERIODIC_RATE);
    if (NX_SUCCESS != status)
    {
        /* Release the TCP packet. */
        nx_packet_release(p_packet);
        /* Disconnect from the TCP server. */
        nx_tcp_socket_disconnect(&g_tcp_client_socket, NET_TCP_CLIENT_WAIT);
        /* Print the error information. */
        INFO_RET(NX_SUCCESS != status, "Appending data to the TCP packet failed");
    }

    /* Send the TCP packet. */
    status = nx_tcp_socket_send(&g_tcp_client_socket, p_packet, NET_TCP_CLIENT_WAIT);
    if(NX_SUCCESS != status)
    {
        /* Release the TCP packet */
        nx_packet_release(p_packet);
        /* Disconnect from the TCP server. */
        nx_tcp_socket_disconnect(&g_tcp_client_socket, NET_TCP_CLIENT_WAIT);
        /* Print the error information */
        INFO_RET(NX_SUCCESS != status, "Sending the TCP packet to the TCP server failed");
    }

    /* Send an output queue to print the TCP sent data. */
    PRINT_ID_STR(TERMINAL_OUTPUT_TCP_SEND_DATA, NET_TCP_CLIENT_SEND_DATA);

    /* Receive a TCP packet from the TCP server.  */
    status = nx_tcp_socket_receive(&g_tcp_client_socket, &p_packet, NET_TCP_CLIENT_WAIT);
    if (NX_SUCCESS != status)
    {
        /* Disconnect from the TCP server. */
        nx_tcp_socket_disconnect(&g_tcp_client_socket, NET_TCP_CLIENT_WAIT);
        /* Print the error information. */
        INFO_RET(NX_SUCCESS != status, "Receiving a TCP packet from the TCP server failed");
    }

    /* Retrieve data from the packet. */
    status = nx_packet_data_retrieve(p_packet, buffer, &len);
    if (NX_SUCCESS != status)
    {
        /* Release the TCP packet. */
        nx_packet_release(p_packet);
        /* Disconnect from the TCP server. */
        nx_tcp_socket_disconnect(&g_tcp_client_socket, NET_TCP_CLIENT_WAIT);
        /* Print the error information. */
        INFO_RET(NX_SUCCESS != status, "Retrieving data from the TCP packet failed");
    }

    /* Null-terminate the buffer string. */
    buffer[len] = NX_NULL;

    /* Send an output queue to print the TCP response data. */
    PRINT_ID_STR(TERMINAL_OUTPUT_TCP_RECV_DATA, buffer);

    /* Release TCP packet. */
    nx_packet_release(p_packet);

    /* Disconnect from the TCP server. */
    nx_tcp_socket_disconnect(&g_tcp_client_socket, NET_TCP_CLIENT_WAIT);

    return status;
}

/*******************************************************************************************************************//**
 * @brief Performs UDP client operations including sending data to a server and receiving a response.
 *
 * @param[in] p_input A string containing the server IP address and port in the format "x.x.x.x:port".
 *
 * @retval NX_SUCCESS on successful operation, other error codes otherwise.
 **********************************************************************************************************************/
static UINT udp_client_operation(CHAR * p_input)
{
    UINT status = NX_SUCCESS;
    UINT ipv4_a = NX_NULL;
    UINT ipv4_b = NX_NULL;
    UINT ipv4_c = NX_NULL;
    UINT ipv4_d = NX_NULL;
    UINT port = NX_NULL;
    ULONG ipv4 = NX_NULL;
    ULONG len = NX_NULL;
    CHAR buffer [APP_BUFF_SIZE];
    NX_PACKET * p_packet = NX_NULL;

    /* Check length of input string */
    INFO_RET(NX_NULL == strlen(p_input), "The input arguments are invalid");

    /* Parse UDP client operation parameters. */
    sscanf(p_input, "%d.%d.%d.%d:%d", &ipv4_a, &ipv4_b, &ipv4_c, &ipv4_d, &port);
    ipv4 = IP_ADDRESS(ipv4_a, ipv4_b, ipv4_c, ipv4_d);

    /* Ping the UDP server to verify the connection. */
    status = net_system_ping(ipv4);
    INFO_RET(NX_SUCCESS != status, "Ping to the UDP server failed");

    /* Allocate a UDP packet. */
    status = nx_packet_allocate(&g_packet_pool0, &p_packet, NX_UDP_PACKET, NX_IP_PERIODIC_RATE);
    INFO_RET(NX_SUCCESS != status, "Allocating a UDP packet failed");

    /* Write fixed data into the packet payload. */
    status = nx_packet_data_append(p_packet, NET_UDP_CLIENT_SEND_DATA, sizeof(NET_UDP_CLIENT_SEND_DATA), &g_packet_pool0, NX_IP_PERIODIC_RATE);
    if (NX_SUCCESS != status)
    {
        /* Release the UDP packet. */
        nx_packet_release(p_packet);
        /* Print the error information. */
        INFO_RET(NX_SUCCESS != status, "Appending data to the UDP packet failed");
    }

    /* Send the UDP packet. */
    status = nx_udp_socket_send(&g_udp_client_socket, p_packet, ipv4, port);
    if(NX_SUCCESS != status)
    {
        /* Release the UDP packet. */
        nx_packet_release(p_packet);
        /* Print the error information. */
        INFO_RET(NX_SUCCESS != status, "Sending the UDP packet to the UDP server failed");
    }

    /* Send an output queue to print the sent data. */
    PRINT_ID_STR(TERMINAL_OUTPUT_UDP_SEND_DATA, NET_UDP_CLIENT_SEND_DATA);

    /* Receive a UDP packet from the UDP server. */
    status = nx_udp_socket_receive(&g_udp_client_socket, &p_packet, NET_UDP_CLIENT_WAIT);
    INFO_RET(NX_SUCCESS != status, "Receiving a UDP packet from the UDP server failed");

    /* Retrieve data from the packet. */
    status = nx_packet_data_retrieve(p_packet, buffer, &len);
    if (NX_SUCCESS != status)
    {
        /* Release the UDP packet. */
        nx_packet_release(p_packet);
        /* Print the error information. */
        INFO_RET(NX_SUCCESS != status, "Retrieving data from the UDP packet failed");
    }

    /* Null-terminate the buffer string. */
    buffer[len] = NX_NULL;

    /* Send an output queue to print the received data. */
    PRINT_ID_STR(TERMINAL_OUTPUT_UDP_RECV_DATA, buffer);

    /* Release the UDP packet */
    nx_packet_release(p_packet);

    return status;
}

/*******************************************************************************************************************//**
 * @brief Performs DNS client operations, including resolving hostnames to IP addresses and vice versa.
 *
 * @param[in] p_input A string containing either a hostname or an IP address in the format "x.x.x.x".
 *
 * @retval NX_SUCCESS on successful operation, other error codes otherwise.
 **********************************************************************************************************************/
static UINT dns_client_operation(CHAR * p_input)
{
    UINT status = NX_SUCCESS;
    UINT ipv4_a = NX_NULL;
    UINT ipv4_b = NX_NULL;
    UINT ipv4_c = NX_NULL;
    UINT ipv4_d = NX_NULL;
    ULONG ipv4 = NX_NULL;
    UINT request = NX_FALSE;
    CHAR name[NET_DNS_NAME_MAX_LEN];

    /* Check length of input string */
    INFO_RET(NX_NULL == strlen(p_input), "The input arguments are invalid");

    /* Ping the Google DNS server to verify the Internet connection. */
    status = net_system_ping(NET_DNS_SERVER_ADDR_1);
    INFO_RET(NX_SUCCESS != status, "The Internet connection is not available");

    /* Determine whether the input string is a host name or an IP address. */
    for (UINT i = 0; i < strlen(p_input); i ++)
    {
        if ((CHAR_ZERO > p_input[i]) || (CHAR_NINE < p_input[i]))
        {
            if ((CHAR_DOT != p_input[i]) && (CHAR_COLON != p_input[i]))
            {
                request = NX_TRUE;
                break;
            }
        }
    }

    /* Request an Address Record. */
    if (NX_TRUE == request)
    {
        /* Perform a DNS query to resolve a given hostname into its corresponding IP address. */
        status = nx_dns_host_by_name_get(&g_dns_client, (UCHAR *)p_input, &ipv4, NET_DNS_CLIENT_WAIT);
        INFO_RET(NX_SUCCESS != status, "Resolving the IP address by the host name failed");

        /* Send an output queue to print the DNS Address record. */
        PRINT_ID_DATA(TERMINAL_OUTPUT_DNS_HOST_ADDR, ipv4);
    }

    /* Request a Pointer Record. */
    else
    {
        /* Parse the input string to obtain the host IP address */
        sscanf(p_input, "%d.%d.%d.%d", &ipv4_a, &ipv4_b, &ipv4_c, &ipv4_d);
        ipv4 = IP_ADDRESS(ipv4_a, ipv4_b, ipv4_c, ipv4_d);

        /* Clean the buffer that stores the hostname. */
        memset(name, RESET_VALUE, sizeof(name));

        /* Perform a DNS query to determine the hostname associated with a given IP address. */
        status = nx_dns_host_by_address_get(&g_dns_client, ipv4, (UCHAR *)name, sizeof(name), NET_DNS_CLIENT_WAIT);
        INFO_RET(NX_SUCCESS != status, "Resolving the host name by the host's IP address failed");

        /* Send an output queue to print the DNS PTR record. */
        PRINT_ID_STR(TERMINAL_OUTPUT_DNS_HOST_NAME, name);
    }

    return status;
}

/*******************************************************************************************************************//**
 * @brief Starts the web server, waits for user input, and then stops the server.
 *
 * @retval NX_SUCCESS on successful operation, other error codes otherwise.
 **********************************************************************************************************************/
static UINT web_server_operation(void)
{
    UINT status = NX_SUCCESS;
    UINT len = NX_NULL;
    CHAR input [INPUT_MAX_SIZE];

    /* Start the web server's internal thread. */
    status = nx_web_http_server_start(&g_web_http_server0);
    INFO_RET(NX_SUCCESS != status, "Starting the web server failed");

    PRINT_INFO_STR("Type any key to stop the web server\r\n");

    /* Wait for any user input form terminal input queue. */
    terminal_get_input_queue(input, (uint32_t *)&len, TX_WAIT_FOREVER);

    /* Stop the web server's internal thread. */
    status = nx_web_http_server_stop(&g_web_http_server0);
    INFO_RET(NX_SUCCESS != status, "Stopping the web server failed");

    return status;
}

/*******************************************************************************************************************//**
 * @brief Callback function to handle HTTP server requests.
 *
 * This function processes incoming HTTP GET requests and sends appropriate responses based on the requested resource.
 *
 * @param[in] p_server Pointer to the web server instance.
 * @param[in] request_type Type of the HTTP request (GET, POST, etc.).
 * @param[in] p_resource Pointer to the requested resource string.
 * @param[in] p_packet Pointer to the packet associated with the request.
 *
 * @retval UINT NX_WEB_HTTP_CALLBACK_COMPLETED if the request is successfully handled, otherwise an error status.
 **********************************************************************************************************************/
static UINT server_request_callback(NX_WEB_HTTP_SERVER *p_server, UINT request_type, CHAR * p_resource, NX_PACKET * p_packet)
{
    UINT status = NX_SUCCESS;

    (void)p_packet;

    if (NX_WEB_HTTP_SERVER_GET_REQUEST == request_type)
    {
        if (NX_NULL == strcmp(HTTP_RESOURCE_NETWORK, p_resource))
        {
            /* Send a response for the GET network configuration request. */
            status = server_callback_response_net_info (p_server, p_resource);
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
            /* Send a response for the GET LED status request. */
            status = server_callback_response_led_status (p_server, p_resource);
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
            /* Set the LED 1 event flag. */
            tx_event_flags_set(&g_led_event_flags, LED_1_TOGGLE_EVENT, TX_OR);

            /* Send a response for the GET Switch 1 request. */
            status = server_callback_response_switch_event (p_server, p_resource);
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
            /* Set the LED 2 event flag. */
            tx_event_flags_set(&g_led_event_flags, LED_2_TOGGLE_EVENT, TX_OR);

            /* Send a response to the Switch 2 resource GET request. */
            status = server_callback_response_switch_event (p_server, p_resource);
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
 * @brief Handles the HTTP response for network configuration requests.
 *
 * This function generates and sends an HTTP response containing the current network configuration,
 * including physical address, IPv4 address, subnet mask, and default gateway.
 *
 * @param[in] p_server Pointer to the web server instance.
 * @param[in] p_resource Pointer to the requested resource string.
 *
 * @retval NX_SUCCESS on successful operation, other error codes otherwise.
 **********************************************************************************************************************/
static UINT server_callback_response_net_info (NX_WEB_HTTP_SERVER * p_server, CHAR * p_resource)
{
    UINT status = NX_SUCCESS;
    UINT length = NX_NULL;
    CHAR content_type[HTTP_TYPE_MAX_LEN];
    CHAR content_body[HTTP_BODY_MAX_LEN];
    NX_PACKET * p_packet = NX_NULL;

    /* Determine the client request type from the resource string. */
    status = nx_web_http_server_type_get(p_server, p_resource, content_type, &length);
    if (NX_SUCCESS != status && NX_WEB_HTTP_EXTENSION_MIME_DEFAULT != status)
    {
        return status;
    }

    /* Null-terminate the content type string. */
    content_type[length] = NX_NULL;
    
    /* Generate an HTTP response header with status OK and no additional headers. */
    status = nx_web_http_server_callback_generate_response_header(p_server, &p_packet, NX_WEB_HTTP_STATUS_OK, 
                                                                   NX_NULL, content_type, NX_NULL);
    if (NX_SUCCESS != status)
    {
        return status;
    }

    /* Clean the HTTP content body buffer. */
    memset(content_body , RESET_VALUE, sizeof(content_body));

    /* Format the current network configuration as a JSON string. */
    length = snprintf(content_body, sizeof(content_body), JSON_NET_INFO,
                      g_network_info.phy_addr.block.a, g_network_info.phy_addr.block.b, g_network_info.phy_addr.block.c,
                      g_network_info.phy_addr.block.d, g_network_info.phy_addr.block.e, g_network_info.phy_addr.block.f,
                      g_network_info.ipv4_addr.block.a, g_network_info.ipv4_addr.block.b,
                      g_network_info.ipv4_addr.block.c, g_network_info.ipv4_addr.block.d,
                      g_network_info.subnet_mask.block.a, g_network_info.subnet_mask.block.b,
                      g_network_info.subnet_mask.block.c, g_network_info.subnet_mask.block.d,
                      g_network_info.default_gateway.block.a, g_network_info.default_gateway.block.b,
                      g_network_info.default_gateway.block.c, g_network_info.default_gateway.block.d);

    /* Append the content body to the response packet. */
    status = nx_packet_data_append(p_packet, content_body, length,
                                   p_server -> nx_web_http_server_packet_pool_ptr, NX_WAIT_FOREVER);
    if (status != NX_SUCCESS)
    {
        nx_packet_release(p_packet);
        return status;
    }

    /* Send the response packet to the client. */
    status = nx_web_http_server_callback_packet_send(p_server, p_packet);
    if (status != NX_SUCCESS)
    {
        nx_packet_release(p_packet);
        return status;
    }
    return status;
}

/*******************************************************************************************************************//**
 * @brief Handles the HTTP response for LED status requests.
 *
 * This function processes the client request, updates the content body with the LED status,
 * and generates an HTTP response that is sent back to the client.
 *
 * @param[in] p_server Pointer to the HTTP server instance.
 * @param[in] p_resource Pointer to the client request resource string.
 *
 * @retval NX_SUCCESS on successful operation, other error codes otherwise.
 **********************************************************************************************************************/
static UINT server_callback_response_led_status (NX_WEB_HTTP_SERVER * p_server, CHAR * p_resource)
{
    UINT status = NX_SUCCESS;
    UINT length = NX_NULL;
    CHAR content_type[HTTP_TYPE_MAX_LEN];
    CHAR content_body[HTTP_BODY_MAX_LEN];
    NX_PACKET * p_packet = NX_NULL;

    /* Determine the client request type from the resource string. */
    status = nx_web_http_server_type_get(p_server, p_resource, content_type, &length);
    if (NX_SUCCESS != status && NX_WEB_HTTP_EXTENSION_MIME_DEFAULT != status)
    {
        return status;
    }

    /* Null-terminate the content type string. */
    content_type[length] = NX_NULL;

    /* Generate an HTTP response header with status OK and no additional headers. */
    status = nx_web_http_server_callback_generate_response_header(p_server, &p_packet,
                                                                  NX_WEB_HTTP_STATUS_OK, NX_NULL, content_type, NX_NULL);
    if (NX_SUCCESS != status)
    {
        return status;
    }

    /* Clean the HTTP content body buffer. */
    memset(content_body , RESET_VALUE, sizeof(content_body));

    /* Format the current LED status as a JSON string. */
    length = snprintf(content_body, sizeof(content_body), JSON_LED_STATUS,
                      (BSP_IO_LEVEL_HIGH == g_led_1_status) ? "true" : "false",
                      (BSP_IO_LEVEL_HIGH == g_led_2_status) ? "true" : "false");

    /* Append the content body to the response packet. */
    status = nx_packet_data_append(p_packet, content_body, length,
                                   p_server -> nx_web_http_server_packet_pool_ptr, NX_WAIT_FOREVER);
    if (status != NX_SUCCESS)
    {
        nx_packet_release(p_packet);
        return status;
    }

    /* Send the response packet to the client. */
    status = nx_web_http_server_callback_packet_send(p_server, p_packet);
    if (status != NX_SUCCESS)
    {
        nx_packet_release(p_packet);
        return status;
    }
    return status;
}

/*******************************************************************************************************************//**
 * @brief Handles the HTTP response for switch control requests.
 *
 * This function generates and sends an HTTP response indicating the status of a switch request.
 *
 * @param[in] p_server Pointer to the web server instance.
 * @param[in] p_resource Pointer to the requested resource string.
 *
 * @retval NX_SUCCESS on successful operation, other error codes otherwise.
 **********************************************************************************************************************/
static UINT server_callback_response_switch_event (NX_WEB_HTTP_SERVER * p_server, CHAR * p_resource)
{
    UINT status = NX_SUCCESS;
    UINT length = NX_NULL;
    CHAR content_type[HTTP_TYPE_MAX_LEN];
    NX_PACKET * p_packet = NX_NULL;

    /* Determine the client request type from the resource string. */
    status = nx_web_http_server_type_get(p_server, p_resource, content_type, &length);
    if (NX_SUCCESS != status && NX_WEB_HTTP_EXTENSION_MIME_DEFAULT != status)
    {
        return status;
    }

    /* Null-terminate the content type string. */
    content_type[length] = NX_NULL;

    /* Generate an HTTP response header with status OK and no additional headers. */
    status = nx_web_http_server_callback_generate_response_header(p_server, &p_packet,
                                                                  NX_WEB_HTTP_STATUS_OK, NX_NULL, content_type, NX_NULL);
    if (NX_SUCCESS != status)
    {
        return status;
    }

    /* Send the response packet to the client. */
    status = nx_web_http_server_callback_packet_send(p_server, p_packet);
    if (status != NX_SUCCESS)
    {
        nx_packet_release(p_packet);
        return status;
    }
    return status;
}
