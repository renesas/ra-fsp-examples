/***********************************************************************************************************************
 * File Name    : ftp_server_thread_entry.c
 * Description  : Contains macros data structures and functions used in ftp_server_thread_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "ftp_server_thread.h"
#include "ftp_server_ep.h"

#define G_FX_MEDIA0_FORMAT_MEDIA  (1)

/* IP instance */
NX_IP g_ip0;

/* FileX Media Instance */
FX_MEDIA g_fx_media0;

uint8_t g_fx_media0_media_memory[G_FX_MEDIA0_MEDIA_MEMORY_SIZE];

/* Stack memory for g_ip0 */
uint8_t g_ip0_stack_memory[G_IP0_TASK_STACK_SIZE] BSP_PLACE_IN_SECTION(".stack.g_ip0")\
                                                  BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);

/* ARP cache memory for g_ip0 */
uint8_t g_ip0_arp_cache_memory[G_IP0_ARP_CACHE_SIZE] BSP_ALIGN_VARIABLE(4);

/* FTP Server instance */
NX_FTP_SERVER g_ftp_server0;
uint8_t g_ftp_server0_stack_memory[G_FTP_SERVER0_STACK_SIZE] BSP_PLACE_IN_SECTION(".stack.g_ftp_server0")\
                                                             BSP_ALIGN_VARIABLE(BSP_STACK_ALIGNMENT);

/* Packet pool instance (If this is a Trustzone part, the memory must be placed in Non-secure memory) */
NX_PACKET_POOL g_packet_pool0;
uint8_t g_packet_pool0_pool_memory[G_PACKET_POOL0_PACKET_NUM * (G_PACKET_POOL0_PACKET_SIZE + sizeof(NX_PACKET))]\
        BSP_ALIGN_VARIABLE(4);

/* User defined global variable */
static UCHAR scratch_memeory_region[SCRATCH_MEM_SIZE];

/* Function declarations */
static fsp_err_t FileX_init(void);
static void network_init(void);
static UINT ftp_serverLoginHandler(struct NX_FTP_SERVER_STRUCT *ftp_server_ptr, NXD_ADDRESS * client_ip_address,
                                   UINT client_port, CHAR *name, CHAR *password, CHAR *extra_info);
static UINT ftp_serverLogoutHandler(struct NX_FTP_SERVER_STRUCT *ftp_server_ptr, NXD_ADDRESS * client_ip_address,
                                    UINT client_port, CHAR *name, CHAR *password, CHAR *extra_info);
static void g_fx_media0_quick_setup();
static UINT read_and_display_FileContent(CHAR *file_name);

/* ftp_server_thread entry function */
void ftp_server_thread_entry(void)
{
    UINT err = RESET_VALUE;
    CHAR rtt_ip_data[IP_MSG_MAX_SIZE] = {'\0'};
    ULONG client_logging_event = RESET_VALUE;

    /* Initialize the RTT Thread */
    rtt_thread_init_check();

    /* Print the banner and EP info */
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_BANNER, RESET_VALUE, NULL);

    /* Initialize FILEX module for storage on FTP Server */
    err = FileX_init();
    HANDLE_API_FAILURE_IF_FAIL(err, FX_SUCCESS, "FileX_init Failed\r\n");

    /* Initialize network set up for FTP Server */
    network_init();

    /* Create FTP Server */
    g_ftp_server0_quick_setup();

    /* Start FTP Server */
    err = nx_ftp_server_start(&g_ftp_server0);
    HANDLE_API_FAILURE_IF_FAIL(err, NX_SUCCESS, "nx_ftp_server_start failed");

    /* Alert display message on RTT viewer for the user to perform FTP related operations */
    PRINT_INFO_STR("FTP server started and ready to use !!\r\n");

    app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_MENU, RESET_VALUE, NULL);

    while (true)
    {
        /* Check for user input to process */
        err = check_for_rtt_user_ip(&rtt_ip_data[RESET_VALUE]);

        if (TX_SUCCESS == err)
        {
            if ((MIN_FILE_NAME_LENGTH < (strlen(rtt_ip_data))) && (FX_MAX_SHORT_NAME_LEN >= (strlen(rtt_ip_data))))
            {
                /* Filter file length for \r or \n and pass file name to process */
                err = read_and_display_FileContent(rtt_ip_data);
                HANDLE_API_FAILURE_IF_FAIL(err, FX_SUCCESS, "Reading file failed, please check file name\r\n");
                memset(rtt_ip_data, '\0', sizeof(rtt_ip_data));
            }
            else
            {
                PRINT_ERR_STR("Please provide valid file name\r\n");
            }
        }

        /* Process further for logged in client */
        tx_event_flags_get(&client_event, LOGIN_EVENT_FLAG, TX_OR, &client_logging_event, 1U);

        if (LOGIN_EVENT_FLAG == client_logging_event)
        {
            client_logging_event = RESET_VALUE;
            app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_FTP_BYTE_SENT, sizeof(ULONG),
                               &g_ftp_server0.nx_ftp_server_total_bytes_sent);
            app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_FTP_BYTE_RECV, sizeof(ULONG),
                               &g_ftp_server0.nx_ftp_server_total_bytes_received);
        }

        tx_thread_sleep(1);
    }
}

/*******************************************************************************************************************//**
 *  @brief       This function initializes network.
 *  @param[in]   None.
 *  @retval      None.
 **********************************************************************************************************************/
static void network_init(void)
{
    /* Primary initialization */
    nx_system_initialize();

    /* Initialize packet pool */
    g_packet_pool0_quick_setup();

    /* IP initialization */
    g_ip0_quick_setup();
}


/*******************************************************************************************************************//**
 *  @brief       This function handles the FTP client login event.
 *  @param[in]   ftp_server_ptr     Pointer to the FTP server instance.
 *  @param[in]   client_ip_address  Pointer to the structure containing the client's IP address.
 *  @param[in]   client_port        Port number used by the client.
 *  @param[in]   name               Pointer to the username (not used).
 *  @param[in]   password           Pointer to the password (not used).
 *  @param[in]   extra_info         Pointer to additional information (not used).
 *  @retval      NX_SUCCESS         Upon successful.
 *  @retval      Any other error code apart from NX_SUCCESS on unsuccessful operation.
 **********************************************************************************************************************/
static UINT ftp_serverLoginHandler(struct NX_FTP_SERVER_STRUCT *ftp_server_ptr, NXD_ADDRESS * client_ip_address,
                                   UINT client_port, CHAR *name, CHAR *password, CHAR *extra_info)
{
    ULONG client_ip = client_ip_address->nxd_ip_address.v4;

    PRINT_INFO_STR("Client Logged IN");
    FSP_PARAMETER_NOT_USED(ftp_server_ptr);
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_CLIENT_IP, sizeof(ULONG *), &client_ip);
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_CLIENT_PORT, sizeof(UINT *), &client_port);
    FSP_PARAMETER_NOT_USED(name);
    FSP_PARAMETER_NOT_USED(password);
    FSP_PARAMETER_NOT_USED(extra_info);

    tx_event_flags_set(&client_event, LOGIN_EVENT_FLAG, TX_OR);

    return NX_SUCCESS;
}

/*******************************************************************************************************************//**
 *  @brief       This function handles the FTP client logout event.
 *  @param[in]   ftp_server_ptr     Pointer to the FTP server instance.
 *  @param[in]   client_ip_address  Pointer to the structure containing the client's IP address.
 *  @param[in]   client_port        Port number used by the client.
 *  @param[in]   name               Pointer to the username (not used).
 *  @param[in]   password           Pointer to the password (not used).
 *  @param[in]   extra_info         Pointer to additional information (not used).
 *  @retval      NX_SUCCESS         Upon successful.
 *  @retval      Any other error code apart from NX_SUCCESS on unsuccessful operation.
 **********************************************************************************************************************/
UINT ftp_serverLogoutHandler(struct NX_FTP_SERVER_STRUCT *ftp_server_ptr, NXD_ADDRESS * client_ip_address,
                             UINT client_port, CHAR *name, CHAR *password, CHAR *extra_info)
{
    ULONG client_ip = client_ip_address->nxd_ip_address.v4;

    PRINT_INFO_STR("Client Logged OUT");
    FSP_PARAMETER_NOT_USED(ftp_server_ptr);
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_PRINT_CLIENT_IP, sizeof(ULONG *), &client_ip);
    app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_CLIENT_PORT, sizeof(UINT *), &client_port);
    FSP_PARAMETER_NOT_USED(name);
    FSP_PARAMETER_NOT_USED(password);
    FSP_PARAMETER_NOT_USED(extra_info);

    ULONG event = RESET_VALUE;

    /* Clear the event set during client log-in */
    tx_event_flags_get(&client_event, LOGIN_EVENT_FLAG, TX_OR_CLEAR, &event, 1U);

    return NX_SUCCESS;
}

/* Quick setup for g_ftp_server0.
 * - g_ip0 must be setup before calling this function
 *     (See Developer Assistance -> g_ftp_server0 -> g_ip0 -> Quick Setup).
 * - g_packet_pool0 must be setup before calling this function
 *     (See Developer Assistance -> g_ftp_server0 -> g_packet_pool0 -> Quick Setup).
 * - g_fx_media0 must be setup before calling this function
 *     (See Developer Assistance -> g_ftp_server0 -> g_fx_media0 -> Quick Setup).
 * - nx_system_initialize() must be called before calling this function. */
/*******************************************************************************************************************//**
 *  @brief       This function performs quick setup for g_ftp_server0.
 *               g_ip0 must be setup before calling this function
 *               (See Developer Assistance -> g_ftp_server0 -> g_ip0 -> Quick Setup).
 *               g_packet_pool0 must be setup before calling this function
 *               (See Developer Assistance -> g_ftp_server0 -> g_packet_pool0 -> Quick Setup).
 *               g_fx_media0 must be setup before calling this function
 *               (See Developer Assistance -> g_ftp_server0 -> g_fx_media0 -> Quick Setup).
 *               nx_system_initialize() must be called before calling this function.
 *  @param[in]   None.
 *  @retval      None.
 **********************************************************************************************************************/
void g_ftp_server0_quick_setup()
{
    /* Create the FTP Server instance */
    UINT status = nxd_ftp_server_create(&g_ftp_server0,
                                        "g_ftp_server0",
                                        &g_ip0,
                                        &g_fx_media0,
                                        g_ftp_server0_stack_memory,
                                        G_FTP_SERVER0_STACK_SIZE,
                                        &g_packet_pool0,
                                        ftp_serverLoginHandler,
                                        ftp_serverLogoutHandler);

    HANDLE_API_FAILURE_IF_FAIL(status, NX_SUCCESS, "nxd_ftp_server_create Failed\r\n");
}

/*******************************************************************************************************************//**
 *  @brief       This function initializes FileX services.
 *  @param[in]   None.
 *  @retval      FX_SUCCESS or any other possible error codes.
 **********************************************************************************************************************/
static fsp_err_t FileX_init(void)
{
    UINT err = RESET_VALUE;
    ULONG actual_Rbyte = RESET_VALUE;
    FX_FILE myFile;
    uint8_t file_write_data[SAMPLE_DATA_SIZE] = SAMPLE_DATA_BYTES;
    uint8_t file_read_data[SAMPLE_DATA_SIZE] = {RESET_VALUE};

    /* Initialize FileX */
    fx_system_initialize();

    /* Open media driver */
    err = RM_FILEX_BLOCK_MEDIA_Open(&g_rm_filex_block_media_0_ctrl, &g_rm_filex_block_media_0_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Set up media */
    g_fx_media0_quick_setup();

    /* Below code will check for storage working before
     * it is utilized by FTP server for storage.
     * here one file operation is done
     * to ensure that FTP server storage device is
     * working */
    /* Create file */
    err = fx_file_create(&g_fx_media0, "Epftp.txt");
    if (FX_SUCCESS != err)
    {
        return err;
    }

    /* Open file on write mode */
    err = fx_file_open(&g_fx_media0, &myFile, "Epftp.txt", FX_OPEN_FOR_WRITE);
    if (FX_SUCCESS != err)
    {
        return err;
    }

    /* Write sample data */
    err = fx_file_write(&myFile, file_write_data, SAMPLE_DATA_SIZE);
    if (FX_SUCCESS != err)
    {
        return err;
    }

    /* Close file */
    err = fx_file_close(&myFile);
    if (FX_SUCCESS != err)
    {
        return err;
    }

    /* Read back for write verify */
    err = fx_file_open(&g_fx_media0, &myFile, "Epftp.txt", FX_OPEN_FOR_READ);
    if (FX_SUCCESS != err)
    {
        return err;
    }

    /* Read data */
    err = fx_file_read(&myFile, file_read_data, SAMPLE_DATA_SIZE, &actual_Rbyte);
    if (FX_SUCCESS != err)
    {
        return err;
    }

    /* Close file */
    err = fx_file_close(&myFile);
    if (FX_SUCCESS != err)
    {
        return err;
    }

    /* Compare read data with written data */
    err = (UINT)memcmp(file_write_data, file_read_data, SAMPLE_DATA_SIZE);

    return err;
}

/*******************************************************************************************************************//**
 *  @brief       This function performs quick Setup for g_packet_pool0.
 *               nx_system_initialize() must be called before calling this function.
 *  @param[in]   None.
 *  @retval      None.
 **********************************************************************************************************************/
void g_packet_pool0_quick_setup()
{
    /* Create the packet pool */
    UINT status = nx_packet_pool_create(&g_packet_pool0,
                                        "g_packet_pool0 Packet Pool",
                                        G_PACKET_POOL0_PACKET_SIZE,
                                        &g_packet_pool0_pool_memory[0],
                                        G_PACKET_POOL0_PACKET_NUM * (G_PACKET_POOL0_PACKET_SIZE + sizeof(NX_PACKET)));

    HANDLE_API_FAILURE_IF_FAIL(status, NX_SUCCESS, "nx_packet_pool_create Failed\r\n");
}

/*******************************************************************************************************************//**
 *  @brief       Callback function.
 *  @param[in]   p_args     Callback arguments.
 *  @retval      None.
 **********************************************************************************************************************/
void g_rm_filex_block_media_0_callback(rm_filex_block_media_callback_args_t *p_args)
{
    /* This EP version uses on board SPI for storage
     * This CB function will be used when removable
     * USB flash drive will be used to check for it removal in current EP we are checking on fileX write read
     * operation during initialization of FILEX media to check for QSPI media storage working check */
    FSP_PARAMETER_NOT_USED(p_args);
}

/*******************************************************************************************************************//**
 *  @brief       This function performs quick setup for g_fx_media0.
 *               g_rm_filex_block_media_0 must be setup before calling this function
 *               (See Developer Assistance -> g_fx_media0 -> g_rm_filex_block_media_0 -> Quick Setup).
 *               fx_system_initialize() must be called before calling this function.
 *  @param[in]   None.
 *  @retval      None.
 **********************************************************************************************************************/
static void g_fx_media0_quick_setup()
{
    UINT fx_ret_val;
    ULONG err_detection=RESET_VALUE;

    /* FSP does initialize QSPI controller on the MCU side but
     * does not initialize QSPI flash device when it is configured as FileX Block Media.
     * hence function is called here to initialize the flash device
     * before any FileX operation begins */
    ospi_b_init();

    /* Open the media */
    fx_ret_val = fx_media_open(&g_fx_media0,
                               "&g_fx_media0",
                               RM_FILEX_BLOCK_MEDIA_BlockDriver,
                               (void *) &g_rm_filex_block_media_0_instance,
                               g_fx_media0_media_memory,
                               G_FX_MEDIA0_MEDIA_MEMORY_SIZE);

    /* Check for FileX media device compatibility
     * for example - if "FAT" is not created or any other
     * file system errors are present This API returns an error and
     * media device will be formatted before further usage
     * As per Azure documentation and function definition
     * media must be opened before performing media check */
    fx_ret_val = fx_media_check(&g_fx_media0, scratch_memeory_region, SCRATCH_MEM_SIZE,
                                (FX_FAT_CHAIN_ERROR | FX_DIRECTORY_ERROR | FX_LOST_CLUSTER_ERROR | FX_FILE_SIZE_ERROR),
                                &err_detection);

    if (FX_SUCCESS != fx_ret_val)
    {
        /* As per Azure FileX documentation
         * "fx_media_format" service must be called prior to opening the media."
         * hence closing the media and opening it back after formatting is successful" */
        fx_ret_val = fx_media_close(&g_fx_media0);

        /* Format the media */
#if G_FX_MEDIA0_FORMAT_MEDIA
        fx_ret_val = fx_media_format(&g_fx_media0,                      /* Pointer to FileX media control block */
                                     RM_FILEX_BLOCK_MEDIA_BlockDriver,  /* Driver entry */
                                     (void *) &g_rm_filex_block_media_0_instance, /* Pointer to Block Media Driver */
                                     g_fx_media0_media_memory,                    /* Media buffer pointer */
                                     G_FX_MEDIA0_MEDIA_MEMORY_SIZE,     /* Media buffer size */
                                     (char *) G_FX_MEDIA0_VOLUME_NAME,  /* Volume name */
                                     G_FX_MEDIA0_NUMBER_OF_FATS,        /* Number of FATs */
                                     G_FX_MEDIA0_DIRECTORY_ENTRIES,     /* Directory entries */
                                     G_FX_MEDIA0_HIDDEN_SECTORS,        /* Hidden sectors */
                                     G_FX_MEDIA0_TOTAL_SECTORS,         /* Total sectors */
                                     G_FX_MEDIA0_BYTES_PER_SECTOR,      /* Sector size */
                                     G_FX_MEDIA0_SECTORS_PER_CLUSTER,   /* Sectors per cluster */
                                     1,                                 /* Heads (disk media) */
                                     1);                                /* Sectors per track (disk media) */

        HANDLE_API_FAILURE_IF_FAIL(fx_ret_val, FX_SUCCESS, "fx_media_format Failed\r\n");
#endif

        /* Open the media */
        fx_ret_val = fx_media_open(&g_fx_media0,
                                   "&g_fx_media0",
                                   RM_FILEX_BLOCK_MEDIA_BlockDriver,
                                   (void *) &g_rm_filex_block_media_0_instance,
                                   g_fx_media0_media_memory,
                                   G_FX_MEDIA0_MEDIA_MEMORY_SIZE);

        HANDLE_API_FAILURE_IF_FAIL(fx_ret_val, FX_SUCCESS, "fx_media_open Failed\r\n");
    }
}

/*******************************************************************************************************************//**
 *  @brief       This function performs quick setup for g_ip0.
 *               g_packet_pool0 must be setup before calling this function
 *               (See Developer Assistance -> g_ip0 -> g_packet_pool0 -> Quick Setup).
 *               nx_system_initialize() must be called before calling this function.
 *  @param[in]   None.
 *  @retval      None.
 **********************************************************************************************************************/
void  g_ip0_quick_setup()
{
    UINT status = NX_SUCCESS;

    /* Create the IP instance */
    status = nx_ip_create(&g_ip0,
                          "g_ip0 IP Instance",
                          G_IP0_ADDRESS,
                          G_IP0_SUBNET_MASK,
                          &g_packet_pool0,
                          g_netxduo_ether_0,
                          &g_ip0_stack_memory[0],
                          G_IP0_TASK_STACK_SIZE,
                          G_IP0_TASK_PRIORITY);

    HANDLE_API_FAILURE_IF_FAIL(status, NX_SUCCESS, "nx_ip_create Failed\r\n");

    /* Set the gateway address if it is configured */
    if (IP_ADDRESS(0, 0, 0, 0) != G_IP0_GATEWAY_ADDRESS)
    {
        status = nx_ip_gateway_address_set(&g_ip0, G_IP0_GATEWAY_ADDRESS);
        HANDLE_API_FAILURE_IF_FAIL(status, NX_SUCCESS, "nx_ip_gateway_address_set Failed\r\n");
    }

    status = nx_arp_enable(&g_ip0, &g_ip0_arp_cache_memory[0], G_IP0_ARP_CACHE_SIZE);
    HANDLE_API_FAILURE_IF_FAIL(status, NX_SUCCESS, "nx_arp_enable Failed\r\n");

    status = nx_tcp_enable(&g_ip0);
    HANDLE_API_FAILURE_IF_FAIL(status, NX_SUCCESS, "nx_tcp_enable Failed\r\n");

    status = nx_icmp_enable(&g_ip0);
    HANDLE_API_FAILURE_IF_FAIL(status, NX_SUCCESS, "nx_icmp_enable Failed\r\n");

    status = nx_ip_fragment_enable(&g_ip0);
    HANDLE_API_FAILURE_IF_FAIL(status, NX_SUCCESS, "nx_ip_fragment_enable Failed\r\n");

    ULONG current_state = RESET_VALUE;

    /* API check for repeated retries internally on wait option (last ARGS) provided */
    status = nx_ip_status_check(&g_ip0, NX_IP_LINK_ENABLED, &current_state, WAIT_OPTION_NETWORK_RETRY);

    /* If needed_status equal states current_status then only API is successful
     * hence no additional validation for current status comparison
     * with NX_IP_ENABLED */
    HANDLE_API_FAILURE_IF_FAIL(status, NX_SUCCESS, "nx_ip_status_check Failed\r\n");
}

/*******************************************************************************************************************//**
 *  @brief       This function reads file from storage device.
 *  @param[in]   file_name  File name.
 *  @retval      FX_SUCCESS or any other possible error codes.
 **********************************************************************************************************************/
static UINT read_and_display_FileContent(CHAR *file_name)
{
    UINT err = FX_SUCCESS;
    FX_FILE myFile;
    uint8_t file_read_data[DEFAULT_SIZE] = {0};
    ULONG actual_Rbyte = RESET_VALUE;

    err = fx_file_open(&g_fx_media0, &myFile, file_name, FX_OPEN_FOR_READ);
    if (FX_SUCCESS != err)
    {
        return err;
    }

    err = fx_file_read(&myFile, file_read_data, DEFAULT_SIZE, &actual_Rbyte);
    if (FX_SUCCESS != err)
    {
        return err;
    }
    err = fx_file_close(&myFile);
    if (FX_SUCCESS != err)
    {
        return err;
    }

    PRINT_INFO_STR("file content is");
    PRINT_INFO_STR(file_read_data);

    return FX_SUCCESS;
}
