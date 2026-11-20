/***********************************************************************************************************************
 * File Name    : dhcpv6_server.c
 * Description  : Contains data structures and functions used in the EP.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "dhcpv6_thread.h"
#include "dhcpv6_server.h"

/* Packet pool instance (If this is a Trustzone part, the memory must be placed in Non-secure memory) */
NX_PACKET_POOL g_packet_pool0;
#if (BSP_PERIPHERAL_ESWM_PRESENT)
uint8_t g_packet_pool0_pool_memory[G_PACKET_POOL0_PACKET_NUM * (G_PACKET_POOL0_PACKET_SIZE + sizeof(NX_PACKET))]
                                   BSP_ALIGN_VARIABLE(4);
#else
uint8_t g_packet_pool0_pool_memory[G_PACKET_POOL0_PACKET_NUM * (G_PACKET_POOL0_PACKET_SIZE + sizeof(NX_PACKET))]
                                   BSP_ALIGN_VARIABLE(4) ETHER_BUFFER_PLACE_IN_SECTION;
#endif /* BSP_PERIPHERAL_ESWM_PRESENT */

/* IP instance */
NX_IP g_ip0;

/* Stack memory for g_ip0 */
uint8_t g_ip0_stack_memory[G_IP0_TASK_STACK_SIZE] BSP_PLACE_IN_SECTION(".stack.g_ip0")
                           BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);

/* ARP cache memory for g_ip0 */
uint8_t g_ip0_arp_cache_memory[G_IP0_ARP_CACHE_SIZE] BSP_ALIGN_VARIABLE(4);

/* DHCPv6 Server instance */
NX_DHCPV6_SERVER g_dhcpv6_server;

/* Stack memory for g_dhcpv6_server */
uint8_t g_dhcpv6_server_stack_memory[G_DHCPV6_SERVER_TASK_STACK_SIZE] BSP_PLACE_IN_SECTION(".stack.g_dhcpv6_server")
                                     BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);

/* IPv6 addresses */
NXD_ADDRESS g_dns_ipv6_address;
NXD_ADDRESS g_start_ipv6_address;
NXD_ADDRESS g_end_ipv6_address;

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
 * @brief     This function creates the IP instance and enables ARP, UDP, ICMP, IPv6, and ICMPv6.
 * @param[in] None.
 * @retval    NX_SUCCESS                Created and enabled successfully.
 * @retval    Any other error codes     Failed to create or enable.
 **********************************************************************************************************************/
UINT ip_init(void)
{
    UINT err = NX_SUCCESS;

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

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function waits for the Ethernet link to be enabled.
 * @param[in] None.
 * @retval    NX_SUCCESS                Link is enabled.
 * @retval    Any other error codes     Link is not enabled.
 **********************************************************************************************************************/
UINT ip_link_check(void)
{
    UINT err = NX_SUCCESS;
    ULONG current_state = RESET_VALUE;

    err = nx_ip_status_check(&g_ip0, NX_IP_LINK_ENABLED, &current_state, LINK_ENABLE_WAIT_TIME);
    if ((NX_SUCCESS == err) && (NX_IP_LINK_ENABLED != current_state))
    {
        err = NX_NOT_ENABLED;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function sets the link local and global IPv6 addresses.
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
    if (NX_SUCCESS != err)
    {
        return err;
    }

    /* Set the global IPv6 address (Prefix set to 64) */
    err = nxd_ipv6_address_set(&g_ip0, interface_index, &g_ip0_ipv6_global_address, 64, NULL);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function creates the DHCPv6 Server instance.
 * @param[in] None.
 * @retval    NX_SUCCESS                Created successfully.
 * @retval    Any other error codes     Failed to create.
 **********************************************************************************************************************/
UINT dhcpv6_server_init(void)
{
    UINT err = NX_SUCCESS;

    /* Create the DHCPv6 Server instance */
    err = nx_dhcpv6_server_create(&g_dhcpv6_server,
                                  &g_ip0,
                                  "g_dhcpv6_server",
                                  &g_packet_pool0,
                                  g_dhcpv6_server_stack_memory,
                                  G_DHCPV6_SERVER_TASK_STACK_SIZE,
                                  NX_NULL,
                                  NX_NULL);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function sets the DNS IPv6 address for the DHCPv6 Server database.
 * @param[in] None.
 * @retval    NX_SUCCESS                Set successfully.
 * @retval    Any other error codes     Failed to set.
 **********************************************************************************************************************/
UINT dhcpv6_dns_address_set(void)
{
    UINT err = NX_SUCCESS;

    /* Fill the DNS IPv6 address */
    FILL_NXD_IPV6_ADDRESS(g_dns_ipv6_address, 0x2001, 0xEE08, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2);

    /* Update the DNS IPv6 address for the DHCPv6 Server database */
    err = nx_dhcpv6_create_dns_address(&g_dhcpv6_server, &g_dns_ipv6_address);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function sets the DHCPv6 Server DUID.
 * @param[in] None.
 * @retval    NX_SUCCESS                Set successfully.
 * @retval    Any other error codes     Failed to set.
 **********************************************************************************************************************/
UINT dhcpv6_server_duid_set(void)
{
    UINT err = NX_SUCCESS;

    /* Set the DUID time as the start of the millennium */
    ULONG duid_time = SECONDS_SINCE_JAN_1_2000_MOD_32;

    err = nx_dhcpv6_set_server_duid(&g_dhcpv6_server,
                                    NX_DHCPV6_SERVER_DUID_TYPE,
                                    NX_DHCPV6_SERVER_HARDWARE_TYPE_ETHERNET,
                                    g_dhcpv6_server.nx_dhcpv6_ip_ptr->nx_ip_arp_physical_address_msw,
                                    g_dhcpv6_server.nx_dhcpv6_ip_ptr->nx_ip_arp_physical_address_lsw,
                                    duid_time);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function creates the DHCPv6 IP address range.
 * @param[in] None.
 * @retval    NX_SUCCESS                Created successfully.
 * @retval    Any other error codes     Failed to create.
 **********************************************************************************************************************/
UINT dhcpv6_ip_address_range_set(void)
{
    UINT err = NX_SUCCESS;
    UINT addresses_added = RESET_VALUE;

    /* Set the IPv6 start address */
    FILL_NXD_IPV6_ADDRESS(g_start_ipv6_address, 0x2001, 0xEE08, 0x0, 0x0, 0x0, 0x0, 0x0, 0x110);

    /* Set the IPv6 end address */
    FILL_NXD_IPV6_ADDRESS(g_end_ipv6_address, 0x2001, 0xEE08, 0x0, 0x0, 0x0, 0x0, 0x0, 0x150);

    /* Create the DHCPv6 IP address range */
    err = nx_dhcpv6_create_ip_address_range(&g_dhcpv6_server, &g_start_ipv6_address, &g_end_ipv6_address,
                                            &addresses_added);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function starts the DHCPv6 Server processing.
 * @param[in] None.
 * @retval    NX_SUCCESS                Started successfully.
 * @retval    Any other error codes     Failed to start.
 **********************************************************************************************************************/
UINT dhcpv6_server_start(void)
{
    UINT err = NX_SUCCESS;

    err = nx_dhcpv6_server_start(&g_dhcpv6_server);

    return err;
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

/*******************************************************************************************************************//**
 * @brief      This function converts a MAC address into a string to print.
 * @param[out] str                      Converted string.
 * @param[in]  mac_msw                  High MAC address.
 * @param[in]  mac_lsw                  Low MAC address.
 * @retval     None.
 **********************************************************************************************************************/
void str_mac(UCHAR * str, USHORT mac_msw, ULONG mac_lsw)
{
    sprintf((char *)str, "%02hX:%02hX:%02lX:%02lX:%02lX:%02lX", ((mac_msw >> 8) & 0xFF), ((mac_msw >> 0) & 0xFF),
            ((mac_lsw >> 24) & 0xFF), ((mac_lsw >> 16) & 0xFF), ((mac_lsw >> 8) & 0xFF), ((mac_lsw >> 0) & 0xFF));
}
