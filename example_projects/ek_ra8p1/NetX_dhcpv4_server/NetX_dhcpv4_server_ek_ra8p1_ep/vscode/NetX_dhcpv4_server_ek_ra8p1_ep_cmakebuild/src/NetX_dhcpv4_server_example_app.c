/***********************************************************************************************************************
 * File Name    : NetX_dhcpv4_server_example_app.c
 * Description  : Example application layer for the NetX DHCPv4 server EP.
 *                - All printing via printf (routed to UART via user stdio library) in the terminal thread.
 *                - dhcpv4_server_thread_entry.c calls a single function: dhcpv4_server_thread_entry_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdlib.h>
#include "dhcpv4_server_ep.h"
#include "NetX_dhcpv4_server_example_app.h"

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the DHCPv4 server thread.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void dhcpv4_server_thread_entry_run(void)
{
    UINT status = NX_SUCCESS;
    UINT active_client_count = RESET_VALUE;
    UINT read_data = RESET_VALUE;
    CHAR input_buff[TERM_IP_MAX_SIZE] = {NULL_CHAR};
    CHAR client_name[NX_DHCP_CLIENT_HOSTNAME_MAX] = {NULL_CHAR};
    ULONG client_ip = RESET_VALUE;
    size_t client_data_len = RESET_VALUE;
    bool b_server_ready = false;

    /* Initialize the Terminal thread */
    term_thread_init_check();

    /* Print the EP banner and information */
    term_print_data(TERM_OP_MSG_BANNER, RESET_VALUE, NULL);

    /* Initialize the NetX system */
    nx_common_init();

    /* Initialize the packet pool */
    status = packet_pool_init();
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("packet_pool_init failed");
        ERROR_TRAP(status);
        return;
    }

    /* Create the IP instance */
    status = ip_init();
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("ip_init failed");
        ERROR_TRAP(status);
        return;
    }

    /* Wait for the Ethernet link to be enabled */
    PRINT_INFO_STR("Checking Ethernet link...");
    status = ip_link_check();
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("ip_link_check failed");
        ERROR_TRAP(status);
        return;
    }
    PRINT_INFO_STR("Ethernet link is up");

    /* Create the DHCP server instance */
    status = dhcpv4_server_init();
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("dhcpv4_server_init failed");
        ERROR_TRAP(status);
        return;
    }
    PRINT_INFO_STR("Network initialization completed successfully");

    /* Create a pool of IP addresses from the given range */
    status = dhcpv4_ip_address_list_create();
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("dhcpv4_ip_address_list_create failed");
        ERROR_TRAP(status);
        return;
    }
    PRINT_INFO_STR("DHCP server IP address pool created successfully");

    /* Set the network parameters returned by the DHCP server */
    status = dhcpv4_network_parameters_set();
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("dhcpv4_network_parameters_set failed");
        ERROR_TRAP(status);
        return;
    }
    PRINT_INFO_STR("DHCP server network parameters set successfully");

    /* Print the DHCPv4 server menu */
    term_print_data(TERM_OP_MSG_MENU, RESET_VALUE, NULL);

    while (true)
    {
        /* Wait for the user input */
        do
        {
            status = term_ip_get(input_buff, sizeof(input_buff));
        } while (TX_SUCCESS != status);

        /* Convert the input string to an integer value */
        read_data = (UINT)atoi(input_buff);

        switch (read_data)
        {
            case DHCPV4_SERVER_START:
            {
                if (false == b_server_ready)
                {
                    /* Start the DHCP server processing */
                    status = dhcpv4_server_start();
                    if (NX_SUCCESS != status)
                    {
                        PRINT_ERR_STR("dhcpv4_server_start failed");
                        ERROR_TRAP(status);
                        return;
                    }
                    PRINT_INFO_STR("DHCPv4 server started");

                    /* Set the server ready flag to true */
                    b_server_ready = true;
                }
                else
                {
                    PRINT_INFO_STR("DHCPv4 server is already running");
                }
                break;
            }

            case DHCPV4_SERVER_STOP:
            {
                if (true == b_server_ready)
                {
                    /* Stop the DHCP server processing */
                    status = dhcpv4_server_stop();
                    if (NX_SUCCESS != status)
                    {
                        PRINT_ERR_STR("dhcpv4_server_stop failed");
                        ERROR_TRAP(status);
                        return;
                    }
                    PRINT_INFO_STR("DHCPv4 server stopped");

                    /* Set the server ready flag to false */
                    b_server_ready = false;
                }
                else
                {
                    PRINT_INFO_STR("DHCPv4 server is not started");
                }
                break;
            }

            case DHCPV4_CLIENT_INFO:
            {
                if (true == b_server_ready)
                {
                    /* Update the number of active clients connected, the client name and the client IP */
                    active_client_count = g_dhcp_server0.nx_dhcp_number_clients;
                    if (RESET_VALUE < active_client_count)
                    {
                        memset(client_name, NULL_CHAR, sizeof(client_name));
                        strncpy(client_name,
                                g_dhcp_server0.client_records[active_client_count - 1U].nx_dhcp_client_name,
                                sizeof(client_name) - 1U);
                        client_data_len = strlen(client_name) + 1U;
                        client_ip = g_dhcp_server0.client_records[active_client_count - 1U].nx_dhcp_assigned_ip_address;

                        /* Print the DHCPv4 client information */
                        term_print_data(TERM_OP_MSG_CLIENT_NUM, sizeof(active_client_count), &active_client_count);
                        term_print_data(TERM_OP_MSG_CLIENT_NAME, client_data_len, client_name);
                        term_print_data(TERM_OP_MSG_CLIENT_IP, sizeof(client_ip), &client_ip);
                    }
                    else
                    {
                        PRINT_INFO_STR("No DHCPv4 clients are connected");
                    }

                }
                else
                {
                    PRINT_INFO_STR("DHCPv4 server is not started");
                }
                break;
            }

            default:
            {
                PRINT_INFO_STR("Invalid menu option selected");
                break;
            }
        }

        /* Print the DHCPv4 server menu */
        term_print_data(TERM_OP_MSG_MENU, RESET_VALUE, NULL);

        tx_thread_sleep(1);
    }
}
