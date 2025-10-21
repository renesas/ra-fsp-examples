/***********************************************************************************************************************
 * File Name    : crypto_ep.h
 * Description  : Contains data structures and functions used in the EP.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef CRYPTO_EP_H_
#define CRYPTO_EP_H_

/* Macro definitions */
#define ECC_256_BIT_LENGTH        (256U)              /* Key size in bits for ECC */
#define RSA_2048_BIT_LENGTH       (2048U)             /* Key size in bits for RSA */

/* Buffer size is calculated by using PSA_KEY_EXPORT_MAX_SIZE (type, bits)
 * where type is the key type and bits is the key size in bits */
/* Size of the data buffer (to store key data) in bytes for RSA */
#define RSA_2048_EXPORTED_SIZE    (1210U)
/* Size of the data buffer (to store key data) in bytes for ECC */
#define ECC_256_EXPORTED_SIZE     (500U)
/* Size of the data buffer (to store key data) in bytes for AES */
#define AES_256_EXPORTED_SIZE     (500U)
/* Number of bytes that make up the returned tag */
#define TAG_LENGTH                ((size_t)16U)

/* Persistent identifier for the key */
#define AES_KEY_ID                ((psa_key_id_t) 5)
#define ECC_KEY_ID                ((psa_key_id_t) 6)
#define RSA_KEY_ID                ((psa_key_id_t) 7)

/* Key size in bits for AES */
#define AES_KEY_BITS              (256U)
#define ECC_RSA_PAYLOAD_SIZE      (30U)

#if defined (BOARD_RA4M1_EK) || defined (BOARD_RA4W1_EK) || defined (BOARD_RA2L1_EK)
#define EP_INFO         "\r\nThis example project demonstrates the functionality of mbed crypto module on\r\n"\
                        "Renesas RA MCUs based on Renesas FSP. The EP performs AES256 crypto operation for GCM\r\n"\
                        "Mode. On successful completion of each operation, success message will be printed on\r\n"\
                        "J-Link RTT Viewer. Error and info messages will be printed on J-Link RTT Viewer.\r\n\n\n"
#elif defined (BOARD_RA4L1_EK) || defined (BOARD_RA4C1_EK)
#define EP_INFO         "\r\nThis example project demonstrates the functionality of mbed crypto module on\r\n"\
                        "Renesas RA MCUs based on Renesas FSP. The EP performs following crypto operations:\r\n"\
						"1. AES256 for GCM Mode.\r\n"\
                        "2. SHA256 Hashing.\r\n"\
                        "3. ECC - P256R1 operation.\r\n"\
                        "On successful completion of each operation, success message will be printed on\r\n"\
                        "J-Link RTT Viewer. Error and info messages will be printed on J-Link RTT Viewer.\r\n\n\n"
#else
#define EP_INFO         "\r\nThis example project demonstrates the functionality of mbed crypto module on\r\n"\
                        "Renesas RA MCUs based on Renesas FSP. The EP performs following crypto operations:\r\n"\
                        "1. AES256 for GCM Mode.\r\n"\
                        "2. SHA256 Hashing.\r\n"\
                        "3. ECC - P256R1 operation.\r\n"\
                        "4. RSA 2048 operation.\r\n"\
                        "On successful completion of each operation, success message will be printed on\r\n"\
                        "J-Link RTT Viewer. Error and info messages will be printed on J-Link RTT Viewer.\r\n\n\n"
#endif

/* Function declarations */
psa_status_t aes_operation(void);
void handle_error(psa_status_t status, char * err_str);
#if (! (defined (BOARD_RA4M1_EK) || defined (BOARD_RA4W1_EK) || defined (BOARD_RA2L1_EK)))
psa_status_t sha_operation(void);
psa_status_t ecc_operation(void);
psa_status_t ecc_rsa_hashing_operation(unsigned char * payload, uint8_t * payload_hash, size_t * payload_hash_len);
#if (!defined (BOARD_RA4L1_EK)) && (!defined (BOARD_RA4C1_EK))
psa_status_t rsa_operation(void);
#endif /* BOARD_RA4L1_EK, BOARD_RA4C1_EK */
fsp_err_t littlefs_init(void);
void deinit_littlefs(void);
#endif /* BOARD_RA4M1_EK, BOARD_RA4W1_EK, BOARD_RA2L1_EK */

#endif /* CRYPTO_EP_H_ */
