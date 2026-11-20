/***********************************************************************************************************************
 * File Name    : NetX_dhcpv6_client_example_app.c
 * Description  : Example application layer for the NetX DHCPv6 Client EP.
 *                - All printing goes through the terminal library (terminal.c), which prints it via printf
 *                  (routed to UART through the console library).
 *                - dhcp_client_thread_entry calls a single function: dhcp_client_thread_entry_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "dhcp_client_thread.h"
#include "dhcpv6_client.h"
#include "NetX_dhcpv6_client_example_app.h"

/* Private functions */
static bool dhcpv6_client_session_run(void);
static bool dhcpv6_client_bound_process(void);

/*******************************************************************************************************************//**
 * @brief     DHCP Client Thread entry-point for the NetX DHCPv6 Client example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void dhcp_client_thread_entry_run(void)
{
    UINT err = NX_SUCCESS;

    /* Initialize the Terminal Thread */
    term_thread_init_check();

    /* Print the banner and EP info */
    term_print_data(TERM_OP_MSG_BANNER, RESET_VALUE, NULL);

    /* Initialize the NetX system */
    nx_common_init();

    /* Initialize the packet pool */
    err = packet_pool_init();
    if (NX_SUCCESS != err)
    {
        PRINT_ERR_STR("packet_pool_init failed.");
        ERROR_TRAP(err);
        return;
    }

    /* Create the IP instance */
    err = ip_init();
    if (NX_SUCCESS != err)
    {
        PRINT_ERR_STR("ip_init failed.");
        ERROR_TRAP(err);
        return;
    }

    /* Set the link-local IPv6 address */
    err = ip_address_set();
    if (NX_SUCCESS != err)
    {
        PRINT_ERR_STR("ip_address_set failed.");
        ERROR_TRAP(err);
        return;
    }

    /* Create the DHCPv6 Client */
    err = dhcpv6_client_init();
    if (NX_SUCCESS != err)
    {
        PRINT_ERR_STR("dhcpv6_client_init failed.");
        ERROR_TRAP(err);
        return;
    }

    PRINT_INFO_STR("Network initialization completed successfully.");

    /* Start and run a brief DHCPv6 Client session */
    if (!dhcpv6_client_session_run())
    {
        return;
    }
    PRINT_INFO_STR("DHCPv6 Client EP completed.");

    while (true)
    {
        tx_thread_sleep(1);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function runs a DHCPv6 Client session: it waits for the Ethernet link, starts the DHCPv6 Client,
 *            waits for the DHCPv6 Client to be bound to an IPv6 address, then stops and deletes the DHCPv6 Client.
 * @param[in] None.
 * @retval    true                      Operation performed successfully.
 * @retval    false                     Failed to perform the operation.
 **********************************************************************************************************************/
static bool dhcpv6_client_session_run(void)
{
    UINT err = NX_SUCCESS;

    /* Wait for the Ethernet link to be up */
    PRINT_INFO_STR("Checking Ethernet link...");
    err = ip_link_check();
    if (NX_SUCCESS != err)
    {
        PRINT_ERR_STR("ip_link_check failed.");
        ERROR_TRAP(err);
        return false;
    }
    PRINT_INFO_STR("Ethernet link is up.");

    /* Create a Link Layer Plus Time DUID for the DHCPv6 Client */
    err = dhcpv6_client_duid_create();
    if (NX_SUCCESS != err)
    {
        PRINT_ERR_STR("dhcpv6_client_duid_create failed.");
        ERROR_TRAP(err);
        return false;
    }

    /* Create the IANA (also required) for the DHCPv6 Client */
    err = dhcpv6_client_iana_create();
    if (NX_SUCCESS != err)
    {
        PRINT_ERR_STR("dhcpv6_client_iana_create failed.");
        ERROR_TRAP(err);
        return false;
    }

    /* Request the time zone from the DHCPv6 Server and the IPv6 address of the DNS Server on the network */
    err = dhcpv6_client_options_request();
    if (NX_SUCCESS != err)
    {
        PRINT_ERR_STR("dhcpv6_client_options_request failed.");
        ERROR_TRAP(err);
        return false;
    }

    /* Start up the NetX DHCPv6 Client thread task */
    err = dhcpv6_client_start();
    if (NX_SUCCESS != err)
    {
        PRINT_ERR_STR("dhcpv6_client_start failed.");
        ERROR_TRAP(err);
        return false;
    }
    PRINT_INFO_STR("DHCPv6 Client started.");

    /* Start the DHCPv6 process by broadcasting a Solicit message */
    err = dhcpv6_client_solicit();
    if (NX_SUCCESS != err)
    {
        PRINT_ERR_STR("dhcpv6_client_solicit failed.");
        ERROR_TRAP(err);
        return false;
    }
    PRINT_INFO_STR("Sent the Solicit message. Waiting for bound state...");

    /* Wait for the DHCPv6 Client to be bound to an IPv6 address. This is set in the state change callback */
    err = dhcpv6_client_bound_wait();
    if (TX_SUCCESS == err)
    {
        if (!dhcpv6_client_bound_process())
        {
            return false;
        }
    }
    else if (TX_NO_EVENTS == err)
    {
        PRINT_ERR_STR("DHCPv6 Client is not bound to an IPv6 address.");
    }
    else
    {
        PRINT_ERR_STR("dhcpv6_client_bound_wait failed.");
        ERROR_TRAP(err);
        return false;
    }

    PRINT_INFO_STR("Now stopping and deleting the DHCPv6 Client.");

    /* Stop the DHCPv6 Client thread task */
    err = dhcpv6_client_stop();
    if (NX_SUCCESS != err)
    {
        PRINT_ERR_STR("dhcpv6_client_stop failed.");
        ERROR_TRAP(err);
        return false;
    }

    /* Delete the DHCPv6 Client */
    err = dhcpv6_client_delete();
    if (NX_SUCCESS != err)
    {
        PRINT_ERR_STR("dhcpv6_client_delete failed.");
        ERROR_TRAP(err);
        return false;
    }

    return true;
}

/*******************************************************************************************************************//**
 * @brief     This function processes the DHCPv6 Client once it is bound to an IPv6 address: it prints the IPv6
 *            address and the DNS Server IPv6 address, pings the DNS Server, prints the IA address lease data, and
 *            releases the IPv6 address back to the DHCPv6 Server.
 * @param[in] None.
 * @retval    true                      Operation performed successfully.
 * @retval    false                     Failed to perform the operation.
 **********************************************************************************************************************/
static bool dhcpv6_client_bound_process(void)
{
    UINT err = NX_SUCCESS;
    NXD_ADDRESS client_address = {RESET_VALUE, {RESET_VALUE}};
    NXD_ADDRESS dns_server_address = {RESET_VALUE, {RESET_VALUE}};
    dhcpv6_lease_info_t lease_info =
    {
        .renew_time = RESET_VALUE,
        .rebind_time = RESET_VALUE,
        .deprecated_time = RESET_VALUE,
        .expired_time = RESET_VALUE,
    };

    /* Check if the IP instance has at least one valid IPv6 address */
    err = dhcpv6_client_valid_ip_address_check();
    if (NX_SUCCESS != err)
    {
        PRINT_ERR_STR("dhcpv6_client_valid_ip_address_check failed.");
        ERROR_TRAP(err);
        return false;
    }

    /* Query the DHCPv6 Client for the IPv6 address, only so that it can be displayed */
    err = dhcpv6_client_ip_address_get(&client_address);
    if (NX_SUCCESS != err)
    {
        PRINT_ERR_STR("dhcpv6_client_ip_address_get failed.");
        ERROR_TRAP(err);
        return false;
    }

    /* Print the IPv6 address the DHCPv6 Client is bound to */
    term_print_data(TERM_OP_MSG_CLIENT_IP, sizeof(NXD_ADDRESS), &client_address);

    /* Obtain the DNS Server IPv6 address */
    err = dhcpv6_dns_server_address_get(&dns_server_address);
    if (NX_SUCCESS != err)
    {
        PRINT_ERR_STR("dhcpv6_dns_server_address_get failed.");
        ERROR_TRAP(err);
        return false;
    }

    /* Print the DNS Server IPv6 address */
    term_print_data(TERM_OP_MSG_DNS_SERVER_IP, sizeof(NXD_ADDRESS), &dns_server_address);

    /* Ping the DNS Server to test the IPv6 address */
    err = dns_server_ping(&dns_server_address);
    if (NX_SUCCESS != err)
    {
        PRINT_ERR_STR("dns_server_ping failed.");
        ERROR_TRAP(err);
        return false;
    }
    PRINT_INFO_STR("Pinged the DNS Server successfully.");

    /* Retrieve the IA address lease data of the DHCPv6 Client */
    err = dhcpv6_lease_info_get(&lease_info);
    if (NX_SUCCESS != err)
    {
        PRINT_ERR_STR("dhcpv6_lease_info_get failed.");
        ERROR_TRAP(err);
        return false;
    }

    /* Print the IA address lease data */
    term_print_data(TERM_OP_MSG_LEASE_INFO, sizeof(dhcpv6_lease_info_t), &lease_info);

    /* Release the IPv6 address back to the DHCPv6 Server */
    err = dhcpv6_client_release();
    if (NX_SUCCESS != err)
    {
        PRINT_ERR_STR("dhcpv6_client_release failed.");
        ERROR_TRAP(err);
        return false;
    }
    PRINT_INFO_STR("Released the IPv6 address back to the DHCPv6 Server.");

    return true;
}
