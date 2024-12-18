
/*
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#ifndef RUNTIME_ENCRYPTION_H_
#define RUNTIME_ENCRYPTION_H_

/***************************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 ***************************************************************************************************************************/

#include "hal_data.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define PLAINTEXT_DATA_SIZE     512
#define ENCRYPTED_DATA_SIZE     512 /*the size of the encrypted data size equals the size of the plaintext data size */


/**********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/
extern uint8_t                encrypted_data[ENCRYPTED_DATA_SIZE];
extern uint8_t                plaintext_data[ENCRYPTED_DATA_SIZE];

/**********************************************************************************************************************
 * Global Function Prototypes
 **********************************************************************************************************************/
bool encrypt_runtime_data_using_aes_ctr ( void );
bool init_mbedtls_crypto(void);
void AES_CTR_encryption(void);
extern uint8_t encryption_key[];
extern uint8_t encryption_iv[];

#endif /* RUNTIME_ENCRYPTION_H_ */
