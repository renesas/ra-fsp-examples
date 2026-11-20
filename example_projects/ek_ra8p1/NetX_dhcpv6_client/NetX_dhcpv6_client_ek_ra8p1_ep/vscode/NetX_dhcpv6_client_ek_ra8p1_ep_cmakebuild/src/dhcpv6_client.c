/***********************************************************************************************************************
 * File Name    : dhcpv6_client.c
 * Description  : Contains data structures and functions used in dhcpv6_client.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "dhcp_client_thread.h"
#include "dhcpv6_client.h"

/* Event flags group to synchronize the DHCPv6 Client bound mechanism */
TX_EVENT_FLAGS_GROUP g_dhcpv6_client_event_flags;

/* Packet pool instance (If this is a Trustzone part, the memory must be placed in Non-secure memory) */
NX_PACKET_POOL g_packet_pool0;
uint8_t g_packet_pool0_pool_memory[G_PACKET_POOL0_PACKET_NUM * (G_PACKET_POOL0_PACKET_SIZE + sizeof(NX_PACKET))]
                                   BSP_ALIGN_VARIABLE(4);

/* IP instance */
NX_IP g_ip0;

/* Stack memory for g_ip0 */
uint8_t g_ip0_stack_memory[G_IP0_TASK_STACK_SIZE] BSP_PLACE_IN_SECTION(".stack.g_ip0")
                           BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);

/* ARP cache memory for g_ip0 */
uint8_t g_ip0_arp_cache_memory[G_IP0_ARP_CACHE_SIZE] BSP_ALIGN_VARIABLE(4);

/* DHCPv6 Client instance */
NX_DHCPV6 g_dhcpv6_client0;

/* Stack memory for g_dhcpv6_client0 */
uint8_t g_dhcpv6_client0_stack_memory[G_DHCPV6_CLIENT0_TASK_STACK_SIZE]
                                      BSP_PLACE_IN_SECTION(".stack.g_dhcpv6_client0")
                                      BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);

/* Private functions */
static VOID dhcpv6_state_change_notify(struct NX_DHCPV6_STRUCT * dhcpv6_ptr, UINT old_state, UINT new_state);
static VOID dhcpv6_server_error_handler(struct NX_DHCPV6_STRUCT * dhcpv6_ptr, UINT op_code, UINT status_code,
                                        UINT message_type);

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
    err = nx_packet_pool_create(&g_packet_pool0,
                                "g_packet_pool0 Packet Pool",
                                G_PACKET_POOL0_PACKET_SIZE,
                                &g_packet_pool0_pool_memory[0],
                                G_PACKET_POOL0_PACKET_NUM * (G_PACKET_POOL0_PACKET_SIZE + sizeof(NX_PACKET)));

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function creates the IP instance, enables ARP, UDP, ICMP, IPv6, and ICMPv6, and waits for the IP
 *            instance initialization to finish so that the MAC address is valid.
 * @param[in] None.
 * @retval    NX_SUCCESS                Created and enabled successfully.
 * @retval    Any other error codes     Failed to create or enable.
 **********************************************************************************************************************/
UINT ip_init(void)
{
    UINT err = NX_SUCCESS;
    UINT interface_index = RESET_VALUE;
    ULONG actual_state = RESET_VALUE;

    /* Create the IP instance */
    err = nx_ip_create(&g_ip0,
                       "g_ip0 IP Instance",
                       G_IP0_ADDRESS,
                       G_IP0_SUBNET_MASK,
                       &g_packet_pool0,
                       g_netxduo_ether_0,
                       &g_ip0_stack_memory[0],
                       G_IP0_TASK_STACK_SIZE,
                       G_IP0_TASK_PRIORITY);
    if (NX_SUCCESS != err)
    {
        return err;
    }

    /* Enable the ARP management component for the specified IP instance */
    err = nx_arp_enable(&g_ip0, &g_ip0_arp_cache_memory[0], G_IP0_ARP_CACHE_SIZE);
    if (NX_SUCCESS != err)
    {
        return err;
    }

    /* Enable UDP */
    err = nx_udp_enable(&g_ip0);
    if (NX_SUCCESS != err)
    {
        return err;
    }

    /* Enable ICMP */
    err = nx_icmp_enable(&g_ip0);
    if (NX_SUCCESS != err)
    {
        return err;
    }

    /* Enable IPv6 */
    err = nxd_ipv6_enable(&g_ip0);
    if (NX_SUCCESS != err)
    {
        return err;
    }

    /* Enable ICMPv6 (Required for neighbor discovery) */
    err = nxd_icmp_enable(&g_ip0);
    if (NX_SUCCESS != err)
    {
        return err;
    }

    /* Wait for the IP instance initialization to finish */
    err = nx_ip_interface_status_check(&g_ip0, interface_index, NX_IP_INITIALIZE_DONE, &actual_state,
                                       NX_WAIT_FOREVER);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function waits for the Ethernet link to be up.
 * @param[in] None.
 * @retval    NX_SUCCESS                Link is up.
 * @retval    Any other error codes     Failed to get the link status.
 **********************************************************************************************************************/
UINT ip_link_check(void)
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
            /* Sleep briefly to yield the CPU to the lower priority threads, e.g. the Terminal Thread */
            tx_thread_sleep(1);
        }
    } while (NX_TRUE != link_status);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function sets the link local IPv6 address.
 * @param[in] None.
 * @retval    NX_SUCCESS                Set successfully.
 * @retval    Any other error codes     Failed to set.
 **********************************************************************************************************************/
UINT ip_address_set(void)
{
    UINT err = NX_SUCCESS;
    UINT interface_index = RESET_VALUE;

    /* Set the link local IPv6 address (Prefix set to 10) */
    err = nxd_ipv6_address_set(&g_ip0, interface_index, NX_NULL, 10, NULL);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function creates the event flags group and the DHCPv6 Client instance.
 * @param[in] None.
 * @retval    NX_SUCCESS                Created successfully.
 * @retval    Any other error codes     Failed to create.
 **********************************************************************************************************************/
UINT dhcpv6_client_init(void)
{
    UINT err = NX_SUCCESS;

    /* Create the event flags group to synchronize the DHCPv6 Client bound mechanism */
    err = tx_event_flags_create(&g_dhcpv6_client_event_flags, "g_dhcpv6_client_event_flags");
    if (TX_SUCCESS != err)
    {
        return err;
    }

    /* Create the DHCPv6 Client instance */
    err = nx_dhcpv6_client_create(&g_dhcpv6_client0,
                                  &g_ip0,
                                  "g_dhcpv6_client0",
                                  &g_packet_pool0,
                                  g_dhcpv6_client0_stack_memory,
                                  G_DHCPV6_CLIENT0_TASK_STACK_SIZE,
                                  dhcpv6_state_change_notify,
                                  dhcpv6_server_error_handler);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function creates a Link Layer Plus Time DUID for the DHCPv6 Client.
 * @param[in] None.
 * @retval    NX_SUCCESS                Created successfully.
 * @retval    Any other error codes     Failed to create.
 **********************************************************************************************************************/
UINT dhcpv6_client_duid_create(void)
{
    UINT err = NX_SUCCESS;

    /* Set the time field to 0, the DHCPv6 Client API will supply one */
    err = nx_dhcpv6_create_client_duid(&g_dhcpv6_client0, NX_DHCPV6_DUID_TYPE_LINK_TIME, NX_DHCPV6_HW_TYPE_IEEE_802, 0);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function creates the IANA (Also required) for the DHCPv6 Client and requests the lease renewal
 *            times T1 and T2.
 * @param[in] None.
 * @retval    NX_SUCCESS                Created successfully.
 * @retval    Any other error codes     Failed to create.
 **********************************************************************************************************************/
UINT dhcpv6_client_iana_create(void)
{
    UINT err = NX_SUCCESS;

    err = nx_dhcpv6_create_client_iana(&g_dhcpv6_client0, DHCPV6_IANA_ID, DHCPV6_T1, DHCPV6_T2);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function adds the time zone and the DNS Server IPv6 address options to the requests sent to the
 *            DHCPv6 Server.
 * @param[in] None.
 * @retval    NX_SUCCESS                Requested successfully.
 * @retval    Any other error codes     Failed to request.
 **********************************************************************************************************************/
UINT dhcpv6_client_options_request(void)
{
    UINT err = NX_SUCCESS;

    /* Request the time zone */
    err = nx_dhcpv6_request_option_timezone(&g_dhcpv6_client0, NX_TRUE);
    if (NX_SUCCESS != err)
    {
        return err;
    }

    /* Request the IPv6 address of the DNS Server on the network */
    err = nx_dhcpv6_request_option_DNS_server(&g_dhcpv6_client0, NX_TRUE);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function starts the NetX DHCPv6 Client thread task.
 * @param[in] None.
 * @retval    NX_SUCCESS                Started successfully.
 * @retval    Any other error codes     Failed to start.
 **********************************************************************************************************************/
UINT dhcpv6_client_start(void)
{
    UINT err = NX_SUCCESS;

    err = nx_dhcpv6_start(&g_dhcpv6_client0);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function starts the DHCPv6 process by broadcasting a Solicit message.
 * @param[in] None.
 * @retval    NX_SUCCESS                Sent successfully.
 * @retval    Any other error codes     Failed to send.
 **********************************************************************************************************************/
UINT dhcpv6_client_solicit(void)
{
    UINT err = NX_SUCCESS;

    err = nx_dhcpv6_request_solicit(&g_dhcpv6_client0);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function waits for the DHCPv6 Client to be bound to an IPv6 address. The event flag is set in the
 *            state change callback. Returns TX_NO_EVENTS if the DHCPv6 Client is not bound within the wait time.
 * @param[in] None.
 * @retval    TX_SUCCESS                Bound successfully.
 * @retval    Any other error codes     Failed to bind.
 **********************************************************************************************************************/
UINT dhcpv6_client_bound_wait(void)
{
    UINT err = TX_SUCCESS;
    ULONG actual_events = RESET_VALUE;

    err = tx_event_flags_get(&g_dhcpv6_client_event_flags, DHCPV6_CLIENT_BOUND_EVENT, TX_AND_CLEAR, &actual_events,
                             DHCPV6_EVENT_WAIT_TIME);
    if ((TX_SUCCESS == err) && (true != actual_events))
    {
        /* The bound event flag is not set */
        err = TX_NO_EVENTS;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function checks that the IP instance has at least one valid IPv6 address registered by the
 *            DHCPv6 Client.
 * @param[in] None.
 * @retval    NX_SUCCESS                At least one valid IPv6 address is registered.
 * @retval    Any other error codes     Fail to register IPv6 address.
 **********************************************************************************************************************/
UINT dhcpv6_client_valid_ip_address_check(void)
{
    UINT err = NX_SUCCESS;
    UINT address_count = RESET_VALUE;

    err = nx_dhcpv6_get_valid_ip_address_count(&g_dhcpv6_client0, &address_count);
    if (NX_SUCCESS != err)
    {
        return err;
    }

    /* Check if the IP instance has at least one valid IPv6 address */
    if (RESET_VALUE >= address_count)
    {
        err = NX_IP_ADDRESS_ERROR;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief      This function gets the global IPv6 address assigned to the DHCPv6 Client.
 * @param[out] p_address                Pointer to store the IPv6 address.
 * @retval     NX_SUCCESS               Got successfully.
 * @retval     Any other error codes    Failed to get.
 **********************************************************************************************************************/
UINT dhcpv6_client_ip_address_get(NXD_ADDRESS * const p_address)
{
    UINT err = NX_SUCCESS;

    err = nx_dhcpv6_get_IP_address(&g_dhcpv6_client0, p_address);

    return err;
}

/*******************************************************************************************************************//**
 * @brief      This function gets the first DNS Server IPv6 address from the DHCPv6 Client DNS Server list.
 * @param[out] p_address                Pointer to store the DNS Server IPv6 address.
 * @retval     NX_SUCCESS               Got successfully.
 * @retval     Any other error codes    Failed to get.
 **********************************************************************************************************************/
UINT dhcpv6_dns_server_address_get(NXD_ADDRESS * const p_address)
{
    UINT err = NX_SUCCESS;
    UINT dns_server_index = RESET_VALUE;

    err = nx_dhcpv6_get_DNS_server_address(&g_dhcpv6_client0, dns_server_index, p_address);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function pings the DNS Server to test the IPv6 address assigned to the DHCPv6 Client.
 * @param[in] p_address                 Pointer to the DNS Server IPv6 address.
 * @retval    NX_SUCCESS                Pinged successfully.
 * @retval    Any other error codes     Failed to ping.
 **********************************************************************************************************************/
UINT dns_server_ping(NXD_ADDRESS * const p_address)
{
    UINT err = NX_SUCCESS;
    NX_PACKET * p_response_packet = NX_NULL;

    err = nxd_icmp_ping(&g_ip0, p_address, PING_DATA, sizeof(PING_DATA), &p_response_packet, PING_WAIT_TIME);
    if (NX_SUCCESS == err)
    {
        /* Release the Echo Reply packet back to the packet pool */
        err = nx_packet_release(p_response_packet);
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief      This function gets the lease time data of the IA address assigned to the DHCPv6 Client.
 * @param[out] p_lease_info             Pointer to store the IA address lease info.
 * @retval     NX_SUCCESS               Got successfully.
 * @retval     Any other error codes    Failed to get.
 **********************************************************************************************************************/
UINT dhcpv6_lease_info_get(dhcpv6_lease_info_t * const p_lease_info)
{
    UINT err = NX_SUCCESS;

    err = nx_dhcpv6_get_lease_time_data(&g_dhcpv6_client0,
                                        &p_lease_info->renew_time,
                                        &p_lease_info->rebind_time,
                                        &p_lease_info->deprecated_time,
                                        &p_lease_info->expired_time);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function releases the IPv6 address back to the DHCPv6 Server.
 * @param[in] None.
 * @retval    NX_SUCCESS                Released successfully.
 * @retval    Any other error codes     Failed to release.
 **********************************************************************************************************************/
UINT dhcpv6_client_release(void)
{
    UINT err = NX_SUCCESS;

    err = nx_dhcpv6_request_release(&g_dhcpv6_client0);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function stops the NetX DHCPv6 Client thread task.
 * @param[in] None.
 * @retval    NX_SUCCESS                Stopped successfully.
 * @retval    Any other error codes     Failed to stop.
 **********************************************************************************************************************/
UINT dhcpv6_client_stop(void)
{
    UINT err = NX_SUCCESS;

    err = nx_dhcpv6_stop(&g_dhcpv6_client0);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function deletes the DHCPv6 Client instance.
 * @param[in] None.
 * @retval    NX_SUCCESS                Deleted successfully.
 * @retval    Any other error codes     Failed to delete.
 **********************************************************************************************************************/
UINT dhcpv6_client_delete(void)
{
    UINT err = NX_SUCCESS;

    err = nx_dhcpv6_client_delete(&g_dhcpv6_client0);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function is called when the DHCPv6 Client changes the DHCPv6 state. Since this is called from the
 *            DHCPv6 stack, the code should be brief with no blocking calls, so an event flag is set for the
 *            application to check.
 * @param[in] dhcpv6_ptr                Pointer to the DHCPv6 Client instance.
 * @param[in] old_state                 Previous state the DHCPv6 Client was in.
 * @param[in] new_state                 Current state the DHCPv6 Client just changed to.
 * @retval    None.
 **********************************************************************************************************************/
static VOID dhcpv6_state_change_notify(struct NX_DHCPV6_STRUCT * dhcpv6_ptr, UINT old_state, UINT new_state)
{
    NX_PARAMETER_NOT_USED(dhcpv6_ptr);
    NX_PARAMETER_NOT_USED(old_state);

    if (NX_DHCPV6_STATE_BOUND_TO_ADDRESS == new_state)
    {
        /* Set the event flag */
        tx_event_flags_set(&g_dhcpv6_client_event_flags, DHCPV6_CLIENT_BOUND_EVENT, TX_OR);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function is called when the DHCPv6 Client receives an error code from the DHCPv6 Server. Since this
 *            is called from the DHCPv6 stack, the code should be brief with no blocking calls.
 * @param[in] dhcpv6_ptr                Pointer to the DHCPv6 Client instance.
 * @param[in] op_code                   Section of DHCPv6 request causing the error, for example IA_ADDRESS, IA-NA,
 *                                      Option Field.
 * @param[in] status_code               Error status code received from the DHCPv6 Server.
 * @param[in] message_type              Type of response (ADVERTISE, REPLY, etc.) associated with the error status.
 * @retval    None.
 **********************************************************************************************************************/
static VOID dhcpv6_server_error_handler(struct NX_DHCPV6_STRUCT * dhcpv6_ptr, UINT op_code, UINT status_code,
                                        UINT message_type)
{
    NX_PARAMETER_NOT_USED(dhcpv6_ptr);
    NX_PARAMETER_NOT_USED(op_code);
    NX_PARAMETER_NOT_USED(status_code);
    NX_PARAMETER_NOT_USED(message_type);
}

/*******************************************************************************************************************//**
 * @brief      This function converts an IPv6 address into a string to print.
 * @param[out] str                      Converted string.
 * @param[in]  ipv6                     IPv6 address.
 * @retval     None.
 **********************************************************************************************************************/
void str_ipv6(UCHAR * str, NXD_ADDRESS ipv6)
{
    sprintf((char *)str, "%X:%X:%X:%X:%X:%X:%X:%X",
            (((uint32_t)ipv6.nxd_ip_address.v6[0] & 0xFFFF0000) >> 16),
            (((uint32_t)ipv6.nxd_ip_address.v6[0] & 0x0000FFFF) >> 0),
            (((uint32_t)ipv6.nxd_ip_address.v6[1] & 0xFFFF0000) >> 16),
            (((uint32_t)ipv6.nxd_ip_address.v6[1] & 0x0000FFFF) >> 0),
            (((uint32_t)ipv6.nxd_ip_address.v6[2] & 0xFFFF0000) >> 16),
            (((uint32_t)ipv6.nxd_ip_address.v6[2] & 0x0000FFFF) >> 0),
            (((uint32_t)ipv6.nxd_ip_address.v6[3] & 0xFFFF0000) >> 16),
            (((uint32_t)ipv6.nxd_ip_address.v6[3] & 0x0000FFFF) >> 0));
}
