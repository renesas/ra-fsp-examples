/***********************************************************************************************************************
 * File Name    : NetX_dhcpv4_client_example_app.c
 * Description  : Example application layer for the NetX DHCPv4 client EP.
 *                - All printing via printf (routed to UART via user stdio library) in the terminal thread.
 *                - dhcp_client_thread_entry.c calls a single function: dhcp_client_thread_entry_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "dhcpv4_client_ep.h"
#include "NetX_dhcpv4_client_example_app.h"

/* Private functions */
static bool run_dhcp_client_session(void);
static bool ping_dns_servers(void);
static bool ping_gateways(void);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the DHCP client thread.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void dhcp_client_thread_entry_run(void)
{
    UINT status = NX_SUCCESS;

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

    /* Wait for the IP link to be enabled */
    status = ip_link_check();
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("ip_link_check failed");
        ERROR_TRAP(status);
        return;
    }

    /* Initialize the DHCP client */
    status = dhcp_client_init();
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("dhcp_client_init failed");
        ERROR_TRAP(status);
        return;
    }
    PRINT_INFO_STR("Network initialization completed successfully");

    /* Start and run a brief DHCP client session */
    if (!run_dhcp_client_session())
    {
        return;
    }
    PRINT_INFO_STR("DHCPv4 client EP completed successfully");

    while (true)
    {
        tx_thread_sleep(1);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function runs a DHCP client session, from getting an IP address to releasing it.
 * @param[in] None.
 * @retval    true                      Operation performed successfully.
 * @retval    false                     Failed to perform the operation.
 **********************************************************************************************************************/
static bool run_dhcp_client_session(void)
{
    UINT status = NX_SUCCESS;
    ULONG client_address = RESET_VALUE;
    ULONG server_address = RESET_VALUE;
    ULONG lease_time = RESET_VALUE;

    /* Create an event flags group */
    status = dhcp_event_flags_create();
    if (TX_SUCCESS != status)
    {
        PRINT_ERR_STR("dhcp_event_flags_create failed");
        ERROR_TRAP(status);
        return false;
    }

    /* Wait for the Ethernet link to come up */
    PRINT_INFO_STR("Checking Ethernet link...");
    status = ethernet_link_wait();
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("ethernet_link_wait failed");
        ERROR_TRAP(status);
        return false;
    }
    PRINT_INFO_STR("Ethernet link is up");

    /* Register a DHCP state change callback function */
    status = dhcp_state_change_notify();
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("dhcp_state_change_notify failed");
        ERROR_TRAP(status);
        return false;
    }

    /* Now we are ready to start the DHCP client */
    status = dhcp_client_start();
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("dhcp_client_start failed");
        ERROR_TRAP(status);
        return false;
    }
    PRINT_INFO_STR("DHCP client is running");

    /* Wait until an IP address is acquired via DHCP */
    status = dhcp_bound_wait();
    if (TX_SUCCESS != status)
    {
        PRINT_ERR_STR("dhcp_bound_wait failed");
        ERROR_TRAP(status);
        return false;
    }

    PRINT_INFO_STR("DHCP client is assigned an IP address");

    /* Obtain the client IP address from this NetX service */
    status = ip_address_get(&client_address);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("ip_address_get failed");
        ERROR_TRAP(status);
        return false;
    }

    /* Print the client IP address */
    term_print_data(TERM_OP_MSG_CLIENT_IP, sizeof(client_address), &client_address);

    /* Get the DHCP server IP address */
    status = dhcp_server_address_get(&server_address);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("dhcp_server_address_get failed");
        ERROR_TRAP(status);
        return false;
    }

    /* Print the DHCP server IP address */
    term_print_data(TERM_OP_MSG_SERVER_IP, sizeof(server_address), &server_address);

    /* Ping the DHCP server to verify the IP address */
    status = ip_address_ping(server_address, "Hello World", sizeof("Hello World"), NX_WAIT_FOREVER);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("ip_address_ping failed");
        ERROR_TRAP(status);
        return false;
    }
    PRINT_INFO_STR("DHCP server pinged successfully");

    /* Perform the lease time operation */
    status = dhcp_lease_time_get(&lease_time);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("dhcp_lease_time_get failed");
        ERROR_TRAP(status);
        return false;
    }

    /* Print the lease time */
    term_print_data(TERM_OP_MSG_LEASE_TIME, sizeof(lease_time), &lease_time);

    /* Stop the DHCP client */
    status = dhcp_client_stop();
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("dhcp_client_stop failed");
        ERROR_TRAP(status);
        return false;
    }

    /* Prepare the DHCP client to restart */
    status = dhcp_client_reinitialize();
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("dhcp_client_reinitialize failed");
        ERROR_TRAP(status);
        return false;
    }
    PRINT_INFO_STR("DHCP client reinitialized successfully");

    /* Restart the DHCP client */
    status = dhcp_client_start();
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("dhcp_client_start failed");
        ERROR_TRAP(status);
        return false;
    }
    PRINT_INFO_STR("DHCP client restarted successfully");

    /* Poll the IP instance directly for a valid IP address */
    status = ip_address_resolved_wait();
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("ip_address_resolved_wait failed");
        ERROR_TRAP(status);
        return false;
    }

    PRINT_INFO_STR("DHCP client is assigned an IP address for the second time");

    /* Ping the DNS servers */
    if (!ping_dns_servers())
    {
        return false;
    }

    /* Ping the gateways */
    if (!ping_gateways())
    {
        return false;
    }

    /* Release the IP address to the DHCP server */
    status = dhcp_client_release();
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("dhcp_client_release failed");
        ERROR_TRAP(status);
        return false;
    }
    PRINT_INFO_STR("IP address released to DHCP server successfully");

    /* Stop the DHCP client and unbind the DHCP UDP socket */
    status = dhcp_client_stop();
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("dhcp_client_stop failed");
        ERROR_TRAP(status);
        return false;
    }

    /* Delete the DHCP client */
    status = dhcp_client_delete();
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("dhcp_client_delete failed");
        ERROR_TRAP(status);
        return false;
    }

    return true;
}

/*******************************************************************************************************************//**
 * @brief     This function queries the DHCP client for the DNS servers and pings each of them.
 * @param[in] None.
 * @retval    true                      Operated successfully.
 * @retval    false                     Failed to operate.
 **********************************************************************************************************************/
static bool ping_dns_servers(void)
{
    UINT status = NX_SUCCESS;
    ULONG dns_servers[MAX_DNS_SERVERS] = {RESET_VALUE};
    UINT dns_count = RESET_VALUE;

    /* Query the DHCP client for the DNS server addresses */
    status = dhcp_address_list_get(NX_DHCP_OPTION_DNS_SVR, dns_servers, MAX_DNS_SERVERS, &dns_count);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("dhcp_address_list_get failed");
        ERROR_TRAP(status);
        return false;
    }

    for (UINT index = RESET_VALUE; index < dns_count; index++)
    {
        /* Send a ping request to the DNS server */
        status = ip_address_ping(dns_servers[index], "Hello", sizeof("Hello"), PING_TIMEOUT);
        if (NX_SUCCESS != status)
        {
            PRINT_ERR_STR("ip_address_ping failed");
            ERROR_TRAP(status);
            return false;
        }

        /* Print the DNS server IP address */
        term_print_data(TERM_OP_MSG_DNS_SERVER_IP, sizeof(dns_servers[index]), &dns_servers[index]);
    }

    return true;
}

/*******************************************************************************************************************//**
 * @brief     This function queries the DHCP client for the gateways and pings each of them.
 * @param[in] None.
 * @retval    true                      Operated successfully.
 * @retval    false                     Failed to operate.
 **********************************************************************************************************************/
static bool ping_gateways(void)
{
    UINT status = NX_SUCCESS;
    ULONG gateways[MAX_GATEWAYS] = {RESET_VALUE};
    UINT gateway_count = RESET_VALUE;

    /* Query the DHCP client for the gateway addresses */
    status = dhcp_address_list_get(NX_DHCP_OPTION_GATEWAYS, gateways, MAX_GATEWAYS, &gateway_count);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("dhcp_address_list_get failed");
        ERROR_TRAP(status);
        return false;
    }

    for (UINT index = RESET_VALUE; index < gateway_count; index++)
    {
        /* Send a ping request to the gateway */
        status = ip_address_ping(gateways[index], "Hello Gateway", sizeof("Hello Gateway"), PING_TIMEOUT);
        if (NX_SUCCESS != status)
        {
            PRINT_ERR_STR("ip_address_ping failed");
            ERROR_TRAP(status);
            return false;
        }

        /* Print the gateway IP address */
        term_print_data(TERM_OP_MSG_GATEWAY_IP, sizeof(gateways[index]), &gateways[index]);
    }

    return true;
}
