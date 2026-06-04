/***********************************************************************************************************************
 * File Name    : pqc_mldsa.h
 * Description  : Contains data structures and macros used in pqc_mldsa.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef PQC_MLDSA_H_
#define PQC_MLDSA_H_

#include "pqc_common_app.h"

/* Enumeration for ML-DSA key types */
typedef enum
{
    MLDSA_KEY_KEYPAIR = 0,
    MLDSA_KEY_PUBLIC  = 1,
} mldsa_key_type_t;

/* Persistent key IDs (only used when lifetime is PERSISTENT) */
#define MLDSA_SIGNER_KEYPAIR_ID         ((mbedtls_svc_key_id_t) 1003)
#define MLDSA_VERIFIER_PUBLIC_KEY_ID    ((mbedtls_svc_key_id_t) 1004)

/* Public functions declarations */
void setup_mldsa_attrs(psa_key_attributes_t *attrs, mldsa_key_type_t key_type, size_t bits,
        psa_key_lifetime_t lifetime);
psa_status_t mldsa_export_import_keypair(size_t bits, psa_key_lifetime_t lifetime);
psa_status_t mldsa_two_party_signing(size_t bits, psa_key_lifetime_t lifetime);

#endif /* PQC_MLDSA_H_ */
