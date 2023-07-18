/***********************************************************************************************************************
 * File Name    : https_client_app.c
 * Description  : This file contains the User Application code for the https_client connection
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
unsigned char ca_cert_der[] = {
                               0x30,0x82,0x04,0x8B,0x30,0x82,0x03,0x73,0xA0,0x03,0x02,0x01,0x02,0x02,0x10,0x05,
                               0x46,0xFE,0x18,0x23,0xF7,0xE1,0x94,0x1D,0xA3,0x9F,0xCE,0x14,0xC4,0x61,0x73,0x30,
                               0x0D,0x06,0x09,0x2A,0x86,0x48,0x86,0xF7,0x0D,0x01,0x01,0x0B,0x05,0x00,0x30,0x61,
                               0x31,0x0B,0x30,0x09,0x06,0x03,0x55,0x04,0x06,0x13,0x02,0x55,0x53,0x31,0x15,0x30,
                               0x13,0x06,0x03,0x55,0x04,0x0A,0x13,0x0C,0x44,0x69,0x67,0x69,0x43,0x65,0x72,0x74,
                               0x20,0x49,0x6E,0x63,0x31,0x19,0x30,0x17,0x06,0x03,0x55,0x04,0x0B,0x13,0x10,0x77,
                               0x77,0x77,0x2E,0x64,0x69,0x67,0x69,0x63,0x65,0x72,0x74,0x2E,0x63,0x6F,0x6D,0x31,
                               0x20,0x30,0x1E,0x06,0x03,0x55,0x04,0x03,0x13,0x17,0x44,0x69,0x67,0x69,0x43,0x65,
                               0x72,0x74,0x20,0x47,0x6C,0x6F,0x62,0x61,0x6C,0x20,0x52,0x6F,0x6F,0x74,0x20,0x43,
                               0x41,0x30,0x1E,0x17,0x0D,0x31,0x37,0x31,0x31,0x30,0x36,0x31,0x32,0x32,0x33,0x34,
                               0x35,0x5A,0x17,0x0D,0x32,0x37,0x31,0x31,0x30,0x36,0x31,0x32,0x32,0x33,0x34,0x35,
                               0x5A,0x30,0x5E,0x31,0x0B,0x30,0x09,0x06,0x03,0x55,0x04,0x06,0x13,0x02,0x55,0x53,
                               0x31,0x15,0x30,0x13,0x06,0x03,0x55,0x04,0x0A,0x13,0x0C,0x44,0x69,0x67,0x69,0x43,
                               0x65,0x72,0x74,0x20,0x49,0x6E,0x63,0x31,0x19,0x30,0x17,0x06,0x03,0x55,0x04,0x0B,
                               0x13,0x10,0x77,0x77,0x77,0x2E,0x64,0x69,0x67,0x69,0x63,0x65,0x72,0x74,0x2E,0x63,
                               0x6F,0x6D,0x31,0x1D,0x30,0x1B,0x06,0x03,0x55,0x04,0x03,0x13,0x14,0x47,0x65,0x6F,
                               0x54,0x72,0x75,0x73,0x74,0x20,0x52,0x53,0x41,0x20,0x43,0x41,0x20,0x32,0x30,0x31,
                               0x38,0x30,0x82,0x01,0x22,0x30,0x0D,0x06,0x09,0x2A,0x86,0x48,0x86,0xF7,0x0D,0x01,
                               0x01,0x01,0x05,0x00,0x03,0x82,0x01,0x0F,0x00,0x30,0x82,0x01,0x0A,0x02,0x82,0x01,
                               0x01,0x00,0xBF,0x8A,0xD1,0x63,0x4D,0xE1,0x18,0xEA,0x87,0x5D,0xE8,0x16,0x3C,0x8F,
                               0x7F,0xB6,0xBE,0x87,0x17,0x37,0xA4,0x0C,0xF8,0x31,0x3F,0x9F,0x45,0x54,0x40,0x21,
                               0xD7,0x9D,0x07,0x9B,0xCA,0x03,0x23,0x4A,0xBD,0x9B,0xED,0x85,0x02,0x63,0x3F,0x9F,
                               0x85,0xB9,0xEC,0x28,0xEF,0xF2,0x86,0x22,0xDB,0xF8,0x4D,0x54,0x41,0xC5,0xB4,0x42,
                               0x7F,0xCF,0x33,0x17,0x01,0x0E,0x82,0x90,0x52,0xD3,0xC7,0x34,0xA4,0xC1,0xA1,0x01,
                               0xDA,0x32,0xA0,0x40,0xAD,0x1F,0x59,0xE4,0x33,0xFC,0xA0,0xC3,0x96,0xAC,0x68,0x6C,
                               0xD3,0xE8,0x99,0x73,0x8C,0x26,0x10,0x77,0xCB,0xB7,0x3F,0x39,0x32,0xE8,0xD2,0x59,
                               0x28,0xEE,0x07,0x86,0xE2,0x09,0x3B,0x85,0xF8,0xAA,0x69,0xF6,0xA9,0x6B,0x9F,0x58,
                               0xAD,0x72,0xC8,0x5B,0x87,0x66,0xAE,0x08,0xE0,0x74,0xFB,0x2D,0x53,0x43,0x62,0x83,
                               0x3D,0x8F,0x85,0x4C,0x11,0x97,0xDC,0x1E,0xFC,0x50,0x30,0xB8,0x83,0x08,0x32,0x5E,
                               0x5C,0x5C,0xC4,0xE1,0x75,0x20,0x4A,0xEB,0xA5,0xD6,0x75,0x2D,0xDC,0x2D,0x7D,0x7C,
                               0xE0,0xD0,0xFE,0x7C,0x75,0xA1,0x4E,0x40,0x02,0x84,0x9A,0xD9,0x0D,0x5A,0x2E,0xA0,
                               0xAC,0xF3,0x35,0x8A,0x2A,0xEA,0xD6,0x5A,0x5A,0x6C,0x8E,0x2C,0xAB,0xF6,0xDE,0xFD,
                               0x78,0x47,0x26,0x79,0x7A,0xAA,0x22,0xEA,0xA9,0xE6,0x71,0x12,0x03,0xD3,0xF8,0xBA,
                               0x53,0xD2,0x79,0x9C,0xBD,0x64,0xAC,0xF6,0x1B,0x63,0xBB,0x4D,0x8F,0x38,0x02,0xF8,
                               0xF0,0x57,0x5D,0xC5,0xAA,0x25,0x5A,0x0C,0x5D,0xC5,0x30,0xFE,0x20,0x53,0x19,0x6C,
                               0xE9,0xC3,0x02,0x03,0x01,0x00,0x01,0xA3,0x82,0x01,0x40,0x30,0x82,0x01,0x3C,0x30,
                               0x1D,0x06,0x03,0x55,0x1D,0x0E,0x04,0x16,0x04,0x14,0x90,0x58,0xFF,0xB0,0x9C,0x75,
                               0xA8,0x51,0x54,0x77,0xB1,0xED,0xF2,0xA3,0x43,0x16,0x38,0x9E,0x6C,0xC5,0x30,0x1F,
                               0x06,0x03,0x55,0x1D,0x23,0x04,0x18,0x30,0x16,0x80,0x14,0x03,0xDE,0x50,0x35,0x56,
                               0xD1,0x4C,0xBB,0x66,0xF0,0xA3,0xE2,0x1B,0x1B,0xC3,0x97,0xB2,0x3D,0xD1,0x55,0x30,
                               0x0E,0x06,0x03,0x55,0x1D,0x0F,0x01,0x01,0xFF,0x04,0x04,0x03,0x02,0x01,0x86,0x30,
                               0x1D,0x06,0x03,0x55,0x1D,0x25,0x04,0x16,0x30,0x14,0x06,0x08,0x2B,0x06,0x01,0x05,
                               0x05,0x07,0x03,0x01,0x06,0x08,0x2B,0x06,0x01,0x05,0x05,0x07,0x03,0x02,0x30,0x12,
                               0x06,0x03,0x55,0x1D,0x13,0x01,0x01,0xFF,0x04,0x08,0x30,0x06,0x01,0x01,0xFF,0x02,
                               0x01,0x00,0x30,0x34,0x06,0x08,0x2B,0x06,0x01,0x05,0x05,0x07,0x01,0x01,0x04,0x28,
                               0x30,0x26,0x30,0x24,0x06,0x08,0x2B,0x06,0x01,0x05,0x05,0x07,0x30,0x01,0x86,0x18,
                               0x68,0x74,0x74,0x70,0x3A,0x2F,0x2F,0x6F,0x63,0x73,0x70,0x2E,0x64,0x69,0x67,0x69,
                               0x63,0x65,0x72,0x74,0x2E,0x63,0x6F,0x6D,0x30,0x42,0x06,0x03,0x55,0x1D,0x1F,0x04,
                               0x3B,0x30,0x39,0x30,0x37,0xA0,0x35,0xA0,0x33,0x86,0x31,0x68,0x74,0x74,0x70,0x3A,
                               0x2F,0x2F,0x63,0x72,0x6C,0x33,0x2E,0x64,0x69,0x67,0x69,0x63,0x65,0x72,0x74,0x2E,
                               0x63,0x6F,0x6D,0x2F,0x44,0x69,0x67,0x69,0x43,0x65,0x72,0x74,0x47,0x6C,0x6F,0x62,
                               0x61,0x6C,0x52,0x6F,0x6F,0x74,0x43,0x41,0x2E,0x63,0x72,0x6C,0x30,0x3D,0x06,0x03,
                               0x55,0x1D,0x20,0x04,0x36,0x30,0x34,0x30,0x32,0x06,0x04,0x55,0x1D,0x20,0x00,0x30,
                               0x2A,0x30,0x28,0x06,0x08,0x2B,0x06,0x01,0x05,0x05,0x07,0x02,0x01,0x16,0x1C,0x68,
                               0x74,0x74,0x70,0x73,0x3A,0x2F,0x2F,0x77,0x77,0x77,0x2E,0x64,0x69,0x67,0x69,0x63,
                               0x65,0x72,0x74,0x2E,0x63,0x6F,0x6D,0x2F,0x43,0x50,0x53,0x30,0x0D,0x06,0x09,0x2A,
                               0x86,0x48,0x86,0xF7,0x0D,0x01,0x01,0x0B,0x05,0x00,0x03,0x82,0x01,0x01,0x00,0x30,
                               0xF1,0x87,0x55,0x3D,0x84,0x08,0xFC,0x2E,0x5E,0x6A,0xBA,0x7C,0xD2,0xCD,0xD5,0x2C,
                               0xE3,0xBE,0x02,0xDA,0x5D,0x89,0x77,0xED,0xF4,0xE9,0x56,0xC0,0x92,0xF0,0x2A,0x55,
                               0x2D,0x45,0xF7,0x1C,0x2A,0x3F,0x10,0x5B,0xF3,0xE9,0xE1,0xBE,0xE1,0xE9,0x00,0x25,
                               0xB9,0xF7,0xA3,0xC1,0x03,0x1B,0xE3,0x9E,0x4E,0x8E,0x92,0x1B,0x09,0x95,0x52,0xF9,
                               0xAC,0x18,0xFD,0x1F,0x29,0x01,0x8B,0x17,0x0A,0x73,0x34,0xF4,0x67,0x12,0x55,0xEE,
                               0x22,0xBC,0xCB,0x30,0xCA,0x80,0x99,0x3F,0xFB,0xCF,0x12,0x7F,0xCB,0x3D,0x18,0x47,
                               0x85,0xD8,0x14,0x3E,0x4F,0x0C,0x94,0x3F,0x7B,0xF5,0x11,0xA8,0x51,0x6C,0xFB,0xA8,
                               0x60,0x30,0xA8,0x90,0xA1,0x8B,0x6F,0x2E,0x45,0xDB,0x37,0xB6,0x1C,0x7E,0xBD,0x16,
                               0x59,0x21,0xB1,0x32,0x67,0xAD,0x8D,0xA3,0x4B,0x49,0x3F,0x3B,0x12,0x19,0x2C,0xFC,
                               0x9D,0x0F,0xFF,0x8C,0xFF,0x01,0x23,0x0A,0xF3,0x04,0x05,0x07,0xE5,0x67,0x01,0x01,
                               0xB9,0xAF,0x81,0x67,0xEB,0x29,0xCB,0xAF,0xF8,0xFC,0x86,0x3E,0xA4,0x5C,0x73,0x84,
                               0xF9,0xE5,0x39,0x73,0xAC,0x19,0xF3,0x03,0x36,0x77,0xA0,0x29,0x68,0xF5,0xF4,0xEF,
                               0x3B,0xD3,0xEE,0x88,0x73,0x0A,0xAC,0x2E,0x95,0xEA,0x68,0x22,0xD2,0xCD,0xAC,0x6B,
                               0xF8,0x1B,0x5E,0x53,0xC2,0x0F,0xD6,0x76,0xE1,0x75,0x0C,0xC4,0x91,0x25,0xC0,0x85,
                               0x53,0x0E,0xE2,0x81,0xD1,0x0E,0x18,0x30,0xC9,0x67,0xA4,0xDF,0xD0,0x0A,0x12,0x78,
                               0x07,0x40,0x05,0xB1,0x0F,0x83,0x53,0x43,0x42,0x3B,0xE7,0xFB,0xF1,0x77,0xFB};

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
