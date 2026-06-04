/***********************************************************************************************************************
 * File Name    : usr_app.h
 * Description  : Contains macros, data structures and functions used  in the Application
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef USR_APP_H_
#define USR_APP_H_

#if( ipconfigUSE_DHCP != 0 )
    #include "FreeRTOS_DHCP.h"
#endif

#define MAC_ADDR_SIZE                           (6)
#define IP_ADDR_SIZE                            (4)
#define NETMASK_SIZE                            (4)
#define GATEWAY_ADDR_SIZE                       (4)
#define DNS_ADDR_SIZE                           (4)

#define SUCCESS                                 (0)

#define SERVER_PORT                             (80)
#define SERVER_BACKLOG                          (1)

#define IP_LINK_UP                              (1U << 0)
#define IP_LINK_DOWN                            (1U << 1)
#define ETHERNET_LINK_UP                        (1U << 2)
#define ETHERNET_LINK_DOWN                      (1U << 3)

#define PING_TIMEOUT_MS                         (2000U)
#define PING_DATA_SIZE                          (32U)
#define PING_QUEUE_WAIT_TIME_MS                 (2000U)
#define PING_DELAY_MS                           (1000U)


#define GET_REQUEST                             ("GET / ")
#define GET_REQUEST_LENGTH                      (6)
#define HTML_GET_REQUEST                        ("GET /index.html")
#define HTML_GET_REQUEST_LENGTH                 (15)
#define IMAGE_GET_REQUEST                       ("GET /mjpeg")
#define IMAGE_GET_REQUEST_LENGTH                (10)
#define STREAM_GET_REQUEST                      ("GET /stream")
#define STREAM_GET_REQUEST_LENGTH               (11)
#define FAVICON_GET_REQUEST                     ("GET /favicon.ico")
#define FAVICON_GET_REQUEST_LENGTH              (16)
#define CLIENT_HANDLER_TASK_BUFFER_SIZE         (2048)
#define STREAM_CONTROL_TASK_BUFFER_SIZE         (512)
#define BUFFER_SIZE                             (2048)
#define RECV_BUFFER_SIZE                        (512)
#define NETWORK_BUFFER_SIZE                     (1024)
#define ZERO_INDEX                              (0)
#define SELECT_MCU_ENCODE                       ('1')
#define SELECT_SENSOR_ENCODE                    ('2')
#define OPTION_STOP_STREAM                      ('1')
#define OPTION_END_DEMO                         ('2')

#define BANNER_INFO     "\r\n********************************************************************************"\
                        "\r\n*                  RA8 CEU WEBCAM GETTING STARTED GUIDE                        *"\
                        "\r\n********************************************************************************"\
                        "\r\nThis application demonstrates the CEU webcam feature on the FreeRTOS + TCP Stack"\
                        "\r\nnetworking using the RA MCU's Ethernet interface and provides a hosted webpage"\
                        "\r\nthat displays webcam operation.\r\n"

#define AP_INFO         "\r\nAfter the DHCP process completes, the board will start an HTTP server over Ethernet."\
                        "\r\nThe user enters a URL in the browser to load the HTML page that continuously"\
                        "\r\nstreams the images. The first HTML request starts the Camera Thread and sends"\
                        "\r\ncaptured frames to the server for real-time JPEG display. When the webcam stream"\
                        "\r\ndisplays in the browser, the user can view status and debug info on RTT Viewer"\
                        "\r\nor Serial Terminal.\r\n"

#define METHOD_MENU     "\r\nEncoding Method Select Menu:"\
		                "\r\n1. MCU Encode"\
		                "\r\n2. Sensor Encode"\
		                "\r\nSelect: \r\n"

#define CONTROL_MENU    "\r\nStream Control Menu:"\
                        "\r\n1. Stop current stream to change encode mode"\
                        "\r\n2. End demo"\
                        "\r\nSelect: \r\n"

/* Define return codes */
typedef enum
{
    PING_SUCCESS = 0,
    PING_TIMEOUT,
    PING_ERROR
} ping_status_t;

typedef enum
{
    STATUS_SUCCESS = 0,   // Indicates successful execution
    STATUS_ERROR,          // Indicates an error occurred
} status_code_t;

typedef struct jpeg_transfer {
    uint8_t * jpeg_address;
    uint32_t size;
} jpeg_transfer_t;

typedef struct st_ping_data
{
    int8_t sent;     // Ping Request
    int8_t received; // Ping Response
    int8_t lost;     // Ping failure
} ping_data_t;

#if( ipconfigUSE_DHCP != 0 )
    void updateDhcpResponseToUsr(void);
#endif

#if( ipconfigUSE_DHCP != 0 )
    eDHCPCallbackAnswer_t xApplicationDHCPHook( eDHCPCallbackPhase_t eDHCPPhase, uint32_t ulIPAddress );
#endif

uint32_t ulApplicationGetNextSequenceNumber( uint32_t ulSourceAddress,
                                             uint16_t usSourcePort,
                                             uint32_t ulDestinationAddress,
                                             uint16_t usDestinationPort );

#endif /* USR_APP_H_ */
