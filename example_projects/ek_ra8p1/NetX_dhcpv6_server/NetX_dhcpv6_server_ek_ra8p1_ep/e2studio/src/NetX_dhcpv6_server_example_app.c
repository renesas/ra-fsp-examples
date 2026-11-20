/***********************************************************************************************************************
 * File Name    : NetX_dhcpv6_server_example_app.c
 * Description  : Example application layer for the NetX DHCPv6 Server EP.
 *                - All printing via printf (routed to UART through the Terminal Thread and the console library).
 *                - dhcpv6_thread_entry calls a single function: dhcpv6_thread_entry_run().
 *                - terminal_thread_entry calls a single function: terminal_thread_entry_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "dhcpv6_thread.h"
#include "terminal.h"
#include "dhcpv6_server.h"
#include "NetX_dhcpv6_server_example_app.h"

/* Private functions */
static void process_terminal_op_msg(VOID);
static void terminal_input_read(char * buf, int len);
static void nx_handle_error(UINT err, const char * err_str);
static void tx_handle_error(UINT err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     DHCPv6 Server Thread entry-point for the NetX DHCPv6 Server example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void dhcpv6_thread_entry_run(void)
{
    UINT err = NX_SUCCESS;
    uint8_t read_data = RESET_VALUE;
    char input_buff[IP_MSG_MAX_SIZE] = {NULL_CHAR};
    bool b_server_ready = false;
    bool b_ip_leased = false;
    ipv6_info_t leased_info =
    {
        .ipv6_address = {RESET_VALUE, {RESET_VALUE}},
        .mac_high = RESET_VALUE,
        .mac_low = RESET_VALUE,
    };

    /* Initialize the Terminal Thread */
    terminal_thread_init_check();

    /* Print the banner and EP info */
    app_terminal_print_data(TERMINAL_OUTPUT_MESSAGE_BANNER, RESET_VALUE, NULL);

    /* Initialize the NetX system */
    nx_common_init();

    /* Initialize the packet pool */
    err = packet_pool_init();
    nx_handle_error(err, "packet_pool_init failed.");

    /* Create the IP instance */
    err = ip_init();
    nx_handle_error(err, "ip_init failed.");

    /* Wait for the Ethernet link to be enabled */
    PRINT_INFO_STR("Checking Ethernet link...");
    err = ip_link_check();
    nx_handle_error(err, "ip_link_check failed.");
    PRINT_INFO_STR("Ethernet link is up.");

    /* Set the IPv6 addresses */
    err = ip_address_set();
    nx_handle_error(err, "ip_address_set failed.");

    /* Create the DHCPv6 Server */
    err = dhcpv6_server_init();
    nx_handle_error(err, "dhcpv6_server_init failed.");

    PRINT_INFO_STR("Network initialization completed successfully.");

    /* The DNS IPv6 address is updated for the DHCPv6 Server database, so that when the clients request
     * for an IP address, the DHCPv6 Server also provides the DNS Server info to the clients */
    err = dhcpv6_dns_address_set();
    nx_handle_error(err, "dhcpv6_dns_address_set failed.");
    PRINT_INFO_STR("Loads the DHCPv6 Server with the DNS Server address successfully.");

    /* Set the DHCPv6 Server DUID */
    err = dhcpv6_server_duid_set();
    nx_handle_error(err, "dhcpv6_server_duid_set failed.");
    PRINT_INFO_STR("DHCPv6 Server DUID sets successfully.");

    /* Create the DHCPv6 IP address range */
    err = dhcpv6_ip_address_range_set();
    nx_handle_error(err, "dhcpv6_ip_address_range_set failed.");
    PRINT_INFO_STR("Created DHCPv6 IP address range successfully.");

    /* Print the DHCPv6 Server menu options */
    app_terminal_print_data(TERMINAL_OUTPUT_MESSAGE_APP_PRINT_MENU, RESET_VALUE, NULL);

    while (true)
    {
        /* Wait for the user input */
        do
        {
            err = check_for_terminal_user_IP(input_buff);
        } while (TX_SUCCESS != err);

        /* Conversion from input string to integer value */
        read_data = (uint8_t)atoi(input_buff);

        switch (read_data)
        {
            case DHCPV6_SERVER_START:
            {
                if (false == b_server_ready)
                {
                    /* Start the DHCPv6 Server processing */
                    err = dhcpv6_server_start();
                    nx_handle_error(err, "dhcpv6_server_start failed.");
                    PRINT_INFO_STR("DHCPv6 Server started.");

                    /* Set the DHCPv6 Server ready flag to true */
                    b_server_ready = true;
                }
                else
                {
                    PRINT_INFO_STR("DHCPv6 Server is already running...");
                }
                break;
            }

            case PRINT_DHCPV6_LEASED_IP_INFO:
            {
                if (true == b_server_ready)
                {
                    b_ip_leased = false;

                    /* Print the DHCPv6 leased IPs info */
                    for (ULONG idx = RESET_VALUE;
                         NULL != g_dhcpv6_server.nx_dhcpv6_lease_list[idx].nx_dhcpv6_lease_assigned_to; idx++)
                    {
                        /* Assign and print the leased IPv6 address and the client's MAC address */
                        memset(&leased_info, RESET_VALUE, sizeof(ipv6_info_t));
                        memcpy(&leased_info.ipv6_address,
                               &g_dhcpv6_server.nx_dhcpv6_lease_list[idx].nx_dhcpv6_lease_IP_address,
                               sizeof(NXD_ADDRESS));
                        leased_info.mac_high = g_dhcpv6_server.nx_dhcpv6_lease_list[idx].nx_dhcpv6_lease_assigned_to->\
                                               nx_dhcpv6_client_duid.nx_link_layer_address_msw;
                        leased_info.mac_low  = g_dhcpv6_server.nx_dhcpv6_lease_list[idx].nx_dhcpv6_lease_assigned_to->\
                                               nx_dhcpv6_client_duid.nx_link_layer_address_lsw;
                        app_terminal_print_data(TERMINAL_OUTPUT_MESSAGE_APP_PRINT_CLIENT_DATA, sizeof(ipv6_info_t),
                                           &leased_info);

                        /* Set the IP leased state to true */
                        b_ip_leased = true;
                    }

                    if (false == b_ip_leased)
                    {
                        PRINT_INFO_STR("No IP leased to DHCPv6 client.");
                    }
                }
                else
                {
                    PRINT_INFO_STR("DHCPv6 Server is not started.");
                }
                break;
            }

            default:
            {
                PRINT_INFO_STR("Invalid menu option selected");
                break;
            }
        }

        /* Print the DHCPv6 Server menu options */
        app_terminal_print_data(TERMINAL_OUTPUT_MESSAGE_APP_PRINT_MENU, RESET_VALUE, NULL);

        tx_thread_sleep(1);
    }
}

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the Terminal Thread.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void terminal_thread_entry_run(void)
{
    UINT err = TX_SUCCESS;
    CHAR terminal_buffer[IP_MSG_MAX_SIZE] = {NULL_CHAR};
    CHAR timestamp_info[TIME_STAMP_MSG_SIZE] = {NULL_CHAR};

    /* Block allocation will be done and address based message exchange happens */
    terminal_msg_t * p_data = NULL;

    err = uart_console_init();
    if (FSP_SUCCESS != err)
    {
        return;     /* UART failed to open - cannot print anything */
    }

    /* Print the current time */
    snprintf(&timestamp_info[0], sizeof(timestamp_info), "Timestamp (Ticks) = 0x%08lx:\r\n", tx_time_get());
    DBG_PRINT(timestamp_info);

    /* Initialize the terminal framework */
    err = terminal_framework_init();
    tx_handle_error(err, "terminal_framework_init failed");

    while (true)
    {
        /* Check for the user input */
        if (uart_console_has_char())
        {
            terminal_input_read(terminal_buffer, sizeof(terminal_buffer));
            UINT read_bytes = (UINT)strlen(terminal_buffer);

            err = memory_allocate_terminal(&g_byte_pool, &p_data, sizeof(terminal_msg_t) + read_bytes + 1U);
            if (TX_SUCCESS != err)
            {
                printf("Error in processing, please check again\r\n");
            }
            else
            {
                /* Send the data to the user thread */
                p_data->msg_id = TERMINAL_INPUT_MESSAGE_TYPE_STR_QUEUED;
                p_data->msg_data_size = strlen(terminal_buffer) + 1U;
                memcpy(p_data->p_msg, terminal_buffer, p_data->msg_data_size);
                tx_queue_send(&g_terminal_ip_data_queue, (terminal_msg_t *)&p_data, TX_WAIT_FOREVER);

                memset(terminal_buffer, NULL_CHAR, sizeof(terminal_buffer));
            }
        }

        /* Print any message on the terminal if the queue has data */
        process_terminal_op_msg();

        /* Sleep briefly to yield the CPU to higher priority threads */
        tx_thread_sleep(1);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function receives messages from the user thread through tx_queue_receive and displays them on
 *            the terminal. Also releases the print message allocated block.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void process_terminal_op_msg(VOID)
{
    UINT err = TX_SUCCESS;
    terminal_msg_t * p_terminal_op_data = NULL;
    CHAR timestamp_info[TIME_STAMP_MSG_SIZE] = {NULL_CHAR};

    /* Pump out all information to print */
    do
    {
        err = tx_queue_receive(&g_terminal_op_data_queue, (VOID *)&p_terminal_op_data, 1);

        if (TX_SUCCESS == err)
        {
            /* Print the time this message was originally queued */
            snprintf(&timestamp_info[0], sizeof(timestamp_info), "Timestamp (Ticks) = 0x%08lx:\r\n",
                     (ULONG)p_terminal_op_data->msg_time);
            DBG_PRINT(timestamp_info);

            /* Message received. Take action and release the buffer */
            switch (p_terminal_op_data->msg_id)
            {
                case TERMINAL_OUTPUT_MESSAGE_BANNER:
                {
                    /* Get the FSP pack version information */
                    fsp_pack_version_t version = {RESET_VALUE};
                    R_FSP_VersionGet(&version);

                    printf(BANNER_INFO, version.version_id_b.major, version.version_id_b.minor,
                              version.version_id_b.patch);
                    uart_console_flush();

                    printf(EP_INFO);
                    uart_console_flush();
                    break;
                }

                case TERMINAL_OUTPUT_MESSAGE_APP_INFO_STR:
                {
                    printf("\r\nINFO: %s", p_terminal_op_data->p_msg);
                    break;
                }

                case TERMINAL_OUTPUT_MESSAGE_APP_ERR_STR:
                {
                    printf("\r\nERROR: %s", p_terminal_op_data->p_msg);
                    break;
                }

                case TERMINAL_OUTPUT_MESSAGE_APP_ERR_TRAP:
                {
                    /* Get the error code from the message */
                    err = *(UINT *)p_terminal_op_data->p_msg;

                    /* Halt the application */
                    printf("\r\nReturned Error Code: 0x%x\r\n", (unsigned int)err);
                    uart_console_flush();
                    __BKPT(0);
                    break;
                }

                case TERMINAL_OUTPUT_MESSAGE_APP_PRINT_MENU:
                {
                    /* Print the DHCPv6 Server menu options */
                    printf(EP_MENU);
                    break;
                }

                case TERMINAL_OUTPUT_MESSAGE_APP_PRINT_CLIENT_DATA:
                {
                    /* Assign the leased IPv6 info to the local buffer */
                    ipv6_info_t lease_info =
                    {
                        .ipv6_address = {RESET_VALUE, {RESET_VALUE}},
                        .mac_high = RESET_VALUE,
                        .mac_low = RESET_VALUE,
                    };
                    UCHAR str[50] = {NULL_CHAR};
                    memcpy(&lease_info, (ipv6_info_t *)p_terminal_op_data->p_msg, sizeof(ipv6_info_t));

                    /* Print the assigned IPv6 address */
                    str_ipv6(&str[0], lease_info.ipv6_address);
                    printf("\r\nAssigned IP address: %s", &str[0]);

                    /* Reset the buffer */
                    memset(str, NULL_CHAR, sizeof(str));

                    /* Print the client's MAC address */
                    str_mac(&str[0], lease_info.mac_high, lease_info.mac_low);
                    printf("\r\nClients MAC address: %s", &str[0]);
                    break;
                }

                default:
                {
                    printf("\r\nTerminal Printing Received Unknown Message ID");
                    break;
                }
            }

            /* Send a newline and return carriage requesting flushing the buffer */
            printf("\r\n");

            /* Release the print message allocated block */
            err = tx_byte_release((void *)p_terminal_op_data);
        }
    } while (TX_QUEUE_EMPTY != err);
}

/*******************************************************************************************************************//**
 * @brief          This function reads a line of user input from stdin and removes trailing newline characters.
 * @param[in, out] buf                  Pointer to buffer where the input string is stored.
 * @param[in]      len                  Maximum number of characters to read (buffer size).
 * @retval         None.
 **********************************************************************************************************************/
static void terminal_input_read(char * buf, int len)
{
    fgets(buf, len, stdin);
    buf[strcspn(buf, "\r\n")] = NULL_CHAR;
}

/*******************************************************************************************************************//**
 * @brief     This function handles a NetX error by printing the error message and code, and entering a trap state.
 * @param[in] err                       Error code.
 * @param[in] err_str                   Error message.
 * @retval    None.
 **********************************************************************************************************************/
static void nx_handle_error(UINT err, const char * err_str)
{
    if (NX_SUCCESS != err)
    {
        PRINT_ERR_STR(err_str);
        ERROR_TRAP(err);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function handles a ThreadX error by printing the error message and code, and entering a trap state.
 * @param[in] err                       Error code.
 * @param[in] err_str                   Error message.
 * @retval    None.
 **********************************************************************************************************************/
static void tx_handle_error(UINT err, const char * err_str)
{
    if (TX_SUCCESS != err)
    {
        printf("\r\nERROR: %s", err_str);
        printf("\r\nReturned Error Code: 0x%x\r\n", (unsigned int)err);
        uart_console_flush();
        __BKPT(0);
    }
}
