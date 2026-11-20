/***********************************************************************************************************************
 * File Name    : dhcpv4_client_ep.c
 * Description  : Contains data structures and functions used in dhcpv4_client_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "dhcpv4_client_ep.h"

/* Global variables */

/* DHCP instance */
NX_DHCP g_dhcp_client0;

/* IP instance */
NX_IP g_ip0;

/* Stack memory for g_ip0 */
uint8_t g_ip0_stack_memory[G_IP0_TASK_STACK_SIZE] BSP_PLACE_IN_SECTION(".stack.g_ip0")\
        BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);

/* ARP cache memory for g_ip0 */
uint8_t g_ip0_arp_cache_memory[G_IP0_ARP_CACHE_SIZE] BSP_ALIGN_VARIABLE(4);

/* Packet pool instance (If this is a Trustzone part, the memory must be placed in Non-secure memory) */
NX_PACKET_POOL g_packet_pool0;
uint8_t g_packet_pool0_pool_memory[G_PACKET_POOL0_PACKET_NUM * (G_PACKET_POOL0_PACKET_SIZE + sizeof(NX_PACKET))]\
        BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);

/* Private variables */
static TX_EVENT_FLAGS_GROUP s_dhcp_event_flags;
static bool s_dhcp_client_created = false;

/* Private function */
static void dhcpv4_client_notify_callback(NX_DHCP * client_ptr, UCHAR state);

/*******************************************************************************************************************//**
 * @brief     This function initializes the NetX system.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void nx_common_init(void)
{
    /* Initialize the NetX system */
    nx_system_initialize();
}

/*******************************************************************************************************************//**
 * @brief     This function creates the packet pool.
 * @param[in] None.
 * @retval    NX_SUCCESS                Created successfully.
 * @retval    Any other error codes     Failed to create.
 **********************************************************************************************************************/
UINT packet_pool_init(void)
{
    UINT err = NX_SUCCESS;

    /* Create the packet pool */
    err = nx_packet_pool_create(&g_packet_pool0, PACKET_POOL_NAME, G_PACKET_POOL0_PACKET_SIZE,
                                &g_packet_pool0_pool_memory[0],
                                G_PACKET_POOL0_PACKET_NUM * (G_PACKET_POOL0_PACKET_SIZE + sizeof(NX_PACKET)));

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function creates the IP instance and enables ARP, UDP, and ICMP.
 * @param[in] None.
 * @retval    NX_SUCCESS                Created successfully.
 * @retval    Any other error codes     Failed to create.
 **********************************************************************************************************************/
UINT ip_init(void)
{
    UINT err = NX_SUCCESS;

    /* Create the IP instance */
    err = nx_ip_create(&g_ip0, IP_NAME, G_IP0_ADDRESS, G_IP0_SUBNET_MASK, &g_packet_pool0, g_netxduo_ether_0,
                       &g_ip0_stack_memory[0], G_IP0_TASK_STACK_SIZE, G_IP0_TASK_PRIORITY);
    if (NX_SUCCESS != err)
    {
        return err;
    }

    /* Set the gateway address if it is configured */
    if (IP_ADDRESS(0, 0, 0, 0) != G_IP0_GATEWAY_ADDRESS)
    {
        err = nx_ip_gateway_address_set(&g_ip0, G_IP0_GATEWAY_ADDRESS);
        if (NX_SUCCESS != err)
        {
            return err;
        }
    }

    /* Enable the Address Resolution Protocol (ARP) */
    err = nx_arp_enable(&g_ip0, &g_ip0_arp_cache_memory[0], G_IP0_ARP_CACHE_SIZE);
    if (NX_SUCCESS != err)
    {
        return err;
    }

    /* Enable the UDP */
    err = nx_udp_enable(&g_ip0);
    if (NX_SUCCESS != err)
    {
        return err;
    }

    /* Enable the ICMP */
    err = nx_icmp_enable(&g_ip0);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function waits for the IP link to be enabled.
 * @param[in] None.
 * @retval    NX_SUCCESS                The link is enabled.
 * @retval    Any other error codes     The link is not enabled within the wait time.
 **********************************************************************************************************************/
UINT ip_link_check(void)
{
    UINT err = NX_SUCCESS;
    ULONG current_state = RESET_VALUE;

    /* Wait for the link to be enabled */
    err = nx_ip_status_check(&g_ip0, NX_IP_LINK_ENABLED, &current_state, LINK_ENABLE_WAIT_TIME);

    return err;
}

/*******************************************************************************************************************//**
 * @brief      This function gets the IP address assigned to the DHCP client.
 * @param[out] p_ip_address             Pointer to the IP address.
 * @retval     NX_SUCCESS               Got successfully.
 * @retval     Any other error codes    Failed to get.
 **********************************************************************************************************************/
UINT ip_address_get(ULONG * const p_ip_address)
{
    UINT err = NX_SUCCESS;
    ULONG network_mask = RESET_VALUE;

    /* Obtain the client IP address from this NetX service */
    err = nx_ip_address_get(&g_ip0, p_ip_address, &network_mask);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function polls the IP instance until a valid IP address is assigned or the wait limit is reached.
 * @param[in] None.
 * @retval    NX_SUCCESS                A valid IP address is assigned.
 * @retval    Any other error codes     No valid IP address is assigned within the wait limit.
 **********************************************************************************************************************/
UINT ip_address_resolved_wait(void)
{
    UINT err = NX_SUCCESS;
    ULONG actual_status = RESET_VALUE;
    UINT wait = RESET_VALUE;

    /* Poll the IP instance directly for a valid IP address */
    do
    {
        /* Check for address resolution */
        err = nx_ip_status_check(&g_ip0, NX_IP_ADDRESS_RESOLVED, &actual_status, TX_TIMER_TICKS_PER_SECOND);
        if (NX_SUCCESS != err)
        {
            /* Sleep for one second */
            tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND);

            wait += TX_TIMER_TICKS_PER_SECOND;
            if (WAIT_TO_BE_BOUND <= wait)
            {
                break;
            }
        }
    } while (NX_SUCCESS != err);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function sends a ping request to the IP address and releases the response packet.
 * @param[in] ip_address                IP address to ping.
 * @param[in] p_data                    Pointer to the data to send.
 * @param[in] size                      Size of the data in bytes.
 * @param[in] wait                      Wait time for the response in ticks.
 * @retval    NX_SUCCESS                Pinged successfully.
 * @retval    Any other error codes     Failed to ping.
 **********************************************************************************************************************/
UINT ip_address_ping(ULONG ip_address, char * p_data, ULONG size, ULONG wait)
{
    UINT err = NX_SUCCESS;
    NX_PACKET * p_packet = NX_NULL;

    /* Send a ping request to the IP address */
    err = nx_icmp_ping(&g_ip0, ip_address, p_data, size, &p_packet, wait);
    if (NX_SUCCESS != err)
    {
        return err;
    }

    /* Release the echo response packet once done */
    err = nx_packet_release(p_packet);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function creates the DHCP client and sets its packet pool.
 * @param[in] None.
 * @retval    NX_SUCCESS                Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
UINT dhcp_client_init(void)
{
    UINT err = NX_SUCCESS;

    /* Create the DHCP instance */
    err = nx_dhcp_create(&g_dhcp_client0, &g_ip0, DHCP_CLIENT_NAME);
    if (NX_SUCCESS != err)
    {
        return err;
    }
    s_dhcp_client_created = true;

    /* Set the DHCP client packet pool */
    err = nx_dhcp_packet_pool_set(&g_dhcp_client0, &g_packet_pool0);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function starts the DHCP client.
 * @param[in] None.
 * @retval    NX_SUCCESS                Started successfully.
 * @retval    Any other error codes     Failed to start.
 **********************************************************************************************************************/
UINT dhcp_client_start(void)
{
    UINT err = NX_SUCCESS;

    /* Start the DHCP client */
    err = nx_dhcp_start(&g_dhcp_client0);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function stops the DHCP client.
 * @param[in] None.
 * @retval    NX_SUCCESS                Stopped successfully.
 * @retval    Any other error codes     Failed to stop.
 **********************************************************************************************************************/
UINT dhcp_client_stop(void)
{
    UINT err = NX_SUCCESS;

    /* Stop the DHCP client */
    err = nx_dhcp_stop(&g_dhcp_client0);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function clears the DHCP client network parameters to prepare for a restart.
 * @param[in] None.
 * @retval    NX_SUCCESS                Reinitialized successfully.
 * @retval    Any other error codes     Failed to reinitialize.
 **********************************************************************************************************************/
UINT dhcp_client_reinitialize(void)
{
    UINT err = NX_SUCCESS;

    /* Clear the DHCP client network parameters */
    err = nx_dhcp_reinitialize(&g_dhcp_client0);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function releases the IP address back to the DHCP server.
 * @param[in] None.
 * @retval    NX_SUCCESS                Released successfully.
 * @retval    Any other error codes     Failed to release.
 **********************************************************************************************************************/
UINT dhcp_client_release(void)
{
    UINT err = NX_SUCCESS;

    /* Release the IP address back to the server */
    err = nx_dhcp_release(&g_dhcp_client0);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function deletes the DHCP client instance.
 * @param[in] None.
 * @retval    NX_SUCCESS                Deleted successfully.
 * @retval    Any other error codes     Failed to delete.
 **********************************************************************************************************************/
UINT dhcp_client_delete(void)
{
    UINT err = NX_SUCCESS;

    /* Delete the DHCP client */
    err = nx_dhcp_delete(&g_dhcp_client0);
    if (NX_SUCCESS == err)
    {
        s_dhcp_client_created = false;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function de-initializes the DHCP client if it has been created.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void dhcp_client_deinit(void)
{
    /* Delete the DHCP client only if it has been created */
    if (s_dhcp_client_created)
    {
        nx_dhcp_delete(&g_dhcp_client0);
        s_dhcp_client_created = false;
    }
}

/*******************************************************************************************************************//**
 * @brief     This function creates the event flags group used to be notified when the DHCP client is bound.
 * @param[in] None.
 * @retval    TX_SUCCESS                Created successfully.
 * @retval    Any other error codes     Failed to create.
 **********************************************************************************************************************/
UINT dhcp_event_flags_create(void)
{
    UINT err = TX_SUCCESS;

    /* Create an event flags group */
    err = tx_event_flags_create(&s_dhcp_event_flags, EVENT_FLAGS_NAME);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function registers the DHCP state change callback.
 * @param[in] None.
 * @retval    NX_SUCCESS                Registered successfully.
 * @retval    Any other error codes     Failed to register.
 **********************************************************************************************************************/
UINT dhcp_state_change_notify(void)
{
    UINT err = NX_SUCCESS;

    /* Register a DHCP state change callback function */
    err = nx_dhcp_state_change_notify(&g_dhcp_client0, dhcpv4_client_notify_callback);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function waits for the DHCP client to be bound, which means it has a valid IP address.
 * @param[in] None.
 * @retval    TX_SUCCESS                The DHCP client is bound.
 * @retval    TX_NO_EVENTS              The DHCP client is not bound within the wait time.
 * @retval    Any other error codes     Failed to wait for the event.
 **********************************************************************************************************************/
UINT dhcp_bound_wait(void)
{
    UINT err = TX_SUCCESS;
    ULONG actual_events = RESET_VALUE;

    /* Wait for the bound event */
    err = tx_event_flags_get(&s_dhcp_event_flags, DHCP_EVENT, TX_AND_CLEAR, &actual_events, DHCP_BOUND_WAIT_TIME);

    return err;
}

/*******************************************************************************************************************//**
 * @brief      This function gets the IP address of the DHCP server.
 * @param[out] p_server_address         Pointer to the IP address of the DHCP server.
 * @retval     NX_SUCCESS               Got successfully.
 * @retval     Any other error codes    Failed to get.
 **********************************************************************************************************************/
UINT dhcp_server_address_get(ULONG * const p_server_address)
{
    UINT err = NX_SUCCESS;

    /* Get the DHCP server IP address */
    err = nx_dhcp_server_address_get(&g_dhcp_client0, p_server_address);

    return err;
}

/*******************************************************************************************************************//**
 * @brief      This function gets the lease time of the current DHCP client state.
 * @param[out] p_lease_time             Pointer to the lease time in ticks.
 * @retval     NX_SUCCESS               Got successfully.
 * @retval     Any other error codes    Failed to get.
 **********************************************************************************************************************/
UINT dhcp_lease_time_get(ULONG * const p_lease_time)
{
    UINT err = NX_SUCCESS;
    NX_DHCP_CLIENT_RECORD dhcp_record;

    /* Obtain a record of the current client state */
    err = nx_dhcp_client_get_record(&g_dhcp_client0, &dhcp_record);
    if (NX_SUCCESS != err)
    {
        return err;
    }

    /* Return the lease time */
    *p_lease_time = dhcp_record.nx_dhcp_lease_time;

    return err;
}

/*******************************************************************************************************************//**
 * @brief      This function gets a list of IP addresses from a DHCP option of the last server response.
 * @param[in]  option                   DHCP option to retrieve, such as the DNS servers or the gateways.
 * @param[out] p_list                   Pointer to the array storing the IP addresses.
 * @param[in]  max_count                Maximum number of IP addresses the array can store.
 * @param[out] p_count                  Pointer to the number of IP addresses retrieved.
 * @retval     NX_SUCCESS               Retrieved successfully.
 * @retval     Any other error codes    Failed to retrieve.
 **********************************************************************************************************************/
UINT dhcp_address_list_get(UINT option, ULONG * const p_list, UINT max_count, UINT * const p_count)
{
    UINT err = NX_SUCCESS;
    UINT size = IP_V4_SIZE;

    /* Query the DHCP client for the address list of the option */
    err = nx_dhcp_user_option_retrieve(&g_dhcp_client0, option, (UCHAR *)p_list, &size);
    while (NX_DHCP_DEST_TO_SMALL == err)
    {
        /* Increase the buffer size */
        if (size < (max_count * IP_V4_SIZE))
        {
            size = size + IP_V4_SIZE;
            err = nx_dhcp_user_option_retrieve(&g_dhcp_client0, option, (UCHAR *)p_list, &size);
        }
        else
        {
            break;
        }
    }

    if (NX_SUCCESS == err)
    {
        *p_count = size / IP_V4_SIZE;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function waits for the Ethernet link to come up.
 * @param[in] None.
 * @retval    NX_SUCCESS                The link is up.
 * @retval    Any other error codes     Failed to get the link status.
 **********************************************************************************************************************/
UINT ethernet_link_wait(void)
{
    UINT err = NX_SUCCESS;
    ULONG link_status = RESET_VALUE;

    /* Wait for the link to come up */
    do
    {
        err = nx_ip_driver_direct_command(&g_ip0, NX_LINK_GET_STATUS, &link_status);
        if (NX_SUCCESS != err)
        {
            return err;
        }

        if (NX_TRUE != link_status)
        {
            /* 1-tick sleep is needed to signal to lower priority threads */
            tx_thread_sleep(1);
        }
    } while (NX_TRUE != link_status);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function is a callback for NetX Duo DHCP client module.
 * @param[in] client_ptr                Previously created DHCP client instance (unused).
 * @param[in] state                     Numeric representation of the DHCP state.
 * @retval    None.
 **********************************************************************************************************************/
static void dhcpv4_client_notify_callback(NX_DHCP * client_ptr, UCHAR state)
{
    NX_PARAMETER_NOT_USED(client_ptr);

    /* Verify the transition to the bound state (has a valid IP address) */
    if (NX_DHCP_STATE_BOUND == (UINT)state)
    {
        /* Set the event flag */
        tx_event_flags_set(&s_dhcp_event_flags, DHCP_EVENT, TX_OR);
    }
}
