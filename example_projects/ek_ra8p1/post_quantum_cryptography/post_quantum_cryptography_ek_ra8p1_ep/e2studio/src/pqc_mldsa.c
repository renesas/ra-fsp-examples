/***********************************************************************************************************************
 * File Name    : pqc_mldsa.c
 * Description  : Contains data structures and functions used in pqc_mldsa.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "pqc_mldsa.h"

/***********************************************************************************************************************
 * Function Name: setup_mldsa_attrs.
 * Description  : This function sets up universal key attributes for ML-DSA.
 * Arguments    : attrs                     Pointer to attributes structure.
 *                key_type                  Type of key (keypair/public).
 *                bits                      Key size in bits.
 *                lifetime                  Key lifetime (volatile/persistent).
 * Return Value : None.
 **********************************************************************************************************************/
void setup_mldsa_attrs(psa_key_attributes_t * attrs, mldsa_key_type_t key_type, size_t bits,
                       psa_key_lifetime_t lifetime)
{
    psa_reset_key_attributes(attrs);
    psa_set_key_algorithm(attrs, PSA_ALG_ML_DSA);
    psa_set_key_lifetime(attrs, lifetime);
    psa_set_key_bits(attrs, bits);

    switch (key_type)
    {
        case MLDSA_KEY_KEYPAIR:
        {
            psa_set_key_usage_flags(attrs, PSA_KEY_USAGE_SIGN_MESSAGE | PSA_KEY_USAGE_EXPORT);
            psa_set_key_type(attrs, PSA_KEY_TYPE_ML_DSA_KEY_PAIR);
            if (PSA_KEY_LIFETIME_IS_PERSISTENT(lifetime))
            {
                psa_set_key_id(attrs, MLDSA_SIGNER_KEYPAIR_ID);
            }
            break;
        }

        case MLDSA_KEY_PUBLIC:
        {
            psa_set_key_usage_flags(attrs, PSA_KEY_USAGE_VERIFY_MESSAGE);
            psa_set_key_type(attrs, PSA_KEY_TYPE_ML_DSA_PUBLIC_KEY);
            if (PSA_KEY_LIFETIME_IS_PERSISTENT(lifetime))
            {
                psa_set_key_id(attrs, MLDSA_VERIFIER_PUBLIC_KEY_ID);
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
 * End of function setup_mldsa_attrs.
 **********************************************************************************************************************/
