/***********************************************************************************************************************
 * File Name    : i3c_master_hdr.h
 * Description  : Contains macros and functions used in i3c_master_hdr.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2024 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef I3C_MASTER_HDR_H_
#define I3C_MASTER_HDR_H_

#include "i3c_master_ep.h"

#ifdef I3C_HDR_DDR_SUPPORT

#define I3C_HDR_COMMAND_CODE            (0x55)
#define I3C_HDR_DDR_MODE                (1U)

/* Public function declaration */
fsp_err_t master_write_read_hdr_verify(void);

#endif /* I3C_HDR_DDR_SUPPORT */

#endif /* I3C_MASTER_HDR_H_ */
