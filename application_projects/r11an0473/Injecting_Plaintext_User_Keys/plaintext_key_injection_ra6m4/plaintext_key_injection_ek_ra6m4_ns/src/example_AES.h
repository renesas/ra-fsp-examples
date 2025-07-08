/***********************************************************************************************************************
 * File Name    : example_AES.h
 * Description  : Contains None-secure callable function definitions to this application
 ***********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
************************************************************************************************************************/

#include "hal_data.h"

BSP_CMSE_NONSECURE_ENTRY bool psacrypto_AES256CBC_example_NIST(void);
BSP_CMSE_NONSECURE_ENTRY bool init_lfs(void);
