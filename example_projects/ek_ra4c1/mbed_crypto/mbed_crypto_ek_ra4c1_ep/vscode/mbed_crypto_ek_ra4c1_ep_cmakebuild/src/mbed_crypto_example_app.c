/***********************************************************************************************************************
 * File Name    : mbed_crypto_example_app.c
 * Description  : Example application layer for the Mbed Crypto EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: mbed_crypto_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include "mbed_crypto_example_app.h"
#include "console/uart_console.h"

/* Private functions */
static void aes_operation(void);
#if (!(defined (BOARD_RA4M1_EK) || defined (BOARD_RA4W1_EK) || defined (BOARD_RA2L1_EK)))
static void sha_operation(void);
static void ecc_operation(void);
#if (!defined (BOARD_RA4L1_EK)) && (!defined (BOARD_RA4C1_EK))
static void rsa_operation(void);
#endif /* BOARD_RA4L1_EK, BOARD_RA4C1_EK */
#endif /* BOARD_RA4M1_EK, BOARD_RA4W1_EK, BOARD_RA2L1_EK */
static void handle_error(psa_status_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the Mbed Crypto example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void mbed_crypto_example_app_run(void)
{
    fsp_pack_version_t version = {RESET_VALUE};
    psa_status_t err = (psa_status_t) PSA_SUCCESS;

    fsp_err_t console_err = uart_console_init();
    if (FSP_SUCCESS != console_err)
    {
        return;     /* UART failed to open — cannot print anything */
    }

    /* Get FLEX pack version information */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the terminal */
    printf(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    printf(EP_INFO);
    uart_console_flush();

    /* Setup the platform, initialize the SCE, and initialize the crypto library */
    err = init_mbedtls();
    handle_error(err, "init_mbedtls failed\r\n");

    /* Perform AES operation */
    aes_operation();

    /* RA4M1, RA4W1, and RA2L1 boards do not support SHA, ECC, and RSA operations */
#if (!(defined (BOARD_RA4M1_EK) || defined (BOARD_RA4W1_EK) || defined (BOARD_RA2L1_EK)))
    /* Perform SHA hashing operation */
    sha_operation();

    /* Perform ECC operation */
    ecc_operation();

    /* RA4L1, RA4C1 boards do not support RSA operation */
#if (!defined (BOARD_RA4L1_EK)) && (!defined (BOARD_RA4C1_EK))
    /* Perform RSA operation */
    rsa_operation();
#endif /* BOARD_RA4L1_EK, BOARD_RA4C1_EK */
#endif /* BOARD_RA4M1_EK, BOARD_RA4W1_EK, BOARD_RA2L1_EK */

    printf("\r\nCrypto operation completed.\r\n");

    /* De-initialize the platform */
    deinit_mbedtls();
}

/*******************************************************************************************************************//**
 * @brief     This function performs AES256 key generation (for GCM Mode), encryption, and decryption.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void aes_operation(void)
{
    /* Status and key attributes */
    psa_status_t err = (psa_status_t) RESET_VALUE;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;  /* Contain key attributes */

    /* Key handle and encrypted/decrypted output buffers */
    psa_key_handle_t aes_key_handle = RESET_VALUE;   /* AES key handle */
    uint8_t * output_data = NULL;                    /* Encrypted output buffer */
    uint8_t * output_data1 = NULL;                   /* Decrypted output buffer */
    size_t output_size = RESET_VALUE;
    size_t output_length = RESET_VALUE;
    size_t output_length1 = RESET_VALUE;

    /* RA4M1, RA4W1 and RA2L1 boards do not support LittleFS, so using key lifetime as volatile */
#if (!(defined (BOARD_RA4M1_EK) || defined (BOARD_RA4W1_EK) || defined (BOARD_RA2L1_EK)))
    /* Exported key data and LittleFS status */
    uint8_t aes_key[AES_256_EXPORTED_SIZE] = {RESET_VALUE};   /* Buffer where the key data is to be written */
    size_t aes_key_length = RESET_VALUE;
    psa_key_lifetime_t lifetime = PSA_KEY_LIFETIME_PERSISTENT;
    psa_key_usage_t key_flag = (PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT | PSA_KEY_USAGE_EXPORT);

    /* Perform LittleFS operation */
    err = (psa_status_t) littlefs_init();
    handle_error(err, "littlefs_init failed\r\n");
#else
    psa_key_lifetime_t lifetime = PSA_KEY_LIFETIME_VOLATILE;
    psa_key_usage_t key_flag = (PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
#endif /* BOARD_RA4M1_EK, BOARD_RA4W1_EK, BOARD_RA2L1_EK */

    /* Set Key uses flags, key_algorithm, key_type, key_bits, key_lifetime, key_id */
    crypto_set_key_attributes(&attributes, key_flag, PSA_ALG_GCM, PSA_KEY_TYPE_AES, AES_KEY_BITS, lifetime, AES_KEY_ID);

    printf("\r\nAES operation started.\r\n");

    /* Generating AES 256 key and allocating to key slot */
    err = crypto_generate_key(&attributes, &aes_key_handle);
    handle_error(err, "crypto_generate_key failed\r\n");
    printf("AES key generated successfully.\r\n");

    /* Calculate output size for encryption and decryption */
    output_size = sizeof(g_input_data) + TAG_LENGTH;
    output_data = (uint8_t *) malloc(output_size);
    output_data1 = (uint8_t *) malloc(output_size);
    if ((NULL == output_data) || (NULL == output_data1))
    {
        handle_error(PSA_ERROR_INSUFFICIENT_STORAGE, "AES output buffer allocation failed\r\n");
    }

    /* Authenticate and encrypt */
    err = aes_encrypt(aes_key_handle, output_data, &output_length);
    handle_error(err, "aes_encrypt failed\r\n");

#if (!(defined (BOARD_RA4M1_EK) || defined (BOARD_RA4W1_EK) || defined (BOARD_RA2L1_EK)))
    /* Export and destroy the key */
    err = crypto_export_and_destroy_key(aes_key_handle, aes_key, sizeof(aes_key), &aes_key_length);
    handle_error(err, "crypto_export_and_destroy_key failed\r\n");
    printf("Exported and destroyed key successfully.\r\n");

    /* This is intended to fail as key was not imported after destroying */
    err = aes_decrypt(aes_key_handle, output_data, output_length, output_data1, &output_length1);
    if (PSA_SUCCESS == err)
    {
        handle_error(PSA_ERROR_BAD_STATE, "aes_decrypt should have failed\r\n");
    }
    printf("AEAD decryption failed as key was destroyed.\r\n");

    /* Import key */
    err = crypto_import_key(&attributes, aes_key, aes_key_length, &aes_key_handle);
    handle_error(err, "crypto_import_key failed\r\n");
    printf("Imported key successfully.\r\n");
#endif /* BOARD_RA4M1_EK, BOARD_RA4W1_EK, BOARD_RA2L1_EK */

    /* Authenticate and decrypt */
    err = aes_decrypt(aes_key_handle, output_data, output_length, output_data1, &output_length1);
    handle_error(err, "aes_decrypt failed\r\n");

    /* Verify the decrypted data matches the original input */
    err = aes_verify_output(output_data1, output_length1);
    handle_error(err, "aes_verify_output failed\r\n");
    printf("AEAD encryption and decryption completed successfully.\r\n");

    /* Free the output buffers now that verification is complete */
    free(output_data);
    free(output_data1);
}

#if (!(defined (BOARD_RA4M1_EK) || defined (BOARD_RA4W1_EK) || defined (BOARD_RA2L1_EK)))
/*******************************************************************************************************************//**
 * @brief     This function performs SHA256 hashing operation.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void sha_operation(void)
{
    psa_status_t err = (psa_status_t)RESET_VALUE;

    /* Buffer containing the message fragment to hash */
    const uint8_t sha256_input_data[] =
    {
        0x2e, 0x7e, 0xa8, 0x4d, 0xa4, 0xbc, 0x4d, 0x7c, 0xfb, 0x46, 0x3e, 0x3f, 0x2c, 0x86, 0x47, 0x05,
        0x7a, 0xff, 0xf3, 0xfb, 0xec, 0xec, 0xa1, 0xd2, 0x00
    };
    uint8_t actual_hash[PSA_HASH_MAX_SIZE] = {RESET_VALUE};     /* Buffer where the hash is to be written */
    size_t actual_hash_len = RESET_VALUE;                       /* Number of bytes that make up the hash value */

    printf("\r\nHash operation started.\r\n");

    /* Set up, update, and finish a multipart hash operation */
    err = crypto_hash(sha256_input_data, sizeof(sha256_input_data), actual_hash, &actual_hash_len);
    handle_error(err, "crypto_hash failed\r\n");

    /* Compare hash value and hash length of calculated value with expected value */
    err = sha_compare_hash(actual_hash, actual_hash_len);
    handle_error(err, "sha_compare_hash failed\r\n");
    printf("Hash value and hash length comparison completed successfully.\r\n");
}

/*******************************************************************************************************************//**
 * @brief     This function performs ECC-P256R1 Key pair generation, signs the hashed message with private key
 *            and verifies hashed message with public key.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void ecc_operation(void)
{
    /* Status and key attributes */
    psa_status_t err = (psa_status_t) RESET_VALUE;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;  /* Contain key attributes */
    psa_key_type_t ecc_key_type;

    /* Key handle and exported key data */
    psa_key_handle_t ecc_key_handle = RESET_VALUE;              /* ECC key handle */
    uint8_t ecc_key[ECC_256_EXPORTED_SIZE] = {RESET_VALUE};     /* Buffer where the key data is to be written */
    size_t ecc_key_length = RESET_VALUE;                        /* Number of bytes that make up the key data */

    /* Payload hash */
    unsigned char payload_ecc[] = "ASYMMETRIC_INPUT_FOR_SIGN_ECC";  /* Buffer containing message to hash */
    uint8_t payload_hash_ecc[PSA_HASH_MAX_SIZE] = {RESET_VALUE};    /* Buffer where the hash is to be written */
    size_t payload_hash_len_ecc = RESET_VALUE;                      /* Number of bytes that make up the hash value */

    /* Signature */
    unsigned char signature[PSA_SIGNATURE_MAX_SIZE] = {RESET_VALUE};    /* Buffer containing signature for ECC */
    size_t signature_length = RESET_VALUE;                              /* Length of signature */

    printf("\r\nECC operation started.\r\n");

    /* Set Key uses flags, key_algorithm, key_type, key_bits, key_lifetime, key_id */
#if defined (BOARD_RA8E1_FPB) || defined (BOARD_RA8E2_EK)
    /* Software crypto */
    ecc_key_type = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1);
#else
    ecc_key_type = PSA_KEY_TYPE_ECC_KEY_PAIR_WRAPPED(PSA_ECC_FAMILY_SECP_R1);
#endif /* BOARD_RA8E1_FPB, BOARD_RA8E2_EK */
    crypto_set_key_attributes(&attributes, PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_VERIFY_HASH | PSA_KEY_USAGE_EXPORT,
                              PSA_ALG_ECDSA(PSA_ALG_SHA_256), ecc_key_type, ECC_256_BIT_LENGTH,
                              PSA_KEY_LIFETIME_PERSISTENT, ECC_KEY_ID);

    /* Generate ECC P256R1 Key pair */
    err = crypto_generate_key(&attributes, &ecc_key_handle);
    handle_error(err, "crypto_generate_key failed\r\n");
    printf("ECC key pair generated successfully.\r\n");

    /* Perform hashing operation */
    err = crypto_hash(payload_ecc, ECC_RSA_PAYLOAD_SIZE, payload_hash_ecc, &payload_hash_len_ecc);
    handle_error(err, "crypto_hash failed\r\n");

    /* Export and destroy the key */
    err = crypto_export_and_destroy_key(ecc_key_handle, ecc_key, sizeof(ecc_key), &ecc_key_length);
    handle_error(err, "crypto_export_and_destroy_key failed\r\n");
    printf("Exported and destroyed key successfully.\r\n");

    /* This is intended to fail as key was not imported after destroying */
    err = crypto_sign_hash(ecc_key_handle, PSA_ALG_ECDSA(PSA_ALG_SHA_256), payload_hash_ecc, payload_hash_len_ecc,
                           signature, sizeof(signature), &signature_length);
    if (PSA_SUCCESS == err)
    {
        handle_error(PSA_ERROR_BAD_STATE, "crypto_sign_hash should have failed\r\n");
    }
    printf("Signing the message failed as key was destroyed.\r\n");

    /* Import the previously exported key pair */
    err = crypto_import_key(&attributes, ecc_key, ecc_key_length, &ecc_key_handle);
    handle_error(err, "crypto_import_key failed\r\n");
    printf("Imported key successfully.\r\n");

    /* Sign message using the private key */
    err = crypto_sign_hash(ecc_key_handle, PSA_ALG_ECDSA(PSA_ALG_SHA_256), payload_hash_ecc, payload_hash_len_ecc,
                           signature, sizeof(signature), &signature_length);
    handle_error(err, "crypto_sign_hash failed\r\n");

    /* Verify the signature using the public key */
    err = crypto_verify_hash(ecc_key_handle, PSA_ALG_ECDSA(PSA_ALG_SHA_256), payload_hash_ecc, payload_hash_len_ecc,
                             signature, signature_length);
    handle_error(err, "crypto_verify_hash failed\r\n");
    printf("ECC signature validated successfully.\r\n");
}

#if (!defined (BOARD_RA4L1_EK)) && (!defined (BOARD_RA4C1_EK))
/*******************************************************************************************************************//**
 * @brief     This function performs RSA 2048 Key pair generation, signs the hashed message with private key
 *            and verifies hashed message with public key.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void rsa_operation(void)
{
    /* Status and key attributes */
    psa_status_t err = (psa_status_t) RESET_VALUE;
    psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;  /* Contain key attributes */
    psa_key_type_t rsa_key_type;

    /* Key handle and exported key data */
    psa_key_handle_t rsa_key_handle = RESET_VALUE;              /* RSA key handle */
    uint8_t rsa_key[RSA_2048_EXPORTED_SIZE] = {RESET_VALUE};    /* Buffer where the key data is to be written */
    size_t rsa_key_len = RESET_VALUE;                           /* Number of bytes that make up the key data */

    /* Payload hash */
    unsigned char payload_rsa[] = "ASYMMETRIC_INPUT_FOR_SIGN_RSA";  /* Buffer containing message to hash */
    uint8_t payload_hash_rsa[PSA_HASH_MAX_SIZE] = {RESET_VALUE};    /* Buffer where the hash is to be written */
    size_t payload_hash_len_rsa = RESET_VALUE;                      /* Number of bytes that make up the hash value */

    /* Signature */
    unsigned char signature[PSA_SIGNATURE_MAX_SIZE] = {RESET_VALUE};    /* Buffer containing signature for RSA */
    size_t signature_length = RESET_VALUE;                              /* Length of signature */
    printf("\r\nRSA operation started.\r\n");

    /* Set Key uses flags, key_algorithm, key_type, key_bits, key_lifetime, key_id */
#if defined (BOARD_RA8E1_FPB) || defined (BOARD_RA8E2_EK)
    /* Software crypto */
    rsa_key_type = PSA_KEY_TYPE_RSA_KEY_PAIR;
#else
    rsa_key_type = PSA_KEY_TYPE_RSA_KEY_PAIR_WRAPPED;
#endif /* BOARD_RA8E1_FPB, BOARD_RA8E2_EK */
    crypto_set_key_attributes(&attributes, PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_VERIFY_HASH | PSA_KEY_USAGE_EXPORT,
                              PSA_ALG_RSA_PKCS1V15_SIGN_RAW, rsa_key_type, RSA_2048_BIT_LENGTH,
                              PSA_KEY_LIFETIME_PERSISTENT, RSA_KEY_ID);

    /* Generate RSA 2048 Key pair */
    err = crypto_generate_key(&attributes, &rsa_key_handle);
    handle_error(err, "crypto_generate_key failed\r\n");
    printf("RSA key pair generated successfully.\r\n");

    /* Perform hashing operation */
    err = crypto_hash(payload_rsa, ECC_RSA_PAYLOAD_SIZE, payload_hash_rsa, &payload_hash_len_rsa);
    handle_error(err, "crypto_hash failed\r\n");

    /* Export and destroy the key */
    err = crypto_export_and_destroy_key(rsa_key_handle, rsa_key, sizeof(rsa_key), &rsa_key_len);
    handle_error(err, "crypto_export_and_destroy_key failed\r\n");
    printf("Exported and destroyed key successfully.\r\n");

    /* This is intended to fail as key was not imported after destroying */
    err = crypto_sign_hash(rsa_key_handle, PSA_ALG_RSA_PKCS1V15_SIGN_RAW, payload_hash_rsa, payload_hash_len_rsa,
                           signature, sizeof(signature), &signature_length);
    if (PSA_SUCCESS == err)
    {
        handle_error(PSA_ERROR_BAD_STATE, "crypto_sign_hash should have failed\r\n");
    }
    printf("Signing the message failed as key was destroyed.\r\n");

    /* Import the key */
    err = crypto_import_key(&attributes, rsa_key, rsa_key_len, &rsa_key_handle);
    handle_error(err, "crypto_import_key failed\r\n");
    printf("Imported key successfully.\r\n");

    /* Sign message using the private key */
    err = crypto_sign_hash(rsa_key_handle, PSA_ALG_RSA_PKCS1V15_SIGN_RAW, payload_hash_rsa, payload_hash_len_rsa,
                           signature, sizeof(signature), &signature_length);
    handle_error(err, "crypto_sign_hash failed\r\n");

    /* Verify the signature using the public key */
    err = crypto_verify_hash(rsa_key_handle, PSA_ALG_RSA_PKCS1V15_SIGN_RAW, payload_hash_rsa, payload_hash_len_rsa,
                             signature, signature_length);
    handle_error(err, "crypto_verify_hash failed\r\n");
    printf("RSA signature validated successfully.\r\n");
}
#endif /* BOARD_RA4L1_EK, BOARD_RA4C1_EK */
#endif /* BOARD_RA4M1_EK, BOARD_RA4W1_EK, BOARD_RA2L1_EK */

/*******************************************************************************************************************//**
 * @brief     This function handles an error by printing error message and code, de-initializing the platform,
 *            and entering a trap state.
 * @param[in] err                       Error status.
 * @param[in] err_str                   Error message.
 * @retval    None.
 **********************************************************************************************************************/
static void handle_error(psa_status_t err, const char * err_str)
{
    if (PSA_SUCCESS != err)
    {
        printf("\r\n[ERR] %s", err_str);
        printf("Returned Error Code: 0x%x\r\n", (unsigned int) err);
        uart_console_flush();
#if (!(defined (BOARD_RA4M1_EK) || defined (BOARD_RA4W1_EK) || defined (BOARD_RA2L1_EK)))
        deinit_littlefs();
#endif /* BOARD_RA4M1_EK, BOARD_RA4W1_EK, BOARD_RA2L1_EK */
        deinit_mbedtls();
        __BKPT(0);
    }
}
