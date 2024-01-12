/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * File Name    : network_thread_entry.c
 * Version      : .
 * Description  : .
 *********************************************************************************************************************/

#include "network_thread.h"

#include "FreeRTOS_IP.h"
#include "FreeRTOS_IP_Private.h"
#include "FreeRTOS_Sockets.h"
#include "FreeRTOS_DHCP.h"
#include "core_http_client.h"
#include "transport_mbedtls_pkcs11.h"
#include "mbedtls/debug.h"

#include "board_cfg.h"
#include "common_init.h"

#include "iot\root_ca.h"

// #define ENABLE_CONSOLE (1)

#define CONNECTION_ABORT_CRTL              (0x00)
#define MENU_EXIT_CRTL                     (0x20)
#define BACKSPACE                          (0x08)
#define CARRIAGE_RETURN                    (0x0D)
#define TRANSACTION_DELAY                  (1000) /* Number Milliseconds between https calls (6 sec) */

#define ABORTING_MESSAGE                   "\r\nAborting data retrieval\r\n"

#define HTTPS_GET_API_WEATHER_HDR          "/v1/forecast.json?key="

#define HTTPS_GET_API_LOCAL_TIME_HDR       "/v1/timezone.json?key="
#define HTTPS_GET_API_LOCAL_TIME_FOOTER    "&q=London"

#define ACTIVE_KEY_NULL                    "cur_live_none"

#define UPDATE_CURRENCY_DATA               (0)

#define RESET_VALUE                        (0x00)
#define USER_BUFF                          (4096)
#define RES_USER_BUFF                      (32 * 1024)

/** HOST Address **/

#define HTTP_REQUEST_KEEP_ALIVE_FLAG       0x1U

#define HTTPS_CONNECTION_NUM_RETRY         ((uint32_t) 5)

#define SOCKET_SEND_RECV_TIME_OUT_MS       ((uint32_t) 10000)

const char * p_char_timezone_prekey = "/v1/timezone.json?key=";

struct NetworkContext
{
    TlsTransportParams_t * pParams;
};

extern void   eth_abort(void);
extern bool_t update_key_data(e_hp_data_flash_iot_keys_t id);

extern uint8_t * stored_api_key;

char_t         p_api_key_target_string[100] = {0};
char_t         newpath[100]                 = {0};
uint32_t     * tptr      = NULL;
uint32_t       test[32U] = {0U};
uint32_t       dummydata = 0xAABBCCDD;
flash_status_t flash_status;
uint8_t      * stored_api_key = NULL;

e_https_certificate_t g_https_certificate = API_CERTIFICATE_CURRENCYAPI;
static char_t         s_net_buffer[32768] = "";

uint8_t ucMACAddress[6]       = {0x00, 0x11, 0x22, 0x33, 0x44, 0x98};
uint8_t ucIPAddress[4]        = {192, 168, 0, 90};
uint8_t ucNetMask[4]          = {255, 255, 255, 0};
uint8_t ucGatewayAddress[4]   = {192, 168, 0, 1};
uint8_t ucDNSServerAddress[4] = {192, 168, 0, 1};

TransportInterface_t xTransportInterface          = {RESET_VALUE};
NetworkContext_t     xNetworkContext              = {RESET_VALUE};
uint8_t              resUserBuffer[RES_USER_BUFF] = {RESET_VALUE};
uint8_t              reqUserBuffer[USER_BUFF]     = {RESET_VALUE};

NetworkInterface_t xInterfaces[1];
NetworkEndPoint_t  xEndPoints[1];

/* #HTTPClient_InitializeRequestHeaders. */
HTTPRequestInfo_t xRequestInfo = {RESET_VALUE};

/* Represents a response returned from an HTTP server. */
HTTPResponse_t xResponse = {RESET_VALUE};

/* Represents header data that will be sent in an HTTP request. */
HTTPRequestHeaders_t xRequestHeaders = {RESET_VALUE};

NetworkCredentials_t connConfig                = {RESET_VALUE};
TlsTransportParams_t xPlaintextTransportParams = {RESET_VALUE};

uint32_t volatile             dhcp_in_use = RESET_VALUE;
NetworkAddressingParameters_t xNd         = {RESET_VALUE};
void print_ipconfig(void);

static void GetNetworkWeather(void);
static void GetNetworkTime(void);
static void GetNetworkCurrency(void);

HTTPStatus_t add_header(HTTPRequestHeaders_t * pRequestHeaders);
HTTPStatus_t connect_https_client(NetworkContext_t    * NetworkContext,
                                  const unsigned char * pTrustedRootCA,
                                  uint16_t              certSize);

static e_hp_data_flash_iot_keys_t s_active_website = IOT_KEY_NONE;

extern NetworkInterface_t * pxFSP_Eth_FillInterfaceDescriptor(BaseType_t xEMACIndex, NetworkInterface_t * pxInterface);
extern void                 update_current_weather_data(char * p_src, size_t src_len, uint32_t region);
extern void                 update_currency_data_tables(char * p_src, size_t src_len, uint32_t region);
extern void                 update_current_time_data(char * p_src, size_t src_len);
extern bool_t               is_key_data_available(e_hp_data_flash_iot_keys_t id);
extern bool_t               get_key_data(e_hp_data_flash_iot_keys_t id, char_t * active_key);

/**********************************************************************************************************************
 * Function Name: GetNetworkTime
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static void GetNetworkTime (void)
{
    HTTPStatus_t httpsClientStatus = HTTPSuccess;

    /* Initialize HTTPS client with presigned URL */
    httpsClientStatus = connect_https_client(&xNetworkContext,
                                             (const unsigned char *) HTTPS_TRUSTED_ROOT_CA_WEATHER,
                                             sizeof(HTTPS_TRUSTED_ROOT_CA_WEATHER));

    /* Handle_error */
    if (HTTPSuccess != httpsClientStatus)
    {
        sprintf(s_net_buffer, "\r\nFailed in server connection establishment");
#ifdef ENABLE_CONSOLE
        print_to_console(s_net_buffer);
#endif
        mbedtls_platform_teardown(NULL);

// APP_ERR_TRAP(httpsClientStatus);
    }
    else
    {
        xTransportInterface.pNetworkContext = &xNetworkContext;
        xTransportInterface.send            = TLS_FreeRTOS_send;
        xTransportInterface.recv            = TLS_FreeRTOS_recv;
    }

    bool_t retry = true;
    while (retry == true)
    {
        /* Initialize all HTTP Client library API structs to 0. */
        (void) memset(&xRequestInfo, 0, sizeof(HTTPRequestInfo_t));
        (void) memset(&xResponse, 0, sizeof(HTTPResponse_t));
        (void) memset(&xRequestHeaders, 0, sizeof(HTTPRequestHeaders_t));

        char_t TimeAPIPath[256] = "";
        char_t active_key[64]   = "";

        get_key_data(s_active_website, (char_t *) &active_key);
        sprintf(TimeAPIPath, "%s%s%s", HTTPS_GET_API_LOCAL_TIME_HDR, active_key, HTTPS_GET_API_LOCAL_TIME_FOOTER);

        /* Initialize the request object. */
        xRequestInfo.pPath   = (const char *) &TimeAPIPath; /* Apply stored key */
        xRequestInfo.pathLen = strlen(TimeAPIPath);

        xRequestInfo.pHost     = HTTPS_WEATHER_HOST_ADDRESS;
        xRequestInfo.hostLen   = strlen(HTTPS_WEATHER_HOST_ADDRESS);
        xRequestInfo.pMethod   = HTTP_METHOD_GET;
        xRequestInfo.methodLen = strlen(HTTP_METHOD_GET);

        /* Set "Connection" HTTP header to "keep-alive" so that multiple requests
         * can be sent over the same established TCP connection. */
        xRequestInfo.reqFlags = HTTP_REQUEST_KEEP_ALIVE_FLAG;

        /* Set the buffer used for storing request headers. */

        xRequestHeaders.pBuffer   = reqUserBuffer;
        xRequestHeaders.bufferLen = sizeof(reqUserBuffer);
        memset(xRequestHeaders.pBuffer, 0, xRequestHeaders.bufferLen);

        httpsClientStatus = HTTPClient_InitializeRequestHeaders(&xRequestHeaders, &xRequestInfo);

        /* Add header */
        if (httpsClientStatus == HTTPSuccess)
        {
            httpsClientStatus = add_header(&xRequestHeaders);
        }
        else
        {
            sprintf(s_net_buffer, "Failed to initialize HTTP request headers: Error=%s. \r\n",
                    HTTPClient_strerror(httpsClientStatus));
#ifdef ENABLE_CONSOLE
            print_to_console(s_net_buffer);
#endif
        }

        xResponse.pBuffer   = resUserBuffer;
        xResponse.bufferLen = sizeof(resUserBuffer);
        memset(xResponse.pBuffer, 0, xResponse.bufferLen);

        if (httpsClientStatus == HTTPSuccess)
        {
            httpsClientStatus = HTTPClient_Send(&xTransportInterface, &xRequestHeaders, NULL, 0, &xResponse, 0);
        }

#if DEBUG_HTTPS
        if (httpsClientStatus == HTTPSuccess)
        {
            sprintf(s_net_buffer, "Received HTTP response from %s %s...\r\n", HTTPS_HOST_ADDRESS, xRequestInfo.pPath);
            print_to_console(s_net_buffer);

            sprintf(s_net_buffer, "Response Headers:\n%s\r\n", xResponse.pHeaders);
            print_to_console(s_net_buffer);

            sprintf(s_net_buffer, "Status Code:\n%u\n", xResponse.statusCode);
            print_to_console(s_net_buffer);
            sprintf(s_net_buffer, "Response Body:\n%s\n", xResponse.pBody);
            print_to_console(s_net_buffer);
        }
        else
        {
            sprintf(s_net_buffer,
                    "Failed to send HTTP %s request to %s%s: Error=%s.",
                    xRequestInfo.pMethod,
                    HTTPS_HOST_ADDRESS,
                    xRequestInfo.pPath,
                    HTTPClient_strerror(httpsClientStatus));
            print_to_console(s_net_buffer);
        }
#endif
        if (HTTPSuccess != httpsClientStatus)
        {
#ifdef ENABLE_CONSOLE
            sprintf(s_net_buffer, "** Failed in GET Request ** \r\n");
            print_to_console(s_net_buffer);
            sprintf(s_net_buffer, " \r\nReturned Error Code: 0x%x  \r\n", httpsClientStatus);
            print_to_console(s_net_buffer);
#endif
            if (false == update_key_data(s_active_website))
            {
// print_to_console(ABORTING_MESSAGE);
// print_to_console("\r\nPress space bar to return to MENU.\r\n");
                eth_abort();
                retry = false;
            }
        }
        else
        {
            char_t err_mess[]  = "\"message\":\"";
            char_t err_mess2[] = "<html>";

            char_t * ptr  = strstr((char_t *) xResponse.pBody, err_mess);
            char_t * ptr2 = strstr((char_t *) xResponse.pBody, err_mess2);

            sprintf(s_net_buffer, "GetNetworkTime Request Header = %s\r\n", xRequestHeaders.pBuffer);

            // print_to_console(s_net_buffer);

            if ((ptr != NULL) || (ptr2 != NULL) || (xResponse.bodyLen == 0))
            {
                // print_to_console("KEY BAD SKIPPING UPDATE\r\n");
                if (false == update_key_data(s_active_website))
                {
// print_to_console(ABORTING_MESSAGE);
// print_to_console("\r\nPress space bar to return to MENU.\r\n");
                    eth_abort();
                    retry = false;
                }
            }
            else
            {
                if (xResponse.bodyLen > 0)
                {
                    sprintf(s_net_buffer, "GetNetworkTime Received data using GET Request = %s\r\n", xResponse.pBody);
                }
                else
                {
                    sprintf(s_net_buffer, "Bad Message : No message body\r\n");
                }

#ifdef ENABLE_CONSOLE
                print_to_console("KEY OK UPDATING TABLES\r\n");
                print_to_console(s_net_buffer);
#endif
                update_current_time_data((char_t *) xResponse.pBody, xResponse.bodyLen);

                print_to_console("\r\nData refresh successful.\r\n");

                print_to_console("\r\nPress space bar to return to MENU, then press 2 to run Interactive AI,");
                print_to_console("\r\nConnectivity & HMI Demos and view recent data on the MIPI Graphics Expansion");
                print_to_console("\r\nBoard.\r\n");
                retry = false;
            }
        }
    }

    TLS_FreeRTOS_Disconnect(&xNetworkContext);

    mbedtls_platform_teardown(NULL);
}

/**********************************************************************************************************************
 * End of function GetNetworkTime
 *********************************************************************************************************************/

const char https_get_request_weather[][64] =
{
    {"&q=hong_kong&days=1&aqi=no&alerts=no"    },
    {"&q=kyoto&days=1&aqi=no&alerts=no"        },
    {"&q=london&days=1&aqi=no&alerts=no"       },
    {"&q=miami&days=1&aqi=no&alerts=no"        },
    {"&q=munich&days=1&aqi=no&alerts=no"       },
    {"&q=new_york&days=1&aqi=no&alerts=no"     },
    {"&q=paris&days=1&aqi=no&alerts=no"        },
    {"&q=prague&days=1&aqi=no&alerts=no"       },
    {"&q=queenstown&days=1&aqi=no&alerts=no"   },
    {"&q=rio&days=1&aqi=no&alerts=no"          },
    {"&q=rome&days=1&aqi=no&alerts=no"         },
    {"&q=san_francisco&days=1&aqi=no&alerts=no"},
    {"&q=shanghai&days=1&aqi=no&alerts=no"     },
    {"&q=singapore&days=1&aqi=no&alerts=no"    },
    {"&q=sydney&days=1&aqi=no&alerts=no"       },
    {"&q=toronto&days=1&aqi=no&alerts=no"      }
};

/**********************************************************************************************************************
 * Function Name: GetNetworkWeather
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static void GetNetworkWeather (void)
{
    HTTPStatus_t httpsClientStatus = HTTPSuccess;
    uint32_t     region            = 0;

    /* Initialize HTTPS client with presigned URL */
    httpsClientStatus = connect_https_client(&xNetworkContext,
                                             (const unsigned char *) HTTPS_TRUSTED_ROOT_CA_WEATHER,
                                             sizeof(HTTPS_TRUSTED_ROOT_CA_WEATHER));

    /* Handle_error */
    if (HTTPSuccess != httpsClientStatus)
    {
#ifdef ENABLE_CONSOLE
        sprintf(s_net_buffer, "\r\nFailed in server connection establishment");
        print_to_console(s_net_buffer);
#endif
        mbedtls_platform_teardown(NULL);

// APP_ERR_TRAP(httpsClientStatus);
    }
    else
    {
        xTransportInterface.pNetworkContext = &xNetworkContext;
        xTransportInterface.send            = TLS_FreeRTOS_send;
        xTransportInterface.recv            = TLS_FreeRTOS_recv;
    }

    for (region = 0; region < 16; )
    {
        /* Initialize all HTTP Client library API structs to 0. */
        (void) memset(&xRequestInfo, 0, sizeof(HTTPRequestInfo_t));
        (void) memset(&xResponse, 0, sizeof(HTTPResponse_t));
        (void) memset(&xRequestHeaders, 0, sizeof(HTTPRequestHeaders_t));

        /* Initialize the request object. */
        char_t WeatherAPIPath[1024 + 128] = "";
        char_t active_key[64]             = "";

        get_key_data(s_active_website, (char_t *) &active_key);
        sprintf(WeatherAPIPath, "%s%s%s", HTTPS_GET_API_WEATHER_HDR, active_key, https_get_request_weather[region]);

        // xRequestInfo.pPath = HTTPS_GET_API_WEATHER;           /* Uses hard coded key */
        // xRequestInfo.pathLen = strlen (HTTPS_GET_API_WEATHER);

        /* Initialize the request object. */
        xRequestInfo.pPath   = (const char *) &WeatherAPIPath; /* Apply stored key */
        xRequestInfo.pathLen = strlen(WeatherAPIPath);

        xRequestInfo.pHost     = HTTPS_WEATHER_HOST_ADDRESS;
        xRequestInfo.hostLen   = strlen(HTTPS_WEATHER_HOST_ADDRESS);
        xRequestInfo.pMethod   = HTTP_METHOD_GET;
        xRequestInfo.methodLen = strlen(HTTP_METHOD_GET);

        /* Set "Connection" HTTP header to "keep-alive" so that multiple requests
         * can be sent over the same established TCP connection. */
        xRequestInfo.reqFlags = HTTP_REQUEST_KEEP_ALIVE_FLAG;

        /* Set the buffer used for storing request headers. */

        xRequestHeaders.pBuffer   = reqUserBuffer;
        xRequestHeaders.bufferLen = sizeof(reqUserBuffer);
        memset(xRequestHeaders.pBuffer, 0, xRequestHeaders.bufferLen);

        httpsClientStatus = HTTPClient_InitializeRequestHeaders(&xRequestHeaders, &xRequestInfo);

        /* Add header */
        if (httpsClientStatus == HTTPSuccess)
        {
            httpsClientStatus = add_header(&xRequestHeaders);
        }
        else
        {
            sprintf(s_net_buffer, "Failed to initialize HTTP request headers: Error=%s. \r\n",
                    HTTPClient_strerror(httpsClientStatus));
#ifdef ENABLE_CONSOLE
            print_to_console(s_net_buffer);
#endif
        }

        xResponse.pBuffer   = resUserBuffer;
        xResponse.bufferLen = sizeof(resUserBuffer);
        memset(xResponse.pBuffer, 0, xResponse.bufferLen);

        if (httpsClientStatus == HTTPSuccess)
        {
            httpsClientStatus = HTTPClient_Send(&xTransportInterface, &xRequestHeaders, NULL, 0, &xResponse, 0);
        }

#if DEBUG_HTTPS
        if (httpsClientStatus == HTTPSuccess)
        {
            sprintf(s_net_buffer, "Received HTTP response from %s %s...\r\n", HTTPS_HOST_ADDRESS, xRequestInfo.pPath);
            print_to_console(s_net_buffer);

            sprintf(s_net_buffer, "Response Headers:\n%s\r\n", xResponse.pHeaders);
            print_to_console(s_net_buffer);

            sprintf(s_net_buffer, "Status Code:\n%u\n", xResponse.statusCode);
            print_to_console(s_net_buffer);
            sprintf(s_net_buffer, "Response Body:\n%s\n", xResponse.pBody);
            print_to_console(s_net_buffer);
        }
        else
        {
            sprintf(s_net_buffer,
                    "Failed to send HTTP %s request to %s%s: Error=%s.",
                    xRequestInfo.pMethod,
                    HTTPS_HOST_ADDRESS,
                    xRequestInfo.pPath,
                    HTTPClient_strerror(httpsClientStatus));
            print_to_console(s_net_buffer);
        }
#endif
        if (HTTPSuccess != httpsClientStatus)
        {
#ifdef ENABLE_CONSOLE
            sprintf(s_net_buffer, "** Failed in GET Request ** \r\n");
            print_to_console(s_net_buffer);

            sprintf(s_net_buffer, " \r\nReturned Error Code: 0x%x  \r\n", httpsClientStatus);
            print_to_console(s_net_buffer);
#endif
            if (false == update_key_data(s_active_website))
            {
// print_to_console(ABORTING_MESSAGE);
// print_to_console("\r\nPress space bar to return to MENU.\r\n");
                eth_abort();
                break;
            }
        }
        else
        {
            char_t err_mess[]  = "\"message\":\"";
            char_t err_mess2[] = "<html>";

            char_t * ptr  = strstr((char_t *) xResponse.pBody, err_mess);
            char_t * ptr2 = strstr((char_t *) xResponse.pBody, err_mess2);

            sprintf(s_net_buffer, "GetNetworkWeather Request Header = %s\r\n", xRequestHeaders.pBuffer);
#ifdef ENABLE_CONSOLE
            print_to_console(s_net_buffer);
#endif

            if ((ptr != NULL) || (ptr2 != NULL) || (xResponse.bodyLen == 0))
            {
// print_to_console("KEY BAD SKIPPING UPDATE\r\n");
                if (false == update_key_data(s_active_website))
                {
// print_to_console(ABORTING_MESSAGE);
// print_to_console("\r\nPress space bar to return to MENU.\r\n");
                    eth_abort();
                    region = 18;
                    break;
                }
            }
            else
            {
                if (xResponse.bodyLen > 0)
                {
// sprintf(s_net_buffer, "GetNetworkWeather Received data using GET Request = %s\r\n", xResponse.pBody);
// print_to_console("GetNetworkWeather Received data using GET Request = ");
// print_to_console((char_t *)xResponse.pBody);
// print_to_console("\r\n");
                }
                else
                {
                    sprintf(s_net_buffer, "Bad Message : No message body\r\n");
#ifdef ENABLE_CONSOLE
                    print_to_console(s_net_buffer);
#endif
                }

                sprintf(s_net_buffer, "GetNetworkWeather len [%d] GET Request = %s \r\n",
                        strlen((char_t *) xResponse.pBody), xRequestHeaders.pBuffer);
#ifdef ENABLE_CONSOLE
                print_to_console("KEY OK UPDATING TABLES\r\n");
                print_to_console(s_net_buffer);
#endif
                update_current_weather_data((char *) xResponse.pBody, strlen((char_t *) xResponse.pBody), region);

                if (region == 0)
                {
                    print_to_console("Updating Data ");
                }

                region++;              // only move one once the current data is successful

                print_to_console(".");
            }
        }

        vTaskDelay(100);
    }

    print_to_console("\r\n");

    if (region == 16)
    {
        print_to_console("\r\nData refresh successful.\r\n");

        print_to_console("\r\nPress space bar to return to MENU, then press 2 to run Interactive AI,");
        print_to_console("\r\nConnectivity & HMI Demos and view recent data on the MIPI Graphics Expansion");
        print_to_console("\r\nBoard.\r\n");
    }

    TLS_FreeRTOS_Disconnect(&xNetworkContext);

    mbedtls_platform_teardown(NULL);
}

/**********************************************************************************************************************
 * End of function GetNetworkWeather
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: GetNetworkCurrency
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static void GetNetworkCurrency (void)
{
    HTTPStatus_t httpsClientStatus = HTTPSuccess;

    char https_get_request[][128] =
    {
        {"/v3/latest?currencies=GBP%2CEUR%2CUSD%2CCAD%2CHKD%2CJPY%2CSGD%2CAUD%2CINR%2CCNY&base_currency=AUD"},
        {"/v3/latest?currencies=GBP%2CEUR%2CUSD%2CCAD%2CHKD%2CJPY%2CSGD%2CAUD%2CINR%2CCNY&base_currency=GBP"},
        {"/v3/latest?currencies=GBP%2CEUR%2CUSD%2CCAD%2CHKD%2CJPY%2CSGD%2CAUD%2CINR%2CCNY&base_currency=CAD"},
        {"/v3/latest?currencies=GBP%2CEUR%2CUSD%2CCAD%2CHKD%2CJPY%2CSGD%2CAUD%2CINR%2CCNY&base_currency=CNY"},
        {"/v3/latest?currencies=GBP%2CEUR%2CUSD%2CCAD%2CHKD%2CJPY%2CSGD%2CAUD%2CINR%2CCNY&base_currency=EUR"},
        {"/v3/latest?currencies=GBP%2CEUR%2CUSD%2CCAD%2CHKD%2CJPY%2CSGD%2CAUD%2CINR%2CCNY&base_currency=HKD"},
        {"/v3/latest?currencies=GBP%2CEUR%2CUSD%2CCAD%2CHKD%2CJPY%2CSGD%2CAUD%2CINR%2CCNY&base_currency=INR"},
        {"/v3/latest?currencies=GBP%2CEUR%2CUSD%2CCAD%2CHKD%2CJPY%2CSGD%2CAUD%2CINR%2CCNY&base_currency=JPY"},
        {"/v3/latest?currencies=GBP%2CEUR%2CUSD%2CCAD%2CHKD%2CJPY%2CSGD%2CAUD%2CINR%2CCNY&base_currency=SGD"},
        {"/v3/latest?currencies=GBP%2CEUR%2CUSD%2CCAD%2CHKD%2CJPY%2CSGD%2CAUD%2CINR%2CCNY&base_currency=USD"}
    };

    uint32_t region = 0;
    {
        for (region = 0; region < 10; )
        {
            httpsClientStatus = HTTPSuccess;

            /* Initialize HTTPS client with presigned URL */

// print_to_console("connect_https_client  \r\n");

            httpsClientStatus = connect_https_client(&xNetworkContext,
                                                     (const unsigned char *) HTTPS_TRUSTED_ROOT_CA_CURRENCY,
                                                     sizeof(HTTPS_TRUSTED_ROOT_CA_CURRENCY));

            /* Handle_error */
            if (HTTPSuccess != httpsClientStatus)
            {
#ifdef ENABLE_CONSOLE
                sprintf(s_net_buffer, "\r\nFailed in server connection establishment");
                print_to_console(s_net_buffer);
#endif
                mbedtls_platform_teardown(NULL);

                // APP_ERR_TRAP(httpsClientStatus);
            }
            else
            {
                xTransportInterface.pNetworkContext = &xNetworkContext;
                xTransportInterface.send            = TLS_FreeRTOS_send;
                xTransportInterface.recv            = TLS_FreeRTOS_recv;
            }

            /* Initialize all HTTP Client library API structs to 0. */
            (void) memset(&xRequestInfo, 0, sizeof(HTTPRequestInfo_t));
            (void) memset(&xResponse, 0, sizeof(HTTPResponse_t));
            (void) memset(&xRequestHeaders, 0, sizeof(HTTPRequestHeaders_t));
            (void) memset(&resUserBuffer, 0, sizeof(RES_USER_BUFF));
            (void) memset(&reqUserBuffer, 0, sizeof(USER_BUFF));

            /* Initialize the request object. */
            xRequestInfo.pPath     = https_get_request[region];
            xRequestInfo.pathLen   = strlen(https_get_request[region]);
            xRequestInfo.pHost     = HTTPS_CURRENCY_HOST_ADDRESS;
            xRequestInfo.hostLen   = strlen(HTTPS_CURRENCY_HOST_ADDRESS);
            xRequestInfo.pMethod   = HTTP_METHOD_GET;
            xRequestInfo.methodLen = strlen(HTTP_METHOD_GET);

            /* Set "Connection" HTTP header to "keep-alive" so that multiple requests
             * can be sent over the same established TCP connection. */
            xRequestInfo.reqFlags = HTTP_REQUEST_KEEP_ALIVE_FLAG;

            /* Set the buffer used for storing request headers. */

            xRequestHeaders.pBuffer   = reqUserBuffer;
            xRequestHeaders.bufferLen = sizeof(reqUserBuffer);
            memset(xRequestHeaders.pBuffer, 0, xRequestHeaders.bufferLen);

            httpsClientStatus = HTTPClient_InitializeRequestHeaders(&xRequestHeaders, &xRequestInfo);

            /* Add header */
            if (httpsClientStatus == HTTPSuccess)
            {
                httpsClientStatus = add_header(&xRequestHeaders);
            }
            else
            {
                sprintf(s_net_buffer, "Failed to initialize HTTP request headers: Error=%s. \r\n",
                        HTTPClient_strerror(httpsClientStatus));
#ifdef ENABLE_CONSOLE
                print_to_console(s_net_buffer);
#endif
            }

            xResponse.pBuffer   = resUserBuffer;
            xResponse.bufferLen = sizeof(resUserBuffer);
            memset(xResponse.pBuffer, 0, xResponse.bufferLen);

            if (httpsClientStatus == HTTPSuccess)
            {
                httpsClientStatus = HTTPClient_Send(&xTransportInterface, &xRequestHeaders, NULL, 0, &xResponse, 0);
            }

#if DEBUG_HTTPS
            if (httpsClientStatus == HTTPSuccess)
            {
                sprintf(s_net_buffer, "Received HTTP response from %s %s...\r\n", HTTPS_HOST_ADDRESS,
                        xRequestInfo.pPath);
                print_to_console(s_net_buffer);

                sprintf(s_net_buffer, "Response Headers:\n%s\r\n", xResponse.pHeaders);
                print_to_console(s_net_buffer);

                sprintf(s_net_buffer, "Status Code:\n%u\n", xResponse.statusCode);
                print_to_console(s_net_buffer);
                sprintf(s_net_buffer, "Response Body:\n%s\n", xResponse.pBody);
                print_to_console(s_net_buffer);
            }
            else
            {
                sprintf(s_net_buffer,
                        "Failed to send HTTP %s request to %s%s: Error=%s.",
                        xRequestInfo.pMethod,
                        HTTPS_HOST_ADDRESS,
                        xRequestInfo.pPath,
                        HTTPClient_strerror(httpsClientStatus));
                print_to_console(s_net_buffer);
            }
#endif
            if (HTTPSuccess != httpsClientStatus)
            {
                sprintf(s_net_buffer, "** Failed in GET Request ** \r\n");
#ifdef ENABLE_CONSOLE
                print_to_console(s_net_buffer);
#endif
                if (false == update_key_data(s_active_website))
                {
// print_to_console(ABORTING_MESSAGE);
// print_to_console("\r\nPress space bar to return to MENU.\r\n");
                    eth_abort();

                    region = 11;
                    break;
                }
            }
            else
            {
                char_t   err_mess[] = "\"message\":\"";
                char_t * ptr        = strstr((char_t *) xResponse.pBody, err_mess);

#ifdef ENABLE_CONSOLE
                sprintf(s_net_buffer, "GetNetworkCurrency Request Header = %s\n", xRequestHeaders.pBuffer);
                print_to_console(s_net_buffer);
#endif
                if (ptr != NULL)
                {
                    // print_to_console("KEY BAD SKIPPING UPDATE\r\n");
                    if (false == update_key_data(s_active_website))
                    {
// print_to_console(ABORTING_MESSAGE);
// print_to_console("\r\nPress space bar to return to MENU.\r\n");
                        eth_abort();

                        region = 11;
                        break;
                    }
                }
                else
                {
#ifdef ENABLE_CONSOLE
                    sprintf(s_net_buffer, "GetNetworkCurrency len [%d] GET Request = %s \r\n",
                            strlen((char_t *) xResponse.pBody), xRequestHeaders.pBuffer);
                    print_to_console("KEY OK UPDATING TABLES\r\n");
                    print_to_console(s_net_buffer);
#endif
                    update_currency_data_tables((char_t *) xResponse.pBody, strlen((char_t *) xResponse.pBody), region);

                    if (region == 0)
                    {
                        print_to_console("Updating Data ");
                    }

                    region++;          // only move one once the current data is successful

                    print_to_console(".");
                }

// print_to_console("Completed 1 loop  \r\n");
            }

            vTaskDelay(TRANSACTION_DELAY);

            TLS_FreeRTOS_Disconnect(&xNetworkContext);

            mbedtls_platform_teardown(NULL);

            // print_to_console("mbedtls_platform_teardown  \r\n");
        }

        print_to_console("\r\n");
        if (region == 10)
        {
            print_to_console("\r\nData refresh successful.\r\n");

            print_to_console("\r\nPress space bar to return to MENU, then press 2 to run Interactive AI,");
            print_to_console("\r\nConnectivity & HMI Demos and view recent data on the MIPI Graphics Expansion");
            print_to_console("\r\nBoard.\r\n");
        }
    }

    if (region == 11)
    {
        TLS_FreeRTOS_Disconnect(&xNetworkContext);

        mbedtls_platform_teardown(NULL);

// print_to_console("final mbedtls_platform_teardown  \r\n");
    }
}

/**********************************************************************************************************************
 * End of function GetNetworkCurrency
 *********************************************************************************************************************/

/* New Thread entry function */
/* pvParameters contains TaskHandle_t */

/**********************************************************************************************************************
 * Function Name: network_thread_entry
 * Description  : .
 * Argument     : pvParameters
 * Return Value : .
 *********************************************************************************************************************/
void network_thread_entry (void * pvParameters)
{
    FSP_PARAMETER_NOT_USED(pvParameters);
    fsp_err_t  ierr;
    BaseType_t status    = pdFALSE;
    uint32_t   ip_status = RESET_VALUE;

    EventBits_t uxBits;

    FSP_PARAMETER_NOT_USED(pvParameters);

    vTaskDelay(100);
    while (1)
    {
        uxBits = xEventGroupWaitBits(g_update_console_event, STATUS_ENABLE_ETHERNET, pdFALSE, pdTRUE, 1);

        if ((uxBits & (STATUS_ENABLE_ETHERNET)) == (STATUS_ENABLE_ETHERNET))
        {
            xEventGroupClearBits(g_update_console_event, STATUS_ENABLE_ETHERNET);
            break;
        }

        vTaskDelay(10);
    }

    // Take the PHY out of Reset
    R_BSP_PinAccessEnable();
    R_IOPORT_PinWrite(&g_ioport_ctrl, ETH_B_RST_CAM_D10, BSP_IO_LEVEL_HIGH);
    R_BSP_PinAccessDisable();
    vTaskDelay(100);

    /* Initialize the crypto hardware acceleration. */
    /* Initialize mbedtls. */
    ierr = mbedtls_platform_setup(NULL);

    /* Error Handler */
    if (FSP_SUCCESS != ierr)
    {
        print_to_console("mbedtls_platform_setup error\r\n");
    }

#if (1 == ipconfigIPv4_BACKWARD_COMPATIBLE)

    /* FreeRTOS IP initializes the IP stack  */
    status = FreeRTOS_IPInit(ucIPAddress, ucNetMask, ucGatewayAddress, ucDNSServerAddress, ucMACAddress);

    /* Error Handler */
    if (pdFALSE == status)
    {
        __BKPT(0);
    }

#else

    /* IF the following function should be declared in the NetworkInterface.c
     * linked in the project. */
    pxFSP_Eth_FillInterfaceDescriptor(0, &(xInterfaces[0]));
    FreeRTOS_FillEndPoint(&(xInterfaces[0]),
                          &(xEndPoints[0]),
                          ucIPAddress,
                          ucNetMask,
                          ucGatewayAddress,
                          ucDNSServerAddress,
                          ucMACAddress);
 #if (ipconfigUSE_DHCP != 0)
    {
        xEndPoints[0].bits.bWantDHCP = pdTRUE;
    }
 #endif                                /* ipconfigUSE_DHCP */

    vTaskDelay(100);
    status = FreeRTOS_IPInit_Multi();
    if (pdFALSE == status)
    {
        print_to_console("FreeRTOS_IPInit_Multi error\r\n");
    }
#endif

    print_ipconfig();

    sprintf(s_net_buffer, "Waiting for network up event...");

// print_to_console(s_net_buffer);

    vTaskDelay(100);
    status = pdFALSE;
    while (status == pdFALSE)
    {
        status = xTaskNotifyWait(pdFALSE, pdFALSE, &ip_status, 0x3000);
        vTaskDelay(100);
    }

    vTaskDelay(100);
    while (1)
    {
        uxBits = xEventGroupWaitBits(g_update_console_event, STATUS_ENABLE_ETHERNET, pdFALSE, pdTRUE, 1);

        if ((uxBits & (STATUS_ENABLE_ETHERNET)) == (STATUS_ENABLE_ETHERNET))
        {
            xEventGroupClearBits(g_update_console_event, STATUS_ENABLE_ETHERNET);
            break;
        }

        vTaskDelay(10);
    }

#if 0

    // Seems to be an error in ra\fsp\src\rm_freertos_plus_tcp\NetworkInterface.c in the func prvCheckLinkStatusTask()
    // it calls vIPNetworkUpCalls() as soon as the PHY detects a link is established
    // However from the FreeRTOS documentation it should only be called :-

    /*
     * If the ipconfigUSE_DHCP or ipconfigUSE_DHCPv6 server
     * is set to 1 in FreeRTOSIPConfig.h then
     *  vApplicationIPNetworkEventHook_Multi( eNetworkUp, struct xNetworkEndPoint *pxEndPoint )
     *   is called when an IP address is obtained from a DHCP server
     *   and when the lease for an IP address previously obtained from a DHCP server is renewed.
     *
     *
     * If the ipconfigUSE_DHCP or ipconfigUSE_DHCPv6 server is set to 0 in FreeRTOSIPConfig.h
     * then vApplicationIPNetworkEventHook_Multi( eNetworkUp, struct xNetworkEndPoint * pxEndPoint )
     *  is called when the network has been initialised with a static IP address. */ // https://www.freertos.org/FreeRTOS-Plus/FreeRTOS_Plus_TCP/API/vApplicationIPNetworkEventHook_Multi.html
#endif

    status = pdFALSE;
    while (status == pdFALSE)
    {
        status = xTaskNotifyWait(pdFALSE, pdFALSE, &ip_status, 0x30000);
        vTaskDelay(100);
    }

    print_ipconfig();

    //
    while (1)
    {
        vTaskDelay(1);

        uxBits = xEventGroupWaitBits(g_update_console_event,
                                     STATUS_IOT_REQUEST_WEATHER | STATUS_IOT_REQUEST_CURRENCY,
                                     pdFALSE,
                                     pdTRUE,
                                     1);

        if ((uxBits & (STATUS_IOT_REQUEST_TIME)) == (STATUS_IOT_REQUEST_TIME))
        {
            xEventGroupClearBits(g_update_console_event, STATUS_IOT_REQUEST_TIME);

            // check for valid credentials
            s_active_website = IOT_KEY_WEATHER;

// is_key_data_available(s_active_website); // Time uses weather key

            /* Set up time request */
            GetNetworkTime();
            xEventGroupSetBits(g_update_console_event, STATUS_IOT_RESPONSE_COMPLETE);
        }

        if ((uxBits & (STATUS_IOT_REQUEST_WEATHER)) == (STATUS_IOT_REQUEST_WEATHER))
        {
            // check for valid credentials
            s_active_website = IOT_KEY_WEATHER;

            xEventGroupClearBits(g_update_console_event, STATUS_IOT_REQUEST_WEATHER);

            /* Set up weather request */
            GetNetworkWeather();
            xEventGroupSetBits(g_update_console_event, STATUS_IOT_RESPONSE_COMPLETE);
        }

        if ((uxBits & (STATUS_IOT_REQUEST_CURRENCY)) == (STATUS_IOT_REQUEST_CURRENCY))
        {
            // check for valid credentials
            s_active_website = IOT_KEY_CURRENCY;

            xEventGroupClearBits(g_update_console_event, STATUS_IOT_REQUEST_CURRENCY);

            /* Set up currency request */
            GetNetworkCurrency();
            xEventGroupSetBits(g_update_console_event, STATUS_IOT_RESPONSE_COMPLETE);
        }
    }
}

/**********************************************************************************************************************
 * End of function network_thread_entry
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: connect_https_client
 * Description  : .
 * Arguments    : NetworkContext
 *              : pTrustedRootCA
 *              : certSize
 * Return Value : .
 *********************************************************************************************************************/
HTTPStatus_t connect_https_client (NetworkContext_t    * NetworkContext,
                                   const unsigned char * pTrustedRootCA,
                                   uint16_t              certSize)
{
    HTTPStatus_t         httpsClientStatus  = HTTPSuccess;
    TlsTransportStatus_t TCP_connect_status = TLS_TRANSPORT_SUCCESS;

    /* The current attempt in the number of connection tries. */
    uint32_t     connAttempt = RESET_VALUE;
    const char * pHostAddr   = NULL;
    assert(NetworkContext != NULL);

    if (0 == memcmp(pTrustedRootCA, HTTPS_TRUSTED_ROOT_CA_WEATHER, sizeof(HTTPS_TRUSTED_ROOT_CA_WEATHER)))
    {
        pHostAddr = HTTPS_WEATHER_HOST_ADDRESS;
    }
    else
    {
        pHostAddr = HTTPS_CURRENCY_HOST_ADDRESS;
    }

    (void) memset(&connConfig, 0U, sizeof(NetworkCredentials_t));
    (void) memset(NetworkContext, 0U, sizeof(NetworkContext_t));
    NetworkContext->pParams = &xPlaintextTransportParams;

    /* Set the connection configurations. */
    connConfig.disableSni       = pdFALSE;
    connConfig.pRootCa          = pTrustedRootCA;
    connConfig.rootCaSize       = certSize;;
    connConfig.pUserName        = NULL;
    connConfig.userNameSize     = 0;
    connConfig.pPassword        = NULL;
    connConfig.passwordSize     = 0;
    connConfig.pClientCertLabel = NULL;
    connConfig.pPrivateKeyLabel = NULL;
    connConfig.pAlpnProtos      = NULL;

    /* Connect to server. */
    for (connAttempt = 1; connAttempt <= HTTPS_CONNECTION_NUM_RETRY; connAttempt++)
    {
        TCP_connect_status = TLS_FreeRTOS_Connect(NetworkContext,
                                                  pHostAddr,
                                                  HTTPS_PORT,
                                                  &connConfig,
                                                  SOCKET_SEND_RECV_TIME_OUT_MS,
                                                  SOCKET_SEND_RECV_TIME_OUT_MS);

        if ((TCP_connect_status != TLS_TRANSPORT_SUCCESS) && (connAttempt < HTTPS_CONNECTION_NUM_RETRY))
        {
            sprintf(s_net_buffer, "Failed to connect the server, retrying after 3000 ms.\r\n");
#ifdef ENABLE_CONSOLE
            print_to_console(s_net_buffer);
#endif
            vTaskDelay(3000);
            continue;
        }
        else
        {
            break;
        }
    }

    if (TLS_TRANSPORT_SUCCESS != TCP_connect_status)
    {
        sprintf(s_net_buffer, "\r\nUnable to connect the server. Error code: %d.\r\n", TCP_connect_status);
#ifdef ENABLE_CONSOLE
        print_to_console(s_net_buffer);
#endif
        httpsClientStatus = HTTPNetworkError;
    }
    else
    {
// print_to_console("\r\nConnected to the server\r\n");
    }

    return httpsClientStatus;
}

/**********************************************************************************************************************
 * End of function connect_https_client
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: add_header
 * Description  : .
 * Argument     : pRequestHeaders
 * Return Value : .
 *********************************************************************************************************************/
HTTPStatus_t add_header (HTTPRequestHeaders_t * pRequestHeaders)
{
    HTTPStatus_t Status = HTTPSuccess;
    configASSERT(pRequestHeaders != NULL);
    char_t active_key[64] = "";

    memset(s_net_buffer, '\0', sizeof(s_net_buffer));

    get_key_data(s_active_website, (char_t *) &active_key);

    Status = HTTPClient_AddHeader(pRequestHeaders,
                                  "apikey",
                                  strlen("apikey"),
                                  (char_t *) &active_key,
                                  strlen(active_key));

    if (Status != HTTPSuccess)
    {
        sprintf(s_net_buffer,
                "An error occurred at adding Active Key in HTTPClient_AddHeader() with error code: Error=%s. \r\n",
                HTTPClient_strerror(Status));
#ifdef ENABLE_CONSOLE
        print_to_console(s_net_buffer);
#endif
    }

    memset(s_net_buffer, '\0', sizeof(s_net_buffer));

    return Status;
}

/**********************************************************************************************************************
 * End of function add_header
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: print_ipconfig
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void print_ipconfig (void)
{
    if (dhcp_in_use)
    {
#if (ipconfigIPv4_BACKWARD_COMPATIBLE == 1)
        ucNetMask[3] = (uint8_t) ((xNd.ulNetMask & 0xFF000000) >> 24);
        ucNetMask[2] = (uint8_t) ((xNd.ulNetMask & 0x00FF0000) >> 16);
        ucNetMask[1] = (uint8_t) ((xNd.ulNetMask & 0x0000FF00) >> 8);
        ucNetMask[0] = (uint8_t) (xNd.ulNetMask & 0x000000FF);

        ucGatewayAddress[3] = (uint8_t) ((xNd.ulGatewayAddress & 0xFF000000) >> 24);
        ucGatewayAddress[2] = (uint8_t) ((xNd.ulGatewayAddress & 0x00FF0000) >> 16);
        ucGatewayAddress[1] = (uint8_t) ((xNd.ulGatewayAddress & 0x0000FF00) >> 8);
        ucGatewayAddress[0] = (uint8_t) (xNd.ulGatewayAddress & 0x000000FF);

        ucDNSServerAddress[3] = (uint8_t) ((xNd.ulDNSServerAddress & 0xFF000000) >> 24);
        ucDNSServerAddress[2] = (uint8_t) ((xNd.ulDNSServerAddress & 0x00FF0000) >> 16);
        ucDNSServerAddress[1] = (uint8_t) ((xNd.ulDNSServerAddress & 0x0000FF00) >> 8);
        ucDNSServerAddress[0] = (uint8_t) (xNd.ulDNSServerAddress & 0x000000FF);

        ucIPAddress[3] = (uint8_t) ((xNd.ulDefaultIPAddress & 0xFF000000) >> 24);
        ucIPAddress[2] = (uint8_t) ((xNd.ulDefaultIPAddress & 0x00FF0000) >> 16);
        ucIPAddress[1] = (uint8_t) ((xNd.ulDefaultIPAddress & 0x0000FF00) >> 8);
        ucIPAddress[0] = (uint8_t) (xNd.ulDefaultIPAddress & 0x000000FF);
#else
        ucNetMask[3] = (uint8_t) ((xEndPoints[0].ipv4_settings.ulNetMask & 0xFF000000) >> 24);
        ucNetMask[2] = (uint8_t) ((xEndPoints[0].ipv4_settings.ulNetMask & 0x00FF0000) >> 16);
        ucNetMask[1] = (uint8_t) ((xEndPoints[0].ipv4_settings.ulNetMask & 0x0000FF00) >> 8);
        ucNetMask[0] = (uint8_t) (xEndPoints[0].ipv4_settings.ulNetMask & 0x000000FF);

        ucGatewayAddress[3] = (uint8_t) ((xEndPoints[0].ipv4_settings.ulGatewayAddress & 0xFF000000) >> 24);
        ucGatewayAddress[2] = (uint8_t) ((xEndPoints[0].ipv4_settings.ulGatewayAddress & 0x00FF0000) >> 16);
        ucGatewayAddress[1] = (uint8_t) ((xEndPoints[0].ipv4_settings.ulGatewayAddress & 0x0000FF00) >> 8);
        ucGatewayAddress[0] = (uint8_t) (xEndPoints[0].ipv4_settings.ulGatewayAddress & 0x000000FF);

        ucDNSServerAddress[3] = (uint8_t) ((xEndPoints[0].ipv4_settings.ulDNSServerAddresses[0] & 0xFF000000) >> 24);
        ucDNSServerAddress[2] = (uint8_t) ((xEndPoints[0].ipv4_settings.ulDNSServerAddresses[0] & 0x00FF0000) >> 16);
        ucDNSServerAddress[1] = (uint8_t) ((xEndPoints[0].ipv4_settings.ulDNSServerAddresses[0] & 0x0000FF00) >> 8);
        ucDNSServerAddress[0] = (uint8_t) (xEndPoints[0].ipv4_settings.ulDNSServerAddresses[0] & 0x000000FF);

        ucIPAddress[3] = (uint8_t) ((xEndPoints[0].ipv4_settings.ulIPAddress & 0xFF000000) >> 24);
        ucIPAddress[2] = (uint8_t) ((xEndPoints[0].ipv4_settings.ulIPAddress & 0x00FF0000) >> 16);
        ucIPAddress[1] = (uint8_t) ((xEndPoints[0].ipv4_settings.ulIPAddress & 0x0000FF00) >> 8);
        ucIPAddress[0] = (uint8_t) (xEndPoints[0].ipv4_settings.ulIPAddress & 0x000000FF);
#endif
    }

#ifdef ENABLE_CONSOLE
    sprintf(s_net_buffer, "\r\nEthernet adapter for Renesas %s:\r\n", KIT_NAME);
    print_to_console(s_net_buffer);

    sprintf(s_net_buffer, "\tDescription                       : Renesas %s Ethernet\r\n", KIT_NAME);
    print_to_console(s_net_buffer);

    sprintf(s_net_buffer,
            "\tPhysical Address                  : %02x-%02x-%02x-%02x-%02x-%02x\r\n",
            ucMACAddress[0],
            ucMACAddress[1],
            ucMACAddress[2],
            ucMACAddress[3],
            ucMACAddress[4],
            ucMACAddress[5]);

    print_to_console(s_net_buffer);

    sprintf(s_net_buffer, "\tDHCP Enabled                      : %s\r\n", dhcp_in_use ? "Yes" : "No");
    print_to_console(s_net_buffer);

    sprintf(s_net_buffer,
            "\tIPv4 Address                      : %d.%d.%d.%d\r\n",
            ucIPAddress[0],
            ucIPAddress[1],
            ucIPAddress[2],
            ucIPAddress[3]);
    print_to_console(s_net_buffer);

    sprintf(s_net_buffer,
            "\tSubnet Mask                       : %d.%d.%d.%d\r\n",
            ucNetMask[0],
            ucNetMask[1],
            ucNetMask[2],
            ucNetMask[3]);
    print_to_console(s_net_buffer);

    sprintf(s_net_buffer,
            "\tDefault Gateway                   : %d.%d.%d.%d\r\n",
            ucGatewayAddress[0],
            ucGatewayAddress[1],
            ucGatewayAddress[2],
            ucGatewayAddress[3]);
    print_to_console(s_net_buffer);

    sprintf(s_net_buffer,
            "\tDNS Servers                       : %d.%d.%d.%d\r\n",
            ucDNSServerAddress[0],
            ucDNSServerAddress[1],
            ucDNSServerAddress[2],
            ucDNSServerAddress[3]);
    print_to_console(s_net_buffer);
#endif
}

/**********************************************************************************************************************
 * End of function print_ipconfig
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: vApplicationMallocFailedHook
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
void vApplicationMallocFailedHook (void)
{
    __BKPT(0);
}

/**********************************************************************************************************************
 * End of function vApplicationMallocFailedHook
 *********************************************************************************************************************/
