
/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef APP_RUNTIME_ENCRYPTION_H_
#define APP_RUNTIME_ENCRYPTION_H_

/***************************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 ***************************************************************************************************************************/

#include "hal_data.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define PLAINTEXT_DATA_SIZE         (512)
#define ENCRYPTED_DATA_SIZE         (512)   /* the size of the encrypted data size equals the size of the plaintext data size */

#define AES128_KEY_BITS             (128)
#define AES_128_BIT_LENGTH          (128)
#define AES_128_EXPORTED_SIZE       (500)

#define PSA_ERR_RETURN(err, action) do {        \
    if (PSA_SUCCESS != err) {                   \
        action;                                 \
    }                                           \
} while (0)

/**********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/
extern uint8_t                encrypted_data[ENCRYPTED_DATA_SIZE];
extern uint8_t                plaintext_data[ENCRYPTED_DATA_SIZE];

extern uint8_t encryption_key[];
extern uint8_t encryption_iv[];

/**********************************************************************************************************************
 * Global Function Prototypes
 **********************************************************************************************************************/

psa_status_t init_mbedtls_crypto(void);
psa_status_t AES_CTR_encryption(void);

#endif /* APP_RUNTIME_ENCRYPTION_H_ */
