/***********************************************************************************************************************
 * File Name    : NetX_https_client_ep.h
 * Description  : Contains macros, data structures and functions used in the NetX_https_client_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef NETX_HTTPS_CLIENT_EP_H_
#define NETX_HTTPS_CLIENT_EP_H_

#include "https_client_thread.h"
#include "terminal.h"

/* Macros for the user inputs */
#define POST_REQUEST                (1U)
#define PUT_REQUEST                 (2U)
#define GET_REQUEST                 (3U)

/* Data size for retrieving feed data */
#define DATA_SIZE                   (1024U)

/* Buffer length and size for sending and receiving data */
#define BUFFER_LENGTH               (800U)
#define BUFFER_SIZE                 (150U)

/* Size of the buffers of the Adafruit IO username, feed name and active key entered by the user */
#define CREDENTIAL_SIZE             (64U)

/* Size of the buffer of the URL path of a request */
#define URL_SIZE                    (192U)

/* Feed ID size for Adafruit server feed */
#define FEED_ID_SIZE                (26U)

/* Timeout options - default 10 seconds for this project */
#define LINK_ENABLE_WAIT_TIME       (10 * 1000U)

/* 10 seconds wait option to connect to HTTPS Server */
#define SECURE_CONNECT_WAIT_TIME    (10 * 1000U)

/* Size of an IPv4 address in bytes */
#define IP_V4_SIZE                  (4U)

/* Maximum number of DNS servers */
#define MAX_DNS_SERVERS             (5U)

/* Macros for TLS setup */
/* Total number of remote certificates */
#define NO_OF_REMOTE_CERTS          (4U)

/* Typical certificates with RSA keys of 2048 bits
 * using SHA-256 for signatures are in the range of 1000-2000 bytes. The
 * buffer should be large enough to at least hold that size, but depending on
 * the remote host certificates may be significantly smaller or larger. Note
 * that if the buffer is too small to hold the incoming certificate, the TLS
 * handshake will end with an error. */
/* Size of remote buffer for the certificate. 2048 bytes */
#define SIZE_OF_REMOTE_CERT_BUF     (2 * 1024U)

/* nx_secure_tls_session_packet_buffer_set() associates a packet re-assembly buffer to a TLS session.
 * In order to decrypt and parse incoming TLS records, the data in each record
 * must be assembled from the underlying TCP packets. TLS records can be
 * up to 16KB in size (though typically are much smaller) so may not fit into a
 * single TCP packet.
 * If you know the incoming message size will be smaller than the TLS
 * record limit of 16KB, the buffer size can be made smaller, but in order to
 * handle unknown incoming data the buffer should be made as large as
 * possible. If an incoming record is larger than the supplied buffer, the TLS
 * session will end with an error. */
/* Size of TLS packet buffer. 16KB */
#define SIZE_OF_TLS_PACKET_BUF      (16 * 1024U)

/* Calculated with nx_secure_tls_metadata_size_calculate() for nx_crypto_tls_ciphers, RSA Modulus 4096 */
#define SIZE_OF_META_BUFFER         (12192U)

/* Timeout for DNS resolution */
#define NETWORK_TIMEOUT             (60 * TX_TIMER_TICKS_PER_SECOND)

/* Size of the buffer used to print an IPv4 address in dotted decimal format, including the NULL char */
#define IPV4_STR_SIZE               (16U)

/* Number of trusted certificates */
#define NO_OF_TRUSTED_CERTS     (sizeof(g_trust_certs) / sizeof(trusted_certificate_info_t))

/* Used to split an IPv4 address into its four bytes */
union IP
{
    ULONG ip_address;
    struct
    {
        UCHAR ip_d;
        UCHAR ip_c;
        UCHAR ip_b;
        UCHAR ip_a;
    } IP_t;
};

/* User defined structure for trusted certificate info */
typedef struct st_trusted_certificate_info
{
    UCHAR * pointer_to_cert;
    USHORT  sizeof_cert;
} trusted_certificate_info_t;

/* Extern variables - Defined in NetX_https_client_ep.c */
extern NX_IP g_ip0;
extern NX_WEB_HTTP_CLIENT g_web_http_client0;
extern UCHAR g_receive_buffer[DATA_SIZE];
extern CHAR g_io_username[CREDENTIAL_SIZE];
extern CHAR g_feed_name[CREDENTIAL_SIZE];
extern CHAR g_aio_key[CREDENTIAL_SIZE];
extern ULONG g_server_addr;
extern ULONG g_dns_server_addr;
extern NX_SECURE_TLS_CRYPTO nx_crypto_tls_ciphers;

/* Function declarations */
void nx_common_init0(void);
UINT packet_pool0_init0(void);
UINT web_http_client0_init(void);
void tls_init(void);
UINT https_server_secure_connect(void);
UINT client_ip_address_get(ULONG * const p_client_ip);
UINT ip0_init(void);
UINT ip_link_check(void);
UINT dhcpv4_client_init(void);
UINT dns_client_init(void);
UINT read_response_from_server(void);
UINT send_additional_header_to_server(CHAR * p_http_request, UINT size);
UINT process_get_request(void);
UINT process_post_request(float adc_val);
UINT process_put_request(float adc_val);
UINT tls_setup_callback(NX_WEB_HTTP_CLIENT * p_client_ptr, NX_SECURE_TLS_SESSION * p_tls_session);
void deinit_https_client(void);

#endif /* NETX_HTTPS_CLIENT_EP_H_ */
