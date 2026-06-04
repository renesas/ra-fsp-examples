/***********************************************************************************************************************
 * File Name    : http_thread_entry.c
 * Description  : The functionality of this file is handle for HTTP Server operation of the application
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "http_thread.h"
#include "common_utils.h"
#include "index_html.h"
#include "usr_app.h"

/***********************************************************************************************************************
 * Private variables declarations
 **********************************************************************************************************************/
/* Server state */
static volatile bool server_running  = true;
static volatile bool client_busy     = false;
static volatile bool stream_allowed  = true;

/* Client connection state */
static TaskHandle_t client_task_handle = NULL;

/***********************************************************************************************************************
 * Global function declarations
 **********************************************************************************************************************/
static Socket_t http_server_init(void);
static void http_client_handler_task(void *pvParameters);
static void stream_control_task(void *pvParameters);
static BaseType_t tcp_send_all(Socket_t xSocket, const uint8_t *buffer, size_t length);
static bool send_html_response(Socket_t xSocket);
static bool send_stream_header(Socket_t xSocket);
static bool send_jpeg_frame(Socket_t xSocket, const uint8_t *data, uint32_t size);

/* HTTP Thread entry function */
void http_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);

    /* Client definition */
    struct freertos_sockaddr xClient;
    socklen_t xSize = sizeof(xClient);

    /* Wait for Camera setup complete */
    xEventGroupWaitBits(g_system_event,
                        EVT_CAM_READY,
                        pdFALSE,
                        pdTRUE,
                        portMAX_DELAY);

    /* Server initialization */
    Socket_t xListeningSocket = http_server_init();
    if (xListeningSocket == FREERTOS_INVALID_SOCKET)
    {
        APP_PRINT("HTTP server initialize failed. Aborting.\r\n");
        return;
    }

    /* Stream control task - For Stop current stream and End the application demo */
    xTaskCreate(stream_control_task,
                "StreamCtrl",
                STREAM_CONTROL_TASK_BUFFER_SIZE,
                NULL,
                3,
                NULL);

    /* Delay for system stable */
    vTaskDelay(pdMS_TO_TICKS(100));
    APP_PRINT("\r\nWeb HTTP Server started.");
    APP_PRINT("\r\nPlease enter the Webpage URL on a browser for Webcam demo.");
    APP_PRINT("\r\nReconnect to the webpage after change the encode mode.\r\n");

    while (server_running)
    {
        /* Wait for incoming connections. */
        Socket_t xConnectedSocket = FreeRTOS_accept(xListeningSocket, &xClient, &xSize);
        if (xConnectedSocket == FREERTOS_INVALID_SOCKET)
        {
            continue;
        }

        /* Check if server already have a client connected */
        if (client_busy)
        {
            const char *busy = "HTTP/1.1 503 Service Unavailable\r\n"
                              "Connection: close\r\n\r\n";
            FreeRTOS_send(xConnectedSocket, busy, strlen(busy), 0);
            FreeRTOS_closesocket(xConnectedSocket);
            continue;
        }

        client_busy = true;
        stream_allowed = true;

        /* Create HTTP client handler task - to handle for client request */
        xTaskCreate(http_client_handler_task,
                    "HTTPClient",
                    CLIENT_HANDLER_TASK_BUFFER_SIZE,
                    (void *)xConnectedSocket,
                    3,
                    &client_task_handle);
    }

    /* Cleanup on shutdown */
    FreeRTOS_closesocket(xListeningSocket);

    APP_PRINT("\r\n=============================================================\r\n");
    APP_PRINT("=                       Server closed                       =\r\n");
    APP_PRINT("=                       Demo end here                       =");
    APP_PRINT("\r\n=============================================================\r\n");
    vTaskDelete(NULL);
}

/*******************************************************************************************************************//**
 *  @brief      This function creates and configures TCP listening socket for HTTP server on port 80.
 *  @param[in]  None
 *  @retval     Valid socket handle on success, FREERTOS_INVALID_SOCKET on failure.
 **********************************************************************************************************************/
Socket_t http_server_init(void)
{
    Socket_t xListeningSocket   = FREERTOS_INVALID_SOCKET;
    struct freertos_sockaddr xBindAddress;
    TickType_t xReceiveTimeOut  = pdMS_TO_TICKS(10000);

    /* Socket create */
    xListeningSocket = FreeRTOS_socket( FREERTOS_AF_INET4,
                                        FREERTOS_SOCK_STREAM,
                                        FREERTOS_IPPROTO_TCP );
    if (xListeningSocket == FREERTOS_INVALID_SOCKET)
    {
        APP_PRINT("Socket create failed\r\n");
        return FREERTOS_INVALID_SOCKET;
    }

    /* Set receive timeout */
    if (FreeRTOS_setsockopt(xListeningSocket,
                           0,
                           FREERTOS_SO_RCVTIMEO,
                           (void *)&xReceiveTimeOut,
                           sizeof(xReceiveTimeOut)) != 0)
    {
        APP_PRINT("Set RCVTIMEO failed\r\n");
        FreeRTOS_closesocket(xListeningSocket);
        goto cleanup;
    }

    /* Bind address */
    memset( &xBindAddress, 0, sizeof(xBindAddress) );
    xBindAddress.sin_family = FREERTOS_AF_INET4;
    xBindAddress.sin_port   = FreeRTOS_htons(80);
    xBindAddress.sin_address.ulIP_IPv4 = FREERTOS_INADDR_ANY;

    /* Bind the socket to the port that the client RTOS task will send to. */
    if(FreeRTOS_bind( xListeningSocket, &xBindAddress, sizeof( xBindAddress ) ) != 0)
    {
        APP_PRINT("Bind failed\r\n");
        goto cleanup;
    }

    /* Start listening */
    if (FreeRTOS_listen(xListeningSocket, SERVER_BACKLOG) != 0)
    {
        APP_PRINT("Listen failed\r\n");
        goto cleanup;
    }

    return xListeningSocket;

cleanup:
    FreeRTOS_closesocket(xListeningSocket);
    return FREERTOS_INVALID_SOCKET;
}

/*******************************************************************************************************************//**
 *  @brief      Sends all data through TCP socket with retry until complete or error occurs.
 *  @param[in]  xSocket     Socket handle to send data through
 *  @param[in]  buffer      Pointer to data buffer to send
 *  @param[in]  length      Length of data to send in bytes
 *  @retval     pdPASS on success (all data sent), pdFAIL on send error or connection closed.
 **********************************************************************************************************************/
static BaseType_t tcp_send_all(Socket_t xSocket, const uint8_t *buffer, size_t length)
{
    size_t sent = 0;

    while (sent < length)
    {
        BaseType_t ret = FreeRTOS_send(xSocket, buffer + sent, length - sent, 0);

        if (ret <= 0)
        {
            return pdFAIL;
        }

        sent += (size_t) ret;
    }

    return pdPASS;
}


/*******************************************************************************************************************//**
 *  @brief      Monitors and controls camera streaming state based on system events.
 *  @param[in]  pvParameters    Unused task parameter
 *  @retval     None
 **********************************************************************************************************************/
static void stream_control_task(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);
    EventBits_t bits;

    while (server_running)
    {
        /* Wait for both Camera stop and Demo end event */
        bits = xEventGroupWaitBits(g_system_event,
                                   EVT_CAM_STOP | EVT_DEMO_END,
                                   pdTRUE,
                                   pdFALSE,
                                   portMAX_DELAY);

        /* Handle state for Demo end event */
        if (bits & EVT_DEMO_END)
        {
            stream_allowed = false;
            server_running = false;
            break;
        }

        /* Handle state for Camera stop event */
        if (bits & EVT_CAM_STOP)
        {
            stream_allowed = false;

            xEventGroupWaitBits(g_system_event,
                                EVT_CAM_READY,
                                pdTRUE,
                                pdFALSE,
                                portMAX_DELAY);
        }
    }

    vTaskDelete(NULL);
}

/*******************************************************************************************************************//**
 *  @brief      Handles HTTP client requests and manages MJPEG streaming for connected clients.
 *  @param[in]  pvParameters    Socket handle cast from void pointer
 *  @retval     None
 **********************************************************************************************************************/
static void http_client_handler_task(void *pvParameters)
{
    Socket_t xSocket = (Socket_t) pvParameters;
    char rx_buffer[BUFFER_SIZE];
    int32_t recv_len;

    TickType_t timeout = pdMS_TO_TICKS(10000);
    FreeRTOS_setsockopt(xSocket, 0,
                        FREERTOS_SO_RCVTIMEO,
                        &timeout,
                        sizeof(timeout));

    while (server_running)
    {
        recv_len = FreeRTOS_recv(xSocket, rx_buffer, sizeof(rx_buffer) - 1, 0);
        if (recv_len <= 0)
        {
            break;
        }
        rx_buffer[recv_len] = '\0';

        /* Handle GET /index.html or GET / */
        if (strncmp(rx_buffer, HTML_GET_REQUEST, HTML_GET_REQUEST_LENGTH) == 0 ||
                strncmp(rx_buffer, GET_REQUEST, GET_REQUEST_LENGTH)  == 0)
        {
            if (!send_html_response(xSocket))
            {
                APP_PRINT("[HTTP] Failed to send HTML\r\n");
                break;
            }
        }
        /* Handle GET /mjpeg or GET /stream */
        else if (strncmp(rx_buffer, IMAGE_GET_REQUEST, IMAGE_GET_REQUEST_LENGTH) == 0 ||
                strncmp(rx_buffer, STREAM_GET_REQUEST, STREAM_GET_REQUEST_LENGTH) == 0)
        {
            /* Send MJPEG stream header */
            if (!send_stream_header(xSocket))
            {
                APP_PRINT("[HTTP] Failed to send stream header\r\n");
                break;
            }

            /* Signal camera to start */
            xEventGroupSetBits(g_system_event, EVT_CAM_START);

            /* Keep connection alive - frames sent by broadcaster task */
            while (server_running && stream_allowed)
            {
                jpeg_transfer_t jpeg_transfer;

                /* Wait for new JPEG frame from camera */
                if (xQueueReceive(g_jpeg_queue, &jpeg_transfer, pdMS_TO_TICKS(1000)) != pdPASS)
                {
                    continue; /* Timeout, try again */
                }

                if (!send_jpeg_frame(xSocket, jpeg_transfer.jpeg_address, jpeg_transfer.size))
                {
                    APP_PRINT("[STREAM] Failed to send frame, client likely disconnected\r\n");
                    break;
                }
            }

            /* Stream stopped */
            goto client_exit;
        }
        /* Handle favicon.ico */
        else if (strncmp(rx_buffer, FAVICON_GET_REQUEST, FAVICON_GET_REQUEST_LENGTH) == 0)
        {
            APP_PRINT("[HTTP] Ignoring favicon request\r\n");
            const char *resp = "HTTP/1.1 404 Not Found\r\n"
                              "Connection: close\r\n\r\n";
            FreeRTOS_send(xSocket, resp, strlen(resp), 0);
            break;
        }
        /* Handle unknown requests */
        else
        {
            APP_PRINT("[HTTP] Unknown request: %.50s\r\n", rx_buffer);
            const char *not_found = "HTTP/1.1 404 Not Found\r\n"
                                   "Content-Type: text/plain\r\n"
                                   "Connection: close\r\n\r\n"
                                   "404 - Not Found\r\n";
            FreeRTOS_send(xSocket, not_found, strlen(not_found), 0);
            break;
        }
    }

/* Client exit handle */
client_exit:

    FreeRTOS_closesocket(xSocket);
    client_busy = false;
    vTaskDelete(NULL);
}

/*******************************************************************************************************************//**
 *  @brief      Sends HTTP response with HTML page content to client.
 *  @param[in]  xSocket    Socket handle to send response through
 *  @retval     true on success, false on send failure.
 **********************************************************************************************************************/
static bool send_html_response(Socket_t xSocket)
{
    char header[256];
    size_t body_len = strlen(index_html);

    int header_len = snprintf(header, sizeof(header),
                             "HTTP/1.1 200 OK\r\n"
                             "Content-Type: text/html\r\n"
                             "Content-Length: %u\r\n"
                             "Connection: keep-alive\r\n"
                             "\r\n",
                             (unsigned)body_len);

    /* Send header */
    if (tcp_send_all(xSocket, (const uint8_t *)header, (size_t)header_len) != pdPASS)
    {
        return false;
    }

    /* Send body */
    if (tcp_send_all(xSocket, (const uint8_t *)index_html, (size_t)body_len) != pdPASS)
    {
        return false;
    }

    return true;
}

/*******************************************************************************************************************//**
 *  @brief      Sends MJPEG stream header with multi-part content type to client.
 *  @param[in]  xSocket   Socket handle to send header through
 *  @retval     true on success, false on send failure.
 **********************************************************************************************************************/
static bool send_stream_header(Socket_t xSocket)
{
    const char *header =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: multipart/x-mixed-replace; boundary=frame\r\n"
        "Cache-Control: no-cache, no-store, must-revalidate\r\n"
        "Pragma: no-cache\r\n"
        "Expires: 0\r\n"
        "Connection: keep-alive\r\n"
        "\r\n";

    return (tcp_send_all(xSocket, (const uint8_t *)header, (size_t)strlen(header)) == pdPASS);
}

/*******************************************************************************************************************//**
 *  @brief      Sends a single JPEG frame with multipart boundary header to client.
 *  @param[in]  xSocket   Socket handle to send frame through
 *  @param[in]  data      Pointer to JPEG image data buffer
 *  @param[in]  size      Size of JPEG image data in bytes
 *  @retval     true on success, false on send failure.
 **********************************************************************************************************************/
static bool send_jpeg_frame(Socket_t xSocket, const uint8_t *data, uint32_t size)
{
    char part_header[256];

    int header_len = snprintf(part_header, sizeof(part_header),
                             "--frame\r\n"
                             "Content-Type: image/jpeg\r\n"
                             "Content-Length: %u\r\n"
                             "\r\n",
                             (unsigned)size);

    /* Send image data to the client */
    if (FreeRTOS_send(xSocket, (const uint8_t *)part_header, (size_t)header_len, 0) <= 0)
    {
        return false;
    }

    if (FreeRTOS_send(xSocket, data, (size_t)size, 0) <= 0)
    {
        return false;
    }

    return true;
}

