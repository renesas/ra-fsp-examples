/***********************************************************************************************************************
 * File Name    : dhcpv4_server_ep.c
 * Description  : Contains data structures and functions used in dhcpv4_server_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "dhcpv4_server_ep.h"

/* Global variables */

/* Packet pool instance (If this is a Trustzone part, the memory must be placed in Non-secure memory) */
NX_PACKET_POOL g_packet_pool0;
#if (BSP_PERIPHERAL_ESWM_PRESENT)
uint8_t g_packet_pool0_pool_memory[G_PACKET_POOL0_PACKET_NUM * (G_PACKET_POOL0_PACKET_SIZE + sizeof(NX_PACKET))]\
        BSP_ALIGN_VARIABLE(4);
#else
uint8_t g_packet_pool0_pool_memory[G_PACKET_POOL0_PACKET_NUM * (G_PACKET_POOL0_PACKET_SIZE + sizeof(NX_PACKET))]\
        BSP_ALIGN_VARIABLE(4) ETHER_BUFFER_PLACE_IN_SECTION;
#endif /* BSP_PERIPHERAL_ESWM_PRESENT */

/* IP instance */
NX_IP g_ip0;

/* Stack memory for g_ip0 */
uint8_t g_ip0_stack_memory[G_IP0_TASK_STACK_SIZE] BSP_PLACE_IN_SECTION(".stack.g_ip0")\
        BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);

/* ARP cache memory for g_ip0 */
uint8_t g_ip0_arp_cache_memory[G_IP0_ARP_CACHE_SIZE] BSP_ALIGN_VARIABLE(4);

/* DHCP server instance */
NX_DHCP_SERVER g_dhcp_server0;

/* Stack memory for g_dhcp_server0 */
uint8_t g_dhcp_server0_stack_memory[G_DHCP_SERVER0_TASK_STACK_SIZE] BSP_PLACE_IN_SECTION(".stack.g_dhcp_server0")\
        BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);

/* Private variables */
static bool s_dhcp_server_created = false;
static bool s_dhcp_server_started = false;

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

    /* Enable the Address Resolution Protocol (ARP) */
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

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function waits for the Ethernet link to be enabled.
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
    if ((NX_SUCCESS == err) && (NX_IP_LINK_ENABLED != current_state))
    {
        err = NX_NOT_SUCCESSFUL;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function creates the DHCP server instance.
 * @param[in] None.
 * @retval    NX_SUCCESS                Created successfully.
 * @retval    Any other error codes     Failed to create.
 **********************************************************************************************************************/
UINT dhcpv4_server_init(void)
{
    UINT err = NX_SUCCESS;

    /* Create the DHCP server instance */
    err = nx_dhcp_server_create(&g_dhcp_server0, &g_ip0, g_dhcp_server0_stack_memory, G_DHCP_SERVER0_TASK_STACK_SIZE,
                                DHCP_SERVER_NAME, &g_packet_pool0);
    if (NX_SUCCESS == err)
    {
        s_dhcp_server_created = true;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function creates the pool of IP addresses from the given range for the DHCP server to assign.
 * @param[in] None.
 * @retval    NX_SUCCESS                Created successfully.
 * @retval    Any other error codes     Failed to create.
 **********************************************************************************************************************/
UINT dhcpv4_ip_address_list_create(void)
{
    UINT err = NX_SUCCESS;
    UINT addresses_added = RESET_VALUE;

    /* Create a pool of IP addresses from the given range */
    err = nx_dhcp_create_server_ip_address_list(&g_dhcp_server0, INTERFACE_INDEX, START_IP_ADDRESS_LIST,
                                                END_IP_ADDRESS_LIST, &addresses_added);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function sets the network parameters returned by the DHCP server.
 * @param[in] None.
 * @retval    NX_SUCCESS                Set successfully.
 * @retval    Any other error codes     Failed to set.
 **********************************************************************************************************************/
UINT dhcpv4_network_parameters_set(void)
{
    UINT err = NX_SUCCESS;

    /* Set the network parameters returned by the DHCP server */
    err = nx_dhcp_set_interface_network_parameters(&g_dhcp_server0, INTERFACE_INDEX, NX_DHCP_SUBNET_MASK,
                                                   NX_DHCP_ROUTER_IP_ADDRESS, NX_DHCP_DNS_IP_ADDRESS);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function starts the DHCP server processing.
 * @param[in] None.
 * @retval    NX_SUCCESS                Started successfully.
 * @retval    Any other error codes     Failed to start.
 **********************************************************************************************************************/
UINT dhcpv4_server_start(void)
{
    UINT err = NX_SUCCESS;

    /* Start the DHCP server processing */
    err = nx_dhcp_server_start(&g_dhcp_server0);
    if (NX_SUCCESS == err)
    {
        s_dhcp_server_started = true;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function stops the DHCP server processing.
 * @param[in] None.
 * @retval    NX_SUCCESS                Stopped successfully.
 * @retval    Any other error codes     Failed to stop.
 **********************************************************************************************************************/
UINT dhcpv4_server_stop(void)
{
    UINT err = NX_SUCCESS;

    /* Stop the DHCP server processing */
    err = nx_dhcp_server_stop(&g_dhcp_server0);
    if (NX_SUCCESS == err)
    {
        s_dhcp_server_started = false;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function cleans up active DHCPv4 server resources.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void dhcpv4_server_deinit(void)
{
    if (s_dhcp_server_started)
    {
        nx_dhcp_server_stop(&g_dhcp_server0);
        s_dhcp_server_started = false;
    }

    if (s_dhcp_server_created)
    {
        nx_dhcp_server_delete(&g_dhcp_server0);
        s_dhcp_server_created = false;
    }
}
