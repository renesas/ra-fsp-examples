/***********************************************************************************************************************
 * File Name    : crypto_ep.h
 * Description  : Contains data structures and functions used in crypto_ep.h.
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef CRYPTO_EP_H_
#define CRYPTO_EP_H_

/* Macro definitions.*/
#define ECC_256_BIT_LENGTH        (256U)              /* key size in bits for ECC*/
#define RSA_2048_BIT_LENGTH       (2048U)             /* key size in bits for RSA*/
/* Buffer size is calculated by using PSA_KEY_EXPORT_MAX_SIZE(type, bits)
 * where type is the key type and bits is the key size in bits.*/
#define RSA_2048_EXPORTED_SIZE    (1210U)             /* Size of the data buffer(to store key data) in bytes for RSA*/
#define ECC_256_EXPORTED_SIZE     (500U)              /* Size of the data buffer(to store key data) in bytes for ECC*/
#define AES_256_EXPORTED_SIZE     (500U)              /* Size of the data buffer(to store key data) in bytes for AES*/
#define TAG_LENGTH                ((size_t)16U)       /* number of bytes that make up the returned tag.*/
#define AES_KEY_ID                ((psa_key_id_t) 5)  /* persistent identifier for the key.*/
#define ECC_KEY_ID                ((psa_key_id_t) 6)
#define RSA_KEY_ID                ((psa_key_id_t) 7)
#define AES_KEY_BITS              (256U)             /* key size in bits for AES*/
#define ECC_RSA_PAYLOAD_SIZE      (30U)




/*Function declaration */
psa_status_t aes_operation(void);
void handle_error(psa_status_t status, char * err_str);
#if (! (defined (BOARD_RA4M1_EK) || defined (BOARD_RA4W1_EK)))

fsp_err_t littlefs_init(void);
void deinit_littlefs(void);
psa_status_t ecc_rsa_hashing_operation(unsigned char * payload, uint8_t * payload_hash, size_t * payload_hash_len);
#endif

#endif /* CRYPTO_EP_H_ */
