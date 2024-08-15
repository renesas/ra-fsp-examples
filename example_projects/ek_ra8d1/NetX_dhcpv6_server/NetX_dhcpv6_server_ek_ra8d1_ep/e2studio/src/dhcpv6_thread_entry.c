/***********************************************************************************************************************
 * File Name    : dhcpv6_server_thread_entry.c
 * Description  : Contains functions from the dhcpv6_server main thread
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "dhcpv6_thread.h"
#include "common_utils.h"
#include "dhcpv6_server.h"

/*******************************************************************************************************************//**
 * @addtogroup NetX_dhcpv6_server_ep
 * @{
 **********************************************************************************************************************/

/* Packet pool instance (If this is a Trustzone part, the memory must be placed in Non-secure memory). */
NX_PACKET_POOL g_packet_pool0;
uint8_t g_packet_pool0_pool_memory[G_PACKET_POOL0_PACKET_NUM * (G_PACKET_POOL0_PACKET_SIZE + sizeof(NX_PACKET))] BSP_ALIGN_VARIABLE(4) ETHER_BUFFER_PLACE_IN_SECTION;

/* IP instance */
NX_IP g_ip0;

/* Stack memory for g_ip0. */
uint8_t g_ip0_stack_memory[G_IP0_TASK_STACK_SIZE] BSP_PLACE_IN_SECTION(".stack.g_ip0") BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);

/* ARP cache memory for g_ip0. */
uint8_t g_ip0_arp_cache_memory[G_IP0_ARP_CACHE_SIZE] BSP_ALIGN_VARIABLE(4);

/* DHCP server instance. */
NX_DHCPV6_SERVER g_dhcpv6_server;
uint8_t g_dhcpv6_server_stack_memory[G_DHCPV6_SERVER_TASK_STACK_SIZE] BSP_PLACE_IN_SECTION(".stack.g_dhcpv6_server") BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);

/* User defined variables.*/
/* Set the DUID time as the start of the millenium. */
static ULONG duid_time = SECONDS_SINCE_JAN_1_2000_MOD_32;
/* variable to capture server state*/
static bool b_serverReady = false;
/* variable to capture IP leased state*/
static bool b_ip_leased = false;
/* IPv6 addresses.*/
NXD_ADDRESS g_dns_ipv6_address;
NXD_ADDRESS g_start_ipv6_address;
NXD_ADDRESS g_end_ipv6_address;

/* Function declaration. */
static void nx_common_init0(void);
static void packet_pool_init0(void);
static void ip_init0(void);
static void dhcpv6_server_init0(void);

/* DHCPv6 Server Thread entry function */
void dhcpv6_thread_entry(void)
{
    UINT    status            = NX_SUCCESS;
    UINT    addresses_added   = RESET_VALUE;
    UINT    err               = TX_SUCCESS;
    uint8_t read_data         = RESET_VALUE;
    ipv6_info_t leased_info     = {RESET_VALUE};

    /* Initialize the RTT Thread.*/
    rtt_thread_init_check();
    /* print the banner and EP info. */
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_BANNER, RESET_VALUE, NULL);

    /* Initialize the NetX system.*/
    nx_common_init0();

    /* Initialize the packet pool.*/
    packet_pool_init0();

    /* create the ip instance.*/
    ip_init0();

    /*create dhcp server */
    dhcpv6_server_init0();

    PRINT_INFO_STR("Network Initialization completed successfully.");

    /* The DNS IPV6 address is updated for DHCPV6 server database, so that when the Clients request
     * for IP address, DHCPV6 server also provides the DNS server info to the clients. */
    FILL_NXD_IPV6_ADDRESS(g_dns_ipv6_address, 0x2001,0xEE08,0x0,0x0,0x0,0x0,0x0,0x2);

    /* Update the  DNS IPv6 address. */
    status = nx_dhcpv6_create_dns_address(&g_dhcpv6_server, &g_dns_ipv6_address);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_dhcpv6_create_dns_address failed.");
        ERROR_TRAP(status);
    }
    PRINT_INFO_STR("Loads the DHCPv6 Server with the DNS server address successfully.");

    /* Set server DUID.*/
    status = nx_dhcpv6_set_server_duid(&g_dhcpv6_server,
                                       NX_DHCPV6_SERVER_DUID_TYPE,
                                       NX_DHCPV6_SERVER_HARDWARE_TYPE_ETHERNET,
                                       g_dhcpv6_server.nx_dhcpv6_ip_ptr -> nx_ip_arp_physical_address_msw,
                                       g_dhcpv6_server.nx_dhcpv6_ip_ptr -> nx_ip_arp_physical_address_lsw,
                                       duid_time);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_dhcpv6_set_server_duid failed.");
        ERROR_TRAP(status);
    }
    PRINT_INFO_STR("Server DUID sets successfully.");

    /* Set the IPv6 start address.  */
    FILL_NXD_IPV6_ADDRESS(g_start_ipv6_address, 0x2001,0xEE08,0x0,0x0,0x0,0x0,0x0,0x110);
    /* Set the IPv6 end address.  */
    FILL_NXD_IPV6_ADDRESS(g_end_ipv6_address  , 0x2001,0xEE08,0x0,0x0,0x0,0x0,0x0,0x150);

    /* create dhcp6 IP address range.*/
    status = nx_dhcpv6_create_ip_address_range(&g_dhcpv6_server,
                                               &g_start_ipv6_address,
                                               &g_end_ipv6_address,
                                               &addresses_added);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_dhcpv6_create_ip_address_range failed.");
        ERROR_TRAP(status);
    }
    PRINT_INFO_STR("Created dhcpv6 IP address range successfully.");

    /* print the dhcpv6 server menu options. */
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_MENU, RESET_VALUE, NULL);

    while (true)
    {
        /* wait for user input. */
        do
        {
            err = check_for_RTT_user_IP(&read_data);
        }while (err != TX_SUCCESS);

        switch (read_data)
        {
            case DHCPV6_SERVER_START :
            {
                if(false == b_serverReady)
                {
                     /*Start DHCPV6 Server processing.*/
                    status = nx_dhcpv6_server_start(&g_dhcpv6_server);
                    if(NX_SUCCESS != status)
                    {
                        PRINT_ERR_STR("nx_dhcpv6_server_start failed.");
                        ERROR_TRAP(status);
                    }
                    PRINT_INFO_STR("DHCPV6 server started.");

                     /*set the server ready flag to true*/
                    b_serverReady = true;
                }
                else
                {
                    PRINT_INFO_STR("DHCPV6 server is already running...");
                }
            }
            break;

            case PRINT_DHCPV6_LEASED_IP_INFO :
            {
                if(true == b_serverReady)
                {
                    b_ip_leased = false;
                    /*Print the DHCPV6 leased IPs info.*/
                    for (ULONG idx = RESET_VALUE; g_dhcpv6_server.nx_dhcpv6_lease_list[idx].nx_dhcpv6_lease_assigned_to != NULL ; idx++)
                    {
                        /* assign and print leased ipv6 address, clients MAC address */
                        memset(&leased_info, RESET_VALUE, sizeof(ipv6_info_t));
                        memcpy(&leased_info.ipv6_address, &g_dhcpv6_server.nx_dhcpv6_lease_list[idx].nx_dhcpv6_lease_IP_address, sizeof(NXD_ADDRESS));
                        leased_info.mac_high = g_dhcpv6_server.nx_dhcpv6_lease_list[idx].nx_dhcpv6_lease_assigned_to->nx_dhcpv6_client_duid.nx_link_layer_address_msw;
                        leased_info.mac_low  = g_dhcpv6_server.nx_dhcpv6_lease_list[idx].nx_dhcpv6_lease_assigned_to->nx_dhcpv6_client_duid.nx_link_layer_address_lsw;
                        app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_CLIENT_DATA, sizeof(ipv6_info_t ), &leased_info);
                        /* set the IP leased state as true.*/
                        b_ip_leased = true;
                    }
                    if(false == b_ip_leased)
                    {
                        PRINT_INFO_STR("No IP leased to dhcpv6 client.");
                    }
                }
                else
                {
                    PRINT_INFO_STR("DHCPV6 server is not started.");
                }
            }
            break;

            default :
            {
                PRINT_INFO_STR("Invalid Menu Option Selected");
            }
            break;
        }
        /* print the dhcpv6 server menu options.*/
        app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_MENU, RESET_VALUE, NULL);

        tx_thread_sleep (1);
    }
}

/*******************************************************************************************************************//**
 * @brief     Initialization of NetX system.
 * @param[IN] None
 * @retval    None
 **********************************************************************************************************************/
static void nx_common_init0(void)
{
    /* Initialize the NetX system. */
    nx_system_initialize ();
}

/*******************************************************************************************************************//**
 * @brief       Creates packet pool
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void packet_pool_init0(void)
{
    /* Create the packet pool. */
    UINT status = nx_packet_pool_create(&g_packet_pool0,
                                        "g_packet_pool0 Packet Pool",
                                        G_PACKET_POOL0_PACKET_SIZE,
                                        &g_packet_pool0_pool_memory[0],
                                        G_PACKET_POOL0_PACKET_NUM * (G_PACKET_POOL0_PACKET_SIZE + sizeof(NX_PACKET)));
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_packet_pool_create failed.");
        ERROR_TRAP(status);
    }
}

/*******************************************************************************************************************//**
 * @brief     Create the ip instance and enables ARP,udp,icmp,ipv6.
 * @param[IN] None
 * @retval    None
 **********************************************************************************************************************/
static void ip_init0(void)
{
    UINT status        = NX_SUCCESS;
    UINT address_index = RESET_VALUE;

    /* Create the ip instance. */
    status = nx_ip_create(&g_ip0,
                          "g_ip0 IP Instance",
                          G_IP0_ADDRESS,
                          G_IP0_SUBNET_MASK,
                          &g_packet_pool0,
                          g_netxduo_ether_0,
                          &g_ip0_stack_memory[0],
                          G_IP0_TASK_STACK_SIZE,
                          G_IP0_TASK_PRIORITY);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_ip_create failed.");
        ERROR_TRAP(status);
    }

    /* enables the ARP management component for the specified IP instance*/
    status = nx_arp_enable(&g_ip0, &g_ip0_arp_cache_memory[0], G_IP0_ARP_CACHE_SIZE);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_arp_enable failed.");
        ERROR_TRAP(status);
    }

    /* Enable UDP */
    status = nx_udp_enable(&g_ip0);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_udp_enable failed.");
        ERROR_TRAP(status);
    }

    /* Enabled ICMP */
    status = nx_icmp_enable(&g_ip0);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_icmp_enable failed.");
        ERROR_TRAP(status);
    }

    /* Enable IPv6. */
    status = nxd_ipv6_enable(&g_ip0);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nxd_ipv6_enable failed.");
        ERROR_TRAP(status);
    }

    /* Enable ICMPv6 (Required for neighbor discovery). */
    status = nxd_icmp_enable(&g_ip0);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nxd_icmp_enable failed.");
        ERROR_TRAP(status);
    }

    /* Wait for the link to be enabled. */
    ULONG current_state;
    PRINT_INFO_STR("Checking Ethernet Link...");

    status = nx_ip_status_check(&g_ip0, NX_IP_LINK_ENABLED, &current_state, LINK_ENABLE_WAIT_TIME);
    if((NX_SUCCESS != status) || (NX_IP_LINK_ENABLED != current_state))
    {
        PRINT_ERR_STR("nx_ip_status_check failed.");
        ERROR_TRAP(status);
    }
    PRINT_INFO_STR("Ethernet link is up.");

    /* Set the link local IPv6 address (Prefix set to 10). */
    status = nxd_ipv6_address_set(&g_ip0, address_index, NX_NULL, 10, NULL);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nxd_ipv6_address_set failed.");
        ERROR_TRAP(status);
    }

    /* Set the global IPv6 address (Prefix set to 64). */
    status = nxd_ipv6_address_set(&g_ip0, address_index, &g_ip0_ipv6_global_address, 64, NULL);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nxd_ipv6_address_set failed.");
        ERROR_TRAP(status);
    }
}

/*******************************************************************************************************************//**
 * @brief     Create the DHCPV6 server instance.
 * @param[IN] None
 * @retval    None
 **********************************************************************************************************************/
static void dhcpv6_server_init0(void)
{
    /* Create the DHCP server instance. */
    UINT status = nx_dhcpv6_server_create(&g_dhcpv6_server,
                                          &g_ip0,
                                          "g_dhcpv6_server",
                                          &g_packet_pool0,
                                          g_dhcpv6_server_stack_memory,
                                          G_DHCPV6_SERVER_TASK_STACK_SIZE,
                                          NX_NULL,
                                          NX_NULL);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_dhcpv6_server_create failed.");
        ERROR_TRAP(status);
    }
}

/*******************************************************************************************************************//**
 * @brief       This function processes ipv6 address into string to print.
 * @param[IN]   ipv6     IPV6 address.
 * @param[OUT]  str      converted string.
 * @retval      None.
 **********************************************************************************************************************/
void str_ipv6(UCHAR * str, NXD_ADDRESS ipv6)
{
    sprintf((char *)str,"%lX:%lX:%lX:%lX:%lX:%lX:%lX:%lX",
    (((uint32_t)ipv6.nxd_ip_address.v6[0] & 0xFFFF0000 ) >> 16),
    (((uint32_t)ipv6.nxd_ip_address.v6[0] & 0x0000FFFF ) >> 0),
    (((uint32_t)ipv6.nxd_ip_address.v6[1] & 0xFFFF0000 ) >> 16),
    (((uint32_t)ipv6.nxd_ip_address.v6[1] & 0x0000FFFF ) >> 0),
    (((uint32_t)ipv6.nxd_ip_address.v6[2] & 0xFFFF0000 ) >> 16),
    (((uint32_t)ipv6.nxd_ip_address.v6[2] & 0x0000FFFF ) >> 0),
    (((uint32_t)ipv6.nxd_ip_address.v6[3] & 0xFFFF0000 ) >> 16),
    (((uint32_t)ipv6.nxd_ip_address.v6[3] & 0x0000FFFF ) >> 0)
    );
}

/*******************************************************************************************************************//**
 * @brief       This function processes MAC address into string to print.
 * @param[IN]   mac_msw     High MAC address.
 * @param[IN]   mac_lsw     Low MAC address.
 * @param[OUT]  str         converted string.
 * @retval      None.
 **********************************************************************************************************************/
void str_mac(UCHAR * str, USHORT mac_msw, ULONG mac_lsw)
{
    sprintf((char *)str,"%02hX:%02hX:%02lX:%02lX:%02lX:%02lX",(mac_msw>>8 & 0xFF),(mac_msw>>0 & 0xFF),
            (mac_lsw>>24 & 0xFF),(mac_lsw>>16 & 0xFF),(mac_lsw>>8 & 0xFF),(mac_lsw>>0 & 0xFF));
}

/*******************************************************************************************************************//**
 * @} (end addtogroup NetX_dhcpv6_server_ep)
 **********************************************************************************************************************/
