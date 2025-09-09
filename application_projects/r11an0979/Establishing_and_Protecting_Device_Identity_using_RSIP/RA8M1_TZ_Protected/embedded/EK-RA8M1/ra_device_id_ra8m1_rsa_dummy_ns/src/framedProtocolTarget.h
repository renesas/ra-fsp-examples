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

#define READ_BUFFER_SIZE            1240

/* Handle a received byte of serial data */
BSP_CMSE_NONSECURE_ENTRY void fpReceiveByte(const uint8_t byte);
/* Provide response to Non-secure region */
BSP_CMSE_NONSECURE_ENTRY void share_with_ns(uint8_t *pBuffer, uint16_t *numBytes);

#endif /* FRAMEDPROTOCOL_H_ */
