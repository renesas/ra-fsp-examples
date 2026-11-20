/***********************************************************************************************************************
 * File Name    : Getting_started_with_Azure_NetX_example_app.c
 * Description  : Example application layer for the Getting_started_with_Azure_NetX EP.
 *                - All printing via printf (routed to UART through the Terminal Thread and the console library).
 *                - net_link_thread_entry calls a single function: net_link_thread_entry_run().
 *                - net_app_thread_entry calls a single function: net_app_thread_entry_run().
 *                - led_control_thread_entry calls a single function: led_control_thread_entry_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "Getting_started_with_Azure_NetX_example_app.h"

/* Private functions */
static UINT tcp_client_operation(CHAR * p_input);
static UINT udp_client_operation(CHAR * p_input);
static UINT dns_client_operation(CHAR * p_input);
static UINT web_server_operation(void);

/*******************************************************************************************************************//**
 * @brief     Net Link Thread entry-point for the Getting_started_with_Azure_NetX example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void net_link_thread_entry_run(void)
{
    UINT status = NX_SUCCESS;
    ULONG events = RESET_VALUE;

    /* Print EP and banner info */
    term_print_data(TERM_OP_MSG_BANNER, RESET_VALUE, NULL);
    tx_thread_sleep(NET_LINK_THREAD_SLEEP_TICK);

    /* Startup the network system */
    status = net_system_startup();
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("net_system_startup failed.");
        ERROR_TRAP(status);
        return;
    }

    /* Print the current network configuration */
    term_print_data(TERM_OP_MSG_NET_INFO, sizeof(g_network_info), &g_network_info);

    /* Start the Net App Thread, which does not start automatically during initialization */
    tx_thread_resume(&net_app_thread);

    while (true)
    {
        /* Wait for any NetX link status change event flags */
        status = net_link_event_get(&events);
        if (NX_SUCCESS != status)
        {
            PRINT_ERR_STR("net_link_event_get failed.");
            ERROR_TRAP(status);
            return;
        }

        /* Handle the physical link-up event */
        if (events & LINK_UP_EVENT)
        {
            /* Perform necessary procedures when physical link-up */
            status = net_system_link_up_process();
            if (NX_SUCCESS != status)
            {
                PRINT_ERR_STR("net_system_link_up_process failed.");
                ERROR_TRAP(status);
                return;
            }

            /* Print the current network configuration */
            term_print_data(TERM_OP_MSG_NET_INFO, sizeof(g_network_info), &g_network_info);

            /* Resume the Net App Thread */
            tx_thread_resume(&net_app_thread);
        }

        /* Handle the physical link-down event */
        if (events & LINK_DOWN_EVENT)
        {
            /* Suspend the Net App Thread */
            tx_thread_suspend(&net_app_thread);
        }

        tx_thread_sleep(NET_LINK_THREAD_SLEEP_TICK);
    }
}

/*******************************************************************************************************************//**
 * @brief     Net App Thread entry-point for the Getting_started_with_Azure_NetX example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void net_app_thread_entry_run(void)
{
    UINT status = NX_SUCCESS;
    UINT select = RESET_VALUE;
    CHAR input[TERM_IP_MAX_SIZE] = {NULL_CHAR};
    CHAR argv[TERM_IP_MAX_SIZE] = {NULL_CHAR};

    /* Create the necessary resources for all the network operations */
    status = net_app_create();
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("net_app_create failed.");
        ERROR_TRAP(status);
        return;
    }

    while (true)
    {
        /* Print the menu options */
        term_print_data(TERM_OP_MSG_MENU, RESET_VALUE, NULL);

        /* Clean the buffers */
        memset(input, NULL_CHAR, sizeof(input));
        memset(argv, NULL_CHAR, sizeof(argv));

        /* Wait for the user input */
        do
        {
            status = term_ip_get(input, sizeof(input));
        } while (TX_SUCCESS != status);

        /* Parse the user's selection and retrieve the network operation argument in string format. The width in the
         * format string is TERM_IP_MAX_SIZE - 1 */
        select = RESET_VALUE;
        sscanf(input, "%u %64s", &select, argv);

        /* Perform the selected operation based on the user's input */
        switch (select)
        {
            case SELECT_TCP:
            {
                PRINT_INFO_STR("The TCP client operation begins");
                status = tcp_client_operation(argv);
                if (NX_SUCCESS != status)
                {
                    PRINT_ERR_STR("The TCP client operation failed");
                }
                else
                {
                    PRINT_INFO_STR("The TCP client operation was successful");
                }
                break;
            }

            case SELECT_UDP:
            {
                PRINT_INFO_STR("The UDP client operation begins");
                status = udp_client_operation(argv);
                if (NX_SUCCESS != status)
                {
                    PRINT_ERR_STR("The UDP client operation failed");
                }
                else
                {
                    PRINT_INFO_STR("The UDP client operation was successful");
                }
                break;
            }

            case SELECT_DNS:
            {
                PRINT_INFO_STR("The DNS client operation begins");
                status = dns_client_operation(argv);
                if (NX_SUCCESS != status)
                {
                    PRINT_ERR_STR("The DNS client operation failed");
                }
                else
                {
                    PRINT_INFO_STR("The DNS client operation was successful");
                }
                break;
            }

            case SELECT_WEB:
            {
                PRINT_INFO_STR("The web HTTP server operation begins");
                status = web_server_operation();
                if (NX_SUCCESS != status)
                {
                    PRINT_ERR_STR("The web HTTP server operation failed");
                }
                else
                {
                    PRINT_INFO_STR("The web HTTP server operation was successful");
                }
                break;
            }

            default:
            {
                PRINT_INFO_STR("User selection is invalid");
                break;
            }
        }

        tx_thread_sleep(NET_APPLICATION_THREAD_SLEEP_TICK);
    }
}

/*******************************************************************************************************************//**
 * @brief     LED Control Thread entry-point for the Getting_started_with_Azure_NetX example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void led_control_thread_entry_run(void)
{
    UINT status = TX_SUCCESS;
    fsp_err_t err = FSP_SUCCESS;
    ULONG events = RESET_VALUE;

    /* Initialize the external interrupt pins for LED1 and LED2 control */
    err = led_irq_init();
    if (FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("led_irq_init failed.");
        ERROR_TRAP(err);
        return;
    }

    /* Create an event flag group for LED control */
    status = led_event_flags_create();
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("led_event_flags_create failed.");
        ERROR_TRAP(status);
        return;
    }

    /* Enable external pins to generate interrupts for SW1 and SW2 */
    err = led_irq_enable();
    if (FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("led_irq_enable failed.");
        ERROR_TRAP(err);
        return;
    }

    while (true)
    {
        /* Wait for any LED control events */
        status = led_event_get(&events);
        if (NX_SUCCESS != status)
        {
            PRINT_ERR_STR("led_event_get failed.");
            ERROR_TRAP(status);
            return;
        }

        /* Toggle the LED states */
        led_toggle(events);

        tx_thread_sleep(LED_CONTROL_THREAD_SLEEP_TICK);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function performs the TCP client operation including pinging and connecting to a server, sending
 *            data, and receiving a response.
 * @param[in] p_input                   A string containing the server IP address and port in the format
 *                                      "x.x.x.x:port".
 * @retval    NX_SUCCESS                Performed successfully.
 * @retval    Any other error codes     Failed to perform.
 **********************************************************************************************************************/
static UINT tcp_client_operation(CHAR * p_input)
{
    UINT status = NX_SUCCESS;
    UINT ipv4_a = RESET_VALUE;
    UINT ipv4_b = RESET_VALUE;
    UINT ipv4_c = RESET_VALUE;
    UINT ipv4_d = RESET_VALUE;
    UINT port = RESET_VALUE;
    ULONG ipv4 = RESET_VALUE;
    CHAR buffer[APP_BUFF_SIZE] = {NULL_CHAR};

    /* Check length of the input string */
    if (RESET_VALUE == strlen(p_input))
    {
        PRINT_ERR_STR("The input arguments are invalid");
        return NX_INVALID_PARAMETERS;
    }

    /* Parse TCP client operation parameters */
    sscanf(p_input, "%u.%u.%u.%u:%u", &ipv4_a, &ipv4_b, &ipv4_c, &ipv4_d, &port);
    ipv4 = IP_ADDRESS(ipv4_a, ipv4_b, ipv4_c, ipv4_d);

    /* Ping the TCP server to verify the connection */
    status = net_system_ping(ipv4);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("Ping to the TCP server failed");
        return status;
    }

    /* Connect to the TCP server */
    status = tcp_client_connect(ipv4, port);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("Connection to the TCP server failed");
        return status;
    }

    /* Send the data to the TCP server */
    status = tcp_client_send(NET_TCP_CLIENT_SEND_DATA, sizeof(NET_TCP_CLIENT_SEND_DATA));
    if (NX_SUCCESS != status)
    {
        tcp_client_disconnect();
        PRINT_ERR_STR("Sending the TCP packet to the TCP server failed");
        return status;
    }

    /* Print the TCP sent data */
    term_print_data(TERM_OP_MSG_TCP_SEND_DATA, sizeof(NET_TCP_CLIENT_SEND_DATA), NET_TCP_CLIENT_SEND_DATA);

    /* Receive the response from the TCP server */
    status = tcp_client_receive(buffer);
    if (NX_SUCCESS != status)
    {
        tcp_client_disconnect();
        PRINT_ERR_STR("Receiving a TCP packet from the TCP server failed");
        return status;
    }

    /* Print the TCP response data */
    term_print_data(TERM_OP_MSG_TCP_RECV_DATA, (uint32_t)(strlen(buffer) + 1U), buffer);

    /* Disconnect from the TCP server */
    tcp_client_disconnect();

    return status;
}

/*******************************************************************************************************************//**
 * @brief     This function performs the UDP client operation including pinging a server, sending data to it, and
 *            receiving a response.
 * @param[in] p_input                   A string containing the server IP address and port in the format
 *                                      "x.x.x.x:port".
 * @retval    NX_SUCCESS                Performed successfully.
 * @retval    Any other error codes     Failed to perform.
 **********************************************************************************************************************/
static UINT udp_client_operation(CHAR * p_input)
{
    UINT status = NX_SUCCESS;
    UINT ipv4_a = RESET_VALUE;
    UINT ipv4_b = RESET_VALUE;
    UINT ipv4_c = RESET_VALUE;
    UINT ipv4_d = RESET_VALUE;
    UINT port = RESET_VALUE;
    ULONG ipv4 = RESET_VALUE;
    CHAR buffer[APP_BUFF_SIZE] = {NULL_CHAR};

    /* Check length of the input string */
    if (RESET_VALUE == strlen(p_input))
    {
        PRINT_ERR_STR("The input arguments are invalid");
        return NX_INVALID_PARAMETERS;
    }

    /* Parse UDP client operation parameters */
    sscanf(p_input, "%u.%u.%u.%u:%u", &ipv4_a, &ipv4_b, &ipv4_c, &ipv4_d, &port);
    ipv4 = IP_ADDRESS(ipv4_a, ipv4_b, ipv4_c, ipv4_d);

    /* Ping the UDP server to verify the connection */
    status = net_system_ping(ipv4);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("Ping to the UDP server failed");
        return status;
    }

    /* Send the data to the UDP server */
    status = udp_client_send(ipv4, port, NET_UDP_CLIENT_SEND_DATA, sizeof(NET_UDP_CLIENT_SEND_DATA));
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("Sending the UDP packet to the UDP server failed");
        return status;
    }

    /* Print the UDP sent data */
    term_print_data(TERM_OP_MSG_UDP_SEND_DATA, sizeof(NET_UDP_CLIENT_SEND_DATA), NET_UDP_CLIENT_SEND_DATA);

    /* Receive the response from the UDP server */
    status = udp_client_receive(buffer);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("Receiving a UDP packet from the UDP server failed");
        return status;
    }

    /* Print the UDP response data */
    term_print_data(TERM_OP_MSG_UDP_RECV_DATA, (uint32_t)(strlen(buffer) + 1U), buffer);

    return status;
}

/*******************************************************************************************************************//**
 * @brief     This function performs the DNS client operation, including resolving a host name to an IP address and
 *            vice versa.
 * @param[in] p_input                   A string containing either a host name or an IP address in the format
 *                                      "x.x.x.x".
 * @retval    NX_SUCCESS                Performed successfully.
 * @retval    Any other error codes     Failed to perform.
 **********************************************************************************************************************/
static UINT dns_client_operation(CHAR * p_input)
{
    UINT status = NX_SUCCESS;
    UINT ipv4_a = RESET_VALUE;
    UINT ipv4_b = RESET_VALUE;
    UINT ipv4_c = RESET_VALUE;
    UINT ipv4_d = RESET_VALUE;
    ULONG ipv4 = RESET_VALUE;
    bool b_request_name = false;
    CHAR name[NET_DNS_NAME_MAX_LEN] = {NULL_CHAR};

    /* Check length of the input string */
    if (RESET_VALUE == strlen(p_input))
    {
        PRINT_ERR_STR("The input arguments are invalid");
        return NX_INVALID_PARAMETERS;
    }

    /* Ping the Google DNS server to verify the Internet connection */
    status = net_system_ping(NET_DNS_SERVER_ADDR_1);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("The Internet connection is not available");
        return status;
    }

    /* Determine whether the input string is a host name or an IP address */
    for (UINT i = 0; i < strlen(p_input); i++)
    {
        if (('0' > p_input[i]) || ('9' < p_input[i]))
        {
            if (('.' != p_input[i]) && (':' != p_input[i]))
            {
                b_request_name = true;
                break;
            }
        }
    }

    if (b_request_name)
    {
        /* Perform a DNS query to resolve a given host name into its corresponding IP address */
        status = dns_host_by_name(p_input, &ipv4);
        if (NX_SUCCESS != status)
        {
            PRINT_ERR_STR("Resolving the IP address by the host name failed");
            return status;
        }

        /* Print the DNS address record */
        term_print_data(TERM_OP_MSG_DNS_HOST_ADDR, sizeof(ipv4), &ipv4);
    }
    else
    {
        /* Parse the input string to obtain the host IP address */
        sscanf(p_input, "%u.%u.%u.%u", &ipv4_a, &ipv4_b, &ipv4_c, &ipv4_d);
        ipv4 = IP_ADDRESS(ipv4_a, ipv4_b, ipv4_c, ipv4_d);

        /* Determine the host name associated with the given IP address */
        status = dns_host_by_address(ipv4, name, sizeof(name));
        if (NX_SUCCESS != status)
        {
            PRINT_ERR_STR("Resolving the host name by the host's IP address failed");
            return status;
        }

        /* Print the DNS PTR record */
        term_print_data(TERM_OP_MSG_DNS_HOST_NAME, (uint32_t)(strlen(name) + 1U), name);
    }

    return status;
}

/*******************************************************************************************************************//**
 * @brief     This function starts the web server, waits for user input, and then stops the server.
 * @param[in] None.
 * @retval    NX_SUCCESS                Performed successfully.
 * @retval    Any other error codes     Failed to perform.
 **********************************************************************************************************************/
static UINT web_server_operation(void)
{
    UINT status = NX_SUCCESS;
    CHAR input[TERM_IP_MAX_SIZE] = {NULL_CHAR};
    CHAR info[WEB_SERVER_INFO_SIZE] = {NULL_CHAR};

    /* Start the web server's internal thread */
    status = web_server_start();
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("Starting the web server failed");
        return status;
    }

    /* Print the address of the web page for the user to open in a browser */
    snprintf(info, sizeof(info), WEB_SERVER_INFO,
             g_network_info.ipv4_addr.block.a, g_network_info.ipv4_addr.block.b,
             g_network_info.ipv4_addr.block.c, g_network_info.ipv4_addr.block.d);
    PRINT_INFO_STR(info);

    PRINT_INFO_STR("Type any key and press ENTER to stop the web server\r\n");

    /* Wait for any user input */
    do
    {
        status = term_ip_get(input, sizeof(input));
    } while (TX_SUCCESS != status);

    /* Stop the web server's internal thread */
    status = web_server_stop();
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("Stopping the web server failed");
    }

    return status;
}
