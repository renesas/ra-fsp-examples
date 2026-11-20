/***********************************************************************************************************************
 * File Name    : ecc_example_app.c
 * Description  : Example application layer for the NetX Crypto EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - ecc_thread_entry.c calls a single function: ecc_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdint.h>
#include <stdlib.h>
#include "terminal.h"
#include "netx_crypto_ep.h"
#include "aes_example_app.h"
#include "ecc_example_app.h"

/* ECDSA key data */
/* NIST vector */
/* d  = 8c14b793cb19137e323a6d2e2a870bca2e7a493ec1153b3a95feb8a4873f8d08
 * Qx = 7a4e287890a1a47ad3457e52f2f76a83ce46cbc947616d0cbaa82323818a793d
 * Qy = eec4084f5b29ebf29c44cce3b3059610922f8b30ea6e8811742ac7238fe87308 */
static const uint8_t ECC_SECP256R1Keydata[] =
{
    0x7d,  0x7d,  0xc5,  0xf7,  0x1e,  0xb2,  0x9d,  0xda,  0xf8,  0x0d,  0x62,  0x14,  0x63,  0x2e,  0xea,  0xe0,
    0x3d,  0x90,  0x58,  0xaf,  0x1f,  0xb6,  0xd2,  0x2e,  0xd8,  0x0b,  0xad,  0xb6,  0x2b,  0xc1,  0xa5,  0x34
};

/* NIST vector */
static const uint8_t ECC_SECP256R1PublicKeydata[] =
{
    0x04,  0xea,  0xd2,  0x18,  0x59,  0x01,  0x19,  0xe8,  0x87,  0x6b,  0x29,  0x14,  0x6f,  0xf8,  0x9c,  0xa6,
    0x17,  0x70,  0xc4,  0xed,  0xbb,  0xf9,  0x7d,  0x38,  0xce,  0x38,  0x5e,  0xd2,  0x81,  0xd8,  0xa6,  0xb2,
    0x30,  0x28,  0xaf,  0x61,  0x28,  0x1f,  0xd3,  0x5e,  0x2f,  0xa7,  0x00,  0x25,  0x23,  0xac,  0xc8,  0x5a,
    0x42,  0x9c,  0xb0,  0x6e,  0xe6,  0x64,  0x83,  0x25,  0x38,  0x9f,  0x59,  0xed,  0xfc,  0xe1,  0x40,  0x51,
    0x41 /* ASN1 Constant */
};

static const uint8_t ECC_SECP256R1Message[] = "ASYMMETRIC_INPUT_FOR_SIGN......";

/* ECDH key data */
/* NIST vector */
/* Private key 59137e38152350b195c9718d39673d519838055ad908dd4757152fd8255c09bf */
static const uint8_t ECC_SECP256R1Keydata_ecdh[] =
{
    /* NOLINT (readability-magic-numbers) */
    0x59, 0x13, 0x7e, 0x38, 0x15, 0x23, 0x50, 0xb1, 0x95, 0xc9, 0x71, 0x8d, 0x39, 0x67, 0x3d, 0x51,

    /* NOLINT(readability-magic-numbers) */
    0x98, 0x38, 0x05, 0x5a, 0xd9, 0x08, 0xdd, 0x47, 0x57, 0x15, 0x2f, 0xd8, 0x25, 0x5c, 0x09, 0xbf,
};

/* NIST vector */
/* Public key 4, a8c5fdce8b62c5ada598f141adb3b26cf254c280b2857a63d2ad783a73115f6b,
   806e1aafec4af80a0d786b3de45375b517a7e5b51ffb2c356537c9e6ef227d4a */
static const uint8_t ECC_SECP256R1PublicKeydata_ecdh[] =
{
    /* NOLINT (readability-magic-numbers) */
    0x04, 0xa8, 0xc5, 0xfd, 0xce, 0x8b, 0x62, 0xc5, 0xad, 0xa5, 0x98, 0xf1, 0x41, 0xad, 0xb3, 0xb2, 0x6c,

    /* NOLINT (readability-magic-numbers) */
    0xf2, 0x54, 0xc2, 0x80, 0xb2, 0x85, 0x7a, 0x63, 0xd2, 0xad, 0x78, 0x3a, 0x73, 0x11, 0x5f, 0x6b, 0x80,

    /* NOLINT(readability-magic-numbers) */
    0x6e, 0x1a, 0xaf, 0xec, 0x4a, 0xf8, 0x0a, 0x0d, 0x78, 0x6b, 0x3d, 0xe4, 0x53, 0x75, 0xb5,

    /* NOLINT (readability-magic-numbers) */
    0x17, 0xa7, 0xe5, 0xb5, 0x1f, 0xfb, 0x2c, 0x35, 0x65, 0x37, 0xc9, 0xe6, 0xef, 0x22, 0x7d,
    0x4a,
};

/* Private functions */
static uint32_t netx_secure_crypto_ecdsa_example(void);
static uint32_t netx_secure_crypto_ecdh_example(void);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the ECC Thread.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void ecc_example_app_run(void)
{
    UINT status = TX_SUCCESS;
    ULONG actual_flags = RESET_VALUE;

    /* Initialize the Terminal Thread */
    term_thread_init_check();

    /* Wait for the ECC flag */
    status = tx_event_flags_get(&g_user_input_event_flags, ECC_EVENT_FLAG, TX_OR_CLEAR, &actual_flags,
                                TX_WAIT_FOREVER);

    /* Set RSA event flag */
    status = tx_event_flags_set(&g_user_input_event_flags, RSA_EVENT_FLAG, TX_OR);
    tx_thread_sleep(THREAD_SLEEP_TIME);

    while (true)
    {
        tx_mutex_get((TX_MUTEX *)gp_sce_resource, TX_WAIT_FOREVER);

        /* Execute the ECDSA example function to perform the cryptographic operation */
        status = netx_secure_crypto_ecdsa_example();
        if (NX_CRYPTO_SUCCESS != status)
        {
            PRINT_ERR_STR("netx_secure_crypto_ecdsa_example failed\r\n");
            ERROR_TRAP(status);
        }

        tx_mutex_put((TX_MUTEX *)gp_sce_resource);
        tx_thread_sleep(THREAD_SLEEP_TIME);
        tx_mutex_get((TX_MUTEX *)gp_sce_resource, TX_WAIT_FOREVER);

        /* Execute the ECDH example function to perform the cryptographic operation */
        status = netx_secure_crypto_ecdh_example();
        if (NX_CRYPTO_SUCCESS != status)
        {
            PRINT_ERR_STR("netx_secure_crypto_ecdh_example failed\r\n");
            ERROR_TRAP(status);
        }
        tx_mutex_put((TX_MUTEX *)gp_sce_resource);
        tx_thread_sleep(THREAD_SLEEP_TIME);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function performs ECDSA using the NetX Crypto API to sign and verify input message data.
 * @param[in] None.
 * @retval    NX_CRYPTO_SUCCESS         Operated successfully.
 * @retval    Any other error codes     Failed to operate.
 **********************************************************************************************************************/
static uint32_t netx_secure_crypto_ecdsa_example(void)
{
    uint8_t metadata[ECDSA_SIZE] = {RESET_VALUE};
    uint32_t metadata_size = ECDSA_SIZE;
    uint32_t status = NX_CRYPTO_SUCCESS;
    ULONG sig_length = RESET_VALUE;
    NX_CRYPTO_EXTENDED_OUTPUT extended_output;
    void * handler = NX_CRYPTO_NULL;
    uint8_t output[RM_NETX_SECURE_CRYPTO_EXAMPLE_OUTPUT_BUFFER_SIZE] = {RESET_VALUE};

    /* Initialize the crypto platform, including SCE and TRNG */
    status = _nx_crypto_initialize();
    if (NX_CRYPTO_SUCCESS != status)
    {
        return status;
    }
    PRINT_INFO_STR("ECDSA algorithm initialized and started successfully.");

    /* Call the crypto initialization function */
    status = _nx_crypto_method_ecdsa_init(&crypto_method_ecdsa, NX_CRYPTO_NULL, NX_CRYPTO_NULL,
                                          &handler, metadata, metadata_size);
    if (NX_CRYPTO_SUCCESS != status)
    {
        return status;
    }

    /* Set the hash method to SHA-256 */
    status = _nx_crypto_method_ecdsa_operation(NX_CRYPTO_HASH_METHOD_SET, handler, &crypto_method_ecdsa,
                                               NX_CRYPTO_NULL, NX_CRYPTO_NULL, (uint8_t *)&crypto_method_sha256,
                                               sizeof(NX_CRYPTO_METHOD *), NX_CRYPTO_NULL, NX_CRYPTO_NULL,
                                               NX_CRYPTO_NULL, metadata, metadata_size, NX_CRYPTO_NULL, NX_CRYPTO_NULL);

    if (NX_CRYPTO_SUCCESS != status)
    {
        return status;
    }

    /* Set the elliptic curve to SECP256R1 */
    status = _nx_crypto_method_ecdsa_operation(NX_CRYPTO_EC_CURVE_SET, handler, &crypto_method_ecdsa,
                                               NX_CRYPTO_NULL, NX_CRYPTO_NULL, (uint8_t *)&crypto_method_ec_secp256,
                                               sizeof(NX_CRYPTO_METHOD *), NX_CRYPTO_NULL, NX_CRYPTO_NULL,
                                               NX_CRYPTO_NULL, metadata, metadata_size, NX_CRYPTO_NULL, NX_CRYPTO_NULL);
    if (NX_CRYPTO_SUCCESS != status)
    {
        return status;
    }

    /* Prepare buffer to receive ECDSA signature */
    extended_output.nx_crypto_extended_output_data = output;
    extended_output.nx_crypto_extended_output_length_in_byte = sizeof(output);

    /* Generate ECDSA signature for the message */
    status = _nx_crypto_method_ecdsa_operation(NX_CRYPTO_SIGNATURE_GENERATE, handler, &crypto_method_ec_secp256,
                                               (uint8_t *)ECC_SECP256R1Keydata, sizeof(ECC_SECP256R1Keydata) << 3,
                                               (uint8_t *)ECC_SECP256R1Message, sizeof(ECC_SECP256R1Message),
                                               NX_CRYPTO_NULL, (uint8_t *)&extended_output,
                                               sizeof(extended_output), metadata, metadata_size,
                                               NX_CRYPTO_NULL, NX_CRYPTO_NULL);
    if (NX_CRYPTO_SUCCESS != status)
    {
        return status;
    }

    /* Retrieve actual length of the generated signature */
    sig_length = extended_output.nx_crypto_extended_output_actual_size;

    /* Verify the generated signature using the public key */
    status = _nx_crypto_method_ecdsa_operation(NX_CRYPTO_SIGNATURE_VERIFY, handler, &crypto_method_ec_secp256,
                                               (uint8_t *)ECC_SECP256R1PublicKeydata,
                                               sizeof(ECC_SECP256R1PublicKeydata) << 3,
                                               (uint8_t *)ECC_SECP256R1Message, sizeof(ECC_SECP256R1Message),
                                               NX_CRYPTO_NULL, output, sig_length, metadata, metadata_size,
                                               NX_CRYPTO_NULL, NX_CRYPTO_NULL);
    if (NX_CRYPTO_SUCCESS != status)
    {
        return status;
    }
    PRINT_INFO_STR("ECDSA algorithm executed successfully, signature was generated and verified.");

    /* Cleanup the ECDSA metadata */
    status = _nx_crypto_method_ecdsa_cleanup(metadata);
    if (NX_CRYPTO_SUCCESS != status)
    {
        return status;
    }

    return status;
}

/*******************************************************************************************************************//**
 * @brief     This function demonstrates the use of the NetX Crypto API to generate a shared secret using ECDH.
 * @param[in] None.
 * @retval    NX_CRYPTO_SUCCESS         Operated successfully.
 * @retval    Any other error codes     Failed to operate.
 **********************************************************************************************************************/
static uint32_t netx_secure_crypto_ecdh_example(void)
{
    uint8_t metadata[ECDH_SIZE] = {RESET_VALUE};
    uint32_t metadata_size = ECDH_SIZE;
    uint32_t status = NX_CRYPTO_SUCCESS;
    uint8_t local_public_key[RM_NETX_SECURE_CRYPTO_EXAMPLE_OUTPUT_BUFFER_SIZE] = {RESET_VALUE};
    uint32_t local_public_key_len = RESET_VALUE;
    uint8_t shared_secret[RM_NETX_SECURE_CRYPTO_EXAMPLE_OUTPUT_BUFFER_SIZE] = {RESET_VALUE};
    uint32_t shared_secret_len = RESET_VALUE;
    uint8_t output[RM_NETX_SECURE_CRYPTO_EXAMPLE_OUTPUT_BUFFER_SIZE] = {RESET_VALUE};
    NX_CRYPTO_EXTENDED_OUTPUT extended_output;

    /* Initialize the crypto platform, including SCE and TRNG */
    status = _nx_crypto_initialize();
    if (NX_CRYPTO_SUCCESS != status)
    {
        return status;
    }
    PRINT_INFO_STR("ECDH algorithm initialized and started successfully.");

    /* Initialize the ECDH method */
    status = _nx_crypto_method_ecdh_init(&crypto_method_ecdh, NX_CRYPTO_NULL, NX_CRYPTO_NULL, NX_CRYPTO_NULL, metadata,
                                         metadata_size);
    if (NX_CRYPTO_SUCCESS != status)
    {
        return status;
    }

    /* Set the elliptic curve to SECP256R1 */
    status = _nx_crypto_method_ecdh_operation(NX_CRYPTO_EC_CURVE_SET, NX_CRYPTO_NULL, &crypto_method_ecdh,
                                              NX_CRYPTO_NULL, NX_CRYPTO_NULL, (uint8_t *)&crypto_method_ec_secp256,
                                              sizeof(NX_CRYPTO_METHOD *), NX_CRYPTO_NULL, NX_CRYPTO_NULL,
                                              NX_CRYPTO_NULL, metadata, metadata_size,
                                              NX_CRYPTO_NULL, NX_CRYPTO_NULL);
    if (NX_CRYPTO_SUCCESS != status)
    {
        return status;
    }

    /* Generate ECDH key pair. The private key will be securely stored in the ECDH context (metadata).
     * The corresponding public key will be returned via 'local_public_key' for sharing with the peer device. */
    extended_output.nx_crypto_extended_output_data = local_public_key;
    extended_output.nx_crypto_extended_output_length_in_byte = sizeof(local_public_key);
    status = _nx_crypto_method_ecdh_operation(NX_CRYPTO_DH_SETUP, NX_CRYPTO_NULL, &crypto_method_ecdh,
                                              NX_CRYPTO_NULL, NX_CRYPTO_NULL, NX_CRYPTO_NULL,
                                              NX_CRYPTO_NULL, NX_CRYPTO_NULL, (uint8_t *)&extended_output,
                                              sizeof(extended_output), metadata, metadata_size,
                                              NX_CRYPTO_NULL, NX_CRYPTO_NULL);
    if (NX_CRYPTO_SUCCESS != status)
    {
        return status;
    }

    /* Retrieve actual length of the local public key */
    local_public_key_len = extended_output.nx_crypto_extended_output_actual_size;

    /* Calculate shared secret using the test (peer's) public key */
    extended_output.nx_crypto_extended_output_data = shared_secret;
    extended_output.nx_crypto_extended_output_length_in_byte = sizeof(shared_secret);
    status = _nx_crypto_method_ecdh_operation(NX_CRYPTO_DH_CALCULATE, NX_CRYPTO_NULL, &crypto_method_ecdh,
                                              NX_CRYPTO_NULL, NX_CRYPTO_NULL,
                                              (uint8_t *)ECC_SECP256R1PublicKeydata_ecdh,
                                              sizeof(ECC_SECP256R1PublicKeydata_ecdh), NX_CRYPTO_NULL,
                                              (uint8_t *)&extended_output, sizeof(extended_output), metadata,
                                              metadata_size, NX_CRYPTO_NULL, NX_CRYPTO_NULL);
    if (NX_CRYPTO_SUCCESS != status)
    {
        return status;
    }

    /* Retrieve actual length of the shared secret key */
    shared_secret_len = extended_output.nx_crypto_extended_output_actual_size;

    /* Cleanup the ECDH metadata */
    status = _nx_crypto_method_ecdh_cleanup(metadata);
    if (NX_CRYPTO_SUCCESS != status)
    {
        return status;
    }

    /* Verify. The below operations will be carried out by the peer. */
    /* Peer side simulation: import test private key and verify */

    /* Initialize the ECDH method */
    status = _nx_crypto_method_ecdh_init(&crypto_method_ecdh, NX_CRYPTO_NULL, NX_CRYPTO_NULL, NX_CRYPTO_NULL, metadata,
                                         metadata_size);
    if (NX_CRYPTO_SUCCESS != status)
    {
        return status;
    }

    /* Set the elliptic curve to SECP256R1 */
    status = _nx_crypto_method_ecdh_operation(NX_CRYPTO_EC_CURVE_SET, NX_CRYPTO_NULL, &crypto_method_ecdh,
                                              NX_CRYPTO_NULL, NX_CRYPTO_NULL, (uint8_t *)&crypto_method_ec_secp256,
                                              sizeof(NX_CRYPTO_METHOD *), NX_CRYPTO_NULL, NX_CRYPTO_NULL,NX_CRYPTO_NULL,
                                              metadata, metadata_size, NX_CRYPTO_NULL, NX_CRYPTO_NULL);
    if (NX_CRYPTO_SUCCESS != status)
    {
        return status;
    }

    /* Import the test private key. The peer could generate its own key pair,
     * in this example a test private key is used for simplicity. */
    status = _nx_crypto_method_ecdh_operation(NX_CRYPTO_DH_KEY_PAIR_IMPORT, NX_CRYPTO_NULL, &crypto_method_ecdh,
                                              (uint8_t *)ECC_SECP256R1Keydata_ecdh,
                                              (NX_CRYPTO_KEY_SIZE)(sizeof(ECC_SECP256R1Keydata_ecdh) << 3),
                                              (uint8_t *)ECC_SECP256R1PublicKeydata_ecdh,
                                              sizeof(ECC_SECP256R1PublicKeydata_ecdh), NX_CRYPTO_NULL,
                                              NX_CRYPTO_NULL, NX_CRYPTO_NULL, metadata, metadata_size,
                                              NX_CRYPTO_NULL, NX_CRYPTO_NULL);
    if (NX_CRYPTO_SUCCESS != status)
    {
        return status;
    }

    /* Calculate the shared secret using the local public key generated above and shared with the peer */
    extended_output.nx_crypto_extended_output_data = output;
    extended_output.nx_crypto_extended_output_length_in_byte = sizeof(output);
    status = _nx_crypto_method_ecdh_operation(NX_CRYPTO_DH_CALCULATE, NX_CRYPTO_NULL, &crypto_method_ecdh,
                                              NX_CRYPTO_NULL, NX_CRYPTO_NULL, local_public_key, local_public_key_len,
                                              NX_CRYPTO_NULL, (uint8_t *)&extended_output, sizeof(extended_output),
                                              metadata, metadata_size, NX_CRYPTO_NULL, NX_CRYPTO_NULL);
    if (NX_CRYPTO_SUCCESS != status)
    {
        return status;
    }

    /* Validate the output. Both the parties must generate the same shared secret */
    status = (extended_output.nx_crypto_extended_output_actual_size != shared_secret_len);
    if (NX_CRYPTO_SUCCESS != status)
    {
        return status;
    }

    status = (uint32_t)memcmp(output, shared_secret, extended_output.nx_crypto_extended_output_actual_size);
    if (NX_CRYPTO_SUCCESS != status)
    {
        return status;
    }
    PRINT_INFO_STR("ECDH algorithm executed successfully. Shared secret verified and matched on both sides.");

    /* Clean up the ECDH metadata */
    status = _nx_crypto_method_ecdh_cleanup(metadata);
    if (NX_CRYPTO_SUCCESS != status)
    {
        return status;
    }

    return status;
}
