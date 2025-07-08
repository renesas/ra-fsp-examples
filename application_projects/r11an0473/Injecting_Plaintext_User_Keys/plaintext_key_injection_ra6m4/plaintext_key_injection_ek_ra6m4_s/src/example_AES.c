/***********************************************************************************************************************
 * File Name    : example_AES.c
 * Description  : Contains AES plaintext key injection and cryptographic operations
 ***********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
************************************************************************************************************************/

#include <example_AES.h>
#include "stdio.h"
#include "hal_data.h"
#include "r_sce_key_injection.h"

//--------------------------------------------------------------------------------------------------

#define AES_BLOCK_SIZE          PSA_BLOCK_CIPHER_BLOCK_LENGTH(PSA_KEY_TYPE_AES)
#define AES_IV_SIZE             AES_BLOCK_SIZE
#define AES256_KEY_BITS         256
#define MESSAGE_SIZE            128

/** Key injection type. */
typedef enum e_sce_key_injection_type
{
    SCE_KEY_INJECTION_TYPE_ENCRYPTED = 0, ///< Input encrypted user key
    SCE_KEY_INJECTION_TYPE_PLAIN     = 1, ///< Input plain user key
} sce_key_injection_type_t;

static const uint8_t NIST_AESCBC256_Key[] = {
    0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe, 0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
    0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7, 0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4,
};

static const uint8_t NIST_AESCBC256_IV[] = {
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
};

static const uint8_t NIST_AESCBC256_PT[] = {
    0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
    0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c, 0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
    0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11, 0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
    0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17, 0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10,
};

static const uint8_t NIST_AESCBC256_CT[] = {
    0xf5, 0x8c, 0x4c, 0x04, 0xd6, 0xe5, 0xf1, 0xba, 0x77, 0x9e, 0xab, 0xfb, 0x5f, 0x7b, 0xfb, 0xd6,
    0x9c, 0xfc, 0x4e, 0x96, 0x7e, 0xdb, 0x80, 0x8d, 0x67, 0x9f, 0x77, 0x7b, 0xc6, 0x70, 0x2c, 0x7d,
    0x39, 0xf2, 0x33, 0x69, 0xa9, 0xd9, 0xba, 0xcf, 0xa5, 0x30, 0xe2, 0x63, 0x04, 0x23, 0x14, 0x61,
    0xb2, 0xeb, 0x05, 0xe2, 0xc3, 0x9b, 0xe9, 0xfc, 0xda, 0x6c, 0x19, 0x07, 0x8c, 0x6a, 0x9d, 0x1b,
};

/*****************************************************************************************
 *  @brief      init_lfs: format and mount the LittleFS system for key management
 *  @retval     bool
 *  **************************************************************************************/
bool init_lfs(void)
{
    /* - To use persistent keys:
     * -    The file system must be initialized prior to calling the generate/import key functions.
     */
    volatile fsp_err_t lfs_status = FSP_SUCCESS;
    lfs_status = RM_LITTLEFS_FLASH_Open(&g_rm_littlefs0_ctrl, &g_rm_littlefs0_cfg);

    if (FSP_SUCCESS == lfs_status)
    {
        lfs_status = lfs_format(&g_rm_littlefs0_lfs, &g_rm_littlefs0_lfs_cfg);
        if (FSP_SUCCESS == lfs_status)
        {
            lfs_status = lfs_mount(&g_rm_littlefs0_lfs, &g_rm_littlefs0_lfs_cfg);
        }

    }
    if (FSP_SUCCESS == lfs_status)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/*****************************************************************************************
 *  @brief      inject plaintext key and perform encryption and decryption operations.
 *  @retval     bool
 *  **************************************************************************************/
bool psacrypto_AES256CBC_example_NIST(void)
{
    const psa_algorithm_t       alg         = PSA_ALG_CBC_NO_PADDING;  // Use PSA_ALG_CBC_PKCS7 for non-aligned data
    psa_key_attributes_t        attributes  = PSA_KEY_ATTRIBUTES_INIT;
    mbedtls_platform_context    ctx         = {0};
    uint8_t                     decrypted_data[sizeof (NIST_AESCBC256_PT)] = {0};
    size_t                      decrypted_length;
    uint8_t                     encrypted_data[sizeof (NIST_AESCBC256_CT)] = {0};
    size_t                      encrypted_length;
    sce_aes_wrapped_key_t       injected_aes_key;
    psa_key_handle_t            key_handle  = 0;
    size_t                      length_portion;
    psa_cipher_operation_t      operation_1 = PSA_CIPHER_OPERATION_INIT;
    psa_cipher_operation_t      operation_2 = PSA_CIPHER_OPERATION_INIT;
    psa_status_t                status;
    fsp_err_t                   status_fsp;
    uint8_t                     key_type    = SCE_KEY_INJECTION_TYPE_PLAIN;

    if (PSA_SUCCESS != mbedtls_platform_setup(&ctx))
    {
        return false;
    }

    if (PSA_SUCCESS != psa_crypto_init())
    {
        return false;
    }
    //---------------------------------------------------------------------------------------------
    // Import key

    // Set key attributes
    psa_set_key_usage_flags(&attributes, PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT);
    psa_set_key_algorithm(&attributes, alg);
    psa_set_key_type(&attributes, PSA_KEY_TYPE_AES_WRAPPED);

    psa_set_key_bits(&attributes, AES256_KEY_BITS);

    psa_set_key_lifetime(&attributes, PSA_KEY_LIFETIME_PERSISTENT);
    psa_set_key_id(&attributes, (psa_key_id_t)1);

    status_fsp = R_SCE_AES256_InitialKeyWrap(&key_type, NULL, NULL,
                                             NIST_AESCBC256_Key, &injected_aes_key);
    if (FSP_SUCCESS != status_fsp)
    {
        return false;
    }
    /* inject the wrapped AES key */
    status = psa_import_key(&attributes, (uint8_t *)&injected_aes_key.value[0], SIZE_AES_256BIT_KEYLEN_BYTES_WRAPPED, &key_handle);
    if (FSP_SUCCESS != status)
    {
        return false;
    }

     //---------------------------------------------------------------------------------------------
    // AES256-CBC Encryption - Multi-part operation is required for CBC with fixed IV

    // Initialise the encryption
    operation_1 = psa_cipher_operation_init();
    status = psa_cipher_encrypt_setup(&operation_1, key_handle, alg);
    if (PSA_SUCCESS != status)
    {
        return false;
    }
    // Set the initial counter
    status = psa_cipher_set_iv(&operation_1, NIST_AESCBC256_IV, sizeof(NIST_AESCBC256_IV));
    if (PSA_SUCCESS == status)
    {
        // Encrypt plain text
        encrypted_length = 0;
        status = psa_cipher_update(&operation_1, NIST_AESCBC256_PT, sizeof(NIST_AESCBC256_PT), encrypted_data, sizeof(encrypted_data), &length_portion);
        if (PSA_SUCCESS == status)
        {

            encrypted_length += length_portion;

            // Finish the encryption.  Set *output and output_size to account for the data that has been encrypted.
            status = psa_cipher_finish(&operation_1, encrypted_data + encrypted_length, sizeof(encrypted_data) - encrypted_length, &length_portion);
            if (PSA_SUCCESS == status)
            {

                encrypted_length += length_portion;

                //---------------------------------------------------------------------------------------------
                // Confirm generated cipher text

                if (memcmp((void *)NIST_AESCBC256_CT, (void *)encrypted_data, sizeof(NIST_AESCBC256_CT)))
                {
                  return false;
                }
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }


    //---------------------------------------------------------------------------------------------
    // AES256-CBC Decryption

    // Initialise the decryption
    operation_2 = psa_cipher_operation_init();
    status = psa_cipher_decrypt_setup(&operation_2, key_handle, alg);
    if (PSA_SUCCESS == status)
    {
         // Set the initial counter
        status = psa_cipher_set_iv(&operation_2, NIST_AESCBC256_IV, sizeof(NIST_AESCBC256_IV));
        if (PSA_SUCCESS == status)
        {

           // Decrypt cipher text
            decrypted_length = 0;
            status = psa_cipher_update(&operation_2, NIST_AESCBC256_CT, sizeof(NIST_AESCBC256_CT),
                                        decrypted_data, sizeof(decrypted_data), &length_portion);
            if (PSA_SUCCESS == status)
            {

                decrypted_length += length_portion;

                // Finish the decryption
                status = psa_cipher_finish(&operation_2, decrypted_data + decrypted_length, sizeof(decrypted_data) - decrypted_length, &length_portion);
                if (PSA_SUCCESS == status)
                {


                    decrypted_length += length_portion;

                    //---------------------------------------------------------------------------------------------
                    // Confirm decrypted plaintext

                    if (memcmp(NIST_AESCBC256_PT, decrypted_data, sizeof(NIST_AESCBC256_CT)))
                    {
                       return false;
                    }
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }


        //---------------------------------------------------------------------------------------------
        // Destroy the key

        status = psa_destroy_key(key_handle);
        if (PSA_SUCCESS != status)
        {
            return false;
        }

        mbedtls_platform_teardown(&ctx);
        mbedtls_psa_crypto_free();
    }

    return true;
}
