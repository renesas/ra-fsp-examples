/***********************************************************************************************************************
 * File Name    : crypto_operations.h
 * Description  : Contains cryptographic operation definitions  of the non-secure callables
 ***********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#ifndef CRYPTO_OPERATION_H_
#define CRYPTO_OPERATION_H_

#include <stdint.h>
#include "hal_data.h"


BSP_CMSE_NONSECURE_ENTRY bool cipher_operation_with_directly_injected_aes_key(void);
BSP_CMSE_NONSECURE_ENTRY bool update_aes_key(void);
BSP_CMSE_NONSECURE_ENTRY bool cipher_operation_with_updated_aes_key(void);
BSP_CMSE_NONSECURE_ENTRY bool cryptograhpic_operation_with_directly_injected_ecc_pub_key(void);
BSP_CMSE_NONSECURE_ENTRY bool update_ecc_pub_key(void);
BSP_CMSE_NONSECURE_ENTRY bool cryptographic_operation_with_updated_ecc_pub_key(void);

#endif /* CRYPTO_OPERATION_H_ */
