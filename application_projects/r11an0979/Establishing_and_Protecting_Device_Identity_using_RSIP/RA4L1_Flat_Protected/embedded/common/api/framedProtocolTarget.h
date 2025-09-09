/***********************************************************************************************************************
 * File Name    : framedProtocolTarget.h
 * Description  : framedProtocol MCU related definitions
 * flash routine related definitions.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef FRAMEDPROTOCOL_H_
#define FRAMEDPROTOCOL_H_
#include "hal_data.h"

#define READ_BUFFER_SIZE            1030

/* Handle a received byte of serial data */
extern void fpReceiveByte(const uint8_t byte);

#endif /* FRAMEDPROTOCOL_H_ */
