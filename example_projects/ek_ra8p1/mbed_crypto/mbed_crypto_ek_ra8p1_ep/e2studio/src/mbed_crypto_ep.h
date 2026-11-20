/***********************************************************************************************************************
 * File Name    : mbed_crypto_ep.h
 * Description  : Contains macros, data structures, and functions used in mbed_crypto_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef MBED_CRYPTO_EP_H_
#define MBED_CRYPTO_EP_H_

#include <stdbool.h>
#include <stddef.h>
#include "hal_data.h"

/* Macro for the reset value */
#define RESET_VALUE                     (0x00)

/* Key size in bits for ECC */
#define ECC_256_BIT_LENGTH              (256U)

/* Key size in bits for RSA */
#define RSA_2048_BIT_LENGTH             (2048U)

/* Buffer size is calculated by using PSA_KEY_EXPORT_MAX_SIZE (type, bits)
 * where type is the key type and bits is the key size in bits */
/* Size of the data buffer (to store key data) in bytes for RSA */
#define RSA_2048_EXPORTED_SIZE          (1210U)

/* Size of the data buffer (to store key data) in bytes for ECC */
#define ECC_256_EXPORTED_SIZE           (500U)

/* Size of the data buffer (to store key data) in bytes for AES */
#define AES_256_EXPORTED_SIZE           (500U)

/* Number of bytes that make up the returned tag */
#define TAG_LENGTH                      ((size_t) 16U)

/* Persistent identifier for the key */
#define AES_KEY_ID                      ((psa_key_id_t) 5)
#define ECC_KEY_ID                      ((psa_key_id_t) 6)
#define RSA_KEY_ID                      ((psa_key_id_t) 7)

/* Key size in bits for AES */
#define AES_KEY_BITS                    (256U)

/* Length in bytes of the payload message hashed for ECC/RSA sign and verify */
#define ECC_RSA_PAYLOAD_SIZE            (30U)

#if (!(defined (BOARD_RA4M1_EK) || defined (BOARD_RA4W1_EK) || defined (BOARD_RA2L1_EK)))
/* Macros for LittleFS based on the underlying storage peripheral present on the board */
#if (BSP_PERIPHERAL_FLASH_PRESENT)
#define LITTLEFS_OPEN(ctrl, cfg)        (RM_LITTLEFS_FLASH_Open((ctrl), (cfg)))
#define LITTLEFS_CLOSE(ctrl)            (RM_LITTLEFS_FLASH_Close((ctrl)))
#else
#define LITTLEFS_OPEN(ctrl, cfg)        (RM_LITTLEFS_SPI_FLASH_Open((ctrl), (cfg)))
#define LITTLEFS_CLOSE(ctrl)            (RM_LITTLEFS_SPI_FLASH_Close((ctrl)))
#endif /* Specific LittleFS type */
#endif /* BOARD_RA4M1_EK, BOARD_RA4W1_EK, BOARD_RA2L1_EK */

/* External variable - Defined in mbed_crypto_ep.c */
extern const uint8_t g_input_data[19];

/* Function declarations */
psa_status_t init_mbedtls(void);
void deinit_mbedtls(void);
void crypto_set_key_attributes(psa_key_attributes_t * attributes, psa_key_usage_t usage_flags, psa_algorithm_t alg,
                               psa_key_type_t key_type, size_t key_bits, psa_key_lifetime_t lifetime,
                               psa_key_id_t key_id);
psa_status_t crypto_generate_key(psa_key_attributes_t * attributes, psa_key_handle_t * key);
psa_status_t aes_encrypt(psa_key_handle_t key, uint8_t * ciphertext, size_t * ciphertext_length);
psa_status_t aes_decrypt(psa_key_handle_t key, uint8_t * ciphertext, size_t ciphertext_length, uint8_t * plaintext,
                         size_t * plaintext_length);
psa_status_t aes_verify_output(uint8_t * plaintext, size_t plaintext_length);

#if (!(defined (BOARD_RA4M1_EK) || defined (BOARD_RA4W1_EK) || defined (BOARD_RA2L1_EK)))
psa_status_t crypto_export_and_destroy_key(psa_key_handle_t key, uint8_t * data, size_t data_size,
                                           size_t * data_length);
psa_status_t crypto_import_key(psa_key_attributes_t * attributes, uint8_t * data, size_t data_length,
                               psa_key_handle_t * key);
psa_status_t crypto_sign_hash(psa_key_handle_t key, psa_algorithm_t alg, uint8_t * hash, size_t hash_length,
                              unsigned char * signature, size_t signature_size, size_t * signature_length);
psa_status_t crypto_verify_hash(psa_key_handle_t key, psa_algorithm_t alg, uint8_t * hash, size_t hash_length,
                                unsigned char * signature, size_t signature_length);
psa_status_t crypto_hash(const uint8_t * input, size_t input_length, uint8_t * hash, size_t * hash_length);
psa_status_t sha_compare_hash(uint8_t * p_hash, size_t hash_len);
fsp_err_t littlefs_init(void);
void deinit_littlefs(void);
#endif /* BOARD_RA4M1_EK, BOARD_RA4W1_EK, BOARD_RA2L1_EK */

#endif /* MBED_CRYPTO_EP_H_ */
