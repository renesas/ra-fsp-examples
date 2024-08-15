/***********************************************************************************************************************
 * File Name    : user_app.h
 * Description  : Contains macros, data structures and functions used  in the Application
 ***********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef USER_APP_H_
#define USER_APP_H_

#include "FreeRTOS_DHCP.h"

/******************************************************************************
 * Macro definitions
 ******************************************************************************/

/**
 *  User Configurable Settings
 **/

/** User has to update according to their LAN/WAN test setup for testing of ping IP address **/
#define HTTPS_TEST_PING_IP              "8.8.8.8"

/** HOST Address **/
#define HTTPS_HOST_ADDRESS              "io.adafruit.com"

/**
 *  @brief User has to be update according to their credentials viz., {user_name} and {feed_key} in the respective
 *  GET, PUT and POST URLs in the specified formats.
 *  For more information of obtaining user credentials can follow the steps provided in mark down file.
 **/

/** @brief To get the most recent value. Get API from GET url (https://ioadafruit.com/api/v2/{username}/feeds/{feed_key}/data?limit=1): /api/v2/{username}/feeds/{feed_key}/data?limit=1 **/
#define HTTPS_GET_API                   "/api/v2/canhtran/feeds/adc/data?limit=1"

/** @brief HTTPS_PUT_POST_API can be used in PUT and POST methods.
 *  PUT method will update data point at requested <id>.
 *  PUT url: https://ioadafruit.com/api/v2/{username}/feeds/{feed_key}/data/{id} , where the <id> will append to the url in the process of
 *  PUT request, once it fetch from the GET request.
 *  API from PUT url: /api/v2/{username}/feeds/{feed_key}/data/{id}
 *  POST method will send new data to the server.
 *  POST url:https://ioadafruit.com/api/v2/{username}/feeds/{feed_key}/data/
 *  API from POST url: /api/v2/{username}/feeds/{feed_key}/data/
 **/
#define HTTPS_PUT_POST_API              "/api/v2/canhtran/feeds/adc/data/"

/** @brief User has to update their generated active key from the io.adafruit.com server. */
#define ACTIVE_KEY                      "aio_aARe12Pl5fc0I2QwOdHiRnfNBGif"

/* TLS port for HTTPS. */
#define HTTPS_PORT                      ((uint16_t) 443U)

/* Number of times to retry the HTTPS connection. A connection is only attempted again if
 * TLS_TRANSPORT_SUCCESS is not returned from TLS_FreeRTOS_Connect(). This indicates an error in the network
 * layer.
 */
#define HTTPS_CONNECTION_NUM_RETRY      ((uint32_t) 5)

#define SOCKET_SEND_RECV_TIME_OUT_MS    ((uint32_t) 10000)

/* Extracted ID from GET request to be updated in HTTPS_PUT_POST_API */

#define ID_LEN                          ((size_t) 26)
#define ID_START_INDEX                  (8)
#define INDEX_ZERO                      (0)
#define URL_SIZE                        (128)
#define USER_BUFF                       (2048)

/**
 *  Client certificate to be updated by the user by following the process specified in the mark down file
 **/

#define CLIENT_CERTIFICATE_PEM                                           \
    "-----BEGIN CERTIFICATE-----\n"                                      \
    "MIID9zCCAt+gAwIBAgIUEJWi4aQTJ79x7GsCX4imnMyyoOYwDQYJKoZIhvcNAQEL\n" \
    "BQAwgYoxCzAJBgNVBAYTAlZOMQswCQYDVQQIDAJTRzEPMA0GA1UEBwwGU2FpZ29u\n" \
    "MQwwCgYDVQQKDANSVkMxDDAKBgNVBAsMA3NzMjEYMBYGA1UEAwwPaW8uYWRhZnJ1\n" \
    "aXQuY29tMScwJQYJKoZIhvcNAQkBFhhjYW5oLnRyYW4ueGpAcmVuZXNhcy5jb20w\n" \
    "HhcNMjMwMTA5MDk0NjQxWhcNMjQwMTA5MDk0NjQxWjCBijELMAkGA1UEBhMCVk4x\n" \
    "CzAJBgNVBAgMAlNHMQ8wDQYDVQQHDAZTYWlnb24xDDAKBgNVBAoMA1JWQzEMMAoG\n" \
    "A1UECwwDc3MyMRgwFgYDVQQDDA9pby5hZGFmcnVpdC5jb20xJzAlBgkqhkiG9w0B\n" \
    "CQEWGGNhbmgudHJhbi54akByZW5lc2FzLmNvbTCCASIwDQYJKoZIhvcNAQEBBQAD\n" \
    "ggEPADCCAQoCggEBANHjj+OUKVYGDYbd1N2dcHZy5aeMpKzRfED8+MlQsoXJBwP0\n" \
    "6Xl8485Klo19qrFGD+NsiGylSdF5lg87iICxkCShTPyYEB5gN29wVIzgPv8HiKLa\n" \
    "Zm66BF2yTcXRbFj36BplqsTq36hiqoJeTaTXnbjGn3Mf/fdb/UIoy0zgCZzXP9Yo\n" \
    "2DlEacp0L+ORisH+J5I8cP4Q8eD04tHr3rSp5pR9SOAh8tIeWxm6VD1/rK8IFFN8\n" \
    "F9Mv/S+p75hPEwrPlhV5XmR9BSFD2Yakc6fbiECnRtyUBgMpKinK+XC98EI3iWh+\n" \
    "rNJONNSRyUMsKXbIpK+KQB80IE/+j3PEPyS7G0ECAwEAAaNTMFEwHQYDVR0OBBYE\n" \
    "FAeoe9a6wgYcmHfXZU0HzuSmOLq7MB8GA1UdIwQYMBaAFAeoe9a6wgYcmHfXZU0H\n" \
    "zuSmOLq7MA8GA1UdEwEB/wQFMAMBAf8wDQYJKoZIhvcNAQELBQADggEBAFxzzHeK\n" \
    "jp4RXUKmXiVnR2XyXVV5Ev5c05OiKd7PTRynhzpbPAoCbiM902RD83ZBA5EzhvK1\n" \
    "cs6/7E8O65PQJjmFKDpfLV7mG+iRzf5+0vn6U4oDMjSpp2mDVdQi8Si4DHiyOkXZ\n" \
    "w/LlDQ0UwVQDJ5zMAAJw9Yrz6Tltkpi5xMK0fnrW8LMyNpJj40Bic7trWu7TTW+w\n" \
    "ls0g/AR7JxlOvAIFdA/VixcalaonTjISD/rm+sPzNNPwC2gzxTyq+1P9uHVGIe1m\n" \
    "I+GA6PmFG28G6TwL81knzYzQppZEwatE0ripTWZ9t4hdsD7tmnZh2y+sZ+BvjTdV\n" \
    "JxlM3y7ATO95Tp4=\n"                                                 \
    "-----END CERTIFICATE-----\n"

/**
 *  client key to be updated by the user
 **/
#define CLIENT_KEY_PEM                                                   \
    "-----BEGIN PRIVATE KEY-----\n"                                      \
    "MIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQDR44/jlClWBg2G\n" \
    "3dTdnXB2cuWnjKSs0XxA/PjJULKFyQcD9Ol5fOPOSpaNfaqxRg/jbIhspUnReZYP\n" \
    "O4iAsZAkoUz8mBAeYDdvcFSM4D7/B4ii2mZuugRdsk3F0WxY9+gaZarE6t+oYqqC\n" \
    "Xk2k1524xp9zH/33W/1CKMtM4Amc1z/WKNg5RGnKdC/jkYrB/ieSPHD+EPHg9OLR\n" \
    "6960qeaUfUjgIfLSHlsZulQ9f6yvCBRTfBfTL/0vqe+YTxMKz5YVeV5kfQUhQ9mG\n" \
    "pHOn24hAp0bclAYDKSopyvlwvfBCN4lofqzSTjTUkclDLCl2yKSvikAfNCBP/o9z\n" \
    "xD8kuxtBAgMBAAECggEADnGYj1K83XyhJazSX0XcXWpev38APhxUbqx5+nChOG+z\n" \
    "GeSaxI+BpcqgPbBuNLDbmwz2/nMJd27kkrvIEwMlJkArah6UzWJVrkeOqfssmqRM\n" \
    "FNjuv6PxdPk2POzChgTZZiek7tQy/AV+MmaG9VcZNYQFqQiEk6cy8ICaLPol7zMI\n" \
    "8StwpadhF03SfOWv44RH8pdCFQEEpZS0/8R8kulF6WDc/HZOZx0XN0ExSDV+gi3N\n" \
    "MSOpaHVANuwJI0CHhcrM+sObTy0cwpVIvZK5ydWxsVNLhOBf8YOwp9moZkoOh6Fp\n" \
    "oJEYw43GcEWmOyjDQ+iVOMVRBm4Q5Ed6Gv/GUdkIgQKBgQD0LxhditcBHfrln3gc\n" \
    "7kUSGL1HruYCEDHCtt9LADzdC+iavuGJpZklEM7Y8k6Pp4WUQhYIeJl32dpGWgMh\n" \
    "4EbhoxXSLCa1E3bX1k5V9lcuWrcBI7qKZM02WbpidGGQFUWF6lUML1t4ugjUIrE3\n" \
    "gcOZ0eV7ghciA2jhwdfUVQuiSQKBgQDcC6BTkQXU0yJXZOX1FMNx8sl7fKEF60AV\n" \
    "hVaTgw0PvY2e9K0egc6Ho9vl7ZpPkRXHTv6bcAHDpqCLbTOahycHdWu3p+imk981\n" \
    "hXQ8imFMcb70UIaQDzluVTZhLX3RuARyaI06MxhP6UKLDFRvl3u52GQR8GEInEI3\n" \
    "Jbnv4nQxOQKBgQDpa3unL+mHr9j/WAnB3iM4wV1vAMnufvySXa3mTDviMlgQVRfU\n" \
    "NPDIdeDFkht3R3Rx3RqizmYmI6J8Td9gqrwXRYa+emYmAo1AliQcGHgOX7lA5Z6U\n" \
    "XPeuVOfO5eG9khf7g535H+wY4/08jE1L+ke46uSrxBfI8+l2pY5j80hDwQKBgQC1\n" \
    "vSR94JlgXcFPTRZghPWnd07Rnu2fZd1X6PjIfMSc64G8d3lEaRGbFcrY7JU2gi3q\n" \
    "n2PKHYhXf5e5kEe+qUGi92gwF44fhqIkIkUOe3xBFvEP+DIraWeVqOtw95GjVMjX\n" \
    "dWKvXuQFnDJJhbV3NTYLGJ4N8usxoV1PDumzJuWwgQKBgGDvZs1eQ6mkbAoxydRT\n" \
    "6Vp324k/KHmS/yv3uvRiGEYeMePwTo0kszZCpEW0z3r5IiqIwV8x9aUb+SAoBEIC\n" \
    "Z1gUZFeHkT75SLmHhr0xs27OogMCqEvIVQqDju7pvCp0atbcoO+UdaqT7X2MQ2bs\n" \
    "D5OdQJo5LRFBMCBoQJCazsed\n"                                         \
    "-----END PRIVATE KEY-----\n"

/**
 *  @brief Trusted ROOT certificate can be update by following the process specified in the mark down file
 **/
#define HTTPS_TRUSTED_ROOT_CA                                            \
    "-----BEGIN CERTIFICATE-----\n"                                      \
    "MIIDjjCCAnagAwIBAgIQAzrx5qcRqaC7KGSxHQn65TANBgkqhkiG9w0BAQsFADBh\n" \
    "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
    "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBH\n" \
    "MjAeFw0xMzA4MDExMjAwMDBaFw0zODAxMTUxMjAwMDBaMGExCzAJBgNVBAYTAlVT\n" \
    "MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n" \
    "b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IEcyMIIBIjANBgkqhkiG\n" \
    "9w0BAQEFAAOCAQ8AMIIBCgKCAQEAuzfNNNx7a8myaJCtSnX/RrohCgiN9RlUyfuI\n" \
    "2/Ou8jqJkTx65qsGGmvPrC3oXgkkRLpimn7Wo6h+4FR1IAWsULecYxpsMNzaHxmx\n" \
    "1x7e/dfgy5SDN67sH0NO3Xss0r0upS/kqbitOtSZpLYl6ZtrAGCSYP9PIUkY92eQ\n" \
    "q2EGnI/yuum06ZIya7XzV+hdG82MHauVBJVJ8zUtluNJbd134/tJS7SsVQepj5Wz\n" \
    "tCO7TG1F8PapspUwtP1MVYwnSlcUfIKdzXOS0xZKBgyMUNGPHgm+F6HmIcr9g+UQ\n" \
    "vIOlCsRnKPZzFBQ9RnbDhxSJITRNrw9FDKZJobq7nMWxM4MphQIDAQABo0IwQDAP\n" \
    "BgNVHRMBAf8EBTADAQH/MA4GA1UdDwEB/wQEAwIBhjAdBgNVHQ4EFgQUTiJUIBiV\n" \
    "5uNu5g/6+rkS7QYXjzkwDQYJKoZIhvcNAQELBQADggEBAGBnKJRvDkhj6zHd6mcY\n" \
    "1Yl9PMWLSn/pvtsrF9+wX3N3KjITOYFnQoQj8kVnNeyIv/iPsGEMNKSuIEyExtv4\n" \
    "NeF22d+mQrvHRAiGfzZ0JFrabA0UWTW98kndth/Jsw1HKj2ZL7tcu7XUIOGZX1NG\n" \
    "Fdtom/DzMNU+MeKNhJ7jitralj41E6Vf8PlwUHBHQRFXGU7Aj64GxJUTFy8bJZ91\n" \
    "8rGOmaFvE7FBcf6IKshPECBV1/MUReXgRPTqh5Uykw7+U0b6LJ3/iyK5S9kJRaTe\n" \
    "pLiaWN0bfVKfjllDiIGknibVb63dDcY3fe0Dkhvld1927jyNxF1WW6LZZm6zNTfl\n" \
    "MrY=\n"                                                             \
    "-----END CERTIFICATE-----\n"                                        \


/**
 *  End of User Configurable Settings
 **/

/* Set up macro for debugging */
#define DEBUG_HTTPS       (1)

/* ENABLE, DIABLE MACROs */
#define ENABLE            (1)
#define DISABLE           (0)

/* Ethernet setup macros */
#define USR_PING_COUNT    (50)
#define PING_DELAY        (10)
#define TASK_DELAY        (100)
#define BUFF_SIZE         (16)
#define EP_INFO                                                                                                    \
    "\r\n This project demonstrates the basic functionality of AWS https client library with Ethernet driver"      \
    "\r\n on Renesas RA MCUs based on Renesas FSP using FreeRTOS. AWS client is used to connect to https adafruit" \
    "\r\n server which is cloud platform. On successful connection, menu is displayed enabling user to send"       \
    "\r\n GET,PUT,POST requests to adafruit server. On POST/PUT request, ADC output is read and uploaded to "      \
    "\r\n adafruit server. On GET request, the last updated data is received from adafruit. JlinkRTTViewer is "    \
    "\r\n used to display the status and responses of the requests made to server.\r\n"

#define PRINT_MENU        "\r\nSelect from the below menu options " \
                          "\r\n 1. GET Request"                     \
                          "\r\n 2. PUT Request"                     \
                          "\r\n 3. POST Request\r\n"

typedef struct st_ping_data
{
    uint32_t sent;                     // Ping Request
    uint32_t received;                 // Ping Response
    uint32_t lost;                     // Ping failure
} ping_data_t;

typedef enum Userinput
{
    GET  = 1,
    PUT  = 2,
    POST = 3
} user_input_t;

#if (ipconfigDHCP_REGISTER_HOSTNAME == 1)

/* DHCP has an option for clients to register their hostname.  It doesn't
 * have much use, except that a device can be found in a router along with its
 * name. If this option is used the callback below must be provided by the
 * application writer to return a const string, denoting the device's name. */
const char * pcApplicationHostnameHook(void);

#endif                                 /* ipconfigDHCP_REGISTER_HOSTNAME */

/*
 * function declarations
 */
void                  print_ipconfig(void);
void                  print_pingResult(void);
void                  dnsQuerryFunc(char * domain_name);
void                  updateDhcpResponseToUsr(void);
BaseType_t            vSendPing(const char * pcIPAddress);
BaseType_t            provision_alt_key(void);
eDHCPCallbackAnswer_t xApplicationDHCPHook(eDHCPCallbackPhase_t eDHCPPhase, uint32_t lulIPAddress);
HTTPStatus_t          connect_aws_https_client(NetworkContext_t * NetworkContext);
HTTPStatus_t          add_header(HTTPRequestHeaders_t * pRequestHeaders);

#endif                                 /* USER_APP_H_ */
