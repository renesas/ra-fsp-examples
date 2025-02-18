/***********************************************************************************************************************
 * File Name    : framedProtocolTarget.h
 * Description  : framedProtocol MCU related definitions
 * flash routine related definitions.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
 ***********************************************************************************************************************/

#ifndef FRAMEDPROTOCOL_H_
#define FRAMEDPROTOCOL_H_

#include "framedProtocolCommon.h"

/*
 * Interface to serial I/O
 */

/* Handle a received byte of serial data */
extern void fpReceiveByte(const uint8_t byte);

/* Function signature for serial transmit callback */

typedef bool (*FpDataTransmitCallback)(const uint8_t *pBuffer, const uint16_t numBytes);

/*
 * Initialization
 */

/* Framed protocol initialization function */
extern bool fpInitialise(const FpDataTransmitCallback pDataTransmitCallback);

#endif /* FRAMEDPROTOCOL_H_ */
