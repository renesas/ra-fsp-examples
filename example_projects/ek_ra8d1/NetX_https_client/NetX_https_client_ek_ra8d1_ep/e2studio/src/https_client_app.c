/***********************************************************************************************************************
 * File Name    : https_client_app.c
 * Description  : This file contains the User Application code for the https_client connection
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/* Include Header Files. */
#include "common_utils.h"
#include "https_client_app.h"
#include "adc_app.h"
#include "user_app.h"

/*******************************************************************************************************************//**
 * @addtogroup NetX_https_client_ep
 * @{
 **********************************************************************************************************************/

/* Exported global variable. */
/* Web HTTP Client instance. */
extern NX_WEB_HTTP_CLIENT  g_web_http_client0;

/* Global Variables.*/
/* Buffer to read Response from server*/
UCHAR g_receive_buffer[DATA_SIZE] = {NULL_CHAR};

/* Declarations to setup TLS. */
/* Certificate in .der converted to c array of hex format. */
unsigned char ca_cert_der[] = {0x30,0x82,0x04,0x8d,0x30,0x82,0x03,0x75,0xa0,0x03,0x02,0x01,
                               0x02,0x02,0x10,0x0d,0x07,0x78,0x2a,0x13,0x3f,0xc6,0xf9,0xa5,
                               0x72,0x96,0xe1,0x31,0xff,0xd1,0x79,0x30,0x0d,0x06,0x09,0x2a,
                               0x86,0x48,0x86,0xf7,0x0d,0x01,0x01,0x0b,0x05,0x00,0x30,0x61,
                               0x31,0x0b,0x30,0x09,0x06,0x03,0x55,0x04,0x06,0x13,0x02,0x55,
                               0x53,0x31,0x15,0x30,0x13,0x06,0x03,0x55,0x04,0x0a,0x13,0x0c,
                               0x44,0x69,0x67,0x69,0x43,0x65,0x72,0x74,0x20,0x49,0x6e,0x63,
                               0x31,0x19,0x30,0x17,0x06,0x03,0x55,0x04,0x0b,0x13,0x10,0x77,
                               0x77,0x77,0x2e,0x64,0x69,0x67,0x69,0x63,0x65,0x72,0x74,0x2e,
                               0x63,0x6f,0x6d,0x31,0x20,0x30,0x1e,0x06,0x03,0x55,0x04,0x03,
                               0x13,0x17,0x44,0x69,0x67,0x69,0x43,0x65,0x72,0x74,0x20,0x47,
                               0x6c,0x6f,0x62,0x61,0x6c,0x20,0x52,0x6f,0x6f,0x74,0x20,0x47,
                               0x32,0x30,0x1e,0x17,0x0d,0x31,0x37,0x31,0x31,0x30,0x32,0x31,
                               0x32,0x32,0x33,0x33,0x37,0x5a,0x17,0x0d,0x32,0x37,0x31,0x31,
                               0x30,0x32,0x31,0x32,0x32,0x33,0x33,0x37,0x5a,0x30,0x60,0x31,
                               0x0b,0x30,0x09,0x06,0x03,0x55,0x04,0x06,0x13,0x02,0x55,0x53,
                               0x31,0x15,0x30,0x13,0x06,0x03,0x55,0x04,0x0a,0x13,0x0c,0x44,
                               0x69,0x67,0x69,0x43,0x65,0x72,0x74,0x20,0x49,0x6e,0x63,0x31,
                               0x19,0x30,0x17,0x06,0x03,0x55,0x04,0x0b,0x13,0x10,0x77,0x77,
                               0x77,0x2e,0x64,0x69,0x67,0x69,0x63,0x65,0x72,0x74,0x2e,0x63,
                               0x6f,0x6d,0x31,0x1f,0x30,0x1d,0x06,0x03,0x55,0x04,0x03,0x13,
                               0x16,0x47,0x65,0x6f,0x54,0x72,0x75,0x73,0x74,0x20,0x54,0x4c,
                               0x53,0x20,0x52,0x53,0x41,0x20,0x43,0x41,0x20,0x47,0x31,0x30,
                               0x82,0x01,0x22,0x30,0x0d,0x06,0x09,0x2a,0x86,0x48,0x86,0xf7,
                               0x0d,0x01,0x01,0x01,0x05,0x00,0x03,0x82,0x01,0x0f,0x00,0x30,
                               0x82,0x01,0x0a,0x02,0x82,0x01,0x01,0x00,0xbe,0x17,0xe8,0xec,
                               0xbe,0x29,0x0a,0xcb,0xfe,0xb9,0x2d,0x61,0x31,0xfd,0x33,0x24,
                               0x08,0x32,0x2e,0x59,0xe8,0x21,0xd4,0xd8,0x30,0xbe,0x6e,0x10,
                               0xc8,0x84,0xa0,0x3f,0xba,0x14,0xe5,0xde,0xfd,0x7a,0x8c,0x92,
                               0x1b,0x7b,0xce,0x84,0x2d,0xf0,0xff,0x78,0xc4,0x32,0xe8,0xa9,
                               0xa0,0x7d,0x5f,0x06,0xda,0x7b,0x9b,0x4b,0x53,0xa6,0xc6,0x1b,
                               0x02,0x17,0x21,0xe1,0x70,0x3b,0xad,0xfb,0x83,0xeb,0x08,0x54,
                               0x81,0xa8,0xde,0x12,0xb2,0xd5,0xc6,0x88,0x96,0x30,0xf9,0x02,
                               0xfc,0x39,0xd4,0xbd,0xb8,0x22,0xef,0x80,0x49,0x99,0xd0,0x62,
                               0xb8,0x61,0xd0,0x49,0xde,0xcb,0xc2,0xcb,0x97,0xa5,0x31,0x06,
                               0x1b,0xd7,0xd8,0x5d,0xc6,0xd3,0x54,0xde,0x52,0x01,0x36,0x2a,
                               0x0d,0xf6,0xde,0xc5,0xb6,0x31,0x4c,0xcc,0x15,0x25,0x6a,0x15,
                               0x6f,0xa9,0x6b,0x04,0x48,0x0c,0xde,0x00,0x41,0xaa,0x28,0x80,
                               0x8b,0x2f,0x34,0xd3,0x1b,0xb5,0x36,0xad,0x3b,0x25,0xd0,0x88,
                               0x42,0x40,0x6c,0x36,0x91,0x6d,0x65,0xb2,0x19,0x86,0xc0,0xd2,
                               0x7f,0x39,0x46,0x58,0xfe,0x30,0x12,0x60,0x50,0xdc,0xee,0xbb,
                               0x73,0xe6,0x57,0x90,0x5a,0xf6,0x0d,0xca,0xd7,0x04,0x4b,0x47,
                               0x6a,0x6f,0x34,0x1a,0x9d,0x92,0x36,0x1a,0x2e,0xd9,0x4e,0x54,
                               0xed,0x47,0xac,0x0c,0xbf,0xf1,0x80,0xb2,0xba,0xff,0x47,0x7b,
                               0xe9,0x39,0xc4,0x54,0xc4,0x94,0x54,0x99,0x19,0xf1,0x57,0x99,
                               0xaf,0xe2,0x14,0x22,0x5b,0xe8,0x2e,0xbb,0x63,0x2d,0xba,0xae,
                               0x81,0xbd,0x13,0xdc,0xe6,0x17,0x5b,0xe0,0x90,0x53,0x49,0x01,
                               0x02,0x03,0x01,0x00,0x01,0xa3,0x82,0x01,0x40,0x30,0x82,0x01,
                               0x3c,0x30,0x1d,0x06,0x03,0x55,0x1d,0x0e,0x04,0x16,0x04,0x14,
                               0x94,0x4f,0xd4,0x5d,0x8b,0xe4,0xa4,0xe2,0xa6,0x80,0xfe,0xfd,
                               0xd8,0xf9,0x00,0xef,0xa3,0xbe,0x02,0x57,0x30,0x1f,0x06,0x03,
                               0x55,0x1d,0x23,0x04,0x18,0x30,0x16,0x80,0x14,0x4e,0x22,0x54,
                               0x20,0x18,0x95,0xe6,0xe3,0x6e,0xe6,0x0f,0xfa,0xfa,0xb9,0x12,
                               0xed,0x06,0x17,0x8f,0x39,0x30,0x0e,0x06,0x03,0x55,0x1d,0x0f,
                               0x01,0x01,0xff,0x04,0x04,0x03,0x02,0x01,0x86,0x30,0x1d,0x06,
                               0x03,0x55,0x1d,0x25,0x04,0x16,0x30,0x14,0x06,0x08,0x2b,0x06,
                               0x01,0x05,0x05,0x07,0x03,0x01,0x06,0x08,0x2b,0x06,0x01,0x05,
                               0x05,0x07,0x03,0x02,0x30,0x12,0x06,0x03,0x55,0x1d,0x13,0x01,
                               0x01,0xff,0x04,0x08,0x30,0x06,0x01,0x01,0xff,0x02,0x01,0x00,
                               0x30,0x34,0x06,0x08,0x2b,0x06,0x01,0x05,0x05,0x07,0x01,0x01,
                               0x04,0x28,0x30,0x26,0x30,0x24,0x06,0x08,0x2b,0x06,0x01,0x05,
                               0x05,0x07,0x30,0x01,0x86,0x18,0x68,0x74,0x74,0x70,0x3a,0x2f,
                               0x2f,0x6f,0x63,0x73,0x70,0x2e,0x64,0x69,0x67,0x69,0x63,0x65,
                               0x72,0x74,0x2e,0x63,0x6f,0x6d,0x30,0x42,0x06,0x03,0x55,0x1d,
                               0x1f,0x04,0x3b,0x30,0x39,0x30,0x37,0xa0,0x35,0xa0,0x33,0x86,
                               0x31,0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x63,0x72,0x6c,0x33,
                               0x2e,0x64,0x69,0x67,0x69,0x63,0x65,0x72,0x74,0x2e,0x63,0x6f,
                               0x6d,0x2f,0x44,0x69,0x67,0x69,0x43,0x65,0x72,0x74,0x47,0x6c,
                               0x6f,0x62,0x61,0x6c,0x52,0x6f,0x6f,0x74,0x47,0x32,0x2e,0x63,
                               0x72,0x6c,0x30,0x3d,0x06,0x03,0x55,0x1d,0x20,0x04,0x36,0x30,
                               0x34,0x30,0x32,0x06,0x04,0x55,0x1d,0x20,0x00,0x30,0x2a,0x30,
                               0x28,0x06,0x08,0x2b,0x06,0x01,0x05,0x05,0x07,0x02,0x01,0x16,
                               0x1c,0x68,0x74,0x74,0x70,0x73,0x3a,0x2f,0x2f,0x77,0x77,0x77,
                               0x2e,0x64,0x69,0x67,0x69,0x63,0x65,0x72,0x74,0x2e,0x63,0x6f,
                               0x6d,0x2f,0x43,0x50,0x53,0x30,0x0d,0x06,0x09,0x2a,0x86,0x48,
                               0x86,0xf7,0x0d,0x01,0x01,0x0b,0x05,0x00,0x03,0x82,0x01,0x01,
                               0x00,0x82,0x1c,0x04,0x3a,0x82,0xe9,0xc5,0xa9,0xc8,0x65,0x12,
                               0x5c,0x08,0xe3,0x01,0xc6,0x30,0xb0,0xae,0x22,0x88,0x61,0x7b,
                               0x2b,0x07,0x86,0xf7,0xb8,0xb5,0x44,0x9c,0xf5,0x93,0x06,0x61,
                               0x46,0x8a,0x3a,0xa2,0xb5,0x35,0x26,0xd0,0x58,0x9e,0x3c,0xc1,
                               0x73,0x8f,0xbf,0xb1,0x79,0xa7,0x1c,0x99,0x37,0x9a,0x53,0xf2,
                               0x2f,0x5c,0x6e,0x20,0x0b,0x7d,0xb0,0xc7,0x82,0xef,0xeb,0xeb,
                               0x79,0x12,0x05,0xe7,0x01,0x41,0x0b,0x43,0x02,0x37,0x9b,0x1b,
                               0x1f,0x08,0x4d,0x37,0x52,0x73,0x29,0xb8,0x9f,0x35,0xe3,0xf9,
                               0xa3,0x3a,0x59,0x75,0x01,0x66,0x90,0x2a,0x3e,0x9d,0xe0,0x62,
                               0x29,0x1e,0x87,0xa8,0x80,0x3e,0x2c,0xc4,0xcc,0x08,0xc3,0x2e,
                               0xbf,0x69,0x21,0x18,0x6b,0x8f,0x1e,0x6b,0x43,0xcd,0xee,0x06,
                               0x98,0x41,0x99,0xb5,0x82,0xf2,0x17,0xc1,0x10,0xa5,0xae,0x58,
                               0x8c,0xfb,0x95,0xea,0x4d,0x82,0xaf,0xf3,0x77,0x5f,0x11,0x69,
                               0x4f,0x78,0x51,0xa6,0xdf,0xf7,0xba,0xc5,0x3f,0x65,0xbf,0x70,
                               0x7c,0xa3,0xed,0x5a,0x9a,0x33,0x9d,0x9e,0x3f,0xe3,0x87,0x35,
                               0xa7,0xda,0xf3,0x15,0xc6,0x58,0xad,0x69,0x23,0xdc,0x0f,0xbb,
                               0x4c,0xd0,0x49,0x13,0x41,0xa6,0x3f,0x67,0xe1,0x5e,0x13,0xf5,
                               0x0f,0xf5,0xd5,0x7c,0x85,0xbf,0x87,0x4c,0x82,0x24,0x61,0x22,
                               0x57,0xd1,0xa5,0x9b,0xfb,0x86,0x39,0xad,0xc3,0x5c,0x79,0x66,
                               0x6c,0x07,0x31,0x4b,0x20,0x91,0xad,0xa1,0xbe,0x64,0x19,0x00,
                               0xee,0x1e,0x12,0x78,0xce,0x98,0xf2,0x5f,0xfb,0x30,0x14,0x69,
                               0x3c,0x2c,0xfa,0x97,0xc6};

/* Trusted Certificate Info. */
const trusted_certifcate_info_t g_trust_certs[] =
{
 [0]
  {
     .pointer_to_cert = (UCHAR *)(ca_cert_der),
     .sizeof_cert     = sizeof(ca_cert_der),
  },
};

/* Number of Trusted Certificates. */
#define NO_OF_TRUSTED_CERTS         (sizeof(g_trust_certs)/sizeof(trusted_certifcate_info_t))

/* TLS buffers and certificate containers. */
extern NX_SECURE_TLS_CRYPTO nx_crypto_tls_ciphers;
/* Certificate variable for NetX secure TLS. */
NX_SECURE_X509_CERT g_trusted_certificate[NO_OF_TRUSTED_CERTS];
/* Certificate variable for remote TLS host. */
NX_SECURE_X509_CERT g_remote_cert[NO_OF_REMOTE_CERTS];
/* TLS packet Buffer. */
static UCHAR g_tls_packet_buffer[SIZE_OF_TLS_PACKET_BUF] = {NULL_CHAR};
/* Remote certificate buffer. */
static UCHAR g_remote_cert_buffer[NO_OF_REMOTE_CERTS][SIZE_OF_REMOTE_CERT_BUF] = {NULL_CHAR};
/* Pointer to space for cryptography metadata in TLS session for secure communication. */
static CHAR g_tls_session_meta_data[SIZE_OF_META_BUFFER] = {NULL_CHAR};

/* Function Definitions. */
/*******************************************************************************************************************//**
 * @brief      Function to disconnect from the HTTP Client
 *
 * @param[in]  None
 * @retval     NX_SUCCESS                   Upon successful deletion of HTTP CLient.
 * @retval     Any other Error Code         Upon unsuccessful deletion of HTTP CLient.
 **********************************************************************************************************************/
void deinit_https_client(void)
{
    /* To check the return status of the API. */
    UINT status = NX_SUCCESS;
    /* Close the HTTPS client.*/
    status = nx_web_http_client_delete (&g_web_http_client0);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_web_http_client_delete API failed.");
    }
}

/*******************************************************************************************************************//**
 * @brief      Function to read response from the server.
 *
 * @param[in]  None
 * @retval     NX_SUCCESS                   Upon successful reading of data from the server.
 * @retval     Any other Error Code         Upon unsuccessful reading of data from the server.
 **********************************************************************************************************************/
UINT readResponsefromServer(void)
{
    /* Total number of bytes copied from server.*/
    ULONG bytes_copied          = RESET_VALUE;
    /* To check the return status of the APIs. */
    UINT status                 = NX_SUCCESS;
    /* To check the return status of the nx_web_http_client_response_body_get API. */
    UINT body_get_status        = NX_SUCCESS;
    /* Pointer to the receive data packet. */
    NX_PACKET *receive_packet   = NULL;

    /* Clear the receive buffer. */
    memset (g_receive_buffer, RESET_VALUE, sizeof(g_receive_buffer));

    /* Receive response data from the server. Check until all data is received. */
    while (NX_WEB_HTTP_GET_DONE != body_get_status)
    {
        /* Retrieve the next packet of content data from the server.
         * Successive calls to this routine should be made until the return status of NX_WEB_HTTP_GET_DONE is received. */
        body_get_status = nx_web_http_client_response_body_get (&g_web_http_client0, &receive_packet, NX_WAIT_FOREVER);
        if((NX_SUCCESS != body_get_status) && (NX_WEB_HTTP_GET_DONE != body_get_status))
        {
            PRINT_ERR_STR("nx_web_http_client_response_body_get API failed.");
            return body_get_status;
        }

        /* Write data contents to buffer. */
        status = nx_packet_data_extract_offset (receive_packet, RESET_VALUE, g_receive_buffer, BUFFER_LENGTH, &bytes_copied);
        if(NX_SUCCESS != status)
        {
            PRINT_ERR_STR("nx_packet_data_extract_offset API failed.");
            return status;
        }

        /* Release previously acquired packet.*/
        status = nx_packet_release(receive_packet);
        if(NX_SUCCESS != status)
        {
            PRINT_ERR_STR("nx_packet_release API failed.");
        }
    }
    return status;
}

/*******************************************************************************************************************//**
 * @brief      Function to send additional header to the server.
 *
 * @param[in]  http_request_ptr             Pointer to the http_request buffer.
 * @param[in]  size                         Size of the http_request buffer.
 * @retval     NX_SUCCESS                   Upon successful sending of the HTTPS Request.
 * @retval     Any other Error Code         Upon unsuccessful sending of the HTTPS Request.
 **********************************************************************************************************************/
UINT sendAdditionalHeadertoServer(CHAR *http_request_ptr, UINT size)
{
    /* To check the return status of the APIs. */
    UINT status                     = NX_SUCCESS;
    /* Data Buffer for HTTP request. */
    CHAR http_request[BUFFER_SIZE]  = {NULL_CHAR};
    /* Pointer to the HTTP data packet. */
    NX_PACKET *http_packet          = NULL;

    /* Clear the local buffer and copy the http request to local buffer. */
    memset (http_request, RESET_VALUE, sizeof(http_request));
    memcpy(http_request,http_request_ptr,size);

    /* Add AIO key header */
    status = nx_web_http_client_request_header_add (&g_web_http_client0,
                                                    X_AIO_KEY,
                                                    strlen (X_AIO_KEY),
                                                    IO_KEY,
                                                    strlen (IO_KEY),
                                                    NX_WAIT_FOREVER);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR ("nx_web_http_client_request_header_add (AIO Key Header) API failed.");
        return status;
    }

    /* Add Content type header. */
    status = nx_web_http_client_request_header_add (&g_web_http_client0,
                                                    X_CONTENT_TYPE,
                                                    strlen (X_CONTENT_TYPE),
                                                    CONTENT_TYPE,
                                                    strlen (CONTENT_TYPE), NX_WAIT_FOREVER);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_web_http_client_request_header_add (Content Type Header) API failed.");
        return status;
    }

    /* Send the HTTP request. */
    status = nx_web_http_client_request_send (&g_web_http_client0, NX_WAIT_FOREVER);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_web_http_client_request_send API failed.");
        return status;
    }

    /* Create a new data packet request on the HTTP(S) client instance. */
    status = nx_web_http_client_request_packet_allocate (&g_web_http_client0, &http_packet, NX_WAIT_FOREVER);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_web_http_client_request_packet_allocate API failed.");
        return status;
    }

    /* Append the Data packet to the request.  */
    status = nx_packet_data_append (http_packet, http_request, strlen (http_request), &g_packet_pool0, NX_WAIT_FOREVER);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_packet_data_append API failed.");
        return status;
    }

    /* Send the HTTPS request. */
    status = nx_web_http_client_request_packet_send (&g_web_http_client0, http_packet, RESET_VALUE, NX_WAIT_FOREVER);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_web_http_client_request_packet_send API failed.");
    }
    return status;
}

/*******************************************************************************************************************//**
 * @brief      Process GET request and read the response from the server
 *
 * @param[in]  None
 * @retval     NX_SUCCESS                   Upon successful execution of GET request
 *                                          from the Adafruit Server.
 *                                          Data received from the Adafruit server is
 *                                          displayed on the RTT Viewer.
 * @retval     Any other Error Code         Upon unsuccessful execution of GET request.
 **********************************************************************************************************************/
UINT processGetRequest()
{
    /* To check the return status of APIs. */
    UINT status                = NX_SUCCESS;
    /* Buffer to store the URL path for Adafruit IO Server. */
    CHAR url_path[BUFFER_SIZE] = {NULL_CHAR};

    PRINT_INFO_STR("Processing Get Request...");

    /* Clear the Resource for post buffer. */
    memset (url_path, RESET_VALUE, sizeof(url_path));
    /* Add Adafruit details in the buffer. */
    snprintf (url_path, BUFFER_SIZE, "%s%s%s%s%s", API_V2, IO_USERNAME, FEEDS, FEED_NAME, DATA);

    /* Initialize the GET request on HTTP Client Instance. */
    status = nx_web_http_client_request_initialize (&g_web_http_client0,
                                                    NX_WEB_HTTP_METHOD_GET,
                                                    url_path,
                                                    ADAFRUIT_IO_SERVER,
                                                    RESET_VALUE,
                                                    NX_FALSE,
                                                    NX_NULL,     /* username */
                                                    NX_NULL,     /* password */
                                                    NX_WAIT_FOREVER);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR ("nx_web_http_client_request_initialize API failed.");
        return status;
    }

    /* Add AIO key header */
    status = nx_web_http_client_request_header_add (&g_web_http_client0,
                                                    X_AIO_KEY,
                                                    strlen (X_AIO_KEY),
                                                    IO_KEY,
                                                    strlen (IO_KEY),
                                                    NX_WAIT_FOREVER);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR ("nx_web_http_client_request_header_add (AIO Key Header) API failed.");
        return status;
    }

    /* Send request to the HTTP Server. */
    status = nx_web_http_client_request_send (&g_web_http_client0, NX_WAIT_FOREVER);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR ("nx_web_http_client_request_send API failed.");
    }
    return status;
}

/*******************************************************************************************************************//**
 * @brief      Process POST request and read the response from the server
 *
 * @param[in]  adc_val                      The ADC reading for the MCU die Temperature.
 * @retval     NX_SUCCESS                   Upon successful execution of POST request
 *                                          to the Adafruit Server.
 *                                          The temperature data from the ADC is added
 *                                          to the Adafruit Server.
 * @retval     Any other Error Code         Upon unsuccessful execution of POST request.
 **********************************************************************************************************************/
UINT processPostRequest(float adc_val)
{
    /* To check the return status of APIs. */
    UINT status                         = NX_SUCCESS;
    /* Buffer to store ADC data for https client. */
    CHAR http_request[BUFFER_SIZE]      = {NULL_CHAR};
    /* Buffer to store the URL path for Adafruit IO Server. */
    CHAR url_path[BUFFER_SIZE]          = {NULL_CHAR};
    /* To get size of the http request to be sent to Adafruit IO Server. */
    UINT size                           = RESET_VALUE;

    PRINT_INFO_STR("Processing Post Request....");

    /* Set up the strings for the HTTP requests */
    memset (http_request, RESET_VALUE, sizeof(http_request));
    size = snprintf (http_request, BUFFER_SIZE, "{\"value\":\"%.02f\"}", adc_val);

    memset (url_path, RESET_VALUE, sizeof(url_path));
    snprintf (url_path, BUFFER_SIZE, "%s%s%s%s%s", API_V2, IO_USERNAME, FEEDS, FEED_NAME, DATA);

    /* Initialize the HTTP request. */
    status = nx_web_http_client_request_initialize(&g_web_http_client0,
                                                   NX_WEB_HTTP_METHOD_POST,
                                                   url_path,
                                                   ADAFRUIT_IO_SERVER,
                                                   strlen(http_request),
                                                   NX_FALSE,
                                                   NX_NULL, /* username */
                                                   NX_NULL, /* password */
                                                   NX_WAIT_FOREVER);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_web_http_client_request_initialize API failed.");
        return status;
    }

    /* Send Additional Header request to Server. */
    status = sendAdditionalHeadertoServer(http_request,size);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("Error in sendAdditionalHeadertoServer() function.");
    }
    return status;
}

/*******************************************************************************************************************//**
 * @brief      Process PUT request and read the response from the server
 *
 * @param[in]  adc_val                      The ADC reading for the MCU die Temperature
 * @retval     NX_SUCCESS                   Upon successful execution of PUT request
 *                                          to the Adafruit Server.
 *                                          The temperature data from the ADC is added
 *                                          to the Adafruit Server.
 * @retval     Any other Error Code         Upon unsuccessful execution of PUT request.
 **********************************************************************************************************************/
UINT processPutRequest(float adc_val)
{
    /* To check the return status of APIs. */
    UINT status                         = NX_SUCCESS;
    /* Buffer to store ADC data for https client. */
    CHAR http_request[BUFFER_SIZE]      = {NULL_CHAR};
    /* Buffer to store the URL path for Adafruit IO Server. */
    CHAR url_path[BUFFER_SIZE]          = {NULL_CHAR};
    /* Buffer to get retrieve the feed id from Adafruit Server. */
    CHAR feed_id[FEED_ID_SIZE + 1]          = {NULL_CHAR};
    /* To get size of the http request to be sent to Adafruit IO Server. */
    UINT size                           = RESET_VALUE;

    PRINT_INFO_STR("Processing Put Request....");

    /* Set up the strings for the HTTP requests. */
    memset (http_request, RESET_VALUE, sizeof(http_request));
    size = snprintf (http_request, BUFFER_SIZE, "{\"value\":\"%.02f\"}", adc_val);

    /*Get the last feed ID from server.*/
    status = processGetRequest();
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("Error in processGetRequest() function.");
        return status;
    }

    /* Read the response from the server and extract the Feed ID*/
    status = readResponsefromServer();
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("Error in readResponsefromServer() function.");
        return status;
    }

    /* Get the Feed ID. */
    memcpy(feed_id,strstr((char*)g_receive_buffer, "id") + 5,FEED_ID_SIZE);

    memset (url_path, RESET_VALUE, sizeof(url_path));
    snprintf (url_path, BUFFER_SIZE, "%s%s%s%s%s%s%s", API_V2, IO_USERNAME, FEEDS, FEED_NAME, DATA, SEPERATOR ,feed_id);

    /* Initialize the HTTP request. */
    status = nx_web_http_client_request_initialize(&g_web_http_client0,
                                                   NX_WEB_HTTP_METHOD_PUT,
                                                   url_path,
                                                   ADAFRUIT_IO_SERVER,
                                                   strlen(http_request),
                                                   NX_FALSE,
                                                   NX_NULL, /* username */
                                                   NX_NULL, /* password */
                                                   NX_WAIT_FOREVER);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_web_http_client_request_initialize API failed.");
        return status;
    }

    /* Send Additional Header request to Server.*/
    status = sendAdditionalHeadertoServer(http_request, size);
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("Error in sendAdditionalHeadertoServer() function.");
    }
    return status;
}

/*******************************************************************************************************************//**
 * @brief      Create a callback to setup TLS parameters for secure HTTPS.
 *
 * @param[in]  *client_ptr                Pointer to the Web HTTP Client.
 * @param[in]  *tls_session               Pointer to the Secure TLS Session.
 * @retval     NX_SUCCESS                 Upon successful processing of user input(s).
 * @retval     Any other Error Code       Upon unsuccessful processing of user input(s).
 **********************************************************************************************************************/
UINT tls_setup_callback(NX_WEB_HTTP_CLIENT *p_client_ptr, NX_SECURE_TLS_SESSION *p_tls_session)
{
    /* To check the return status of the APIs. */
    UINT status     = NX_SUCCESS;
    /* Count for certificates. */
    uint32_t count  = RESET_VALUE;

    NX_PARAMETER_NOT_USED(p_client_ptr);

    /* Clear TLS meta data buffer */
    memset(g_tls_session_meta_data, RESET_VALUE, sizeof(g_tls_session_meta_data));

    /* Create a TLS session. */
    status = nx_secure_tls_session_create (p_tls_session, &nx_crypto_tls_ciphers ,
                                           g_tls_session_meta_data,
                                           sizeof(g_tls_session_meta_data));
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_secure_tls_session_create API failed.");
        ERROR_TRAP(status);
    }

    /* Allocate space for packet re-assembly */
    memset(g_tls_packet_buffer, RESET_VALUE, sizeof(g_tls_packet_buffer));

    status = nx_secure_tls_session_packet_buffer_set(p_tls_session, g_tls_packet_buffer, sizeof(g_tls_packet_buffer));
    if (NX_SUCCESS != status)
    {
        PRINT_ERR_STR("nx_secure_tls_session_packet_buffer_set API failed.");
        ERROR_TRAP(status);
    }

    for( count = RESET_VALUE; count < NO_OF_TRUSTED_CERTS; count++)
    {
        /* Add a CA Certificate to our trusted store for verifying incoming server certificates. */
        memset(&g_trusted_certificate[count], RESET_VALUE, sizeof(NX_SECURE_X509_CERT));
        /* Initialize X.509 Certificate for NetX Secure TLS. */
        status = nx_secure_x509_certificate_initialize(&g_trusted_certificate[count],
                                                       g_trust_certs[count].pointer_to_cert,
                                                       g_trust_certs[count].sizeof_cert,
                                                       NX_NULL,
                                                       NX_NULL,
                                                       NX_NULL,
                                                       NX_NULL,
                                                       NX_SECURE_X509_KEY_TYPE_NONE);
        if (NX_SUCCESS != status)
        {
            PRINT_ERR_STR("_nx_secure_tls_session_packet_buffer_set API failed.");
            ERROR_TRAP(status);
        }

        /* Add the Certificate. */
        status = nx_secure_tls_trusted_certificate_add(p_tls_session, &g_trusted_certificate[count]);
        if (NX_SUCCESS != status)
        {
            PRINT_ERR_STR("nx_secure_tls_trusted_certificate_add API failed.");
            ERROR_TRAP(status);
        }
    }

    for( count = RESET_VALUE; count < NO_OF_REMOTE_CERTS; count++)
    {
        /* Allocate buffers for in-coming certificates */
        memset(&g_remote_cert_buffer[count][RESET_VALUE], RESET_VALUE, SIZE_OF_REMOTE_CERT_BUF);
        memset(&g_remote_cert[count], RESET_VALUE, sizeof(NX_SECURE_X509_CERT));

        /* Allocate the space for the certificate coming in from remote server. */
        status = nx_secure_tls_remote_certificate_allocate(p_tls_session,
                                                           &g_remote_cert[count],
                                                           &g_remote_cert_buffer[count][RESET_VALUE],
                                                           SIZE_OF_REMOTE_CERT_BUF);
        if (NX_SUCCESS != status)
        {
            PRINT_ERR_STR("nx_secure_tls_remote_certificate_allocate API failed.");
            ERROR_TRAP(status);
        }
    }
    return status;
}

/*******************************************************************************************************************//**
 * @} (end addtogroup NetX_https_client_ep)
 **********************************************************************************************************************/
