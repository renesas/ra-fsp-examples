/***********************************************************************************************************************
 * File Name    : pqc_mlkem.h
 * Description  : Contains macros, data structures, and functions used in pqc_mlkem.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef PQC_MLKEM_H_
#define PQC_MLKEM_H_

#include "hal_data.h"

/* Persistent key IDs (only used when lifetime is PERSISTENT) */
#define MLKEM_DECAPSULATOR_KEYPAIR_ID           ((mbedtls_svc_key_id_t)1001)
#define MLKEM_ENCAPSULATOR_PUBLIC_KEY_ID        ((mbedtls_svc_key_id_t)1002)

/* Enumeration for ML-KEM key types */
typedef enum
{
    MLKEM_KEY_KEYPAIR,
    MLKEM_KEY_PUBLIC,
    MLKEM_KEY_SHARED_SECRET
} mlkem_key_type_t;

/* Function declarations */
void setup_mlkem_attrs(psa_key_attributes_t * attrs, mlkem_key_type_t key_type, size_t bits,
                       psa_key_lifetime_t lifetime);
psa_status_t compare_shared_secrets(psa_key_id_t ss1_handle, psa_key_id_t ss2_handle);

#endif /* PQC_MLKEM_H_ */
