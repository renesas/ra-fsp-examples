/***********************************************************************************************************************
 * File Name    : https_client_app.h
 * Description  : Contains macros, data structures and functions used in the https_client_app.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef HTTPS_CLIENT_APP_H_
#define HTTPS_CLIENT_APP_H_

#include "https_client_thread.h"

/* Macro definitions */
/* EP banner information */
#define EP_INFO     "\r\nThis project demonstrates the basic functionality of NetX Duo HTTPS Client"\
                    "\r\nwith ethernet driver on Renesas RA MCUs based on Renesas FSP using AzureRTOS."\
                    "\r\nThe EP uses the DHCP Client to get the IP address from the DHCP Server and"\
                    "\r\nHTTPS Client to connect to HTTPS Adafruit IO Server which is a cloud platform."\
                    "\r\nOn successful connection, the menu is displayed enabling the user to send POST,"\
                    "\r\nPUT, GET requests to Adafruit IO server. On POST/PUT request, MCU die temperature"\
                    "\r\nis read using ADC and uploaded to server. On GET request, the last MCU die temperature"\
                    "\r\nis read from the Adafruit IO server. Error and info messages will be printed on"\
                    "\r\nJ-Link RTT Viewer.\r\n\n\n"

/* User menu options */
#define MENUOPTIONS "\r\nHTTPS Menu Options:\r\n"\
		            "1.POST Request\r\n"\
		            "2.PUT Request\r\n"\
		            "3.GET Request\r\n"\
		            "User Input:\r\n"

/* Macros for the user inputs */
#define POST_REQUEST                (1)
#define PUT_REQUEST                 (2)
#define GET_REQUEST                 (3)

/* Data size for retrieving feed data */
#define DATA_SIZE                   (1024U)

/* Buffer length and size for sending and receiving data */
#define BUFFER_LENGTH               (800U)
#define BUFFER_SIZE                 (150U)

/* Feed ID size for Adafruit server feed */
#define FEED_ID_SIZE                (26U)

#define NULL_CHAR                   ('\0')

/* Timeout options */
#define LINK_ENABLE_WAIT_TIME       (1000U)

/* Wait option to connect to HTTPS Server */
#define SECURE_CONNECT_WAIT_TIME    (100U)

/* DNS buffer size */
#define IP_V4_SIZE                  (4)

/* Maximum number of DNS servers */
#define MAX_DNS_SERVERS             (5)

/* Macros for TLS setup */
/* Total number of remote certificates */
#define NO_OF_REMOTE_CERTS          (4)

/* Typical certificates with RSA keys of 2048 bits
 * using SHA-256 for signatures are in the range of 1000-2000 bytes. The
 * buffer should be large enough to at least hold that size, but depending on
 * the remote host certificates may be significantly smaller or larger. Note
 * that if the buffer is too small to hold the incoming certificate, the TLS
 * handshake will end with an error. */
/* Size of remote buffer for the certificate. 2048 bits */
#define SIZE_OF_REMOTE_CERT_BUF     (2 * 1024)

/* nx_secure_tls_session_packet_buffer_set() associates a packet re-assembly buffer to a TLS session.
 * In order to decrypt and parse incoming TLS records, the data in each record
 * must be assembled from the underlying TCP packets. TLS records can be
 * up to 16KB in size (though typically are much smaller) so may not fit into a
 * single TCP packet.
 * If you know the incoming message size will be smaller than the TLS
 * record limit of 16KB, the buffer size can be made smaller, but in order to
 * handle unknown incoming data the buffer should be made as large as
 * possible. If an incoming record is larger than the supplied buffer, the TLS
 * session will end with an error.*/
/* Size of TLS packet buffer. 16KB */
#define SIZE_OF_TLS_PACKET_BUF      (16 * 1024)

/* Calculated with nx_secure_tls_metadata_size_calculate() for nx_crypto_tls_ciphers_synergys7, RSA Modulus 4096 */
#define SIZE_OF_META_BUFFER         (12192)

/* Timeout for DNS resolution */
#define NETWORK_TIMEOUT             (60 * TX_TIMER_TICKS_PER_SECOND)

/* Macros to print info, error and trap the error */
#define PRINT_INFO_STR(str)         (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_INFO_STR, sizeof(str), (str)))
#define PRINT_ERR_STR(str)          (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_STR, sizeof(str), (str)))
#define ERROR_TRAP(err)             (app_rtt_print_data(RTT_OUTPUT_MESSAGE_APP_ERR_TRAP, sizeof(UINT *), &(err)))

/* User defined structure for trusted certificate info */
typedef struct st_trusted_certifcate_info
{
    UCHAR * pointer_to_cert;
    USHORT  sizeof_cert;
} trusted_certifcate_info_t;

/* Function declarations */
UINT readResponsefromServer(void);
UINT sendAdditionalHeadertoServer(CHAR * http_request_ptr, UINT size);
UINT processGetRequest(void);
UINT processPostRequest(float adc_val);
UINT processPutRequest(float adc_val);
UINT tls_setup_callback(NX_WEB_HTTP_CLIENT * client_ptr, NX_SECURE_TLS_SESSION * tls_session);
void deinit_https_client(void);

#endif /* HTTPS_CLIENT_APP_H_ */
