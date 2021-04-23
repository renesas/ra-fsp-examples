/***********************************************************************************************************************
 * File Name    : user_app.h
 * Description  : Contains macros, data structures and functions used  in the Application
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

#ifndef USER_APP_H_
#define USER_APP_H_

#include "FreeRTOS_DHCP.h"


/******************************************************************************
 Macro definitions
 ******************************************************************************/

/**
 *  User Configurable Settings
 **/

/** User has to update according to their LAN/WAN test setup for testing of ping IP address **/
#define IOT_DEMO_TEST_PING_IP                       "192.168.0.51"

/** HOST Address **/
#define IOT_DEMO_HOST_ADDRESS                       "io.adafruit.com"

/**
 *  @brief User has to be update according to their credentials viz., {user_name} and {feed_key} in the respective
 *  GET, PUT and POST URLs in the specified formats.
 *  For more information of obtaining user credentials can follow the steps provided in mark down file.
 **/

/** @brief To get the most recent value. Use GET url: https://io.adafruit.com/api/v2/{username}/feeds/{feed_key}/data?limit=1 **/
#define IOT_DEMO_HTTPS_PRESIGNED_GET_URL   "https://io.adafruit.com/api/v2/revanth_t/feeds/temp/data?limit=1"

/** @brief IOT_DEMO_HTTPS_PRESIGNED_PUT_POST_URL can be used in PUT and POST methods.
 *  PUT method will update data point at requested <id>.
 *  PUT url: https://ioadafruit.com/{user_name}/{feeds}/{feed_key}/data/{id} , where the <id> will append to the url in the process of
 *  PUT request, once it fetch from the GET request.
 *  POST method will send new data to the server.
 *  POST url:https://ioadafruit.com/{user_name}/{feeds}/{feed_key}/data/
 **/
#define IOT_DEMO_HTTPS_PRESIGNED_PUT_POST_URL    "https://io.adafruit.com/api/v2/revanth_t/feeds/temp/data/"

/** @brief User has to update their generated active key from the io.adafruit.com server. */
#define ACTIVE_KEY                             "aio_TWoG86KKxqAunvBjI0qbXJPHXr52"

/** @brief If the privacy of feed data visibility is set to private then 'ENABLE' the PRIVATE_KEY for processing of GET request **/
#define PRIVATE_KEY                        DISABLE


/**
 *  Client certificate to be updated by the user by following the process specified in the mark down file
 **/

#define CLIENT_CERTIFICATE_PEM                                                  \
        "-----BEGIN CERTIFICATE-----\n"                                         \
        "MIIDqTCCApGgAwIBAgIULy6LArtEWFKapw5tm89xTJt+46gwDQYJKoZIhvcNAQEL\n"    \
        "BQAwZDELMAkGA1UEBhMCSU4xCzAJBgNVBAgMAktBMRIwEAYDVQQHDAlCZW5nYWx1\n"    \
        "cnUxDDAKBgNVBAoMA3RhdDEMMAoGA1UECwwDb2RjMRgwFgYDVQQDDA9pby5hZGFm\n"    \
        "cnVpdC5jb20wHhcNMjEwMjE1MDY0OTM1WhcNMjIwMjE1MDY0OTM1WjBkMQswCQYD\n"    \
        "VQQGEwJJTjELMAkGA1UECAwCS0ExEjAQBgNVBAcMCUJlbmdhbHVydTEMMAoGA1UE\n"    \
        "CgwDdGF0MQwwCgYDVQQLDANvZGMxGDAWBgNVBAMMD2lvLmFkYWZydWl0LmNvbTCC\n"    \
        "ASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBANI3+Ea313G+d1/SPqLo4H9O\n"    \
        "Pi3OygIad3dIy/+Pd8QZ/1M1CA5hMxbNXMAwWXnx0mj6Cd/sj/UycoDueM0p9MqB\n"    \
        "M5KxI+RYdS9jdljohI11Po3bBtSJ2EmXsUMLABjxto/4I55YYHz+5LNFKXa0dh9l\n"    \
        "l2EsKKzEtBE4j1P6WT2+qbeFdMgO6PAuWJzLxO7BKS33tr/Xvozhnps5S2ySLIcL\n"    \
        "5yqHvsSbmSygKXuhdGiABvj8ZBexKq+pYU+0uNnYc8e9pVFDCpOPGxZSKkfF8GDK\n"    \
        "9Tc3akgOrdALcaZjFqYd4v+ZRnRgTwhLIeDUH4L/VftGahYWoa23BhwLkvqgXysC\n"    \
        "AwEAAaNTMFEwHQYDVR0OBBYEFEOuwHA4E3PlnjWXea2Eky0siSG3MB8GA1UdIwQY\n"    \
        "MBaAFEOuwHA4E3PlnjWXea2Eky0siSG3MA8GA1UdEwEB/wQFMAMBAf8wDQYJKoZI\n"    \
        "hvcNAQELBQADggEBAEfIIUyBofOdmWJ6whLA5N7NfQugXDy9x+fGB6vQKDPB5zPX\n"    \
        "oQ0YyDOJs2jyLs/dRssjoModdtHso1ybJRnB/AdS090NnjluM3sEEy/vWUdQw+P/\n"    \
        "MjqsMCipk6bkoDDxq1N0pIxbIRy3sD4rlg1SLUQLPHXOeHcmTPuE0zul/qibOqKQ\n"    \
        "vpXax6bU1EFxOZGIIF2QyR3kgZSveCw3H3SfssMh7BRGqQdChxLz/VUMb6TCK+sY\n"    \
        "E34Pe6XASob3g6/wjdd74BrjmPpuj8pD0Ugx4SnDZ/TFuH6AQ3UmZyP6A3IZ45Al\n"    \
        "vcgEn2MSMQdXA6t24DtAQM4eNm9i7zJCiVeXPGI=\n"                            \
        "-----END CERTIFICATE-----\n"

/**
 *  client key to be updated by the user
 **/

#define CLIENT_KEY_PEM                                                          \
        "-----BEGIN PRIVATE KEY-----\n"                                         \
        "MIIEvwIBADANBgkqhkiG9w0BAQEFAASCBKkwggSlAgEAAoIBAQDSN/hGt9dxvndf\n"    \
        "0j6i6OB/Tj4tzsoCGnd3SMv/j3fEGf9TNQgOYTMWzVzAMFl58dJo+gnf7I/1MnKA\n"    \
        "7njNKfTKgTOSsSPkWHUvY3ZY6ISNdT6N2wbUidhJl7FDCwAY8baP+COeWGB8/uSz\n"    \
        "RSl2tHYfZZdhLCisxLQROI9T+lk9vqm3hXTIDujwLlicy8TuwSkt97a/176M4Z6b\n"    \
        "OUtskiyHC+cqh77Em5ksoCl7oXRogAb4/GQXsSqvqWFPtLjZ2HPHvaVRQwqTjxsW\n"    \
        "UipHxfBgyvU3N2pIDq3QC3GmYxamHeL/mUZ0YE8ISyHg1B+C/1X7RmoWFqGttwYc\n"    \
        "C5L6oF8rAgMBAAECggEBALupnWk8KwduedC/tozmCat8JMPt66YPDIEfKoWdBNS6\n"    \
        "RJV6LRPxL5N2hwtG3kmfWC/ArtoIq8a/Xzq8KHk2sEGrL2gP3LO3R/2i8/6Qq+xA\n"    \
        "MzSdx64q1Dt2QJdSFacu+MPzR5DO4mwTfASRhF9k4GJM5eOocTMycnFBJapZPhuA\n"    \
        "HxJAsJHTggICyYhwTSfZuTJe0i8WJ4GKMDSSrUelOVFKYuBMWq0kW10MiNHOz341\n"    \
        "wrcMXiBhaF9Cq8WtkW/b8tZf2lb9GEC0BCF0zmYRDm+21+J9zRuiQlr5L067eR3S\n"    \
        "LFCHNYhzVhfrk7isrjeEYkwsI+16fh+cGNvI0y0d+AECgYEA/A+DBFNiZANj4Flh\n"    \
        "hDT0lMEwDqemZ4hX1OO5WD0akPzZ5b2VUGiuBIYP2mNFyGv4hnt5IjwuZSauaasK\n"    \
        "hQ0d0k4olau1wW72TBsJg9qGYsGbag4hyzQsNSKWqGAyBYbh5n+E2LPZ3ciznvk/\n"    \
        "NF6OQsFbonyKJuLhzHSD7yGL+ysCgYEA1YEMpOjJeDT5JtSnYPF/dT0q+4I+tZ4B\n"    \
        "AT7SswguFlrEuwoe9VED0fKt6l17iMitHICLcqJqGAJ2ag/NX6jpiz2Y5B9SOdVS\n"    \
        "Py5aUL8wnMV/gbz3gDkGIQSsrzr8iY6teCijVHLrHbBj0qVrYWqgDamMQ6XlJoU2\n"    \
        "8y4UVzU7LAECgYEAgMRL4kHW2dEBjPMcZjXHXNb0fggXcnysog1T+kf88mS5zCv1\n"    \
        "w4Y2oAj8wExGsgS2ib06PMsRyIriPwkp7p6sxWs4QH3f9FmDaY3GZp2UWY8enidq\n"    \
        "DC0zCFILNSkvZLNrZB1w+GOowSxu1p/uMgq8Ek5FSdTlnsf5IkHDxL4WjNECgYEA\n"    \
        "wDscJlLOKyQO/G5VccyehPGZBLDFQ4LKxtNIpBeGX2xEW1+GdjwGY37MJgsbUhHV\n"    \
        "A6edZo5jVdDJyz8FG88az9qbETI+N4IhNm/Gs5Mg8tZDso1wjotJQGtUC8XhYP0M\n"    \
        "pPZ58Gwz0xxhFGUKJMD68LIqbzIr+x3QVu2zNL3h1AECgYB2yWubUseGL3E8cBMV\n"    \
        "vfbf2LvFU+hfRfV21C7o5Zr7mCz3Ez91q/T3/FVH/K7Qp5/VyXyf1klvfcTHeOBT\n"    \
        "rxWgVfQFxp23OhLSxEIYlqTi4tq7ED3gNrOKaRn8f+F3O1MtIJeKED+nTPUjGjd0\n"    \
        "pK1gR3xk/pFBJmbQthNGsfscaQ==\n"                                        \
        "-----END PRIVATE KEY-----\n"

/**
 *  @brief Trusted ROOT certificate can be update by following the process specified in the mark down file
 **/

#define IOT_DEMO_HTTPS_TRUSTED_ROOT_CA                                \
"-----BEGIN CERTIFICATE-----\n"                                       \
"MIIEizCCA3OgAwIBAgIQBUb+GCP34ZQdo5/OFMRhczANBgkqhkiG9w0BAQsFADBh\n"  \
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"  \
"d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n"  \
"QTAeFw0xNzExMDYxMjIzNDVaFw0yNzExMDYxMjIzNDVaMF4xCzAJBgNVBAYTAlVT\n"  \
"MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n"  \
"b20xHTAbBgNVBAMTFEdlb1RydXN0IFJTQSBDQSAyMDE4MIIBIjANBgkqhkiG9w0B\n"  \
"AQEFAAOCAQ8AMIIBCgKCAQEAv4rRY03hGOqHXegWPI9/tr6HFzekDPgxP59FVEAh\n"  \
"150Hm8oDI0q9m+2FAmM/n4W57Cjv8oYi2/hNVEHFtEJ/zzMXAQ6CkFLTxzSkwaEB\n"  \
"2jKgQK0fWeQz/KDDlqxobNPomXOMJhB3y7c/OTLo0lko7geG4gk7hfiqafapa59Y\n"  \
"rXLIW4dmrgjgdPstU0Nigz2PhUwRl9we/FAwuIMIMl5cXMThdSBK66XWdS3cLX18\n"  \
"4ND+fHWhTkAChJrZDVouoKzzNYoq6tZaWmyOLKv23v14RyZ5eqoi6qnmcRID0/i6\n"  \
"U9J5nL1krPYbY7tNjzgC+PBXXcWqJVoMXcUw/iBTGWzpwwIDAQABo4IBQDCCATww\n"  \
"HQYDVR0OBBYEFJBY/7CcdahRVHex7fKjQxY4nmzFMB8GA1UdIwQYMBaAFAPeUDVW\n"  \
"0Uy7ZvCj4hsbw5eyPdFVMA4GA1UdDwEB/wQEAwIBhjAdBgNVHSUEFjAUBggrBgEF\n"  \
"BQcDAQYIKwYBBQUHAwIwEgYDVR0TAQH/BAgwBgEB/wIBADA0BggrBgEFBQcBAQQo\n"  \
"MCYwJAYIKwYBBQUHMAGGGGh0dHA6Ly9vY3NwLmRpZ2ljZXJ0LmNvbTBCBgNVHR8E\n"  \
"OzA5MDegNaAzhjFodHRwOi8vY3JsMy5kaWdpY2VydC5jb20vRGlnaUNlcnRHbG9i\n"  \
"YWxSb290Q0EuY3JsMD0GA1UdIAQ2MDQwMgYEVR0gADAqMCgGCCsGAQUFBwIBFhxo\n"  \
"dHRwczovL3d3dy5kaWdpY2VydC5jb20vQ1BTMA0GCSqGSIb3DQEBCwUAA4IBAQAw\n"  \
"8YdVPYQI/C5earp80s3VLOO+AtpdiXft9OlWwJLwKlUtRfccKj8QW/Pp4b7h6QAl\n"  \
"ufejwQMb455OjpIbCZVS+awY/R8pAYsXCnM09GcSVe4ivMswyoCZP/vPEn/LPRhH\n"  \
"hdgUPk8MlD979RGoUWz7qGAwqJChi28uRds3thx+vRZZIbEyZ62No0tJPzsSGSz8\n"  \
"nQ//jP8BIwrzBAUH5WcBAbmvgWfrKcuv+PyGPqRcc4T55TlzrBnzAzZ3oClo9fTv\n"  \
"O9PuiHMKrC6V6mgi0s2sa/gbXlPCD9Z24XUMxJElwIVTDuKB0Q4YMMlnpN/QChJ4\n"  \
"B0AFsQ+DU0NCO+f78Xf7\n"  \
"-----END CERTIFICATE-----\n"


/**
 *  End of User Configurable Settings
 **/


/* ENABLE, DIABLE MACROs */
#define ENABLE      (1)
#define DISABLE     (0)

/* Ethernet setup macros */
#define SUCCESS                     0
#define ETHERNET_LINK_DOWN          0x01
#define ETHERNET_LINK_UP            0x00
#define IP_LINK_DOWN                0x02
#define IP_LINK_UP                  0x00
#define USR_PING_COUNT              50

#define EP_INFO                 "\r\n This project demonstrates the basic functionality of AWS https client library with Ethernet driver"\
		                        "\r\n on Renesas RA MCUs based on Renesas FSP using FreeRTOS. AWS client is used to connect to https adafruit"\
		                        "\r\n server which is cloud platform. On successful connection, menu is displayed enabling user to send"\
                                "\r\n GET,PUT,POST requests to adafruit server. On POST/PUT request, ADC output is read and uploaded to "\
                                "\r\n adafruit server. On GET request, the last updated data is received from adafruit. JlinkRTTViewer is "\
                                "\r\n used to display the status and responses of the requests made to server.\r\n"

#define PRINT_MENU              "\r\nSelect from the below menu options "\
                                "\r\n 1. GET Request"\
                                "\r\n 2. PUT Request"\
                                "\r\n 3. POST Request\r\n"


typedef struct st_ping_data
{
    uint32_t sent;     // Ping Request
    uint32_t received; // Ping Response
    uint32_t lost;     // Ping failure
} ping_data_t;

typedef enum Userinput
{
    GET = 1,
    PUT = 2,
    POST = 3
}Userinput_t;

#if( ipconfigDHCP_REGISTER_HOSTNAME == 1 )
    /* DHCP has an option for clients to register their hostname.  It doesn't
    have much use, except that a device can be found in a router along with its
    name. If this option is used the callback below must be provided by the
    application writer to return a const string, denoting the device's name. */
    const char *pcApplicationHostnameHook( void );
#endif /* ipconfigDHCP_REGISTER_HOSTNAME */

void print_ipconfig(void);
void print_pingResult(void);
void dnsQuerryFunc(char *domain_name);
void updateDhcpResponseToUsr(void);
BaseType_t vSendPing( const char *pcIPAddress);
BaseType_t network_init(void);
BaseType_t checkEthernetLinkedConnectivity(void);
eDHCPCallbackAnswer_t  xApplicationDHCPHook(eDHCPCallbackPhase_t eDHCPPhase, uint32_t lulIPAddress);
uint32_t isNetworkUp(void);



#endif /* USER_APP_H_ */
