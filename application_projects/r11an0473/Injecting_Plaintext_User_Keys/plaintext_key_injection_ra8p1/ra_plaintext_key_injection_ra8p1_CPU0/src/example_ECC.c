/***********************************************************************************************************************
 * File Name    : example_ECC.h
 * Description  : Contains function prototypes for ECC plaintext key injection and cryptographic operations
 ***********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
************************************************************************************************************************/

#include "hal_data.h"
#include "common_utils.h"
#include "example_ECC.h"

#include "r_rsip_key_injection.h"
#include "hw_sce_ra_private.h"

#define ECC256_KEY_BITS             (256U)

#define ECC_PUBLIC_KEY_LENGTH       (64U)
#define ECC_UNCOMPRESSED_LENGTH     (1U)

typedef enum {
    ECC_PRIVATE_KEY_ID = 1,
    ECC_PUBLIC_KEY_ID
} key_id_t;

/* NIST Test Vector */
static const uint8_t NIST_MSG[] = {
   0x59, 0x05, 0x23, 0x88, 0x77, 0xc7, 0x74, 0x21, 0xf7, 0x3e, 0x43, 0xee, 0x3d, 0xa6, 0xf2, 0xd9,
   0xe2, 0xcc, 0xad, 0x5f, 0xc9, 0x42, 0xdc, 0xec, 0x0c, 0xbd, 0x25, 0x48, 0x29, 0x35, 0xfa, 0xaf,
   0x41, 0x69, 0x83, 0xfe, 0x16, 0x5b, 0x1a, 0x04, 0x5e, 0xe2, 0xbc, 0xd2, 0xe6, 0xdc, 0xa3, 0xbd,
   0xf4, 0x6c, 0x43, 0x10, 0xa7, 0x46, 0x1f, 0x9a, 0x37, 0x96, 0x0c, 0xa6, 0x72, 0xd3, 0xfe, 0xb5,
   0x47, 0x3e, 0x25, 0x36, 0x05, 0xfb, 0x1d, 0xdf, 0xd2, 0x80, 0x65, 0xb5, 0x3c, 0xb5, 0x85, 0x8a,
   0x8a, 0xd2, 0x81, 0x75, 0xbf, 0x9b, 0xd3, 0x86, 0xa5, 0xe4, 0x71, 0xea, 0x7a, 0x65, 0xc1, 0x7c,
   0xc9, 0x34, 0xa9, 0xd7, 0x91, 0xe9, 0x14, 0x91, 0xeb, 0x37, 0x54, 0xd0, 0x37, 0x99, 0x79, 0x0f,
   0xe2, 0xd3, 0x08, 0xd1, 0x61, 0x46, 0xd5, 0xc9, 0xb0, 0xd0, 0xde, 0xbd, 0x97, 0xd7, 0x9c, 0xe8
};

static const uint8_t NIST_PRIVATE_KEY[] = {
     0x51, 0x9b, 0x42, 0x3d, 0x71, 0x5f, 0x8b, 0x58, 0x1f, 0x4f, 0xa8, 0xee, 0x59, 0xf4, 0x77, 0x1a,
     0x5b, 0x44, 0xc8, 0x13, 0x0b, 0x4e, 0x3e, 0xac, 0xca, 0x54, 0xa5, 0x6d, 0xda, 0x72, 0xb4, 0x64
};

/* Combine R and S */
static const uint8_t NIST_SIGNATURE[] = {
    0xf3, 0xac, 0x80, 0x61, 0xb5, 0x14, 0x79, 0x5b, 0x88, 0x43, 0xe3, 0xd6, 0x62, 0x95, 0x27, 0xed,
    0x2a, 0xfd, 0x6b, 0x1f, 0x6a, 0x55, 0x5a, 0x7a, 0xca, 0xbb, 0x5e, 0x6f, 0x79, 0xc8, 0xc2, 0xac,
    0x8b, 0xf7, 0x78, 0x19, 0xca, 0x05, 0xa6, 0xb2, 0x78, 0x6c, 0x76, 0x26, 0x2b, 0xf7, 0x37, 0x1c,
    0xef, 0x97, 0xb2, 0x18, 0xe9, 0x6f, 0x17, 0x5a, 0x3c, 0xcd, 0xda, 0x2a, 0xcc, 0x05, 0x89, 0x03
};

fsp_err_t psacrypto_ECC256_example_NIST()
{
    psa_status_t psa_status = PSA_SUCCESS;
    fsp_err_t fsp_status = FSP_SUCCESS;
    int mbedtls_status = RESET_VALUE;

    mbedtls_platform_context ctx = {0};

    psa_key_attributes_t priv_attributes = PSA_KEY_ATTRIBUTES_INIT;
    psa_key_attributes_t pub_attributes  = PSA_KEY_ATTRIBUTES_INIT;

    psa_key_handle_t priv_key_handle = 0;
    psa_key_handle_t pub_key_handle  = 0;

    rsip_ecc_private_wrapped_key_t  injected_ecc_priv_key;

    mbedtls_status = mbedtls_platform_setup(&ctx);
    if (RESET_VALUE != mbedtls_status)
    {
        APP_PRINT("mbedtls_platform_setup failed.\r\n");
        return (fsp_err_t)mbedtls_status;
    }

    psa_status = psa_crypto_init();
    if (PSA_SUCCESS != psa_status)
    {
        APP_PRINT("psa_crypto_init failed.\r\n");
        return (fsp_err_t)psa_status;
    }

    /* Set private key attributes */
    psa_set_key_usage_flags(&priv_attributes, PSA_KEY_USAGE_VERIFY_MESSAGE);
    psa_set_key_algorithm(&priv_attributes, PSA_ALG_ECDSA(PSA_ALG_SHA_256));
    psa_set_key_type(&priv_attributes, PSA_KEY_TYPE_ECC_KEY_PAIR_WRAPPED(PSA_ECC_FAMILY_SECP_R1));
    psa_set_key_bits(&priv_attributes, ECC256_KEY_BITS);
    psa_set_key_lifetime(&priv_attributes, PSA_KEY_LIFETIME_PERSISTENT);
    psa_set_key_id(&priv_attributes, (psa_key_id_t)(ECC_PRIVATE_KEY_ID));

    fsp_status = R_RSIP_ECC_secp256r1_InitialPrivateKeyWrap(RSIP_KEY_INJECTION_TYPE_PLAIN, NULL, NULL, NIST_PRIVATE_KEY, &injected_ecc_priv_key);
    if (FSP_SUCCESS == fsp_status)
    {
        APP_PRINT("R_RSIP_ECC_secp256r1_InitialPrivateKeyWrap is successful.\r\n");

        /* Import private key */
        psa_status = psa_import_key(&priv_attributes, (uint8_t *)&injected_ecc_priv_key.value[0], ECC_256_PRIVATE_KEY_HRK_LENGTH_WORDS * 4 , &priv_key_handle);
        if (PSA_SUCCESS != psa_status)
        {
            APP_PRINT("Failed to import public key, status: %d.\r\n", psa_status);
            return (fsp_err_t)psa_status;
        }
        else
        {
            APP_PRINT("Private key imported successfully.\r\n");
        }
    }
    else
    {
        APP_PRINT("R_RSIP_ECC_secp256r1_InitialPrivateKeyWrap failed.\r\n");
        return fsp_status;
    }

    uint8_t public_key[ECC_PUBLIC_KEY_LENGTH + ECC_UNCOMPRESSED_LENGTH];
    size_t public_key_length;
    psa_status = psa_export_public_key(priv_key_handle, public_key, sizeof(public_key), &public_key_length);
    if (PSA_SUCCESS != psa_status)
    {
        APP_PRINT("Export public key failed\n");
        return (fsp_err_t)psa_status;
    }

    /* Set public key attributes */
    psa_set_key_usage_flags(&pub_attributes, PSA_KEY_USAGE_VERIFY_HASH);
    psa_set_key_algorithm(&pub_attributes, PSA_ALG_ECDSA(PSA_ALG_SHA_256));
    psa_set_key_type(&pub_attributes, PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1));
    psa_set_key_bits(&pub_attributes, ECC256_KEY_BITS);
    psa_set_key_lifetime(&pub_attributes, PSA_KEY_LIFETIME_PERSISTENT);
    psa_set_key_id(&pub_attributes, (psa_key_id_t)(ECC_PUBLIC_KEY_ID));

    /* Import public key */
    psa_status = psa_import_key(&pub_attributes, public_key, public_key_length, &pub_key_handle);
    if (PSA_SUCCESS != psa_status)
    {
        APP_PRINT("Failed to import public key, status: %d.\r\n", psa_status);
        return (fsp_err_t)psa_status;
    }
    else
    {
        APP_PRINT("Public key imported successfully.\r\n");
    }

    /* Verify message with public key */
    psa_status = psa_verify_message(pub_key_handle, PSA_ALG_ECDSA(PSA_ALG_SHA_256), NIST_MSG, sizeof(NIST_MSG), NIST_SIGNATURE, sizeof(NIST_SIGNATURE));
    if (PSA_SUCCESS == psa_status)
    {
      APP_PRINT("NIST signature verified successfully\r\n");
    }
    else
    {
      APP_PRINT("NIST signature verification failed, status: %d\r\n", psa_status);
      return (fsp_err_t)psa_status;
    }

    /* Destroy key */
    psa_status = psa_destroy_key(priv_key_handle);
    psa_status |= psa_destroy_key(pub_key_handle);
    if (PSA_SUCCESS != psa_status)
    {
        APP_PRINT("Destroy key is failed.\r\n");
        return (fsp_err_t)psa_status;
    }

    mbedtls_platform_teardown(&ctx);
    mbedtls_psa_crypto_free();

   return fsp_status;
}
