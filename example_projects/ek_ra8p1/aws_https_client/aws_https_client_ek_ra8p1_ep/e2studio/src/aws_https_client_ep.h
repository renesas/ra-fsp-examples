/***********************************************************************************************************************
 * File Name    : aws_https_client_ep.h
 * Description  : Contains macros, data structures and functions used in the aws_https_client_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef AWS_HTTPS_CLIENT_EP_H_
#define AWS_HTTPS_CLIENT_EP_H_

#include <stdbool.h>
#include <stdint.h>
#include "user_app_thread.h"
#include "board_cfg.h"
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_DHCP.h"
#include "core_http_client.h"
#include "transport_mbedtls_pkcs11.h"
#include "user_app.h"

/* Setup macro for debugging */
#define DEBUG_HTTPS             (0)

/* Macros for the sizes of the buffers */
#define SIZE_64                     (64U)
#define SIZE_32                     (32U)
#define IPV4_STR_SIZE               (16U)

/* Extracted ID from GET request to be appended to the PUT API path */
#define ID_LEN                      ((size_t) 26)
#define ID_START_INDEX              (8U)

/* Size of the URL buffer and of the HTTP request/response buffers */
#define URL_SIZE                    (192U)
#define USER_BUFF                   (2048U)

/* Format of the API paths, the username and the feed name are entered by the user.
 * GET: to get the most recent data point.
 * PUT: to update the data point at requested <id>, the <id> fetched by the GET request is appended to the path.
 * POST: to send a new data point. */
#define HTTPS_GET_API_FORMAT        "/api/v2/%s/feeds/%s/data?limit=1"
#define HTTPS_PUT_POST_API_FORMAT   "/api/v2/%s/feeds/%s/data/"

/* Delay in ticks before retrying the connection to the server */
#define CONNECTION_RETRY_DELAY      (3000U)

/* Ethernet setup macros */
#define USR_PING_COUNT          (50U)
#define PING_DELAY              (10U)

/* Macro for the size of the user input buffer */
#define INPUT_BUFFER_SIZE       (16U)

/* Delay in ticks between two checks of the user input */
#define INPUT_POLL_DELAY        (10U)

/* Structure to hold the ping statistics */
typedef struct st_ping_data
{
    uint32_t sent;              /* Ping request */
    uint32_t received;          /* Ping response */
    uint32_t lost;              /* Ping failure */
} ping_data_t;

/* Network context structure */
struct NetworkContext
{
    TlsTransportParams_t * pParams;
};

/* State of the feed ID used by the PUT request */
typedef enum e_feed_id_state
{
    FEED_ID_UNAVAILABLE = 0,    /* GET request has not been processed yet, no feed ID to use */
    FEED_ID_LATEST,             /* Feed ID was fetched by the last GET request */
    FEED_ID_STALE               /* Feed ID was already used, it may not be the latest data point's ID */
} feed_id_state_t;

/* User menu options */
typedef enum Userinput
{
    GET = 1,
    PUT = 2,
    POST = 3
} user_input_t;

#if (ipconfigDHCP_REGISTER_HOSTNAME == 1)
/* DHCP has an option for clients to register their host name. It doesn't
 * have much use, except that a device can be found in a router along with its
 * name. If this option is used the callback below must be provided by the
 * application writer to return a const string, denoting the device's name. */
const char * pcApplicationHostnameHook(void);
#endif /* ipconfigDHCP_REGISTER_HOSTNAME */

/* Extern variables - Defined in aws_https_client_ep.c */
extern ping_data_t g_ping_data;
extern uint32_t g_dhcp_in_use;
extern char * domain_name;
extern char remote_ip_address[];
extern char g_aio_key[SIZE_64];
extern uint8_t ucMACAddress[6];
extern uint8_t ucIPAddress[4];
extern uint8_t ucNetMask[4];
extern uint8_t ucGatewayAddress[4];
extern uint8_t ucDNSServerAddress[4];
extern HTTPRequestInfo_t g_request_info;
extern HTTPResponse_t g_response;

/* Extern variable - Task handle of the user thread, defined in the generated user_app_thread.c */
extern TaskHandle_t user_app_thread;

/* Function declarations */
int32_t crypto_init(void);
fsp_err_t ip_stack_init(void);
fsp_err_t network_link_wait(void);
void ip_config_update(void);
fsp_err_t dns_lookup(char * p_domain, char * p_ip_str);
void ping_request_send(void);
int32_t provision_alt_key(void);
TlsTransportStatus_t connect_aws_https_client(void);
HTTPStatus_t http_request_send(const char * p_method, const char * p_path, float temp_c);
const char * http_error_string_get(HTTPStatus_t status);
void deinit_aws_https_client(void);
eDHCPCallbackAnswer_t xApplicationDHCPHook(eDHCPCallbackPhase_t eDHCPPhase, uint32_t lulIPAddress);

#endif /* AWS_HTTPS_CLIENT_EP_H_ */
