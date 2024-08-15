/***********************************************************************************************************************
 * File Name    : ftp_client_thread_entry.c
 * Description  : Contains macros and functions used in ftp_client_thread_entry.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "netx_duo_ftp_client_ep.h"

/*******************************************************************************************************************//**
 * @addtogroup ftp_client_ep
 * @{
 **********************************************************************************************************************/

/******************************************************************************
Macro definitions
 ******************************************************************************/

/******************************************************************************
 Exported global functions (to be accessed by other files)
 ******************************************************************************/

/******************************************************************************
 Exported global variables
 ******************************************************************************/

/******************************************************************************
 Private global variables and functions
 ******************************************************************************/
/* Initialize ftp client instance */
static void ftp_client_init(void);
/* Initialize packet pool instance */
static void packet_pool_init(void);
/*Initialize the NetX System */
static void nx_common_init(void);
/* Function to 'create' a file */
static UINT create_ftp_file(ULONG file_size);
/* Function to initialize network */
static void ip_init(void);

/* IP instance */
NX_IP g_ip0;
/* Stack memory for g_ip0. */
uint8_t g_ip0_stack_memory[G_IP0_TASK_STACK_SIZE] BSP_PLACE_IN_SECTION(".stack.g_ip0") BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);
/* ARP cache memory for g_ip0. */
uint8_t g_ip0_arp_cache_memory[G_IP0_ARP_CACHE_SIZE] BSP_ALIGN_VARIABLE(4);

/* FTP Client instance. */
NX_FTP_CLIENT g_ftp_client0;
uint8_t g_ftp_client0_stack_memory[G_FTP_CLIENT0_WINDOW_SIZE] BSP_PLACE_IN_SECTION(".stack.g_ftp_client0") BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);

/* Packet pool instance (If this is a Trustzone part, the memory must be placed in Non-secure memory). */
NX_PACKET_POOL g_packet_pool0;
uint8_t g_packet_pool0_pool_memory[G_PACKET_POOL0_PACKET_NUM * (G_PACKET_POOL0_PACKET_SIZE + sizeof(NX_PACKET))] BSP_ALIGN_VARIABLE(4) ETHER_BUFFER_PLACE_IN_SECTION;

/* Buffer for file data. */
UCHAR file_buffer[FILE_SIZE];

/* FTP Client Thread entry function */
void ftp_client_thread_entry(void)
{
    UINT  status = NX_SUCCESS;                      //to get the status from the function return value
    ULONG actual_status = RESET_VALUE;              //to check the actual status
    NXD_ADDRESS server_ip_address = {RESET_VALUE};  //server ip address

    /* Initialize the RTT Thread.*/
    rtt_thread_init_check();
    /* print the banner and EP info. */
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_BANNER, RESET_VALUE, NULL);

    /* Initialize Netx System */
    nx_common_init ();

    /* Initialize packet pool */
    packet_pool_init ();

    /* Create ip instance */
    ip_init ();

    /* Create ftp_client instance */
    ftp_client_init ();

    /* Wait for the network link to be enabled. */
    do
    {
        status = nx_ip_status_check (&g_ip0, NX_IP_LINK_ENABLED, &actual_status, NX_IP_PERIODIC_RATE);
    }
    while (NX_SUCCESS != status);
    PRINT_INFO_STR("Network Initialization completed successfully! ");


    /* Fill the file buffer with 'file data'. */
    status = create_ftp_file (FILE_SIZE);
    /* Check status */
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("Failed in creating FTP file");
        ERROR_TRAP(status);
    }

    PRINT_INFO_STR("File created successfully!");

     /* Set the NetX Duo FTP Server address and start the FTP session. */
    /* If using IPv4, put the IP address in the NXD_ADDRESS.v4 field. */
    server_ip_address.nxd_ip_address.v4 = SERVER_ADDRESS;
    server_ip_address.nxd_ip_version = NX_IP_VERSION_V4;

    /* Run the ftp client session to write and read the opened file */
    status = run_ftp_client_session (&g_ftp_client0, &g_packet_pool0, USERNAME, PASSWORD, &server_ip_address,
                                            FILE_NAME, &file_buffer[0], FILE_SIZE, FTP_TIMEOUT);

    /* Check status */
    if(NX_SUCCESS != status)
    {
        ERROR_TRAP(status);
    }

    /* Delete the FTP Client. */
    status = nx_ftp_client_delete(&g_ftp_client0);
    /* Check status */
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_ftp_client_delete api failed.");
        ERROR_TRAP(status);
    }

    PRINT_INFO_STR("FTP Client EP deleted...");
    PRINT_INFO_STR("FTP Client EP operation completed successfully!");
    while (1)
    {
        tx_thread_sleep (1);
    }
}

/*******************************************************************************************************************//**
 * @brief     Initializes the NetX system.
 * @param[IN] None
 * @retval    None
 **********************************************************************************************************************/
static void nx_common_init(void)
{
    /*Initialize the NetX System */
    nx_system_initialize ();
}

/*******************************************************************************************************************//**
 * @brief     Creates the packet pool.
 * @param[IN] None
 * @retval    None
 **********************************************************************************************************************/
static void packet_pool_init(void)
{
    UINT  status = NX_SUCCESS;                      //to get the status from the function return value
    /* Create the packet pool. */
    status = nx_packet_pool_create(&g_packet_pool0,
                "g_packet_pool0 Packet Pool",
                G_PACKET_POOL0_PACKET_SIZE,
                &g_packet_pool0_pool_memory[0],
                G_PACKET_POOL0_PACKET_NUM * (G_PACKET_POOL0_PACKET_SIZE + sizeof(NX_PACKET)));
    /* Check status */
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_packet_pool_create api failed.");
        ERROR_TRAP(status);
    }
}

/*******************************************************************************************************************//**
 * @brief     Creates the ip instance and enables ARP,TCP,ICMP.
 * @param[IN] None
 * @retval    None
 **********************************************************************************************************************/
static void ip_init(void)
{
    UINT  status = NX_SUCCESS;                      //to get the status from the function return value
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
    /* Check status */
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_ip_create api failed.");
        ERROR_TRAP(status);
    }

    /* Set the gateway address if it is configured. */
    if(IP_ADDRESS(0, 0, 0, 0) != G_IP0_GATEWAY_ADDRESS)
    {
        status = nx_ip_gateway_address_set(&g_ip0, G_IP0_GATEWAY_ADDRESS);
        /* Check status */
        if(NX_SUCCESS != status)
        {
            PRINT_ERR_STR("nx_ip_gateway_address_set api failed.");
            ERROR_TRAP(status);
        }
    }

    status = nx_arp_enable(&g_ip0, &g_ip0_arp_cache_memory[0], G_IP0_ARP_CACHE_SIZE);
    /* Check status */
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_arp_enable api failed.");
        ERROR_TRAP(status);
    }

    /* Enable NetX modules that are required by the application. */
     status = nx_tcp_enable(&g_ip0);
     /* Check status */
     if(NX_SUCCESS != status)
     {
         PRINT_ERR_STR("nx_tcp_enable api failed.");
         ERROR_TRAP(status);
     }

     status = nx_icmp_enable(&g_ip0);
     /* Check status */
     if(NX_SUCCESS != status)
     {
         PRINT_ERR_STR("nx_icmp_enable api failed.");
         ERROR_TRAP(status);
     }

     status = nx_ip_fragment_enable(&g_ip0);
     /* Check status */
     if(NX_SUCCESS != status)
     {
         PRINT_ERR_STR("nx_ip_fragment_enable api failed.");
         ERROR_TRAP(status);
     }

    /* Wait for the link to be enabled. */
    status = nx_ip_status_check(&g_ip0, NX_IP_LINK_ENABLED, &current_state, NX_WAIT_FOREVER);
    /* Check status */
    if((NX_SUCCESS != status) && (NX_IP_LINK_ENABLED != current_state))
    {
        PRINT_ERR_STR("nx_ip_status_check api failed.");
        ERROR_TRAP(status);
    }
}


/*******************************************************************************************************************//**
 * @brief     Creates the ftp_client instance.
 * @param[IN] None
 * @retval    None
 **********************************************************************************************************************/
static void ftp_client_init(void)
{
    UINT  status = NX_SUCCESS;                      //to get the status from the function return value
    /* Create the FTP Client instance. */
    status = nx_ftp_client_create(&g_ftp_client0,
                        "g_ftp_client0",
                        &g_ip0,
                        G_FTP_CLIENT0_WINDOW_SIZE,
                        &g_packet_pool0);
    /* Check status */
    if(NX_SUCCESS != status)
    {
        PRINT_ERR_STR("FTP Client instance creation failed.");
        ERROR_TRAP(status);
    }
}


/*******************************************************************************************************************//**
 * @brief  This function creates a file of specified size. Data is not important, we just use this buffer to
 *         demonstrate transferring files to and from the server.
 *
 * @param[in]  file_size         Size of the file
 * @retval     NX_SECCESS        Upon successful filled buffer
 **********************************************************************************************************************/
static UINT create_ftp_file(ULONG file_size)
{

    UCHAR counter = RESET_VALUE;
    /* Filling the file buffer to the provided file size */
    for (UINT size = RESET_VALUE; size < file_size; size++)
    {
        file_buffer[size] = counter++;
    }

    return NX_SUCCESS;
}
/*******************************************************************************************************************//**
 * @} (end defgroup ftp_client_ep)
 **********************************************************************************************************************/


