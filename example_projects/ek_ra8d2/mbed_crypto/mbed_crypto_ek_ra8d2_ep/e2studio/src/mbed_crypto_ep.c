/***********************************************************************************************************************
 * File Name    : mbed_crypto_ep.c
 * Description  : Contains data structures and functions used in mbed_crypto_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <string.h>
#include "mbed_crypto_ep.h"

/* Data that will be authenticated and encrypted */
const uint8_t g_input_data[] =
{
    0xB9, 0x6B, 0x49, 0xE2, 0x1D, 0x62, 0x17, 0x41, 0x63, 0x28, 0x75, 0xDB, 0x7F, 0x6C, 0x92, 0x43, 0xD2, 0xD7, 0xC2
};

/* Nonce or IV to use */
static const uint8_t nonce[12] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B};

/* Additional data that will be authenticated but not encrypted */
static const uint8_t additional_data[12] = {RESET_VALUE};

static bool s_b_mbedtls_setup_flag = false;
static bool s_b_crypto_init_flag = false;

/* Platform context structure - owned by the driver layer, only used internally by init_mbedtls/deinit_mbedtls */
static mbedtls_platform_context ctx = {RESET_VALUE};

/*******************************************************************************************************************//**
 * @brief     This function sets up the mbedtls platform, initializes the SCE, and initializes the crypto library.
 * @param[in] None.
 * @retval    PSA_SUCCESS               Set up and initialized successfully.
 * @retval    Any other error codes     Failed to set up or initialize.
 **********************************************************************************************************************/
psa_status_t init_mbedtls(void)
{
    psa_status_t err = PSA_SUCCESS;

    /* Setup the platform, initialize the SCE */
    err = (psa_status_t)mbedtls_platform_setup(&ctx);
    if (PSA_SUCCESS != err)
    {
        return err;
    }
    s_b_mbedtls_setup_flag = true;

    /* Initialize crypto library */
    err = psa_crypto_init();
    if (PSA_SUCCESS != err)
    {
        return err;
    }
    s_b_crypto_init_flag = true;

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function de-initializes the crypto library and tears down the mbedtls platform.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void deinit_mbedtls(void)
{
    if (s_b_crypto_init_flag)
    {
        mbedtls_psa_crypto_free();
        s_b_crypto_init_flag = false;
    }

    if (s_b_mbedtls_setup_flag)
    {
        mbedtls_platform_teardown(&ctx);
        s_b_mbedtls_setup_flag = false;
    }
}

/*******************************************************************************************************************//**
 * @brief      This function sets the key attributes used for key generation and import.
 * @param[out] attributes               Pointer to the key attributes to set.
 * @param[in]  usage_flags              Key usage flags.
 * @param[in]  alg                      Key algorithm.
 * @param[in]  key_type                 Key type.
 * @param[in]  key_bits                 Key size in bits.
 * @param[in]  lifetime                 Key lifetime.
 * @param[in]  key_id                   Persistent identifier for the key.
 * @retval     None.
 **********************************************************************************************************************/
void crypto_set_key_attributes(psa_key_attributes_t * attributes, psa_key_usage_t usage_flags, psa_algorithm_t alg,
                               psa_key_type_t key_type, size_t key_bits, psa_key_lifetime_t lifetime,
                               psa_key_id_t key_id)
{
    psa_set_key_usage_flags(attributes, usage_flags);
    psa_set_key_algorithm(attributes, alg);
    psa_set_key_type(attributes, key_type);
    psa_set_key_bits(attributes, key_bits);
    psa_set_key_lifetime(attributes, lifetime);

    if (PSA_KEY_LIFETIME_IS_PERSISTENT(lifetime))
    {
        /* Set the id to a positive integer */
        psa_set_key_id(attributes, key_id);
    }
}

/*******************************************************************************************************************//**
 * @brief      This function generates a key and allocates it to a key slot.
 * @param[in]  attributes               Pointer to the key attributes.
 * @param[out] key                      Pointer to the key handle.
 * @retval     PSA_SUCCESS              Generated successfully.
 * @retval     Any other error codes    Failed to generate.
 **********************************************************************************************************************/
psa_status_t crypto_generate_key(psa_key_attributes_t * attributes, psa_key_handle_t * key)
{
    psa_status_t err = PSA_SUCCESS;

    err = psa_generate_key(attributes, key);

    return err;
}

/*******************************************************************************************************************//**
 * @brief      This function authenticates and encrypts the fixed input data using the AES key.
 * @param[in]  key                      Key handle to encrypt with.
 * @param[out] ciphertext               Buffer to write the encrypted output into.
 * @param[out] ciphertext_length        Number of bytes written to ciphertext.
 * @retval     PSA_SUCCESS              Encrypted successfully.
 * @retval     Any other error codes    Failed to encrypt.
 **********************************************************************************************************************/
psa_status_t aes_encrypt(psa_key_handle_t key, uint8_t * ciphertext, size_t * ciphertext_length)
{
    psa_status_t err = PSA_SUCCESS;

    err = psa_aead_encrypt(key, PSA_ALG_GCM,
                           nonce, sizeof(nonce),
                           additional_data, sizeof(additional_data),
                           g_input_data, sizeof(g_input_data),
                           ciphertext, sizeof(g_input_data) + TAG_LENGTH,
                           ciphertext_length);

    return err;
}

/*******************************************************************************************************************//**
 * @brief      This function decrypts data using the AES key.
 * @param[in]  key                      Key handle to decrypt with.
 * @param[in]  ciphertext               Buffer containing the encrypted data.
 * @param[in]  ciphertext_length        Number of bytes in ciphertext.
 * @param[out] plaintext                Buffer to write the decrypted data into.
 * @param[out] plaintext_length         Number of bytes written to plaintext.
 * @retval     PSA_SUCCESS              Decrypted successfully.
 * @retval     Any other error codes    Failed to decrypt.
 **********************************************************************************************************************/
psa_status_t aes_decrypt(psa_key_handle_t key, uint8_t * ciphertext, size_t ciphertext_length, uint8_t * plaintext,
                         size_t * plaintext_length)
{
    psa_status_t err = PSA_SUCCESS;

    err = psa_aead_decrypt(key, PSA_ALG_GCM,
                           nonce, sizeof(nonce),
                           additional_data, sizeof(additional_data),
                           ciphertext, ciphertext_length,
                           plaintext, ciphertext_length,
                           plaintext_length);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function verifies that decrypted AES output matches the original fixed input data.
 * @param[in] plaintext                 Buffer containing the decrypted data.
 * @param[in] plaintext_length          Number of bytes in plaintext.
 * @retval    PSA_SUCCESS               Output matched.
 * @retval    Any other error codes     Output not matched.
 **********************************************************************************************************************/
psa_status_t aes_verify_output(uint8_t * plaintext, size_t plaintext_length)
{
    psa_status_t err = PSA_SUCCESS;

    /* Compare the encrypted data length and decrypted data length */
    if (plaintext_length != sizeof(g_input_data))
    {
        err = (psa_status_t)FSP_ERR_CRYPTO_INVALID_SIZE;
        return err;
    }

    /* Compare the encrypted data and decrypted data */
    for (int len = RESET_VALUE; len < (int)plaintext_length; len++)
    {
        if (g_input_data[len] != *(plaintext + len))
        {
            err = (psa_status_t)FSP_ERR_ABORTED;
            return err;
        }
    }

    return err;
}

#if (!(defined (BOARD_RA4M1_EK) || defined (BOARD_RA4W1_EK) || defined (BOARD_RA2L1_EK)))
/*******************************************************************************************************************//**
 * @brief      This function exports the key data then destroys the key and handle.
 * @param[in]  key                      Key handle to export and destroy.
 * @param[out] data                     Buffer to export the key data into.
 * @param[in]  data_size                Size of data in bytes.
 * @param[out] data_length              Number of bytes written to data.
 * @retval     PSA_SUCCESS              Exported and destroyed successfully.
 * @retval     Any other error codes    Failed to export or destroy.
 **********************************************************************************************************************/
psa_status_t crypto_export_and_destroy_key(psa_key_handle_t key, uint8_t * data, size_t data_size,
                                           size_t * data_length)
{
    psa_status_t err = PSA_SUCCESS;

    /* Export key */
    err = psa_export_key(key, data, data_size, data_length);
    if (PSA_SUCCESS != err)
    {
        return err;
    }

    /* Destroy key */
    err = psa_destroy_key(key);

    return err;
}

/*******************************************************************************************************************//**
 * @brief      This function imports a previously exported key.
 * @param[in]  attributes               Pointer to the key attributes used when the key was generated.
 * @param[in]  data                     Buffer containing the exported key data.
 * @param[in]  data_length              Number of bytes in data.
 * @param[out] key                      Pointer to the key handle.
 * @retval     PSA_SUCCESS              Imported successfully.
 * @retval     Any other error codes    Failed to import.
 **********************************************************************************************************************/
psa_status_t crypto_import_key(psa_key_attributes_t * attributes, uint8_t * data, size_t data_length,
                               psa_key_handle_t * key)
{
    psa_status_t err = PSA_SUCCESS;

    err = psa_import_key(attributes, data, data_length, key);

    return err;
}

/*******************************************************************************************************************//**
 * @brief      This function signs a payload hash with the private key.
 * @param[in]  key                      Key handle to sign with.
 * @param[in]  alg                      Algorithm to sign with.
 * @param[in]  hash                     Buffer containing the payload hash to sign.
 * @param[in]  hash_length              Number of bytes in hash.
 * @param[out] signature                Buffer to write the signature into.
 * @param[in]  signature_size           Size of signature in bytes.
 * @param[out] signature_length         Number of bytes written to signature.
 * @retval     PSA_SUCCESS              Signed successfully.
 * @retval     Any other error codes    Failed to sign.
 **********************************************************************************************************************/
psa_status_t crypto_sign_hash(psa_key_handle_t key, psa_algorithm_t alg, uint8_t * hash, size_t hash_length,
                              unsigned char * signature, size_t signature_size, size_t * signature_length)
{
    psa_status_t err = PSA_SUCCESS;

    err = psa_sign_hash(key, alg, hash, hash_length, signature, signature_size, signature_length);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function verifies a signature against a payload hash with the public key.
 * @param[in] key                       Key handle to verify with.
 * @param[in] alg                       Algorithm to verify with.
 * @param[in] hash                      Buffer containing the payload hash that was signed.
 * @param[in] hash_length               Number of bytes in hash.
 * @param[in] signature                 Buffer containing the signature to verify.
 * @param[in] signature_length          Number of bytes in signature.
 * @retval    PSA_SUCCESS               Verified successfully.
 * @retval    Any other error codes     Failed to verify.
 **********************************************************************************************************************/
psa_status_t crypto_verify_hash(psa_key_handle_t key, psa_algorithm_t alg, uint8_t * hash, size_t hash_length,
                                unsigned char * signature, size_t signature_length)
{
    psa_status_t err = PSA_SUCCESS;

    err = psa_verify_hash(key, alg, hash, hash_length, signature, signature_length);

    return err;
}

/*******************************************************************************************************************//**
 * @brief      This function performs a multipart SHA256 hash of the given input.
 * @param[in]  input                    Buffer containing the message fragment to hash.
 * @param[in]  input_length             Number of bytes in input.
 * @param[out] hash                     Buffer where the hash is to be written.
 * @param[out] hash_length              Number of bytes that make up the hash value.
 * @retval     PSA_SUCCESS              Hashed successfully.
 * @retval     Any other error codes    Failed to hash.
 **********************************************************************************************************************/
psa_status_t crypto_hash(const uint8_t * input, size_t input_length, uint8_t * hash, size_t * hash_length)
{
    psa_status_t err = PSA_SUCCESS;

    /* Operation object to set up for hashing operation */
    psa_hash_operation_t operation;
    memset(&operation, RESET_VALUE, sizeof(operation));

    /* Set up a multipart hash operation */
    err = psa_hash_setup(&operation, PSA_ALG_SHA_256);
    if (PSA_SUCCESS != err)
    {
        return err;
    }

    /* Add a message fragment to a multipart hash operation */
    err = psa_hash_update(&operation, input, input_length);
    if (PSA_SUCCESS != err)
    {
        return err;
    }

    /* Finish the calculation of the hash of a message */
    err = psa_hash_finish(&operation, &hash[RESET_VALUE], PSA_HASH_MAX_SIZE, hash_length);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function compares the calculated hash value and length against the expected values.
 * @param[in] p_hash                    Buffer containing the calculated hash value.
 * @param[in] hash_len                  Number of bytes that make up the calculated hash value.
 * @retval    PSA_SUCCESS               Compared successfully.
 * @retval    Any other error codes     Failed to compare.
 **********************************************************************************************************************/
psa_status_t sha_compare_hash(uint8_t * p_hash, size_t hash_len)
{
    psa_status_t err = PSA_SUCCESS;

    const psa_algorithm_t alg = PSA_ALG_SHA_256;
    size_t expected_hash_len = PSA_HASH_LENGTH(alg);   /* Expected hash length */

    /* Buffer containing expected hash */
    const uint8_t sha256_expected_data[] =
    {
        0x76, 0xe3, 0xac, 0xbc, 0x71, 0x88, 0x36, 0xf2, 0xdf, 0x8a, 0xd2, 0xd0, 0xd2, 0xd7, 0x6f, 0x0c,
        0xfa, 0x5f, 0xea, 0x09, 0x86, 0xbe, 0x91, 0x8f, 0x10, 0xbc, 0xee, 0x73, 0x0d, 0xf4, 0x41, 0xb9
    };

    /* Compare hash value of calculated value with expected value */
    err = (psa_status_t)memcmp(&p_hash[RESET_VALUE], &sha256_expected_data[RESET_VALUE], hash_len);
    if (PSA_SUCCESS != err)
    {
        return PSA_ERROR_DATA_CORRUPT;
    }

    /* Compare hash length of calculated value with expected value */
    err = (psa_status_t)memcmp(&expected_hash_len, &hash_len, sizeof(expected_hash_len));
    if (PSA_SUCCESS != err)
    {
        return PSA_ERROR_DATA_CORRUPT;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function initializes the LittleFS module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t littlefs_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open the LittleFS port */
    err = LITTLEFS_OPEN(&g_rm_littlefs0_ctrl, &g_rm_littlefs0_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Format the file system */
    err = (fsp_err_t) lfs_format(&g_rm_littlefs0_lfs, &g_rm_littlefs0_lfs_cfg);
    if (RESET_VALUE != err)
    {
        return err;
    }

    /* Mount the file system */
    err = (fsp_err_t) lfs_mount(&g_rm_littlefs0_lfs, &g_rm_littlefs0_lfs_cfg);
    if (RESET_VALUE != err)
    {
        return err;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function de-initializes the opened LittleFS file system.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void deinit_littlefs(void)
{
    /* Close the opened lower level driver */
    if (g_rm_littlefs0_ctrl.open)
    {
        LITTLEFS_CLOSE(&g_rm_littlefs0_ctrl);
    }
}

#endif /* BOARD_RA4M1_EK, BOARD_RA4W1_EK, BOARD_RA2L1_EK */
