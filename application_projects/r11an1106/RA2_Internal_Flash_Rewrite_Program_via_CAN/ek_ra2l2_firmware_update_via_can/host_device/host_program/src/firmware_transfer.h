/***********************************************************************************************************************
 * File Name    : firmware_transfer.h
 * Description  : Contains firmware transfer macro definitions and function definitions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
************************************************************************************************************************/


#ifndef _XMODEM_H
#define _XMODEM_H

#include "hal_data.h"
#include "can.h"

#define MAX_RETRIES             (10)

#define HEADER_INDEX            (0)
#define CHECKSUM_INDEX          (131)
#define DATA_INDEX              (3)

fsp_err_t firmware_transfer();

#endif
