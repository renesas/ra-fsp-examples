/***********************************************************************************************************************
* File Name    : i3c_master_hdr.h
* Description  : Contains macros data structures and functions used for demonstrate HDR-DDR mode.
***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef I3C_MASTER_DDR_H_
#define I3C_MASTER_DDR_H_

#include "i3c_master_ep.h"

#ifdef I3C_HDR_DDR_SUPPORT

#define I3C_HDR_COMMAND_CODE            (0x55)
#define I3C_HDR_DDR_MODE                (1U)

#define DELAY_FOR_SLAVE_PREPARE         (10U)

/* Public function declaration */
fsp_err_t master_write_read_hdr_verify(void);

#endif /* I3C_HDR_DDR_SUPPORT */
#endif /* I3C_MASTER_DDR_H_ */
