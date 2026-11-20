/***********************************************************************************************************************
 * File Name    : pqc_mldsa.h
 * Description  : Contains macros, data structures, and functions used in pqc_mldsa.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef PQC_MLDSA_H_
#define PQC_MLDSA_H_

#include "hal_data.h"

/* Persistent key IDs (only used when lifetime is PERSISTENT) */
#define MLDSA_SIGNER_KEYPAIR_ID         ((mbedtls_svc_key_id_t)1003)
#define MLDSA_VERIFIER_PUBLIC_KEY_ID    ((mbedtls_svc_key_id_t)1004)

/* Enumeration for ML-DSA key types */
typedef enum
{
    MLDSA_KEY_KEYPAIR = 0,
    MLDSA_KEY_PUBLIC
} mldsa_key_type_t;

/* Function declaration */
void setup_mldsa_attrs(psa_key_attributes_t * attrs, mldsa_key_type_t key_type, size_t bits,
                       psa_key_lifetime_t lifetime);

#endif /* PQC_MLDSA_H_ */
