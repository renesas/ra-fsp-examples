/***********************************************************************************************************************
 * File Name    : snmp_agent_thread_entry.c
 * Description  : Contains entry function of SNMP Agent.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#include "snmp_thread.h"
#include "common_utils.h"
#include "snmp_agent_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup NetX_snmp_agent_ep
 * @{
 **********************************************************************************************************************/
/* IP instance */
NX_IP g_ip0;

/* Stack memory for g_ip0. */
uint8_t g_ip0_stack_memory[G_IP0_TASK_STACK_SIZE] BSP_PLACE_IN_SECTION(".stack.g_ip0") BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);

/* ARP cache memory for g_ip0. */
uint8_t g_ip0_arp_cache_memory[G_IP0_ARP_CACHE_SIZE] BSP_ALIGN_VARIABLE(4);
NX_DHCP g_dhcp_client0;

/* Packet pool instance (If this is a Trustzone part, the memory must be placed in Non-secure memory). */
NX_PACKET_POOL g_packet_pool0;
uint8_t g_packet_pool0_pool_memory[G_PACKET_POOL0_PACKET_NUM * (G_PACKET_POOL0_PACKET_SIZE + sizeof(NX_PACKET))] BSP_ALIGN_VARIABLE(4) ETHER_BUFFER_PLACE_IN_SECTION;

/* SNMP Agent instance. */
NX_SNMP_AGENT g_snmp_agent0;
uint8_t g_snmp_agent0_stack_memory[NX_SNMP_AGENT_THREAD_STACK_SIZE] BSP_PLACE_IN_SECTION(".stack.g_snmp_agent0") BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);

/*******************************************************************************************************************//**
 * @brief     Create the packet pool.
 * @param[IN] None
 * @retval    None
 **********************************************************************************************************************/
void g_packet_pool0_quick_setup()
{
    /* Create the packet pool. */
    UINT status = nx_packet_pool_create(&g_packet_pool0,
                                        "g_packet_pool0 Packet Pool",
                                        G_PACKET_POOL0_PACKET_SIZE,
                                        &g_packet_pool0_pool_memory[0],
                                        G_PACKET_POOL0_PACKET_NUM * (G_PACKET_POOL0_PACKET_SIZE + sizeof(NX_PACKET)));
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_packet_pool_create failed ");
        ERROR_TRAP(status);
    }
}


/*******************************************************************************************************************//**
 * @brief     Create the ip instance and enables UDP,ICMP.
 * @param[IN] None
 * @retval    NX_SUCCESS   On successful IP create operation
 * @retval    Any Other Error code apart from NX_SUCCESS on Unsuccessful operation.
 **********************************************************************************************************************/
static UINT ip_init0(void)
{
    UINT status = NX_SUCCESS;
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
        PRINT_ERR_STR("nx_ip_create failed");
        return status;
    }

    /* Set the gateway address if it is configured. */
    if(IP_ADDRESS(0, 0, 0, 0) != G_IP0_GATEWAY_ADDRESS)
    {
        status = nx_ip_gateway_address_set(&g_ip0, G_IP0_GATEWAY_ADDRESS);
        if(NX_SUCCESS != status)
        {
            PRINT_ERR_STR("nx_ip_gateway_address_set failed");
            return status;
        }
    }

    /* Enables Address Resolution Protocol (ARP).*/
    status = nx_arp_enable(&g_ip0, &g_ip0_arp_cache_memory[0], G_IP0_ARP_CACHE_SIZE);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_arp_enable failed");
        return status;
    }

    /* Enable udp.*/
    status = nx_udp_enable(&g_ip0);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_udp_enable failed");
        return status;
    }

    /* Enable icmp.*/
    status = nx_icmp_enable(&g_ip0);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_icmp_enable failed");
        return status;
    }

    /*Enable fragmentation*/
    status = nx_ip_fragment_enable(&g_ip0);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_ip_fragment_enable failed");
        return status;
    }

    /* Wait for the link to be enabled. */
    ULONG current_state;
    status = nx_ip_status_check(&g_ip0, NX_IP_LINK_ENABLED, &current_state, LINK_ENABLE_WAIT_TIME);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_ip_status_check failed");
        return status;
    }
    PRINT_INFO_STR("Ethernet Link is Up!");
    return status;
}

/*******************************************************************************************************************//**
 * @brief     Create the DHCP instance and Set the DHCP Client packet pool.
 * @param[IN] None
 * @retval    NX_SUCCESS   On successful DHCP operation
 * @retval    Any Other Error code apart from NX_SUCCESS on Unsuccessful operation.
 **********************************************************************************************************************/
static UINT dhcpv4_client_init()
{
    UINT status = NX_SUCCESS;
    ULONG requested_status;

    /* Create the DHCP instance. */
    status = nx_dhcp_create (&g_dhcp_client0, &g_ip0, "g_dhcp_client0");
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_dhcp_create failed");
        return status;
    }

    /* Set the DHCP Packet Pool */
    status = nx_dhcp_packet_pool_set(&g_dhcp_client0, &g_packet_pool0);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_dhcp_packet_pool_set failed");
        return status;
    }

    /* Start DHCP service. */
    status = nx_dhcp_start (&g_dhcp_client0);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_dhcp_start failed");
        return status;
    }

    /* Wait until an IP address is acquired via DHCP. */
    status = nx_ip_status_check (&g_ip0, NX_IP_ADDRESS_RESOLVED, &requested_status, LINK_ENABLE_WAIT_TIME);
    if ((NX_SUCCESS != status) || (requested_status != NX_IP_ADDRESS_RESOLVED))
    {
        PRINT_ERR_STR("nx_ip_status_check failed");
        return status;
    }

    PRINT_INFO_STR("Acquired the IP address via DHCP successfully!.");
    return status;
}

/*******************************************************************************************************************//**
 * @brief     Create the DHCP instance and Set the DHCP Client packet pool.
 * @param[IN] None
 * @retval    NX_SUCCESS   On successful network_init operation
 * @retval    Any Other Error code apart from NX_SUCCESS on Unsuccessful operation.
 **********************************************************************************************************************/
static UINT network_init(void)
{
    UINT  status            = RESET_VALUE;
    /* Client IP address and Network Mask. */
    ULONG client_ip_address = RESET_VALUE;
    ULONG network_mask      = RESET_VALUE;

    /* Initialize NetX. */
    nx_system_initialize ();

    /* initialize packet pool */
    g_packet_pool0_quick_setup();

    /* IP initialization */
    status = ip_init0();
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("ip_init0 failed");
        return status;
    }

    /* Initialize DHCP v4 Client. */
    status = dhcpv4_client_init();
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("dhcpv4_client_init failed");
        return status;
    }

    /*Gets IP address*/
    status = nx_ip_address_get (&g_ip0, &client_ip_address, &network_mask);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_ip_address_get failed");
        return status;
    }

    /* print client IP address. */
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_CLIENT_IP, sizeof(ULONG *), &client_ip_address);
    return status;
}

/*******************************************************************************************************************//**
 * @brief     Create the snmp agent.
 * @param[IN] None
 * @retval    NX_SUCCESS   on successful agent creation
 * @retval    Any Other Error code apart from NX_SUCCESS on Unsuccessful operation.
 **********************************************************************************************************************/
static UINT snmp_agent_init()
{
    /* Create the SNMP Agent instance. */
    UINT status = nx_snmp_agent_create(&g_snmp_agent0,
                                       "g_snmp_agent0",
                                       &g_ip0,
                                       g_snmp_agent0_stack_memory,
                                       NX_SNMP_AGENT_THREAD_STACK_SIZE,
                                       &g_packet_pool0,
                                       g_snmp_agent0UsernameHandler,
                                       g_snmp_agent0GetHandler,
                                       g_snmp_agent0GetHandler,
                                       g_snmp_agent0SetHandler);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_snmp_agent_create failed");
        return status;
    }
    return status;
}

/*SNMP Thread entry function */
void snmp_thread_entry(void)
{
    UINT       status     = NX_SNMP_ERROR;
    uint8_t    input_data = RESET_VALUE;
    UINT       err        = NX_SUCCESS;

    /* Initialize the RTT Thread.*/
    rtt_thread_init_check();

    /* print the banner and EP info. */
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_BANNER, RESET_VALUE, NULL);

    /*Initializes network and dhcp client*/
    status = network_init();
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("network_init failed.");
        ERROR_TRAP(status);
    }
    else
    {
        PRINT_INFO_STR("Network initialization done.. now SNMP agent services initializing.");
    }

    /*create snmp agent*/
    status = snmp_agent_init();
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("snmp_agent_init failed.");
        ERROR_TRAP(status);
    }

    PRINT_INFO_STR("SNMP Agent created successfully.");

    status = snmp_agent_services_init();
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("snmp_agent_services_init failed");
        ERROR_TRAP(status);
    }
    else
    {
        PRINT_INFO_STR("SNMP agent is started and ready to be used.");
    }

    app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_MENU, RESET_VALUE, NULL);
    while (true)
    {
        input_data = RESET_VALUE;
        err = check_for_RTT_user_IP(&input_data);
        if (err == TX_SUCCESS)
        {
            status = async_trap_v2_send();
            if (NX_SUCCESS != status)
            {
                PRINT_ERR_STR("async_trap_v2_send failed.");
                ERROR_TRAP(status);
            }

            app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_MENU, RESET_VALUE, NULL);
            tx_thread_sleep (1);
        }
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup NetX_snmp_agent_)ep
 **********************************************************************************************************************/
