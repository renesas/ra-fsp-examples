/***********************************************************************************************************************
 * File Name    : sntp_client_thread_entry.c
 * Description  : Contains functions for the SNTP client main thread.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "sntp_client_thread.h"
#include "common_utils.h"
#include "sntp_client_ep.h"
#include "rtc_app.h"

/*******************************************************************************************************************//**
 * @addtogroup NetX_sntp_client_ep
 * @{
 **********************************************************************************************************************/

/* Auto-generated code */
/* Packet pool instance (If this is a Trustzone part, the memory must be placed in Non-secure memory) */
NX_PACKET_POOL g_packet_pool0;
uint8_t g_packet_pool0_pool_memory[G_PACKET_POOL0_PACKET_NUM * (G_PACKET_POOL0_PACKET_SIZE + sizeof(NX_PACKET))]\
                                   BSP_ALIGN_VARIABLE(4);

/* IP instance */
NX_IP g_ip0;

/* Stack memory for g_ip0 */
uint8_t g_ip0_stack_memory[G_IP0_TASK_STACK_SIZE] BSP_PLACE_IN_SECTION(".stack.g_ip0")\
                                                  BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);

/* ARP cache memory for g_ip0 */
uint8_t g_ip0_arp_cache_memory[G_IP0_ARP_CACHE_SIZE] BSP_ALIGN_VARIABLE(4);

/* DHCP instance */
NX_DHCP g_dhcp_client0;

/* DNS instance */
NX_DNS g_dns0;

/* SNTP client instance */
NX_SNTP_CLIENT g_sntp_client0;

/* Private function declarations */
static UINT ip0_init(void);
static UINT packet_pool0_init(void);
static UINT dhcpv4_client_init(ULONG * dns_server_addr);
static UINT dns_client_init(ULONG dns_server_addr);
static UINT sntp_client_init(void);
static void sntp_client_deinit(void);
static UINT network_init(void);
static UINT sntp_client_run(sntp_client_mode_t mode, UCHAR * server_url);
static UINT get_print_sntp_time(void);
static UINT fetch_ntp_server_ip_address(ULONG *server_addr, UCHAR * server_url);
VOID time_update_callback(NX_SNTP_TIME_MESSAGE * time_update_ptr, NX_SNTP_TIME * local_time);

/* SNTP Client Thread entry function */
void sntp_client_thread_entry(void)
{
    /* Variable to store the API return status */
    UINT      status = NX_SUCCESS;
    fsp_err_t err    = FSP_SUCCESS;
    /* Variable to capture the retry count */
    int8_t   retry_count = SERVER_URL_MAX_VAL;
    /* Variable to capture the SNTP time update event */
    ULONG actual_events  = RESET_VALUE;
    /* Buffer to store the NTP server pool */
    UCHAR * server_url_str[SERVER_URL_MAX_VAL] = {(UCHAR *)"0.pool.ntp.org",
                                                  (UCHAR *)"1.pool.ntp.org",
                                                  (UCHAR *)"2.pool.ntp.org",
                                                  (UCHAR *)"3.pool.ntp.org"};

    /* Initialize the RTT thread */
    rtt_thread_init_check();
    /* Print the EP banner information */
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_BANNER, RESET_VALUE, NULL);

    /* Initialize the RTC and display RTC block time */
    err = rtc_init_and_display_time();
    if (FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("rtc_init_and_display_time function failed.");
        ERROR_TRAP(err);
    }

    /* Initialize the network and print the assigned IP address using DHCP */
    status = network_init();
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("network_init function failed.");
        /* Close the RTC module */
        rtc_deinit();
        ERROR_TRAP(status);
    }

    /* Initialize the SNTP client */
    status = sntp_client_init();
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("sntp_client_init function failed.");
        /* Close the RTC module */
        rtc_deinit();
        ERROR_TRAP(status);
    }

    /* Initialize SNTP-related services for the first server from the NTP pool list */
    status = sntp_client_run(INIT_MODE, server_url_str[RESET_VALUE]);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("sntp_client_run function failed.");
        /* Close the RTC and SNTP client module */
        rtc_deinit();
        sntp_client_deinit();
        ERROR_TRAP(status);
    }

    /* Wait for a server update event */
    status = tx_event_flags_get(&g_sntp_flags, SNTP_UPDATE_EVENT, TX_OR_CLEAR, &actual_events,
                                INIT_MODE_EVENT_WAIT_TIME);
    /* Check if we have valid event */
    if ((TX_SUCCESS == status) && (SNTP_UPDATE_EVENT == actual_events))
    {
        PRINT_INFO_STR("SNTP time update event received.");
        /* Receive and process network time */
        status = get_print_sntp_time();
        if (NX_SUCCESS != status)
        {
            PRINT_ERR_STR("get_print_sntp_time function failed.");
            /* Close the RTC and SNTP client module.*/
            rtc_deinit();
            sntp_client_deinit();
            ERROR_TRAP(status);
        }
    }
    else
    {
        PRINT_INFO_STR("No network time received in init mode --> retry mode on.");
        /* Retry with all available servers from the server list */
        while (-- retry_count)
        {
            /* Initialize SNTP related services in retry mode */
            status = sntp_client_run(RETRY_MODE, server_url_str[retry_count]);
            if (NX_SUCCESS != status)
            {
                PRINT_ERR_STR("sntp_client_run function failed.");
                /* Close the RTC and SNTP client module */
                rtc_deinit();
                sntp_client_deinit();
                ERROR_TRAP(status);
            }

            /* Reset the event variable */
            actual_events = RESET_VALUE;
            /* Wait for a server update event */
            status = tx_event_flags_get(&g_sntp_flags, SNTP_UPDATE_EVENT, TX_OR_CLEAR, &actual_events,
                                        RETRY_MODE_EVENT_WAIT_TIME);
            if ((TX_SUCCESS == status) && (SNTP_UPDATE_EVENT == actual_events))
            {
                PRINT_INFO_STR("SNTP time update event received in retry mode.");
                /* Receive and process network time */
                status = get_print_sntp_time();
                if (NX_SUCCESS != status)
                {
                    PRINT_ERR_STR("get_print_sntp_time function failed.");
                    /* Close the RTC and SNTP client module */
                    rtc_deinit();
                    sntp_client_deinit();
                    ERROR_TRAP(status);
                }
                break;
            }
        }
        /* Check for all available NTP servers tried in retry mode */
        if (RESET_VALUE >= retry_count)
        {
            PRINT_INFO_STR("No time update received in retry mode.");
        }
    }

    /* Stop SNTP client service */
    status = nx_sntp_client_stop(&g_sntp_client0);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_sntp_client_stop API failed.");
        /* Close the RTC and SNTP client module */
        rtc_deinit();
        sntp_client_deinit();
        ERROR_TRAP(status);
    }

    /* Delete SNTP client service */
    sntp_client_deinit();
    PRINT_INFO_STR("Stopped and deleted SNTP client services.");
    PRINT_INFO_STR("SNTP client EP completed.");

    while (true)
    {
        tx_thread_sleep(1);
    }
}

/*******************************************************************************************************************//**
 * @brief       Creates the NetX Duo packet pool.
 * @param[in]   None.
 * @retval      NX_SUCCESS          Packet pool successfully created.
 * @retval      NX_NOT_SUCCESSFUL   Packet pool creation failed.
 **********************************************************************************************************************/
static UINT packet_pool0_init(void)
{
    /* Variable to store the API return status */
    UINT status = NX_SUCCESS;

    /* Create the packet pool for NetX Duo */
    status = nx_packet_pool_create(&g_packet_pool0,
                                   "g_packet_pool0 Packet Pool",
                                   G_PACKET_POOL0_PACKET_SIZE,
                                   &g_packet_pool0_pool_memory[0],
                                   G_PACKET_POOL0_PACKET_NUM * (G_PACKET_POOL0_PACKET_SIZE + sizeof(NX_PACKET)));

    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_packet_pool_create API failed.");
    }
    return status;
}

/*******************************************************************************************************************//**
 * @brief       Creates the IP instance and enables ARP, UDP, and ICMP for the NetX Duo network stack.
 * @param[in]   None.
 * @retval      NX_SUCCESS          IP instance successfully created and initialized.
 * @retval      NX_NOT_SUCCESSFUL   IP initialization failed.
 **********************************************************************************************************************/
static UINT ip0_init(void)
{
    /* Variable to store the API return status */
    UINT status = NX_SUCCESS;

    /* Create the IP instance */
    status = nx_ip_create(&g_ip0,
                          "g_ip0 IP Instance",
                          G_IP0_ADDRESS,
                          G_IP0_SUBNET_MASK,
                          &g_packet_pool0,
                          g_netxduo_ether_0,
                          &g_ip0_stack_memory[0],
                          G_IP0_TASK_STACK_SIZE,
                          G_IP0_TASK_PRIORITY);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_ip_create API failed.");
        return status;
    }

    /* Enable ARP */
    status = nx_arp_enable(&g_ip0, &g_ip0_arp_cache_memory[RESET_VALUE], G_IP0_ARP_CACHE_SIZE);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_arp_enable API failed.");
        return status;
    }

    /* Enable UDP */
    status = nx_udp_enable(&g_ip0);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_udp_enable API failed.");
        return status;
    }

    /* Enable Internet Control Message Protocol (ICMP) for ping */
    status = nx_icmp_enable(&g_ip0);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_icmp_enable API failed.");
        return status;
    }
    PRINT_INFO_STR("Checking for Ethernet link...");

    /* Check Ethernet link status */
    ULONG current_state;
    status = nx_ip_status_check(&g_ip0, NX_IP_LINK_ENABLED, &current_state, LINK_ENABLE_WAIT_TIME);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_ip_status_check API failed.");
        return status;
    }
    PRINT_INFO_STR("Ethernet link is up.");
    return status;
}

/*******************************************************************************************************************//**
 * @brief       Initializes the DHCPv4 client by creating the client instance and assigning the required packet pool.
 *              Once the DHCP server is running, it provides an IP address to the client.
 * @param[in]   None.
 * @param[out]  dns_server_addr     Pointer to store the DNS server address obtained from the DHCP server.
 * @retval      NX_SUCCESS          DHCP client successfully initialized and IP address acquired.
 * @retval      NX_NOT_SUCCESSFUL   DHCP client initialization failed.
 **********************************************************************************************************************/
static UINT dhcpv4_client_init(ULONG * dns_server_addr)
{
    /* Variable to store the API return status */
    UINT  status                        = NX_SUCCESS;
    ULONG requested_status              = RESET_VALUE;
    UCHAR dns_buffer[4*MAX_DNS_SERVERS] = {NULL_CHAR};
    UINT dns_buffer_size                = IP_V4_SIZE;
    ULONG *dns_server_ptr               = NULL;

    /* Create the DHCP instance */
    status = nx_dhcp_create(&g_dhcp_client0, &g_ip0, "g_dhcp_client0");
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_dhcp_create API failed.");
        return status;
    }

    /* Assign the packet pool to the DHCP client */
    status = nx_dhcp_packet_pool_set(&g_dhcp_client0, &g_packet_pool0);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_dhcp_packet_pool_set API failed.");
        return status;
    }

    /* Start DHCP service */
    status = nx_dhcp_start(&g_dhcp_client0);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_dhcp_start API failed.");
        return status;
    }

    /* Wait until an IP address is acquired via DHCP */
    status = nx_ip_status_check(&g_ip0, NX_IP_ADDRESS_RESOLVED, &requested_status, LINK_ENABLE_WAIT_TIME);
    if (NX_IP_ADDRESS_RESOLVED != requested_status)
    {
        PRINT_ERR_STR("Error in acquiring the IP address.");
        return status;
    }
    PRINT_INFO_STR("Acquired the IP address via DHCP successfully!");

    /* Query the DHCP Client for the DNS server address */
    status = nx_dhcp_user_option_retrieve(&g_dhcp_client0, NX_DHCP_OPTION_DNS_SVR, dns_buffer, &dns_buffer_size);
    while (NX_DHCP_DEST_TO_SMALL == status)
    {
        if ((MAX_DNS_SERVERS*IP_V4_SIZE) > dns_buffer_size)
        {
            dns_buffer_size = dns_buffer_size + IP_V4_SIZE;
            status = nx_dhcp_user_option_retrieve(&g_dhcp_client0, NX_DHCP_OPTION_DNS_SVR, dns_buffer,
                                                  &dns_buffer_size);
        }
        else
        {
            break;
        }
    }

    if (NX_SUCCESS == status)
    {
        dns_server_ptr = (ULONG *)(dns_buffer);
        for (uint8_t cnt = RESET_VALUE; cnt < (dns_buffer_size/IP_V4_SIZE); cnt++, dns_server_ptr++)
        {
            /* Print DNS server address */
            app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_DNS_SERVER_IP, sizeof(ULONG *), dns_server_ptr);
            /* Retrieve the DNS server address */
            *dns_server_addr = *dns_server_ptr;
        }
    }
    return status;
}

/*******************************************************************************************************************//**
 * @brief       Initializes the DNS client by creating the client instance and assigning the required packet pool.
 *              It then adds the specified DNS server address to the client list.
 * @param[in]   dns_server_addr       DNS server address.
 * @retval      NX_SUCCESS            DNS client successfully initialized and server address added.
 * @retval      NX_NOT_SUCCESSFUL     DNS client initialization failed.
 **********************************************************************************************************************/
static UINT dns_client_init(ULONG dns_server_addr)
{
    /* Variable to store the API return status */
    UINT status = NX_SUCCESS;

    /* Create the DNS instance */
    status = nx_dns_create(&g_dns0, &g_ip0, (UCHAR *)"g_dns0");
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_dns_create API failed.");
        return status;
    }

    /* Assign the packet pool to the DNS client */
    status = nx_dns_packet_pool_set(&g_dns0, &g_packet_pool0);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_dns_packet_pool_set API failed.");
        return status;
    }

    /* Add the IPv4 DNS server address to the client list */
    status = nx_dns_server_add(&g_dns0, dns_server_addr);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_dns_server_add API failed.");
    }
    return status;
}

/*******************************************************************************************************************//**
 * @brief       Retrieves and prints the IP address of the SNTP server.
 * @param[in]   server_url      Pointer to the SNTP server URL.
 * @param[out]  server_addr     Pointer to the variable that receives the SNTP server IP address.
 * @retval      NX_SUCCESS      SNTP server address successfully retrieved.
 * @retval      Any other error code apart from NX_SUCCESS    Error occurred while resolving the SNTP server address.
 **********************************************************************************************************************/
static UINT fetch_ntp_server_ip_address(ULONG * server_addr, UCHAR * server_url)
{
    UINT status = NX_SUCCESS;

    /* Print the message for getting the server IP from the server URL */
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_GET_SERVER_STR, SERVER_URL_LEN, server_url);

    /* Get the IP address from the SNTP server */
    status = nx_dns_host_by_name_get(&g_dns0, server_url, server_addr, NETWORK_TIMEOUT);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_dns_host_by_name_get API failed.");
        return status;
    }
    else
    {
        /* Print the message for acquired server IP from the server URL */
        app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_SERVER_STR, SERVER_URL_LEN, server_url);
    }
    return status;
}

/*******************************************************************************************************************//**
 * @brief       Creates the SNTP client and sets up the time update callback function.
 * @param[in]   None.
 * @retval      NX_SUCCESS      SNTP client successfully created and callback configured.
 * @retval      Others          Error occurred during SNTP client initialization.
 **********************************************************************************************************************/
static UINT sntp_client_init(void)
{
    UINT status   = NX_SUCCESS;

    /* Create the SNTP client instance */
    status = nx_sntp_client_create(&g_sntp_client0,
                                   &g_ip0,
                                   G_SNTP_CLIENT0_INDEX_NETWORK_INTERFACE,
                                   &g_packet_pool0,
                                   NULL,
                                   NULL,
                                   NULL);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_sntp_client_create API failed.");
        return status;
    }

    /* Set up the time update callback function */
    status = nx_sntp_client_set_time_update_notify(&g_sntp_client0, time_update_callback);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_sntp_client_set_time_update_notify API failed.");
        return status;
    }
    PRINT_INFO_STR("SNTP client created successfully.");
    return status;
}

/*******************************************************************************************************************//**
 * @brief     Deletes the SNTP client instance.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void sntp_client_deinit(void)
{
    UINT status = NX_SUCCESS;

    /* Delete the SNTP client instance */
    status = nx_sntp_client_delete(&g_sntp_client0);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_sntp_client_delete API failed.");
    }
}

/*******************************************************************************************************************//**
 * @brief       Application-defined callback function that notifies the SNTP time update event.
 * @param[in]   time_update_ptr   Pointer to the SNTP time update message.
 * @param[in]   local_time        Pointer to the local time structure.
 * @retval      None.
 **********************************************************************************************************************/
VOID time_update_callback(NX_SNTP_TIME_MESSAGE * time_update_ptr, NX_SNTP_TIME * local_time)
{
    NX_PARAMETER_NOT_USED(time_update_ptr);
    NX_PARAMETER_NOT_USED(local_time);

    /* Set the SNTP time update event flag */
    tx_event_flags_set(&g_sntp_flags, SNTP_UPDATE_EVENT, TX_OR);
}

/*******************************************************************************************************************//**
 * @brief       Runs the SNTP client and fetches the NTP server time.
 * @param[in]   mode            SNTP client mode (initialize or retry).
 * @param[in]   server_url      SNTP server URL.
 * @retval      NX_SUCCESS      SNTP client ran successfully and fetched the server time.
 * @retval      Others          Error occurred while running the SNTP client.
 **********************************************************************************************************************/
static UINT sntp_client_run(sntp_client_mode_t mode, UCHAR * server_url)
{
    UINT   status              = NX_SUCCESS;
    ULONG  base_seconds        = RESET_VALUE;
    ULONG  base_fraction       = RESET_VALUE;
    ULONG  sntp_server_ip_addr = RESET_VALUE;

    if (RETRY_MODE == mode)
    {
        /* Stop the SNTP client before retrying */
        status = nx_sntp_client_stop(&g_sntp_client0);
        if (NX_SUCCESS != status)
        {
            PRINT_ERR_STR("nx_sntp_client_stop API failed.");
            return status;
        }
        PRINT_INFO_STR("Stopped SNTP client services.");
        PRINT_INFO_STR("Re-checking for SNTP server IP address.");
    }

    /* Obtain the SNTP server IP address */
    status = fetch_ntp_server_ip_address(&sntp_server_ip_addr, server_url);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("fetch_ntp_server_ip_address function failed.");
        return status;
    }

    /* Print SNTP server IP address */
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_SERVER_IP, sizeof(ULONG *), &sntp_server_ip_addr);

#if UNICAST
    /* Initialize the SNTP client for unicast mode */
    status = nx_sntp_client_initialize_unicast(&g_sntp_client0, sntp_server_ip_addr);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_sntp_client_initialize_unicast API failed.");
        return status;
    }
#else
    status = nx_sntp_client_initialize_broadcast(&g_sntp_client0,  NX_NULL, sntp_server_ip_addr);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_sntp_client_initialize_broadcast API failed.");
        return status;
    }
#endif

    base_seconds  = 0xd2c96b90;  /* Jan 24, 2012 UTC */
    base_fraction = 0xa132db1e;

    /* Set the base local time for the SNTP client */
    status = nx_sntp_client_set_local_time(&g_sntp_client0, base_seconds, base_fraction);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_sntp_client_set_local_time API failed.");
        return status;
    }

    /* Start the SNTP client service based on the configured mode */
#if UNICAST
    status = nx_sntp_client_run_unicast(&g_sntp_client0);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_sntp_client_run_unicast API failed.");
        return status;
    }
#else
    status = nx_sntp_client_run_broadcast(&g_sntp_client0);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_sntp_client_run_broadcast API failed.");
        return status;
    }
#endif  /* USE_UNICAST */

    PRINT_INFO_STR("SNTP client initialized and running.");
    return status;
}

/*******************************************************************************************************************//**
 * @brief     Gets the current network time from the SNTP client and prints it.
 * @param[in] None.
 * @retval    NX_SUCCESS            Time successfully retrieved and printed.
 * @retval    Any other error code  SNTP time retrieval or print operation failed.
 **********************************************************************************************************************/
static UINT get_print_sntp_time(void)
{
    fsp_err_t err        = FSP_SUCCESS;
    UINT   status        = NX_SUCCESS;
    UINT   server_status = NX_FALSE;
    ULONG  seconds       = RESET_VALUE;
    ULONG  fraction      = RESET_VALUE;

    /* Check if SNTP client is receiving updates from the server */
    status = nx_sntp_client_receiving_updates(&g_sntp_client0, &server_status);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_sntp_client_receiving_updates API failed.");
        return status;
    }

    /* Verify that the client is successfully communicating with the SNTP server */
    if (NX_TRUE == server_status)
    {
        CHAR write_time_data[MAX_BYTES] = {NULL_CHAR};
        /* Retrieve the current local time from the SNTP client */
        status = nx_sntp_client_get_local_time_extended(&g_sntp_client0, &seconds, &fraction,
                                                        &write_time_data[0], 100U);
        if (NX_SUCCESS != status)
        {
            PRINT_ERR_STR("nx_sntp_client_get_local_time_extended API failed.");
            return status;
        }

        /* Convert NTP time to a readable date and time string */
        status = nx_sntp_client_utility_display_date_time(&g_sntp_client0, &write_time_data[0],
                                                          sizeof(write_time_data));
        if (NX_SUCCESS != status)
        {
            PRINT_ERR_STR("nx_sntp_client_utility_display_date_time API failed.");
            return status;
        }

        /* Update the RTC block with the current timestamp */
        err = update_time_stamp_to_rtc_block(write_time_data);
        if (FSP_SUCCESS != err)
        {
            PRINT_ERR_STR("update_time_stamp_to_rtc_block function failed.");
            return err;
        }
    } /* Server status NX TRUE check if condition */
    return status;
}

/*******************************************************************************************************************//**
 * @brief     Initializes the NetX system, packet pool, IP instance, DNS client, and DHCPv4 client,
 *            then prints the client IP address.
 * @param[in] None.
 * @retval    NX_SUCCESS            Network initialization completed successfully.
 * @retval    Any other error code  Network initialization failed.
 **********************************************************************************************************************/
static UINT network_init(void)
{
    UINT status = NX_SUCCESS;
    /* Client IP address, network mask, and DNS address */
    ULONG client_ip_address = RESET_VALUE;
    ULONG network_mask      = RESET_VALUE;
    ULONG dns_addr          = RESET_VALUE;

    /* Initialize NetX core system */
    nx_system_initialize();

    /* Initialize packet pool */
    status = packet_pool0_init();
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("packet_pool0_init function failed.");
        return status;
    }

    /* Initialize IP instance */
    status = ip0_init();
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("ip0_init function failed.");
        return status;
    }

    /* Initialize DHCPv4 client */
    status = dhcpv4_client_init(&dns_addr);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("dhcpv4_client_init function failed.");
        return status;
    }

    /* Retrieve the client IP address and network mask */
    status = nx_ip_address_get(&g_ip0, &client_ip_address, &network_mask);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_ip_address_get API failed.");
        return status;
    }

    /* Print the client IP address */
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_CLIENT_IP, sizeof(ULONG *), &client_ip_address);

    /* Initialize DNS client */
    status = dns_client_init(dns_addr);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("dns_client_init function failed.");
    }
    return status;
}

/*******************************************************************************************************************//**
 * @} (end addtogroup NetX_sntp_client_ep)
 **********************************************************************************************************************/
