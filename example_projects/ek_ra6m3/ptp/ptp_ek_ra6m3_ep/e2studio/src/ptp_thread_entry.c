/***********************************************************************************************************************
 * File Name    : ptp_thread_entry.c
 * Description  : Contains functions from the ptp main thread
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

#include "ptp_thread.h"
#include "common_utils.h"
#include "ptp_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup ptp_ep
 * @{
 **********************************************************************************************************************/

/* Auto generated code.*/
/* Packet pool instance (If this is a Trustzone part, the memory must be placed in Non-secure memory). */
NX_PACKET_POOL g_packet_pool0;

#if defined(ETHER_BUFFER_PLACE_IN_SECTION)
uint8_t g_packet_pool0_pool_memory[G_PACKET_POOL0_PACKET_NUM * (G_PACKET_POOL0_PACKET_SIZE + sizeof(NX_PACKET))] BSP_ALIGN_VARIABLE(4) ETHER_BUFFER_PLACE_IN_SECTION;
#elif defined(WIFI_BUFFER_PLACE_IN_SECTION)
uint8_t g_packet_pool0_pool_memory[G_PACKET_POOL0_PACKET_NUM * (G_PACKET_POOL0_PACKET_SIZE + sizeof(NX_PACKET))] BSP_ALIGN_VARIABLE(4) WIFI_BUFFER_PLACE_IN_SECTION;
#endif

/* IP instance */
NX_IP g_ip0;

/* Stack memory for g_ip0. */
uint8_t g_ip0_stack_memory[G_IP0_TASK_STACK_SIZE] BSP_PLACE_IN_SECTION(".stack.g_ip0") BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);

/* ARP cache memory for g_ip0. */
uint8_t g_ip0_arp_cache_memory[G_IP0_ARP_CACHE_SIZE] BSP_ALIGN_VARIABLE(4);

/* DHCP instance. */
NX_DHCP g_dhcp_client0;

TX_TIMER g_announce_flag_rec;
uint16_t g_port_id = PORT_ID_MAX_VAL;

/* Private function declarations.*/
static UINT ip0_init(void);
static UINT packet_pool0_init(void);
static UINT dhcpv4_client_init(void);
static UINT network_init(uint32_t * ip_addr);
static fsp_err_t ptp_init(void);
static void ptp_deinit(void);
#if PTP_MASTER_MODE
static void set_local_clock_value(void);
#else
static void get_and_display_time_update(void);
static VOID timeout_handler(ULONG value);
static void month_year_readability_update(struct tm * time);
#endif

/* PTP Thread entry function */
void ptp_thread_entry(void)
{
    /* To capture the status(Success/Failure) of each Function/API. */
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize the RTT Thread.*/
    rtt_thread_init_check();
    /* Print EP and Banner info. */
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_BANNER, RESET_VALUE, NULL);

    /* Initialize ptp.*/
    err = ptp_init();
    if(FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("ptp_init function failed.");
        ERROR_TRAP(err);
    }

#if !PTP_MASTER_MODE
    /* wait here in RA board for 0xFFF ticks to check info from master or grand-master clock
     * ptp4l running in Linux
     *
     *  The timer will call timeout_handler function after 0xFFF ticks
     *  If RA Board is able to receive message from Master clock (ptp4l) then
     *  this timer is deactivated.
     */
    UINT status   = TX_SUCCESS;
    status = tx_timer_create(&g_announce_flag_rec,(CHAR*)"announce_timer_check",timeout_handler,
            RESET_VALUE, TIMER_EXPIRATION_TICKS, RESET_VALUE, TX_AUTO_ACTIVATE);
    if (TX_SUCCESS != status)
    {
        PRINT_ERR_STR("Timeout handling is not taken care by EP because tx_time_create API failed");
    }
#endif

    while (true)
    {
#if !PTP_MASTER_MODE
        /* Reset the event variable. */
        ULONG actual_event = RESET_VALUE;
        /* Wait for a time sync event */
        status = tx_event_flags_get(&g_ptp_event_flags0,PTP_EVENT,TX_OR_CLEAR, &actual_event, EVENT_WAIT_TIME);
        if ((TX_SUCCESS == status) && (PTP_EVENT == actual_event))
        {
            get_and_display_time_update();
        }
        else
        {
            /* We have not received message from master
             * activate the timer to verify if there is
             * no message from Master when EP did run successful and
             * received time-stamp from master but in the middle of running
             * communication with master clock is de-established.
             * An additional check apart from event flags wait time
             *
             */
            tx_timer_change(&g_announce_flag_rec, TIMER_EXPIRATION_TICKS, RESET_VALUE);
            tx_timer_activate(&g_announce_flag_rec);
        }
#endif
        tx_thread_sleep (1);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function initializes ptp driver, set mac_address, IP address, local clock id,
 *            and configure port for ptp instance.
 * @param[IN] None
 * @retval    FSP_SUCCESS or Any Other Error code apart from FSP_SUCCESS upon unsuccessful ptp initialization
 **********************************************************************************************************************/
static fsp_err_t ptp_init(void)
{
    fsp_err_t err = FSP_SUCCESS;
    UINT status   = NX_SUCCESS;
    uint32_t ip_addr = RESET_VALUE;
    ptp_message_flags_t flag_value = {RESET_VALUE};

#if PTP_MASTER_MODE
    const uint8_t local_clock_id[CLOCK_ID_LEN] = {0xDD, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77};
#else
    const uint8_t local_clock_id[CLOCK_ID_LEN] = {0xCC, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77};
#endif
    uint8_t mac_address[MAC_LEN] = { 0x01, 0x1B, 0x19, 0x00, 0x00, 0x00  };

    /* Initialize the network related services.*/
    status = network_init(&ip_addr);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("network_init function Failed.");
        return status;
    }
    PRINT_INFO_STR("Network Initialization completed successfully.");

    /* Initialize ptp driver.*/
    err = R_PTP_Open(&g_ptp0_ctrl, &g_ptp0_cfg);
    if(FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("R_PTP_Open API Failed.");
        return err;
    }

    /* Set the local clock id.*/
    err = R_PTP_LocalClockIdSet(&g_ptp0_ctrl, local_clock_id);
    if(FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("R_PTP_LocalClockIdSet API Failed.");
        ptp_deinit();
        return err;
    }

    /* Set the mac address.*/
    err = R_PTP_MacAddrSet(&g_ptp0_ctrl, mac_address);
    if(FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("R_PTP_MacAddrSet API Failed.");
        ptp_deinit();
        return err;
    }

    /* Set the IP address.*/
    err = R_PTP_IpAddrSet(&g_ptp0_ctrl, ip_addr);
    if(FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("R_PTP_IpAddrSet API Failed.");
        ptp_deinit();
        return err;
    }

#if PTP_MASTER_MODE
    /* Set the local clock value in master mode.*/
    set_local_clock_value();
#endif

    /* update the ptp message flag with default values.*/
    flag_value.value = RESET_VALUE;
    /* Set the PTP message flag.*/
    err = R_PTP_MessageFlagsSet(&g_ptp0_ctrl, PTP_MESSAGE_TYPE_ANNOUNCE, flag_value);
    if(FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("R_PTP_MessageFlagsSet API Failed.");
        ptp_deinit();
        return err;
    }

#if PTP_MASTER_MODE
    /* Configure the PTP instance to operate as a E2E Master.*/
    err = R_PTP_PortStateSet(&g_ptp0_ctrl, PTP_PORT_STATE_E2E_MASTER);
    if(FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("R_PTP_PortStateSet API Failed.");
        ptp_deinit();
        return err;
    }
    PRINT_INFO_STR("PTP Initialized successfully in master mode.");
#else
    /* Configure the PTP instance to operate as a E2E Slave.*/
    err = R_PTP_PortStateSet(&g_ptp0_ctrl, PTP_PORT_STATE_E2E_SLAVE);
    if(FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("R_PTP_PortStateSet API Failed.");
        ptp_deinit();
        return err;
    }
    PRINT_INFO_STR("PTP Initialized successfully in slave mode.");
#endif
    return err;
}

/*******************************************************************************************************************//**
 * @brief      This function initializes the NetX system, Packet pool, dhcpv4 client
 *             and prints client IP address
 * @param[IN]  None
 * @param[OUT] ip_add      Client IP address.
 * @retval     NX_SUCCESS or Any Other Error code apart from NX_SUCCESS upon unsuccessful network initialization
 **********************************************************************************************************************/
static UINT network_init(uint32_t * ip_addr)
{
    UINT status = NX_SUCCESS;
    /* Client IP address and Network Mask. */
    ULONG client_ip_address = RESET_VALUE;
    ULONG network_mask      = RESET_VALUE;

    /* Initialize NetX. */
    nx_system_initialize();

    /* Initialize Packet Pool. */
    status = packet_pool0_init();
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("packet_pool0_init function Failed.");
        return status;
    }

    /* Initialize IP. */
    status = ip0_init();
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("ip0_init function Failed.");
        return status;
    }

    /* Initialize DHCP v4 Client. */
    status = dhcpv4_client_init();
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("dhcpv4_client_init function Failed.");
        return status;
    }

    /* Retrieve IP address and network mask.*/
    status = nx_ip_address_get (&g_ip0, &client_ip_address, &network_mask);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_ip_address_get API Failed.");
        return status;
    }
    else
    {
        *ip_addr = (uint32_t)client_ip_address;
        /* Print the client IP address.*/
        app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_CLIENT_IP, sizeof(ULONG *), &client_ip_address);
    }
    return status;
}

/*******************************************************************************************************************//**
 * @brief     This function creates the packet pool.
 * @param[IN] None
 * @retval    None
 **********************************************************************************************************************/
static UINT packet_pool0_init(void)
{
    /* To capture the status(Success/Failure) of each Function/API. */
    UINT status = NX_SUCCESS;

    /* Create the packet pool. */
    status = nx_packet_pool_create(&g_packet_pool0,
                                   "g_packet_pool0 Packet Pool",
                                   G_PACKET_POOL0_PACKET_SIZE,
                                   &g_packet_pool0_pool_memory[0],
                                   G_PACKET_POOL0_PACKET_NUM * (G_PACKET_POOL0_PACKET_SIZE + sizeof(NX_PACKET)));

    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_packet_pool_create API Failed.");
    }
    return status;
}

/*******************************************************************************************************************//**
 * @brief     This function creates the ip instance and enables the ARP, UPD, ICMP.
 * @param[IN] None
 * @retval    None
 **********************************************************************************************************************/
static UINT ip0_init(void)
{
    /* To capture the status(Success/Failure) of each Function/API. */
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
        PRINT_ERR_STR("nx_ip_create API Failed.");
        return status;
    }

    /* Enable the ARP */
    status = nx_arp_enable(&g_ip0, &g_ip0_arp_cache_memory[RESET_VALUE], G_IP0_ARP_CACHE_SIZE);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_arp_enable API Failed.");
        return status;
    }

    /* Enable the UDP */
    status = nx_udp_enable(&g_ip0);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_udp_enable API Failed.");
        return status;
    }

    /* Enable the ping using Internet Control Message Protocol */
    status = nx_icmp_enable(&g_ip0);
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_icmp_enable API Failed.");
        return status;
    }
    PRINT_INFO_STR("Checking for Ethernet link...");

    /* Check the connection status. */
    ULONG current_state = RESET_VALUE;
    status = nx_ip_status_check(&g_ip0, NX_IP_LINK_ENABLED, &current_state, LINK_ENABLE_WAIT_TIME);
    if(NX_IP_LINK_ENABLED != current_state)
    {
        PRINT_ERR_STR("Ethernet Link is not powered up within the timeout specified.");
    }
    else if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_ip_status_check API Failed.");
    }
    else
    {
        PRINT_INFO_STR("Ethernet Link is Up!");
    }
    return status;
}

/*******************************************************************************************************************//**
 * @brief      This function initializes the dhcpv4 client by creating the dhcp client instance
 *             and allocates required the packet pool for it. DHCP server assigns the client ip
 *             address after server starts running successfully.
 * @param[IN]  None
 * @retval     None
 **********************************************************************************************************************/
static UINT dhcpv4_client_init(void)
{
    /* To capture the status(Success/Failure) of each Function/API. */
    UINT  status                         = NX_SUCCESS;
    ULONG requested_status               = RESET_VALUE;

    /* Create the DHCP instance. */
    status = nx_dhcp_create (&g_dhcp_client0, &g_ip0, "g_dhcp_client0");
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_dhcp_create API Failed.");
        return status;
    }

    /* Set the DHCP Packet Pool */
    status = nx_dhcp_packet_pool_set (&g_dhcp_client0, &g_packet_pool0);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_dhcp_packet_pool_set API Failed.");
        return status;
    }

    /* Start DHCP service. */
    status = nx_dhcp_start (&g_dhcp_client0);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_dhcp_start API Failed.");
        return status;
    }

    /* Wait until an IP address is acquired via DHCP. */
    status = nx_ip_status_check (&g_ip0, NX_IP_ADDRESS_RESOLVED, &requested_status, LINK_ENABLE_WAIT_TIME);
    if (NX_IP_ADDRESS_RESOLVED != requested_status)
    {
        PRINT_ERR_STR("Error in acquiring the IP address.");
        return status;
    }
    PRINT_INFO_STR("Acquired the IP address via DHCP successfully!");
    return status;
}

/*******************************************************************************************************************//**
 * @brief     This function is used for notifying ptp events.
 * @param[IN] p_args  callback arguments.
 * @retval    None
 **********************************************************************************************************************/
void ptp_user_cb(ptp_callback_args_t *p_args)
{
#if !PTP_MASTER_MODE

    switch (p_args->event)
    {
        case PTP_EVENT_SYNC_ACQUIRED:
        case PTP_EVENT_OFFSET_FROM_MASTER_UPDATED:
        {
            /* Set the event flag. */
            tx_event_flags_set(&g_ptp_event_flags0, PTP_EVENT, TX_OR);
            break;
        }
        case PTP_EVENT_MESSAGE_RECEIVED:
        {
            switch (p_args->p_message->header.message_type)
            {
                case PTP_MESSAGE_TYPE_ANNOUNCE:
                {
                    /* deactivate the timer.*/
                    tx_timer_deactivate(&g_announce_flag_rec);
                    if(g_port_id != p_args->p_message->header.source_port_id)
                    {
                        /* update source port id and set the master clock id.*/
                        g_port_id = p_args->p_message->header.source_port_id;
                        R_PTP_MasterClockIdSet(&g_ptp0_ctrl,p_args->p_message->announce.clock_id, p_args->p_message->header.source_port_id);
                    }
                    break;
                }
                default:
                {
                    /* do nothing. */
                }
            }
            break;
        }
        default:
        {
            /* do nothing.*/
        }
    }
#else
    FSP_PARAMETER_NOT_USED(p_args);
#endif
 }

#if PTP_MASTER_MODE
/*******************************************************************************************************************//**
 * @brief     This function set the local clock value.
 * @param[IN] None
 * @retval    None
 **********************************************************************************************************************/
static void set_local_clock_value(void)
{
    fsp_err_t err = FSP_SUCCESS;
    ptp_time_t local_time = {RESET_VALUE};

    uint32_t seconds = (uint32_t)time(NULL);
    /* update local clock values. */
    local_time.seconds_lower = seconds;
    local_time.seconds_upper = RESET_VALUE;
    local_time.nanoseconds   = RESET_VALUE;

    /* set the local clock value.*/
    err = R_PTP_LocalClockValueSet(&g_ptp0_ctrl,&local_time);
    if(FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("R_PTP_LocalClockValueSet API failed.");
        ptp_deinit();
        ERROR_TRAP(err);
    }
    PRINT_INFO_STR("PTP local clock set successfully.");
}
#else
/*******************************************************************************************************************//**
 * @brief     This function get the local clock value and display the time-stamp.
 * @param[IN] None
 * @retval    None
 **********************************************************************************************************************/
static void get_and_display_time_update(void)
{
    fsp_err_t err = FSP_SUCCESS;
    ptp_time_t recv_time = {RESET_VALUE};
    local_ptp_time_t disp_time  = {RESET_VALUE};
    time_t     epoch_val        = (time_t)RESET_VALUE;

    /* get the local clock value.*/
    err = R_PTP_LocalClockValueGet(&g_ptp0_ctrl, &recv_time);
    if(FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("R_PTP_LocalClockValueGet API failed.");
        ptp_deinit();
        ERROR_TRAP(err);
    }
    /* update seconds and nanosecond value to local time*/
    epoch_val = (((time_t)recv_time.seconds_upper << SHIFT_BIT) | recv_time.seconds_lower);
    disp_time.tm_nsec = recv_time.nanoseconds;

    /* update epoch value as per time zone. */
    epoch_val += TIME_ZONE_OFFSET;

    /* Convert the epoch value to user readable format.*/
    disp_time.local_time = *localtime(&epoch_val);

    /* Modify the month and year in user readable format */
    month_year_readability_update(&(disp_time.local_time));

    /* Print the current local Date and Time in user readable format */
    PRINT_INFO_STR("Acquired current date and time successfully, Current timestamp info is :");
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_DATE_TIME, sizeof(local_ptp_time_t), &disp_time);
}

/*******************************************************************************************************************//**
 * @brief     This function handles the timeout, if master clock is not available on network.
 * @param[IN] value  timeout value.
 * @retval    None
 **********************************************************************************************************************/
static VOID timeout_handler(ULONG value)
{
    FSP_PARAMETER_NOT_USED(value);

    APP_PRINT("ERROR: Time out occurred | NO communication from Master clock "
            "\r\n please run the command on Raspberry pi Linux: ptp4l -m -s -i <eth interface> \r\n"
            "or use another ek_ra6m3 as ptp master if raspberry pi is unavailable");

}

/*******************************************************************************************************************//**
 * @brief       This function modifies the month and year in readable format to the user.
 * @param[IN]   time                    month and year to be modified
 * @retval      None
 **********************************************************************************************************************/
static void month_year_readability_update(struct tm * time)
{
    time->tm_mon  +=  MON_ADJUST_VALUE;
    time->tm_year +=  YEAR_ADJUST_VALUE;
}
#endif

/*******************************************************************************************************************//**
 * @brief     This function closes the PTP module.
 * @param[IN] None
 * @retval    None
 **********************************************************************************************************************/
static void ptp_deinit(void)
{
    fsp_err_t err   = FSP_SUCCESS;
    err = R_PTP_Close(&g_ptp0_ctrl);
    if(FSP_SUCCESS != err)
    {
        PRINT_ERR_STR("R_PTP_Close API Failed.");
    }
}

/*******************************************************************************************************************//**
 * @} (end addtogroup ptp_ep)
 **********************************************************************************************************************/
