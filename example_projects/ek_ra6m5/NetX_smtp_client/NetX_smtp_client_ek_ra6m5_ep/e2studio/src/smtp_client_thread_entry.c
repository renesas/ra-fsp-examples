/***********************************************************************************************************************
 * File Name    : smtp_client_thread_entry.c
 * Description  : Contains functions from the smtp main thread
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
#include "smtp_client_thread.h"
#include "smtp_client_ep.h"
#include "common_utils.h"

/*******************************************************************************************************************//**
 * @addtogroup NetX_smtp_client_ep
 * @{
 **********************************************************************************************************************/

/* Packet pool instance (If this is a Trustzone part, the memory must be placed in Non-secure memory). */
NX_PACKET_POOL g_packet_pool0;
uint8_t g_packet_pool0_pool_memory[G_PACKET_POOL0_PACKET_NUM * (G_PACKET_POOL0_PACKET_SIZE + sizeof(NX_PACKET))] BSP_ALIGN_VARIABLE(4) ETHER_BUFFER_PLACE_IN_SECTION;

NX_PACKET response;
NX_PACKET *response_ptr = &response;

/* IP instance */
NX_IP g_ip0;

/* Stack memory for g_ip0. */
uint8_t g_ip0_stack_memory[G_IP0_TASK_STACK_SIZE] BSP_PLACE_IN_SECTION(".stack.g_ip0") BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);

/* ARP cache memory for g_ip0. */
uint8_t g_ip0_arp_cache_memory[G_IP0_ARP_CACHE_SIZE] BSP_ALIGN_VARIABLE(4);

/* SMTP Client instance. */
NX_SMTP_CLIENT g_smtp_client0;
NXD_ADDRESS g_smtp_client0_ip_address;

/* function declaration */
static void nx_common_init0(void);
static void smtp_client_init0();
static void ip_init0();
static void packet_pool_init0();


/* SMTP Client Thread entry function */
void smtp_client_thread_entry(void)
{
    UINT               status                      = NX_SUCCESS;
    ULONG              client_address              = RESET_VALUE;
    ULONG              server_address              = RESET_VALUE;
    ULONG              network_mask                = RESET_VALUE;
    UINT               err                         = TX_SUCCESS;
    NX_PACKET          *my_packet                  = NULL;
    CHAR              rtt_ip_data[IP_MSG_MAX_SIZE] = {'\0',};

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

    /*create smtp client*/
    smtp_client_init0();

    PRINT_INFO_STR("Network Initialization completed successfully.");
    status = nx_ip_fragment_enable(&g_ip0);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_ip_fragment_enable failed.");
        ERROR_TRAP(status);
    }
    PRINT_INFO_STR("WAITING FOR IP RESOLUTION    : ");


    /* Wait for IP address to be resolved through DHCP. */
    nx_ip_status_check(&g_ip0, NX_IP_ADDRESS_RESOLVED, (ULONG *) &status, NX_IP_PERIODIC_RATE);
    if(status != NX_IP_ADDRESS_RESOLVED)
    {
        PRINT_ERR_STR("nx_ip_status_check failed.");
        ERROR_TRAP(status);
    }
    else
    {
        PRINT_INFO_STR("SUCCESS");
    }

    /* It is. Get the client IP address from this NetX service. */
    status = nx_ip_address_get(&g_ip0, &client_address, &network_mask);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_ip_address_get failed.");
        ERROR_TRAP(status);
    }
    /*Print client ip address*/
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_CLIENT_DATA, sizeof(ULONG *), &client_address);

    /*Print server ip address*/
    server_address =  g_smtp_client0_ip_address.nxd_ip_address.v4;
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_SERVER_DATA, sizeof(ULONG *), &server_address);

    /* Check that the device is able to send and receive packets with this IP address. */
    status =  nx_icmp_ping(&g_ip0, server_address, "Hello", sizeof("Hello"), &my_packet, WAIT_TIME);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_icmp_ping failed.");
        ERROR_TRAP(status);
    }
    /* Release the echo response packet when we are done with it. */
    nx_packet_release(my_packet);
    PRINT_INFO_STR("Successfully Pinged SMTP server");

    /*Print Menu */
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_MENU, RESET_VALUE, NULL);
    /* wait for user input  - as based on user input only EP actions will be proceeded */
    while (true)
    {
        uint8_t read_data = RESET_VALUE;
        /* wait for user input. */
        do
        {
            err = check_for_RTT_user_IP(&rtt_ip_data[RESET_VALUE]);
        }while (err != TX_SUCCESS);

        /* Conversion from input string to integer value */
        read_data = (uint8_t) (atoi((char *)rtt_ip_data));

        switch (read_data)
        {
            case SEND :
            {
                /* Sends an SMTP mail to the server*/
                status = nx_smtp_mail_send(&g_smtp_client0, "epdesign@test.local.com", NX_SMTP_MAIL_PRIORITY_NORMAL, SUBJECT_LINE, MAIL_BODY, strlen(MAIL_BODY));
                if(NX_SUCCESS != status)
                {
                    PRINT_ERR_STR("nx_smtp_mail_send failed.");
                    ERROR_TRAP(status);
                }
                PRINT_INFO_STR("SMTP CLIENT MAIL SENT TO           : epdesign@test.local.com.");
                PRINT_INFO_STR("SMTP CLIENT TEST                   : SUCCESS");
            }
            break;
            case READ :
            {
                app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_CONFIGURED_DATA, RESET_VALUE, NULL);
            }
            break;

            default :
            {
                break;
            }

        }
        /* Print Main Menu option for smtp client operation.*/
        app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_MENU, RESET_VALUE, NULL);

        tx_thread_sleep (1);
    }
}


/*******************************************************************************************************************//**
 * @brief       Creates smtp client for sending message to the server.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void smtp_client_init0()
{
    /* Create the SMTP Client instance. */
    g_smtp_client0_ip_address.nxd_ip_version = NX_IP_VERSION_V4;
    g_smtp_client0_ip_address.nxd_ip_address.v4 = G_SMTP_CLIENT0_ADDRESS_IPV4;

    /*create smtp client for sending message to the server*/
    UINT status = nxd_smtp_client_create(&g_smtp_client0,
                                         &g_ip0,
                                         &g_packet_pool0,
                                         G_SMTP_CLIENT0_CLIENT_NAME,
                                         G_SMTP_CLIENT0_CLIENT_PASSWORD,
                                         G_SMTP_CLIENT0_CLIENT_ADDRESS,
                                         G_SMTP_CLIENT0_CLIENT_DOMAIN,
                                         G_SMTP_CLIENT0_AUTHENTICATION_TYPE,
                                         &g_smtp_client0_ip_address,
                                         G_SMTP_CLIENT0_SERVER_PORT);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nxd_smtp_client_create failed.");
        ERROR_TRAP(status);
    }
}

/*******************************************************************************************************************//**
 * @brief       Create the ip instance and enables ARP,tcp,icmp.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void ip_init0()
{
    UINT status = NX_SUCCESS;
    ULONG current_state = RESET_VALUE;

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

    /* Set the gateway address if it is configured. */
    if(IP_ADDRESS(0, 0, 0, 0) != G_IP0_GATEWAY_ADDRESS)
    {
        status = nx_ip_gateway_address_set(&g_ip0, G_IP0_GATEWAY_ADDRESS);
        if(NX_SUCCESS != status)
            {
                PRINT_ERR_STR("nx_ip_gateway_address_set failed.");
                ERROR_TRAP(status);
            }
    }

    /* enables the ARP management component for the specified IP instance*/
    status = nx_arp_enable(&g_ip0, &g_ip0_arp_cache_memory[0], G_IP0_ARP_CACHE_SIZE);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_arp_enable failed.");
        ERROR_TRAP(status);
    }

    /* TCP has to be enabled */
    status = nx_tcp_enable(&g_ip0);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_tcp_enable failed.");
        ERROR_TRAP(status);
    }

    /* Enabled ICMP */
    status = nx_icmp_enable(&g_ip0);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_icmp_enable failed.");
        ERROR_TRAP(status);
    }

    PRINT_INFO_STR("Checking Ethernet link....");
    status = nx_ip_status_check(&g_ip0, NX_IP_LINK_ENABLED, &current_state, LINK_ENABLE_WAIT_TIME);
    if((NX_SUCCESS != status) || (NX_IP_LINK_ENABLED != current_state))
    {
        PRINT_ERR_STR("nx_ip_status_check failed.");
        ERROR_TRAP(status);
    }
    PRINT_INFO_STR("Ethernet link is up.");
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
static void packet_pool_init0()
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
 * @} (end addtogroup NetX_smtp_client_ep)
 **********************************************************************************************************************/
