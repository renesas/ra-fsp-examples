/***********************************************************************************************************************
 * File Name    : usr_app.c
 * Description  : This file contains functions used by the network application in net_thread_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_TCP_server.h"
#include "FreeRTOS_HTTP_commands.h"
#include "FreeRTOS_server_private.h"
#include "common_utils.h"
#include "usr_app.h"

extern uint8_t g_mac_address[MAC_ADDR_SIZE];
extern uint8_t g_ip_address[IP_ADDR_SIZE];
extern uint8_t g_netmask[NETMASK_SIZE];
extern uint8_t g_gateway_address[GATEWAY_ADDR_SIZE];

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/
static uint8_t g_write_data[BUFFER_SIZE] = { RESET_VALUE };
static FF_Disk_t g_disk;

/***********************************************************************************************************************
 * Private function prototypes
 **********************************************************************************************************************/
static void update_network_info(void);
static void toggleLED(bsp_io_port_pin_t ledNumber);
static fsp_err_t freertos_fat_init(void);
static status_code_t write_file(const char* fileName, const char *content);
static void update_status_leds(void);

/***********************************************************************************************************************
 * @brief       This function initializes and creates an HTTP server, handling incoming HTTP GET requests.
 * @param[in]   None.
 * @retval      STATUS_SUCCESS    HTTP server created and running successfully.
 * @retval      STATUS_ERROR      An error occurred during server setup.
 **********************************************************************************************************************/
status_code_t http_server_handle(void)
{
    fsp_err_t err                      = FSP_SUCCESS;
    BaseType_t xResult                 = pdFALSE;
    TCPServer_t *pxTCPServer           = NULL;
    const TickType_t xInitialBlockTime = pdMS_TO_TICKS(5000UL);
    static const struct xSERVER_CONFIG xServerConfiguration[] = {{eSERVER_HTTP,      /* Server type */
                                                                  SERVER_PORT,       /* Port number */
                                                                  SERVER_BACKLOG,    /* Backlog */
                                                                  HTTP_ROOT}};       /* Root dir */

    /* Initialize the FreeRTOS+FAT */
    err = freertos_fat_init();
    APP_ERR_RET(FSP_SUCCESS !=  err, (status_code_t) err, "freertos_fat_init failed.\r\n");

    /* Update IP configuration */
    update_network_info();

    /* Create the servers defined by the xServerConfiguration array above */
    pxTCPServer = FreeRTOS_CreateTCPServer(xServerConfiguration,
                                           sizeof(xServerConfiguration) / sizeof(xServerConfiguration[0]));
    APP_ERR_RET(NULL == pxTCPServer, (status_code_t) STATUS_ERROR, "Error: FreeRTOS TCP server creation failed.\r\n");

    APP_PRINT("\r\nTCP server created successfully.\r\n");

    /* Set the socket option for reusing the socket address */
    FreeRTOS_setsockopt(pxTCPServer->xServers[0].xSocket, 0, FREERTOS_SO_REUSE_LISTEN_SOCKET,
                        (void *) pdTRUE, sizeof(pdTRUE));

    APP_PRINT("\r\nPlease open a browser and enter \"%d.%d.%d.%d/index.html\" to go to the web page.\r\n",
              g_ip_address[0], g_ip_address[1], g_ip_address[2], g_ip_address[3]);

    APP_PRINT("\r\nPress any key (excluding Enter) to exit the HTTP server...\r\n");

    while (1)
    {
        /* Run the HTTP server */
        FreeRTOS_TCPServerWork(pxTCPServer, xInitialBlockTime);

        /* Check if the received TCP command is requesting the LED status update */
        if (RESET_VALUE == strncmp((char*) pxTCPServer->pcCommandBuffer, GET_LED_COMMAND, GET_LED_COMMAND_LENGTH))
        {
            /* Get the status of the LEDs */
            update_status_leds();
        }

        /* Check if the received TCP command is for toggling Switch 1 (GET/switch1) */
        if (RESET_VALUE == strncmp((char*) pxTCPServer->pcCommandBuffer,
                                   GET_SWITCH1_COMMAND, GET_SWITCH1_COMMAND_LENGTH))
        {
            /* Toggle the state of LED 1 (BLUE) */
            toggleLED(USER_LED1_BLUE);
            APP_PRINT("\r\nLED 1 (BLUE) state has been toggled.\r\n");
        }

        /* Check if the received TCP command is for toggling Switch 2 (GET/switch2) */
        if (RESET_VALUE == strncmp((char*) pxTCPServer->pcCommandBuffer,
                                   GET_SWITCH2_COMMAND, GET_SWITCH2_COMMAND_LENGTH))
        {
            /* Toggle the state of LED 2 (GREEN) */
            toggleLED(USER_LED2_GREEN);
            APP_PRINT("\r\nLED 2 (GREEN) state has been toggled.\r\n");
        }

        memset((char*) pxTCPServer->pcCommandBuffer, RESET_VALUE, ipconfigTCP_COMMAND_BUFFER_SIZE);

        /* Check if a key has been pressed */
        if (APP_CHECK_KEY)
        {
            /* De-initialize the FreeRTOS+FAT components */
            freertos_fat_deinit();

            /* Close the socket */
            xResult = FreeRTOS_closesocket((Socket_t) pxTCPServer->xServers[0].xSocket);
            APP_ERR_RET(pdTRUE !=  xResult, (status_code_t)xResult,
                                             "Failed to close the TCP socket.\r\n");

            FreeRTOS_DeleteSocketSet(pxTCPServer->xSocketSet);

            APP_PRINT("\r\nClosed the TCP socket successfully...\r\n");

            break;
        }

        vTaskDelay(1);
    }

    return STATUS_SUCCESS;
}

/***********************************************************************************************************************
 * @brief       This function toggles the state of the specified LED.
 * @param[in]   ledNumber   The pin number of the LED to toggle.
 * @retval      None.
 **********************************************************************************************************************/
static void toggleLED(bsp_io_port_pin_t ledNumber)
{
    bsp_io_level_t led_status;
    R_IOPORT_PinRead(&g_ioport_ctrl, ledNumber, &led_status);
    R_IOPORT_PinWrite(&g_ioport_ctrl, ledNumber, (led_status == BSP_IO_LEVEL_LOW)
                      ? BSP_IO_LEVEL_HIGH : BSP_IO_LEVEL_LOW);
}

/***********************************************************************************************************************
 * @brief       This function retrieves the status of each LED (e.g., ON/OFF and color) and updates the HTML content.
 * @param[in]   None.
 * @retval      None.
 **********************************************************************************************************************/
static void update_status_leds(void)
{
    bsp_io_level_t led_state;
    char led_data[LED_BUFFER_SIZE];
    bsp_io_port_pin_t ledNumbers[] = {USER_LED1_BLUE, USER_LED2_GREEN}; /* Array of pin numbers for the LEDs */
    char *statuses[NUM_LEDS]; /* Array of statuses for the LEDs ("ON" or "OFF") */
    char *colors[NUM_LEDS];   /* Array of colors for the LEDs */
    status_code_t write_status = STATUS_SUCCESS;

    for (int i = 0; i < NUM_LEDS; i++)
    {
        R_IOPORT_PinRead(&g_ioport_ctrl, ledNumbers[i], &led_state);

        if (led_state == BSP_IO_LEVEL_HIGH)
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
    write_status = write_file(LED_STATUS_FILE_NAME, led_data);
    if (STATUS_SUCCESS != write_status)
    {
        APP_ERR_PRINT("\r\nWrite to file failed\r\n");
        APP_ERR_TRAP(write_status);
    }
}

/***********************************************************************************************************************
 * @brief       This function initializes the FreeRTOS+FAT instance.
 * @param[in]   None.
 * @retval      FSP_SUCCESS     Initialization completed successfully.
 * @retval      Error Code      Specific error code indicating the reason for failure if initialization fails.
 **********************************************************************************************************************/
static fsp_err_t freertos_fat_init(void)
{
    fsp_err_t err = FSP_SUCCESS;
    FF_Stat_t file_details;
    FF_Error_t ff_err = FF_ERR_NONE;
    rm_freertos_plus_fat_device_t device;

    memset(&file_details, 0, sizeof(file_details));

    memset(&g_disk, 0, sizeof(g_disk));

    /* Open FreeRTOS PLUS FAT */
    err = RM_FREERTOS_PLUS_FAT_Open(&g_rm_freertos_plus_fat0_ctrl, &g_rm_freertos_plus_fat0_cfg);
    APP_ERR_RET(FSP_SUCCESS !=  err, err, "RM_FREERTOS_PLUS_FAT_Open API failed\r\n");

    /* Initialize the mass storage device. This should not be done until the device is plugged in and initialized */
    err = RM_FREERTOS_PLUS_FAT_MediaInit(&g_rm_freertos_plus_fat0_ctrl, &device);
    APP_ERR_RET(FSP_SUCCESS !=  err, err, "RM_FREERTOS_PLUS_FAT_MediaInit API failed\r\n");

    /* Initialize one disk for each partition used in the application */
    err = RM_FREERTOS_PLUS_FAT_DiskInit(&g_rm_freertos_plus_fat0_ctrl, &g_rm_freertos_plus_fat0_disk_cfg, &g_disk);
    APP_ERR_RET(FSP_SUCCESS !=  err, err, "RM_FREERTOS_PLUS_FAT_DiskInit API failed\r\n");

    /* Mount each disk. This assumes the disk is already partitioned and formatted */
    ff_err = FF_Mount(&g_disk, g_disk.xStatus.bPartitionNumber);
    APP_ERR_RET(FF_ERR_NONE !=  ff_err, (fsp_err_t) ff_err, "FF_Mount API failed\r\n");

    /* Add the disk to the file system */
    ff_err = FF_FS_Add("/", &g_disk);
    APP_ERR_RET(FF_ERR_NONE ==  ff_err, (fsp_err_t) ff_err, "FF_FS_Add API failed\r\n");

    return FSP_SUCCESS;
}

/***********************************************************************************************************************
 * @brief       This function updates the network information.
 * @param[in]   None.
 * @retval      None.
 **********************************************************************************************************************/
static void update_network_info(void)
{
    char ip_content_body[NETWORK_BUFFER_SIZE];
    status_code_t write_status = STATUS_SUCCESS;

    snprintf(ip_content_body, sizeof(ip_content_body), INNER_HTML_NET_ID, g_mac_address[0],
             g_mac_address[1], g_mac_address[2], g_mac_address[3], g_mac_address[4], g_mac_address[5], g_ip_address[0],
             g_ip_address[1], g_ip_address[2], g_ip_address[3], g_netmask[0], g_netmask[1], g_netmask[2],
             g_netmask[3], g_gateway_address[0], g_gateway_address[1], g_gateway_address[2], g_gateway_address[3]);

    write_status = write_file(NETWORK_INFO_FILE_NAME, ip_content_body);
    if (STATUS_SUCCESS != write_status)
    {
        APP_ERR_PRINT("\r\nWrite to file failed\r\n");
        APP_ERR_TRAP(write_status);
    }
}

/***********************************************************************************************************************
 * @brief       This function closes the FreeRTOS+FAT instance.
 * @param[in]   None.
 * @retval      None.
 **********************************************************************************************************************/
void freertos_fat_deinit(void)
{
    fsp_err_t freertos_fat_error = FSP_SUCCESS;

    freertos_fat_error = RM_FREERTOS_PLUS_FAT_DiskDeinit(&g_rm_freertos_plus_fat0_ctrl, &g_disk);
    if (FSP_SUCCESS != freertos_fat_error)
    {
        APP_ERR_PRINT("\r\n RM_FREERTOS_PLUS_FAT_DiskDeinit API failed\r\n");
        APP_ERR_TRAP(freertos_fat_error);
    }

    /* Close the FREERTOS_PLUS_FAT_Close instance on safely ejecting */
    freertos_fat_error = RM_FREERTOS_PLUS_FAT_Close(&g_rm_freertos_plus_fat0_ctrl);
    if (FSP_SUCCESS != freertos_fat_error)
    {
        APP_ERR_PRINT("\r\nRM_FREERTOS_PLUS_FAT_Close API failed\r\n");
        APP_ERR_TRAP(freertos_fat_error);
    }
}

/***********************************************************************************************************************
 * @brief       This function writes data to a file utilizing the FreeRTOS+FAT file system.
 * @param[in]   fileName          Name of the file to be written to.
 * @param[in]   content           Pointer to the data content that will be written to the file.
 * @retval      STATUS_SUCCESS    Data written successfully to the file.
 * @retval      STATUS_ERROR      An error occurred during the file write operation.
 **********************************************************************************************************************/
static status_code_t write_file(const char *fileName, const char *content)
{
    int32_t file_error;
    FF_FILE *file_pointer = NULL;
    size_t bytes_written = RESET_VALUE;

    /* Open the file in write mode */
    file_pointer = ff_fopen(fileName, WRITE_MODE);

    if (file_pointer != NULL)
    {
        memset(g_write_data, RESET_VALUE, sizeof(g_write_data));

        /* Write content to buffer */
        snprintf((char*) g_write_data, sizeof(g_write_data), "%s", content);

        /* Write the data to the file and close it */
        bytes_written = ff_fwrite(g_write_data, sizeof(g_write_data[0]), strlen ((char*) g_write_data), file_pointer);

        if (bytes_written != strlen((char*) g_write_data))
        {
            /* Error occurred */
            APP_PRINT("Failed to write all data to %s\n", fileName);
            APP_PRINT(" %d\r\n", stdioGET_ERRNO());
            return STATUS_ERROR;

        }
        /* Close the file */
        file_error = ff_fclose(file_pointer);
        if (SUCCESS != file_error)
        {
            APP_ERR_PRINT("ff_fclose API failed");
            /* Add extra %d as parses string and prints %d as-is */
            APP_PRINT(" %d\r\n", stdioGET_ERRNO());
            return STATUS_ERROR;
        }
    }
    else
    {
        APP_ERR_PRINT("ff_fopen API failed");
        /* Add extra %d as parses string and prints %d as-is */
        APP_PRINT(" %d\r\n", stdioGET_ERRNO());
        return STATUS_ERROR;
    }

    return STATUS_SUCCESS;
}

/***********************************************************************************************************************
 * @brief       This function initializes a TCP client, transmits a data message to the server,
 *              and receives the server's response.
 * @param[in]   None.
 * @retval      Returns STATUS_SUCCESS if the TCP client operation is successful. Otherwise, returns an error.
 **********************************************************************************************************************/
status_code_t tcp_client_handle(char *destination)
{
    /* Socket communication parameters */
    Socket_t            client_socket;
    BaseType_t          send_result      = RESET_VALUE;
    BaseType_t          receive_result   = RESET_VALUE;
    char recv_buffer[RECV_BUFFER_SIZE]   = {RESET_VALUE};
    const char          send_message[]   = "[TCP message]Hello from RA board!";
    struct freertos_sockaddr remote_address;

    /* Remote IP address configuration */
    int                 ipv4_address[4]  = {RESET_VALUE};  /* IPv4 in dot-decimal notation */
    int                 port             = RESET_VALUE;
    char                *remote_ip_address;

    /* Timeout settings for send/receive */
    TickType_t          receive_timeout  = pdMS_TO_TICKS(SOCKET_RECEIVE_TIMEOUT);
    TickType_t          send_timeout     = pdMS_TO_TICKS(SOCKET_SEND_TIMEOUT);

    /* Status codes */
    ping_status_t       ping_status      = PING_SUCCESS;
    uint8_t             retry_count      = RESET_VALUE;
    BaseType_t          status           = STATUS_SUCCESS;

    sscanf(destination, "%d.%d.%d.%d:%d", &ipv4_address[0],
                                          &ipv4_address[1],
                                          &ipv4_address[2],
                                          &ipv4_address[3],
                                          &port);

    /* Extract the IP address part */
    remote_ip_address = strtok(destination, ":");

    /* Set the IP address and port of the remote socket to which this client socket will transmit */
    memset(&remote_address, RESET_VALUE, sizeof(remote_address));
    remote_address.sin_port = FreeRTOS_htons(port);
    remote_address.sin_address.ulIP_IPv4 = FreeRTOS_inet_addr_quick(ipv4_address[0],
                                                                    ipv4_address[1],
                                                                    ipv4_address[2],
                                                                    ipv4_address[3]);
    remote_address.sin_family = FREERTOS_AF_INET4;

    /* Create a new TCP socket */
    client_socket = FreeRTOS_socket(FREERTOS_AF_INET,
                                    FREERTOS_SOCK_STREAM, /* FREERTOS_SOCK_STREAM for TCP */
                                    FREERTOS_IPPROTO_TCP);

    APP_ERR_RET(FREERTOS_INVALID_SOCKET == client_socket, (status_code_t)STATUS_ERROR,
                "Error: Failed to create socket.\r\n");
    APP_PRINT("\r\nCreated new TCP socket.\r\n");

    /* Set a time out so a missing reply does not cause the task to block indefinitely */
    FreeRTOS_setsockopt(client_socket, 0, FREERTOS_SO_RCVTIMEO, &receive_timeout, sizeof(receive_timeout));
    FreeRTOS_setsockopt(client_socket, 0, FREERTOS_SO_SNDTIMEO, &send_timeout, sizeof(send_timeout));

    /* Send the ping request */
    while (retry_count < MAX_PING_RETRIES)
    {
        ping_status = SendPing(remote_ip_address);

        if (ping_status == PING_SUCCESS)
        {
            break;
        }

        retry_count++;
    }

    if (PING_SUCCESS == ping_status)
    {
        /* Connect to the echo server */
        APP_PRINT("\r\nConnecting to server...");
        if (SUCCESS == FreeRTOS_connect(client_socket, &remote_address, sizeof(remote_address)))
        {
            APP_PRINT("\r\nConnected to server at %d.%d.%d.%d:%d\r\n", ipv4_address[0],
                      ipv4_address[1], ipv4_address[2], ipv4_address[3],port);

            /* Send a message to the server */
            send_result = FreeRTOS_send(client_socket, send_message, strlen (send_message), 0);

            if (send_result > RESET_VALUE)
            {
                /* Data was sent successfully */
                APP_PRINT("\r\nSent %d bytes message to TCP server\r\n", send_result);
            }
            else
            {
                /* An error occurred during sending */
                APP_PRINT("\r\nNo data sent or error occurred\r\n");
                status = send_result;
            }

            /* Receive a response from the server */
            memset(&recv_buffer, NULL_CHAR, sizeof(recv_buffer));
            APP_PRINT("\r\nWaiting for server response...");
            receive_result = FreeRTOS_recv(client_socket, recv_buffer, sizeof(recv_buffer) - 1, 0);

            if (receive_result > RESET_VALUE)
            {
                APP_PRINT("\r\nReceived response: %s\r\n", recv_buffer);
            }
            else
            {
                APP_PRINT("\r\nNo response received or error occurred.\n");
                status = STATUS_ERROR;
            }
        }
        else
        {
            APP_PRINT("\r\nFailed to connect to server.\r\n");
            status = STATUS_ERROR;
        }
    }
    else
    {
        APP_PRINT("\r\nFailed to ping to the server, Please check the connection !!!\r\n");
        status = STATUS_ERROR;
    }

    /* Shut down the socket */
    FreeRTOS_shutdown(client_socket, FREERTOS_SHUT_RDWR);

    /* Close the socket */
    FreeRTOS_closesocket(client_socket);

    return (status_code_t) status;
}

/***********************************************************************************************************************
 * @brief       This function initializes a UDP client, transmits a data message to the server,
 *              and receives the server's response.
 * @param[in]   None.
 * @retval      Returns STATUS_SUCCESS if the UDP client operation is successful. Otherwise, returns an error.
 **********************************************************************************************************************/
status_code_t udp_client_handle(char *destination)
{
    /* Socket communication parameters */
    Socket_t client_socket;
    int32_t bytes_sent                   = RESET_VALUE;
    int32_t bytes_received               = RESET_VALUE;
    char recv_buffer[RECV_BUFFER_SIZE]   = {RESET_VALUE};
    const char send_message[]            = "[UDP message]Hello from RA board!";
    struct freertos_sockaddr remote_address;
    uint32_t xAddressLength              = sizeof(remote_address);

    /* Remote IP address configuration */
    int ipv4_address[4]                  = {RESET_VALUE};  /* IPv4 in dot-decimal notation */
    int port                             = RESET_VALUE;
    char *remote_ip_address;

    /* Timeout settings for send/receive */
    TickType_t receive_timeout           = pdMS_TO_TICKS(SOCKET_RECEIVE_TIMEOUT);
    TickType_t send_timeout              = pdMS_TO_TICKS(SOCKET_SEND_TIMEOUT);

    /* Status codes */
    ping_status_t  ping_status           = PING_SUCCESS;
    uint8_t       retry_count            = RESET_VALUE;
    status_code_t status                 = STATUS_SUCCESS;

    sscanf(destination, "%d.%d.%d.%d:%d", &ipv4_address[0],
                                          &ipv4_address[1],
                                          &ipv4_address[2],
                                          &ipv4_address[3],
                                          &port);
    /* Extract the IP address part */
    remote_ip_address = strtok(destination, ":");

    /* Set the IP address and port of the remote socket to which this client socket will transmit */
    memset(&remote_address, RESET_VALUE, sizeof(remote_address));
    remote_address.sin_port = FreeRTOS_htons(port);
    remote_address.sin_address.ulIP_IPv4 = FreeRTOS_inet_addr_quick(ipv4_address[0], ipv4_address[1], ipv4_address[2],
                                                                    ipv4_address[3]);
    remote_address.sin_family = FREERTOS_AF_INET4;
    client_socket = FreeRTOS_socket(FREERTOS_AF_INET, FREERTOS_SOCK_DGRAM, FREERTOS_IPPROTO_UDP);

    APP_ERR_RET(FREERTOS_INVALID_SOCKET ==  client_socket, (status_code_t) STATUS_ERROR,
                "Error: Failed to create socket.\r\n");
    APP_PRINT("\r\nCreated new UDP socket.\r\n");

    /* Set a time out so a missing reply does not cause the task to block indefinitely */
    FreeRTOS_setsockopt(client_socket, 0, FREERTOS_SO_RCVTIMEO, &receive_timeout, sizeof(receive_timeout));
    FreeRTOS_setsockopt(client_socket, 0, FREERTOS_SO_SNDTIMEO, &send_timeout, sizeof(send_timeout));

    /* Send the ping request */
    while (retry_count < MAX_PING_RETRIES)
    {
        ping_status = SendPing(remote_ip_address);

        if (ping_status == PING_SUCCESS)
        {
            break;
        }

        retry_count++;
    }

    if (PING_SUCCESS == ping_status)
    {
        /* Send the string to the socket */
        bytes_sent = FreeRTOS_sendto(client_socket,
                                     send_message,
                                     strlen((char*) send_message) + 1,
                                     0,
                                     &remote_address,
                                     sizeof(remote_address));
        if (bytes_sent > RESET_VALUE)
        {
            /* Data was sent successfully */
            APP_PRINT("\r\nSent %d bytes message to UDP server\r\n", bytes_sent);
        }
        else
        {
            /* An error occurred during sending */
            APP_PRINT("\r\nNo data sent or error occurred\r\n");
            status = STATUS_ERROR;
        }

        /* Receive a response from the server */
        memset((void*) recv_buffer, NULL_CHAR, sizeof(recv_buffer));
        APP_PRINT("\r\nWaiting for server response...");
        bytes_received = FreeRTOS_recvfrom(client_socket,       /* The socket being received from */
                                           recv_buffer,         /* The buffer into which
                                                                   the received data will be written */
                                           sizeof(recv_buffer), /* The size of the buffer provided
                                                                   to receive the data */
                                           0,                   /* ulFlags with the FREERTOS_ZERO_COPY bit clear */
                                           &remote_address,     /* The address from where the data was sent
                                                                   (the source address) */
                                           &xAddressLength);
        if (bytes_received > RESET_VALUE)
        {
            APP_PRINT("\r\nReceived UDP message from server: %s\r\n", recv_buffer);
        }
        else
        {
            APP_PRINT("\r\nNo response received or error occurred.\n");
            status = STATUS_ERROR;
        }
    }
    else
    {
        APP_PRINT("\r\nFailed to ping to the server, Please check the connection !!!\r\n");
        status = STATUS_ERROR;
    }

    /* Close the socket */
    FreeRTOS_closesocket(client_socket);

    return status;
}
