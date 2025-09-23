/***********************************************************************************************************************
 * File Name    : crypto_operations.h
 * Description  : Contains cryptographic operation definitions  of the non-secure callables
 ***********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
************************************************************************************************************************/
#ifndef CRYPTO_OPERATION_H_
#define CRYPTO_OPERATION_H_

#include <stdint.h>
#include "hal_data.h"

bool cryptograhpic_operation_with_ecc_key_pair(void);
bool update_ecc_key_pair(void);

#endif /* CRYPTO_OPERATION_H_ */
