/***********************************************************************************************************************
 * File Name    : freertos_network_http_ep.c
 * Description  : Contains macros, data structures and functions used in freertos_network_http_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "freertos_network_http_ep.h"

/* Global variable */
Socket_t s_client_socket = FREERTOS_INVALID_SOCKET;

/* Private variables */
static uint8_t s_write_data[BUFFER_SIZE] = {RESET_VALUE};
static FF_Disk_t s_disk;
static bool s_freertos_plus_fat_open_flag = false;
static bool s_freertos_plus_fat_init_flag = false;

/***********************************************************************************************************************
 * @brief      This function configures the remote socket address.
 * @param[in]  p_ipv4_address     Pointer to the IPv4 address array.
 * @param[in]  port               Remote port number.
 * @param[out] p_remote_address   Pointer to the remote socket address structure.
 * @retval     None.
 **********************************************************************************************************************/
void remote_address_configure(int * p_ipv4_address, int port, struct freertos_sockaddr * p_remote_address)
{
    memset(p_remote_address, RESET_VALUE, sizeof(*p_remote_address));

    p_remote_address->sin_port = FreeRTOS_htons((uint16_t)port);

    p_remote_address->sin_address.ulIP_IPv4 = FreeRTOS_inet_addr_quick((uint8_t)p_ipv4_address[0],
                                                                       (uint8_t)p_ipv4_address[1],
                                                                       (uint8_t)p_ipv4_address[2],
                                                                       (uint8_t)p_ipv4_address[3]);
    p_remote_address->sin_family = FREERTOS_AF_INET4;
}

/*******************************************************************************************************************//**
 * @brief     This function creates and configures a socket.
 * @param[in] socket_type               Socket type.
 * @param[in] protocol                  Socket protocol.
 * @retval    STATUS_SUCCESS            Created and configured successfully.
 * @retval    Any other error codes     Failed to create.
 **********************************************************************************************************************/
status_code_t socket_create(BaseType_t socket_type, BaseType_t protocol)
{
    TickType_t receive_timeout = pdMS_TO_TICKS(SOCKET_RECEIVE_TIMEOUT);
    TickType_t send_timeout    = pdMS_TO_TICKS(SOCKET_SEND_TIMEOUT);

    /* Create a new socket. */
    s_client_socket = FreeRTOS_socket(FREERTOS_AF_INET, socket_type, protocol);
    if (FREERTOS_INVALID_SOCKET == s_client_socket)
    {
        return STATUS_ERROR;
    }

    /* Configure the socket receive timeout. */
    FreeRTOS_setsockopt(s_client_socket, 0, FREERTOS_SO_RCVTIMEO, &receive_timeout, sizeof(receive_timeout));
    FreeRTOS_setsockopt(s_client_socket, 0, FREERTOS_SO_SNDTIMEO, &send_timeout, sizeof(send_timeout));

    return STATUS_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief     This function checks connectivity to the server by sending ping requests.
 * @param[in] remote_ip_address         Pointer to the server IPv4 address string.
 * @retval    PING_SUCCESS              Ping successfully.
 * @retval    Any other error codes     Failed to ping.
 **********************************************************************************************************************/
ping_status_t ping_server(char * remote_ip_address)
{
    ping_status_t ping_status = PING_SUCCESS;
    uint8_t retry_count = RESET_VALUE;

    while (retry_count < MAX_PING_RETRIES)
    {
        ping_status = send_ping_request(remote_ip_address);

        if (PING_SUCCESS == ping_status)
        {
            break;
        }

        retry_count++;
    }

    return ping_status;
}

/***********************************************************************************************************************
 * @brief     This function connects the TCP socket to the specified remote server.
 * @param[in] p_remote_address          Pointer to the remote socket address.
 * @retval    STATUS_SUCCESS            Connected successfully.
 * @retval    Any other error codes     Failed to connect.
 **********************************************************************************************************************/
status_code_t tcp_server_connect(struct freertos_sockaddr * p_remote_address)
{
    BaseType_t result = FreeRTOS_connect(s_client_socket, p_remote_address, sizeof(*p_remote_address));

    return (SUCCESS == result) ? STATUS_SUCCESS : STATUS_ERROR;
}

/***********************************************************************************************************************
 * @brief     This function shuts down the client TCP socket.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void tcp_socket_shutdown(void)
{
    FreeRTOS_shutdown(s_client_socket, FREERTOS_SHUT_RDWR);
}

/*******************************************************************************************************************//**
 * @brief     This function closes the client socket.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void socket_close(void)
{
    if (FREERTOS_INVALID_SOCKET != s_client_socket)
    {
        FreeRTOS_closesocket(s_client_socket);

        s_client_socket = FREERTOS_INVALID_SOCKET;
    }
}

/***********************************************************************************************************************
 * @brief     This function sends data through the specified TCP socket.
 * @param[in] None.
 * @retval    Number of bytes sent.
 **********************************************************************************************************************/
BaseType_t tcp_send(void)
{
    return FreeRTOS_send(s_client_socket, SEND_MESS_TCP, strlen(SEND_MESS_TCP), 0);
}

/***********************************************************************************************************************
 * @brief      This function receives data through the specified TCP socket.
 * @param[out] p_buffer                 Pointer to the receive buffer.
 * @param[in]  buffer_size              Size of the receive buffer.
 * @retval     Number of bytes received.
 **********************************************************************************************************************/
BaseType_t tcp_receive(char * p_buffer, size_t buffer_size)
{
    return FreeRTOS_recv(s_client_socket, p_buffer, buffer_size, 0);
}

/*******************************************************************************************************************//**
 * @brief     This function sends data to the specified UDP server.
 * @param[in] p_remote_address          Pointer to the remote UDP server address.
 * @retval    Number of bytes sent.
 **********************************************************************************************************************/
int32_t udp_send(struct freertos_sockaddr const * p_remote_address)
{
    return FreeRTOS_sendto(s_client_socket, SEND_MESS_UDP, strlen(SEND_MESS_UDP) + 1U, 0, p_remote_address,
                           sizeof(*p_remote_address));
}

/*******************************************************************************************************************//**
 * @brief      This function receives data from a UDP server.
 * @param[out] p_remote_address         Pointer to the source UDP server address.
 * @param[out] p_receive_buffer         Pointer to the receive buffer.
 * @param[in]  receive_buffer_size      Size of the receive buffer.
 * @retval     Number of bytes received.
 **********************************************************************************************************************/
int32_t udp_receive(struct freertos_sockaddr * p_remote_address, char * p_receive_buffer, size_t receive_buffer_size)
{
    uint32_t address_length = sizeof(*p_remote_address);

    return FreeRTOS_recvfrom(s_client_socket, p_receive_buffer, receive_buffer_size, 0,
                             p_remote_address, &address_length);
}

/***********************************************************************************************************************
 * @brief     This function initializes the FreeRTOS+FAT instance.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialization successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t freertos_fat_init(void)
{
    fsp_err_t err = FSP_SUCCESS;
    rm_freertos_plus_fat_device_t device;

    memset(&s_disk, 0, sizeof(s_disk));

    /* Open FreeRTOS PLUS FAT */
    err = RM_FREERTOS_PLUS_FAT_Open(&g_rm_freertos_plus_fat0_ctrl, &g_rm_freertos_plus_fat0_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    s_freertos_plus_fat_open_flag = true;

    /* Initialize the mass storage device. This should not be done until the device is plugged in and initialized */
    err = RM_FREERTOS_PLUS_FAT_MediaInit(&g_rm_freertos_plus_fat0_ctrl, &device);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Initialize one disk for each partition used in the application */
    err = RM_FREERTOS_PLUS_FAT_DiskInit(&g_rm_freertos_plus_fat0_ctrl, &g_rm_freertos_plus_fat0_disk_cfg, &s_disk);
    if (FSP_SUCCESS != err)
    {
        return err;
    }
    s_freertos_plus_fat_init_flag = true;

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief     This function mounts the disk and adds it to the file system.
 * @param[in] None.
 * @retval    STATUS_SUCCESS            Mounted and added successfully.
 * @retval    Any other error           Failed to mount and add.
 **********************************************************************************************************************/
status_code_t file_system_mount(void)
{
    FF_Stat_t file_details;
    FF_Error_t ff_err = FF_ERR_NONE;
    int err = pdFALSE;

    memset(&file_details, 0, sizeof(file_details));

    /* Mount each disk. This assumes the disk is already partitioned and formatted */
    ff_err = FF_Mount(&s_disk, s_disk.xStatus.bPartitionNumber);
    if (FF_ERR_NONE != ff_err)
    {
        return STATUS_ERROR;
    }

    /* Add the disk to the file system */
    err = FF_FS_Add("/", &s_disk);
    if (pdTRUE != err)
    {
        return STATUS_ERROR;
    }

    return STATUS_SUCCESS;
}

/***********************************************************************************************************************
 * @brief     This function writes data to a file utilizing the FreeRTOS+FAT file system.
 * @param[in] file_name                 Name of the written file.
 * @param[in] content                   Pointer to the data content written to the file.
 * @retval    STATUS_SUCCESS            Written successfully.
 * @retval    Any other error codes     Failed to write.
 **********************************************************************************************************************/
status_code_t write_file (const char * file_name, const char * content)
{
    int32_t file_error;
    FF_FILE * p_file_pointer = NULL;
    size_t bytes_written = RESET_VALUE;

    /* Open the file in write mode */
    p_file_pointer = ff_fopen(file_name, WRITE_MODE);

    if (NULL != p_file_pointer)
    {
        memset(s_write_data, RESET_VALUE, sizeof(s_write_data));

        /* Write content to buffer */
        snprintf((char*)s_write_data, sizeof(s_write_data), "%s", content);

        /* Write the data to the file and close it */
        bytes_written = ff_fwrite(s_write_data, sizeof(s_write_data[0]), strlen ((char*)s_write_data), p_file_pointer);

        if (strlen((char*)s_write_data) != bytes_written)
        {
            return STATUS_ERROR;
        }

        /* Close the file */
        file_error = ff_fclose(p_file_pointer);
        if (SUCCESS != file_error)
        {
            return STATUS_ERROR;
        }
    }
    else
    {
        return STATUS_ERROR;
    }

    return STATUS_SUCCESS;
}

/***********************************************************************************************************************
 * @brief      This function creates and configures the HTTP server.
 * @param[out] pp_tcp_server            Pointer to the created TCP server instance.
 * @retval     STATUS_SUCCESS           Created and configured successfully.
 * @retval     Any other error codes    Failed to create or configure.
 **********************************************************************************************************************/
status_code_t http_server_create(TCPServer_t ** pp_tcp_server)
{
    static const struct xSERVER_CONFIG server_configuration[] =
    {
        {
            eSERVER_HTTP,       /* Server type */
            SERVER_PORT,        /* Port number */
            SERVER_BACKLOG,     /* Backlog */
            HTTP_ROOT           /* Root directory */
        }
    };

    /* Create the HTTP server. */
    *pp_tcp_server = FreeRTOS_CreateTCPServer(server_configuration,
                                 ((sizeof(server_configuration)) / (sizeof(server_configuration[0]))));

    if (NULL == (*pp_tcp_server))
    {
        return STATUS_ERROR;
    }

    /* Enable reuse of the listening socket. */
    FreeRTOS_setsockopt((*pp_tcp_server)->xServers[0].xSocket, 0, FREERTOS_SO_REUSE_LISTEN_SOCKET,
                       (void *) pdTRUE, sizeof(pdTRUE));

    return STATUS_SUCCESS;
}

/***********************************************************************************************************************
 * @brief     This function runs the HTTP server and processes incoming connections.
 * @param[in] p_tcp_server              Pointer to the TCP server instance.
 * @retval    None.
 **********************************************************************************************************************/
void http_server_process(TCPServer_t * p_tcp_server)
{
    FreeRTOS_TCPServerWork(p_tcp_server, pdMS_TO_TICKS(5000UL));
}

/***********************************************************************************************************************
 * @brief     This function closes the HTTP server socket and deletes its socket set.
 * @param[in] p_tcp_server              Pointer to the TCP server instance.
 * @retval    None.
 **********************************************************************************************************************/
void http_server_close(TCPServer_t * p_tcp_server)
{
    freertos_fat_deinit();

    /* Close the HTTP server socket. */
    FreeRTOS_closesocket((Socket_t)p_tcp_server->xServers[0].xSocket);

    /* Delete the server socket set. */
    FreeRTOS_DeleteSocketSet(p_tcp_server->xSocketSet);
}

/***********************************************************************************************************************
 * @brief     This function closes the opened FreeRTOS+FAT instance.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void freertos_fat_deinit(void)
{
    if (s_freertos_plus_fat_init_flag)
    {
        RM_FREERTOS_PLUS_FAT_DiskDeinit(&g_rm_freertos_plus_fat0_ctrl, &s_disk);
        s_freertos_plus_fat_init_flag = false;
    }

    /* Close the FREERTOS_PLUS_FAT_Close instance on safely ejecting */
    if (s_freertos_plus_fat_open_flag)
    {
        RM_FREERTOS_PLUS_FAT_Close(&g_rm_freertos_plus_fat0_ctrl);
        s_freertos_plus_fat_open_flag = false;
    }
}

/*******************************************************************************************************************//**
 * @brief     This function toggles the state of the specified LED.
 * @param[in] led_num                   LED pin number.
 * @retval    None.
 **********************************************************************************************************************/
void toggleLED(bsp_io_port_pin_t led_num)
{
    bsp_io_level_t led_status;
    R_IOPORT_PinRead(&g_ioport_ctrl, led_num, &led_status);
    R_IOPORT_PinWrite(&g_ioport_ctrl, led_num, (led_status == BSP_IO_LEVEL_LOW)
                      ? BSP_IO_LEVEL_HIGH : BSP_IO_LEVEL_LOW);
}

/***********************************************************************************************************************
 * @brief     This function updates the LED status.
 * @param[in] None.
 * @retval    STATUS_SUCCESS            Updated successfully.
 * @retval    Any other error codes     Failed to update.
 **********************************************************************************************************************/
status_code_t update_status_leds(void)
{
    bsp_io_level_t led_state;
    char led_data[LED_BUFFER_SIZE];
    char *statuses[NUM_LEDS]; /* Array of statuses for the LEDs ("ON" or "OFF") */
    char *colors[NUM_LEDS];   /* Array of colors for the LEDs */

    for (uint32_t  i = 0; i < NUM_LEDS; i++)
    {
        R_IOPORT_PinRead(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[i], &led_state);

        if (BSP_IO_LEVEL_HIGH == led_state)
        {
            statuses[i] = "ON";
            colors[i] = LED_COLOR_ON;
        }
        else
        {
            statuses[i] = "OFF";
            colors[i] = LED_COLOR_OFF;
        }
    }

    /* Update the LED status information into the HTML content */
    snprintf(led_data, sizeof(led_data), INNER_HTML_LED_ID, colors[0], statuses[0], colors[1], statuses[1]);
    return write_file(LED_STATUS_FILE_NAME, led_data);
}
