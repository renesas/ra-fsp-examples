/***********************************************************************************************************************
 * File Name    : net_link_ep.c
 * Description  : Contains data structures and functions used in net_link_ep.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "net_link_thread.h"
#include "net_link_ep.h"

/* Private variables */
static uint8_t s_ip0_stack_memory[G_IP0_TASK_STACK_SIZE] BSP_PLACE_IN_SECTION(".stack.g_ip0")\
               BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);
static uint8_t s_ip0_arp_cache_memory[G_IP0_ARP_CACHE_SIZE] BSP_ALIGN_VARIABLE(4);
static TX_EVENT_FLAGS_GROUP s_net_link_event_flags;

/* Global variables */
NX_IP g_ip0;
NX_PACKET_POOL g_packet_pool0;
NX_DHCP g_dhcp_client0;
net_info_t g_network_info;
#if (BSP_PERIPHERAL_ESWM_PRESENT)
uint8_t g_packet_pool0_pool_memory[(G_PACKET_POOL0_PACKET_NUM * (G_PACKET_POOL0_PACKET_SIZE +\
                                   sizeof(NX_PACKET)))] BSP_ALIGN_VARIABLE(4);
#else
uint8_t g_packet_pool0_pool_memory[(G_PACKET_POOL0_PACKET_NUM * (G_PACKET_POOL0_PACKET_SIZE +\
                                   sizeof(NX_PACKET)))] BSP_ALIGN_VARIABLE(4) ETHER_BUFFER_PLACE_IN_SECTION;
#endif /* BSP_PERIPHERAL_ESWM_PRESENT */

/* Private functions */
static VOID net_link_status_change_callback(NX_IP * p_ip, UINT index, UINT status);
static UINT net_system_get_info(net_info_t * const p_info);

/*******************************************************************************************************************//**
 * @brief     This function is called when the link status changes. It sets the event flag to notify the system
 *            of the link status change.
 * @param[in] p_ip                      Pointer to the NX_IP instance.
 * @param[in] index                     Index of the IP instance interface.
 * @param[in] status                    New link status.
 * @retval    None.
 **********************************************************************************************************************/
static VOID net_link_status_change_callback(NX_IP * p_ip, UINT index, UINT status)
{
    NX_PARAMETER_NOT_USED(p_ip);
    NX_PARAMETER_NOT_USED(index);

    /* Set the link status change event flag */
    tx_event_flags_set(&s_net_link_event_flags, NX_TRUE << status, TX_OR);
}

/*******************************************************************************************************************//**
 * @brief     This function starts up the network system.
 * @param[in] None.
 * @retval    NX_SUCCESS                Started successfully.
 * @retval    Any other error codes     Failed to start.
 **********************************************************************************************************************/
UINT net_system_startup(void)
{
    UINT status = NX_SUCCESS;
    ULONG events = RESET_VALUE;
    ULONG link = RESET_VALUE;

    /* Initialize the NetX system */
    nx_system_initialize();

    /* Initialize the FileX file system */
    fx_system_initialize();

    /* Create the network physical link event flags */
    status = tx_event_flags_create(&s_net_link_event_flags, "Network Link Events");
    if (TX_SUCCESS != status)
    {
        return status;
    }

    /* Create a packet pool for the network packet buffer */
    status = nx_packet_pool_create(&g_packet_pool0, "NX Packet Pool",
                                   G_PACKET_POOL0_PACKET_SIZE, &g_packet_pool0_pool_memory[0],
                                   G_PACKET_POOL0_PACKET_NUM * (G_PACKET_POOL0_PACKET_SIZE + sizeof(NX_PACKET)));
    if (NX_SUCCESS != status)
    {
        return status;
    }

    /* Create an IP instance for the Ethernet interface */
    status = nx_ip_create(&g_ip0, "g_ip0 IP Instance", NX_NULL, NX_NULL,
                          &g_packet_pool0, G_IP0_NETWORK_DRIVER, &s_ip0_stack_memory[0],
                          G_IP0_TASK_STACK_SIZE, G_IP0_TASK_PRIORITY);
    if (NX_SUCCESS != status)
    {
        return status;
    }

    /* Set a callback function to notify the physical interface of changes */
    status = nx_ip_link_status_change_notify_set(&g_ip0, net_link_status_change_callback);
    if (NX_SUCCESS != status)
    {
        return status;
    }

    /* Enable the Address Resolution Protocol (ARP) */
    status = nx_arp_enable(&g_ip0, &s_ip0_arp_cache_memory[0], G_IP0_ARP_CACHE_SIZE);
    if (NX_SUCCESS != status)
    {
        return status;
    }

    /* Enable the Internet Control Message Protocol (ICMP) */
    status = nx_icmp_enable(&g_ip0);
    if (NX_SUCCESS != status)
    {
        return status;
    }

    /* Enable the User Datagram Protocol (UDP) */
    status = nx_udp_enable(&g_ip0);
    if (NX_SUCCESS != status)
    {
        return status;
    }

    /* Enable the Transmission Control Protocol (TCP) */
    status = nx_tcp_enable(&g_ip0);
    if (NX_SUCCESS != status)
    {
        return status;
    }

    /* Wait for the network physical link to be established */
    status = tx_event_flags_get(&s_net_link_event_flags, LINK_UP_EVENT, TX_AND_CLEAR, &events, TX_WAIT_FOREVER);
    if (TX_SUCCESS != status)
    {
        return status;
    }

    /* Create a DHCP instance for the DHCP client service */
    status = nx_dhcp_create(&g_dhcp_client0, &g_ip0, "NX DHCP Client");
    if (NX_SUCCESS != status)
    {
        return status;
    }

    /* Set the packet pool for the DHCP client instance */
    status = nx_dhcp_packet_pool_set(&g_dhcp_client0, &g_packet_pool0);
    if (NX_SUCCESS != status)
    {
        return status;
    }

    /* Start the DHCP client service process */
    status = nx_dhcp_start(&g_dhcp_client0);
    if (NX_SUCCESS != status)
    {
        return status;
    }

    /* Wait for the DHCP client to resolve the IP address */
    status = nx_ip_status_check(&g_ip0, NX_IP_ADDRESS_RESOLVED, &link, NET_IP_ADDRESS_RESOLVED_WAIT);
    if (NX_SUCCESS != status)
    {
        return status;
    }

    /* Get the network configuration details */
    status = net_system_get_info(&g_network_info);

    return status;
}

/*******************************************************************************************************************//**
 * @brief      This function waits for any network physical link event.
 * @param[out] p_events                 Pointer to store the received event flags.
 * @retval     TX_SUCCESS               Event flags received successfully.
 * @retval     Any other error codes    Failed to receive the event flags.
 **********************************************************************************************************************/
UINT net_link_event_get(ULONG * const p_events)
{
    return tx_event_flags_get(&s_net_link_event_flags, LINK_ANY_EVENT, TX_OR_CLEAR, p_events, TX_WAIT_FOREVER);
}

/*******************************************************************************************************************//**
 * @brief     This function checks if the DHCP client has resolved the IP address.
 * @param[in] None.
 * @retval    NX_SUCCESS                Processed successfully.
 * @retval    Any other error codes     Failed to process.
 **********************************************************************************************************************/
UINT net_system_link_up_process(void)
{
    UINT status = NX_SUCCESS;
    ULONG link = RESET_VALUE;

    /* Wait for the DHCP client to resolve the IP address */
    status = nx_ip_status_check(&g_ip0, NX_IP_ADDRESS_RESOLVED, &link, NET_IP_ADDRESS_RESOLVED_WAIT);
    if (NX_SUCCESS != status)
    {
        /* Send a direct command to enable the link */
        status = nx_ip_driver_direct_command(&g_ip0, NX_LINK_ENABLE, &link);
        if (NX_SUCCESS != status)
        {
            return status;
        }

        /* Stop the DHCP client service */
        status = nx_dhcp_stop(&g_dhcp_client0);
        if (NX_SUCCESS != status)
        {
            return status;
        }

        /* Reinitialize the DHCP client to clean up parameters */
        status = nx_dhcp_reinitialize(&g_dhcp_client0);
        if (NX_SUCCESS != status)
        {
            return status;
        }

        /* Start the DHCP client service */
        status = nx_dhcp_start(&g_dhcp_client0);
        if (NX_SUCCESS != status)
        {
            return status;
        }

        /* Wait for the DHCP client to resolve the IP address */
        status = nx_ip_status_check(&g_ip0, NX_IP_ADDRESS_RESOLVED, &link, NET_IP_ADDRESS_RESOLVED_WAIT);
        if (NX_SUCCESS != status)
        {
            return status;
        }
    }

    /* Get the network configuration details */
    status = net_system_get_info(&g_network_info);

    return status;
}

/*******************************************************************************************************************//**
 * @brief      This function gets the MAC address, IPv4 address, subnet mask, default gateway, and DHCP server
 *             address for the specified network.
 * @param[out] p_info                   Pointer to a net_info_t structure.
 * @retval     NX_SUCCESS               Retrieved successfully.
 * @retval     Any other error codes    Failed to retrieve.
 **********************************************************************************************************************/
static UINT net_system_get_info(net_info_t * const p_info)
{
    UINT status = NX_SUCCESS;
    ULONG mac_addr_msb = RESET_VALUE;
    ULONG mac_addr_lsb = RESET_VALUE;
    ULONG dev_ip_addr = RESET_VALUE;
    ULONG dev_subnet_mask = RESET_VALUE;
    UCHAR dhcp_option[NET_DHCP_OPTION_SIZE];
    UINT dhcp_option_size = NET_DHCP_OPTION_SIZE;

    /* Get the MAC address of the RA board */
    status = nx_ip_interface_physical_address_get(&g_ip0, 0, &mac_addr_msb, &mac_addr_lsb);
    if (NX_SUCCESS != status)
    {
        return status;
    }

    /* Get the IPv4 address of the RA board */
    status = nx_ip_address_get(&g_ip0, &dev_ip_addr, &dev_subnet_mask);
    if (NX_SUCCESS != status)
    {
        return status;
    }

    /* Retrieve the IP address of the default gateway */
    status = nx_dhcp_user_option_retrieve(&g_dhcp_client0, NX_DHCP_OPTION_GATEWAYS, dhcp_option, &dhcp_option_size);
    if (NX_SUCCESS != status)
    {
        return status;
    }
    p_info->default_gateway.full = IP_ADDRESS(dhcp_option[3], dhcp_option[2], dhcp_option[1], dhcp_option[0]);

    /* Retrieve the IP address of the DHCP server */
    dhcp_option_size = NET_DHCP_OPTION_SIZE;
    status = nx_dhcp_user_option_retrieve(&g_dhcp_client0, NX_DHCP_OPTION_DHCP_SERVER, dhcp_option, &dhcp_option_size);
    if (NX_SUCCESS != status)
    {
        return status;
    }
    p_info->dhcp_addr.full = IP_ADDRESS(dhcp_option[3], dhcp_option[2], dhcp_option[1], dhcp_option[0]);

    /* Store the network configuration */
    p_info->phy_addr.group.msb = mac_addr_msb;
    p_info->phy_addr.group.lsb = mac_addr_lsb;
    p_info->ipv4_addr.full = dev_ip_addr;
    p_info->subnet_mask.full = dev_subnet_mask;

    return status;
}

/*******************************************************************************************************************//**
 * @brief     This function sends a ping request to the specified address and releases the response packet.
 * @param[in] ip_addr                   Destination IP address.
 * @retval    NX_SUCCESS                Ping response received successfully.
 * @retval    Any other error codes     Failed to ping.
 **********************************************************************************************************************/
UINT net_system_ping(ULONG ip_addr)
{
    UINT status = NX_SUCCESS;
    NX_PACKET * p_packet = NX_NULL;

    /* Send a ping request to the specified address. The response packet is returned in p_packet */
    status = nx_icmp_ping(&g_ip0, ip_addr, "Hello", sizeof("Hello"), &p_packet, NET_ICMP_PING_WAIT);
    if (NX_SUCCESS == status)
    {
        /* Release the response packet */
        nx_packet_release(p_packet);
    }

    return status;
}
