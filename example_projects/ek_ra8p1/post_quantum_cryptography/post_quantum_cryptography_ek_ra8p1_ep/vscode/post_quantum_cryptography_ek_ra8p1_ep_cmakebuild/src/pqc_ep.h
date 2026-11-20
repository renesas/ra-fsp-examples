/***********************************************************************************************************************
 * File Name    : pqc_ep.h
 * Description  : Contains macros, data structures, and functions used in pqc_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef PQC_EP_H_
#define PQC_EP_H_

#include <stdbool.h>
#include "hal_data.h"

/* Macro for the reset value */
#define RESET_VALUE         (0x00)

/* Macro for the null char */
#define NULL_CHAR           ('\0')

/* Macro for the buffer size */
#define BUFFER_SIZE         (16U)

/* Enumeration for PQC demonstration menu options */
typedef enum e_pqc_demonstration
{
    ML_KEM_KEY_EXCHANGE = 1,
    ML_KEM_EXPORT_IMPORT,
    ML_DSA_SIGN_VERIFY,
    ML_DSA_EXPORT_IMPORT,
    PQC_REMOTE_VERIFY
} pqc_demonstration_t;

/* Function declarations */
fsp_err_t littlefs_init(void);
void littlefs_deinit(void);
psa_status_t cleanup_keys(psa_key_id_t * handles, size_t count);

#endif /* PQC_EP_H_ */
