

/***********************************************************************************************************************
* Copyright [2015] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
*
* The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
* and/or its licensors ("Renesas") and subject to statutory and contractual protections.
*
* Unless otherwise expressly agreed in writing between Renesas and you: 1) you may not use, copy, modify, distribute,
* display, or perform the contents; 2) you may not use any name or mark of Renesas for advertising or publicity
* purposes or in connection with your use of the contents; 3) RENESAS MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE
* SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED ÅeAS ISÅf WITHOUT ANY EXPRESS OR IMPLIED
* WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND
* NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR CONSEQUENTIAL DAMAGES,
* INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF CONTRACT OR TORT, ARISING
* OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents included in this file may
* be subject to different terms.
**********************************************************************************************************************/

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
