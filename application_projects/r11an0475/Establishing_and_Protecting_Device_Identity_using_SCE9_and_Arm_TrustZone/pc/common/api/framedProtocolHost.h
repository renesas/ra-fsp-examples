

/***********************************************************************************************************************
* Copyright (c) 2015 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/ 

/*
*  Framed protocol support for the host
*/


#ifndef FRAMED_PROTOCOL_HOST_H_
#define FRAMED_PROTOCOL_HOST_H_

#include <stdint.h>


//Maximum number of connections supported
#define MAX_FP_CONNECTIONS		(10)
//Minimum frame size
#define MIN_FRAME_SIZE			(6)

#define DEV_KEY_AVAILABLE		(1)




//Function prototypes
void fpInitialize(void);
HANDLE OpenSerialPort(const uint8_t portNumber, const uint32_t baudrate);

void fpCloseCommPort(const HANDLE handle);
/**************************************************************
* Transport layer interface API
**************************************************************/

// Function signature for blocking transmit data function
// Input: comms layer handle
// Input: pointer to data to transmit
// Input: number of bytes to transmit
// Returns: true if data transmitted OK, false otherwise.
typedef bool(*FpBlockingDataTransmit)(const HANDLE commHandle, const uint8_t *pBuffer, const uint32_t numBytes);

// Implemented by framed protocol layer:

// Handle a received byte of data. This function is thread safe.
// Input: handle; the framed protocol handle to the connection on which the byte has been received
// Input: byte; received data byte
extern void fpReceiveByte(const HANDLE handle, const uint8_t byte);

extern uint8_t* fpSendChallengeResponseMessage(const HANDLE handle, const uint8_t *theHash, const uint16_t data_len);
uint8_t *fpGetDeviceKey(const HANDLE handle, uint32_t *pKeyLength);


bool fpSendDeviceCert(const HANDLE handle, const uint8_t *theCert, const uint16_t data_len);
#endif /* FRAMED_PROTOCOL_HOST_H_ */
