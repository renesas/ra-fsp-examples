/***********************************************************************************************************************
 * File Name    : example_AES.c
 * Description  : Contains AES plaintext key injection and cryptographic operations
 ***********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
************************************************************************************************************************/

#include "stdio.h"
#include "hal_data.h"
#include "common_utils.h"
#include "example_AES.h"
#include "r_rsip_key_injection.h"
#include "hw_sce_ra_private.h"

#define AES_BLOCK_SIZE          PSA_BLOCK_CIPHER_BLOCK_LENGTH(PSA_KEY_TYPE_AES)
#define AES_IV_SIZE             AES_BLOCK_SIZE

#define AES256_KEY_BITS         256

/* NIST Test Vector */
static const uint8_t NIST_AESCBC256_Key[] = {
    0xfe, 0x89, 0x01, 0xfe, 0xcd, 0x3c, 0xcd, 0x2e, 0xc5, 0xfd, 0xc7, 0xc7, 0xa0, 0xb5, 0x05, 0x19,
    0xc2, 0x45, 0xb4, 0x2d, 0x61, 0x1a, 0x5e, 0xf9, 0xe9, 0x02, 0x68, 0xd5, 0x9f, 0x3e, 0xdf, 0x33
};

static const uint8_t NIST_AESCBC256_IV[] = {
    0xbd, 0x41, 0x6c, 0xb3, 0xb9, 0x89, 0x22, 0x28, 0xd8, 0xf1, 0xdf, 0x57, 0x56, 0x92, 0xe4, 0xd0
};

static const uint8_t NIST_AESCBC256_PT[] = {
    0x8d, 0x3a, 0xa1, 0x96, 0xec, 0x3d, 0x7c, 0x9b, 0x5b, 0xb1, 0x22, 0xe7, 0xfe, 0x77, 0xfb, 0x12,
    0x95, 0xa6, 0xda, 0x75, 0xab, 0xe5, 0xd3, 0xa5, 0x10, 0x19, 0x4d, 0x3a, 0x8a, 0x41, 0x57, 0xd5,
    0xc8, 0x9d, 0x40, 0x61, 0x97, 0x16, 0x61, 0x98, 0x59, 0xda, 0x3e, 0xc9, 0xb2, 0x47, 0xce, 0xd9
};

static const uint8_t NIST_AESCBC256_CT[] = {
    0x60, 0x8e, 0x82, 0xc7, 0xab, 0x04, 0x00, 0x7a, 0xdb, 0x22, 0xe3, 0x89, 0xa4, 0x47, 0x97, 0xfe,
    0xd7, 0xde, 0x09, 0x0c, 0x8c, 0x03, 0xca, 0x8a, 0x2c, 0x5a, 0xcd, 0x9e, 0x84, 0xdf, 0x37, 0xfb,
    0xc5, 0x8c, 0xe8, 0xed, 0xb2, 0x93, 0xe9, 0x8f, 0x02, 0xb6, 0x40, 0xd6, 0xd1, 0xd7, 0x24, 0x64
};

/**************************************************************************************************
 * FUNCTION:
 *  void psacrypto_AES256CBC_example_NIST(void);
 *
 * DESCRIPTION:
 *  This function shows AES256-CBC encryption/decryption operations on a NIST test vector.
 *
 * PARAMETERS:
 *  none
 *
 * RETURN:
 *  bool
 *
 * NOTES:
 *  NIST test vector taken from SP800-38a.
 *
 *  Since we're using a known IV, we have to use multi-part operation.
 *
 *  The output of the multipart operation does not include the IV.
 *
 *  Since the NIST test vector is block-aligned, we can use PSA_ALG_CBC_NO_PADDING.
**************************************************************************************************/
fsp_err_t psacrypto_AES256CBC_example_NIST(void)
{
    const psa_algorithm_t       alg         = PSA_ALG_CBC_NO_PADDING;  // Use PSA_ALG_CBC_PKCS7 for non-aligned data
    psa_key_attributes_t        attributes  = PSA_KEY_ATTRIBUTES_INIT;
    mbedtls_platform_context    ctx         = {0};
    uint8_t                     decrypted_data [sizeof(NIST_AESCBC256_PT)] = {0};
    size_t                      decrypted_length;
    uint8_t                     encrypted_data [sizeof(NIST_AESCBC256_CT)] = {0};
    size_t                      encrypted_length;
    rsip_aes_wrapped_key_t      injected_aes_key;
    psa_key_handle_t            key_handle              = 0;
    size_t                      length_portion;
    psa_cipher_operation_t      operation_1             = PSA_CIPHER_OPERATION_INIT;
    psa_cipher_operation_t      operation_2             = PSA_CIPHER_OPERATION_INIT;

    psa_status_t                psa_status              = PSA_SUCCESS;
    fsp_err_t                   fsp_status              = FSP_SUCCESS;
    int                         status                  = RESET_VALUE;

    status = mbedtls_platform_setup(&ctx);
    if (RESET_VALUE != status)
    {
       APP_PRINT("mbedtls_platform_setup failed.\r\n");
       return (fsp_err_t)status;
    }

    psa_status = psa_crypto_init();
    if (PSA_SUCCESS != psa_status)
    {
       APP_PRINT("psa_crypto_init failed.\r\n");
       return (fsp_err_t)psa_status;
    }

    /* Set key attributes */
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES_WRAPPED);

    psa_set_key_bits(&attributes, AES256_KEY_BITS);
    psa_set_key_lifetime(&attributes, PSA_KEY_LIFETIME_PERSISTENT);
    psa_set_key_id(&attributes, (psa_key_id_t)1);

    fsp_status = R_RSIP_AES256_InitialKeyWrap(RSIP_KEY_INJECTION_TYPE_PLAIN, NULL, NULL, NIST_AESCBC256_Key, &injected_aes_key);
    if (FSP_SUCCESS == fsp_status)
    {
        APP_PRINT("R_RSIP_AES256_InitialKeyWrap is successful.\r\n");
        /* Inject the wrapped AES key */
        psa_status = psa_import_key(&attributes, (uint8_t *)&injected_aes_key.value[0], SIZE_AES_256BIT_KEYLEN_BYTES_WRAPPED, &key_handle);
        if (PSA_SUCCESS != psa_status)
        {
           APP_PRINT("psa_import_key failed.\r\n");
           return (fsp_err_t)psa_status;
        }
    }
    else
    {
        APP_PRINT("R_RSIP_AES256_InitialKeyWrap failed.\r\n");
        return fsp_status;
    }

    /* AES256-CBC Encryption - Multi-part operation is required for CBC with fixed IV */

    /* Initialize the encryption */
    operation_1 = psa_cipher_operation_init();
    psa_status = psa_cipher_encrypt_setup(&operation_1, key_handle, alg);
    if (PSA_SUCCESS != psa_status)
    {
        APP_PRINT("psa_cipher_encrypt_setup failed.\r\n");
        return (fsp_err_t)psa_status;
    }

    /* Set the initial counter */
    psa_status = psa_cipher_set_iv(&operation_1, NIST_AESCBC256_IV, sizeof(NIST_AESCBC256_IV));
    if (PSA_SUCCESS != status)
    {
        APP_PRINT("psa_cipher_set_iv for encryption failed.\r\n");
        return (fsp_err_t)psa_status;
    }

    /* Encrypt plain text */
    encrypted_length = 0;
    psa_status = psa_cipher_update(&operation_1, NIST_AESCBC256_PT, sizeof(NIST_AESCBC256_PT), encrypted_data, sizeof(encrypted_data), &length_portion);
    if (PSA_SUCCESS != status)
    {
       APP_PRINT("psa_cipher_update for encryption failed.\r\n");
       return (fsp_err_t)psa_status;
    }
    encrypted_length += length_portion;

    /* Finish the encryption.  Set *output and output_size to account for the data that has been encrypted. */
    psa_status = psa_cipher_finish(&operation_1, encrypted_data + encrypted_length, sizeof(encrypted_data) - encrypted_length, &length_portion);
    if (PSA_SUCCESS != psa_status)
    {
       APP_PRINT("psa_cipher_finish for encryption failed. \r\n");
       return (fsp_err_t)psa_status;
    }

    encrypted_length += length_portion;

    /* Confirm generated cipher text */
    status = memcmp((void *)NIST_AESCBC256_CT, (void *)encrypted_data, sizeof(NIST_AESCBC256_CT));
    if (RESET_VALUE != status)
    {
        APP_PRINT("Encryption failed.\r\n");
        return (fsp_err_t)status;
    }
    else
    {
        APP_PRINT("Encryption operation is successful.\r\n");
    }

    /* AES256-CBC Decryption */

    /* Initialize the decryption */
    operation_2 = psa_cipher_operation_init();
    psa_status = psa_cipher_decrypt_setup(&operation_2, key_handle, alg);
    if (PSA_SUCCESS != psa_status)
    {
        APP_PRINT("psa_cipher_decrypt_setup failed.\r\n");
        return (fsp_err_t)psa_status;
    }

    /* Set the initial counter */
    psa_status = psa_cipher_set_iv(&operation_2, NIST_AESCBC256_IV, sizeof(NIST_AESCBC256_IV));
    if (PSA_SUCCESS != psa_status)
    {
        APP_PRINT("psa_cipher_set_iv for decryption failed.\r\n");
        return (fsp_err_t)psa_status;
    }

    /* Decrypt cipher text */
    decrypted_length = 0;
    psa_status = psa_cipher_update(&operation_2, NIST_AESCBC256_CT, sizeof(NIST_AESCBC256_CT),
                                   decrypted_data, sizeof(decrypted_data), &length_portion);
    if (PSA_SUCCESS != psa_status)
    {
        APP_PRINT("psa_cipher_update for decryption failed.\r\n");
        return (fsp_err_t)psa_status;
    }
    decrypted_length += length_portion;

    // Finish the decryption
    psa_status = psa_cipher_finish(&operation_2, decrypted_data + decrypted_length, sizeof(decrypted_data) - decrypted_length, &length_portion);
    if (PSA_SUCCESS != psa_status)
    {
        APP_PRINT("psa_cipher_finish for decryption failed. \r\n");
        return (fsp_err_t)psa_status;
    }

    decrypted_length += length_portion;

    /* Confirm decrypted plaintext */
    status = memcmp(NIST_AESCBC256_PT, decrypted_data, sizeof(NIST_AESCBC256_CT));
    if (status != RESET_VALUE)
    {
        APP_PRINT("Decryption failed.\r\n");
        return (fsp_err_t)status;
    }
    else
    {
        APP_PRINT("Decryption operation is successful.\r\n");
    }


    /* Destroy the key */
    psa_status = psa_destroy_key(key_handle);
    if (PSA_SUCCESS != psa_status)
    {
      APP_PRINT("psa_destroy_key failed.\r\n");
      return (fsp_err_t)psa_status;
    }

    mbedtls_platform_teardown(&ctx);
    mbedtls_psa_crypto_free();

    return fsp_status;
}
