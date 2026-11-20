/***********************************************************************************************************************
 * File Name    : pqc_mlkem.c
 * Description  : Contains data structures and functions used in pqc_mlkem.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "pqc_mlkem.h"

/***********************************************************************************************************************
 * Function Name: setup_mlkem_attrs.
 * Description  : This function sets up universal key attributes for ML-KEM.
 * Arguments    : attrs                     Pointer to attributes structure.
 *                key_type                  Type of key (keypair/public/shared_secret).
 *                bits                      Key size in bits (ignored for shared_secret).
 *                lifetime                  Key lifetime (volatile/persistent).
 * Return Value : None.
 **********************************************************************************************************************/
void setup_mlkem_attrs(psa_key_attributes_t * attrs, mlkem_key_type_t key_type, size_t bits,
                       psa_key_lifetime_t lifetime)
{
    psa_reset_key_attributes(attrs);
    psa_set_key_algorithm(attrs, PSA_ALG_ML_KEM);
    psa_set_key_lifetime(attrs, lifetime);

    switch (key_type)
    {
        case MLKEM_KEY_KEYPAIR:
        {
            psa_set_key_usage_flags(attrs, PSA_KEY_USAGE_DECAPSULATE | PSA_KEY_USAGE_EXPORT);
            psa_set_key_type(attrs, PSA_KEY_TYPE_ML_KEM_KEY_PAIR);
            psa_set_key_bits(attrs, bits);
            if (PSA_KEY_LIFETIME_IS_PERSISTENT(lifetime))
            {
                psa_set_key_id(attrs, MLKEM_DECAPSULATOR_KEYPAIR_ID);
            }
            break;
        }

        case MLKEM_KEY_PUBLIC:
        {
            psa_set_key_usage_flags(attrs, PSA_KEY_USAGE_ENCAPSULATE);
            psa_set_key_type(attrs, PSA_KEY_TYPE_ML_KEM_PUBLIC_KEY);
            psa_set_key_bits(attrs, bits);
            if (PSA_KEY_LIFETIME_IS_PERSISTENT(lifetime))
            {
                psa_set_key_id(attrs, MLKEM_ENCAPSULATOR_PUBLIC_KEY_ID);
            }
            break;
        }

        case MLKEM_KEY_SHARED_SECRET:
        {
            psa_set_key_usage_flags(attrs, PSA_KEY_USAGE_EXPORT);
            psa_set_key_type(attrs, PSA_KEY_TYPE_RAW_DATA);
            psa_set_key_bits(attrs, PSA_BYTES_TO_BITS(PSA_ML_KEM_SHARED_SECRET_SIZE));

            /* Force shared secrets to be VOLATILE even if keypair is PERSISTENT */
            if (PSA_KEY_LIFETIME_IS_PERSISTENT(lifetime))
            {
                psa_set_key_lifetime(attrs, PSA_KEY_LIFETIME_VOLATILE);
            }
            break;
        }

        default:
        {
            break;
        }
    }
}
/***********************************************************************************************************************
 * End of function setup_mlkem_attrs.
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: compare_shared_secrets.
 * Description  : This function exports and compares two shared secrets.
 * Arguments    : ss1_handle                First shared secret.
 *                ss2_handle                Second shared secret.
 * Return Value : PSA_SUCCESS               Operated successfully.
 *                Any other error codes     Failed to operate.
 **********************************************************************************************************************/
psa_status_t compare_shared_secrets(psa_key_id_t ss1_handle, psa_key_id_t ss2_handle)
{
    uint8_t ss1[PSA_ML_KEM_SHARED_SECRET_SIZE];
    uint8_t ss2[PSA_ML_KEM_SHARED_SECRET_SIZE];
    size_t ss1_len = 0, ss2_len = 0;
    psa_status_t status = PSA_SUCCESS;

    status = psa_export_key(ss1_handle, ss1, sizeof(ss1), &ss1_len);
    if (PSA_SUCCESS != status)
    {
        return status;
    }

    status = psa_export_key(ss2_handle, ss2, sizeof(ss2), &ss2_len);
    if (PSA_SUCCESS != status)
    {
        return status;
    }

    if (ss1_len != ss2_len || memcmp(ss1, ss2, ss1_len) != 0)
    {
        return PSA_ERROR_GENERIC_ERROR;
    }

    return status;
}
/***********************************************************************************************************************
 * End of function compare_shared_secrets.
 **********************************************************************************************************************/
