

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
 *  Windows serial port driver.
 */


#ifndef SERIAL_H_
#define SERIAL_H_

#include <stdint.h>

#ifndef HANDLE
    typedef void* HANDLE;
#endif

// Open the given serial port, returning Handle on success,
// Input: fpHandle; the framed protocol handle to use when submitting rx data
// Input: portNumber; the serial port number to open
// Input: baudrate; the baudrate to configure (will always use 8 bits, no parity, 1 stop bit)
// Output: byte of received data
// Returns: the serial context handle if open succeeeded, INVALID_HANDLE_VALUE otherwise.
extern HANDLE serialOpenCommPort(const HANDLE fpHandle, const uint8_t portNumber, const uint32_t baudrate);

// Update the baudrate
// Input: handle; serial context handle
// Input: baudrate; the baudrate to configure (will always use 8 bits, no parity, 1 stop bit)
// Returns: true if byte read, false otherwise
extern bool serialUpdateBaudrate(const HANDLE handle, const uint32_t baudrate);

// Close the serial port
// Input: handle; serial context handle
extern void serialCloseCommPort(const HANDLE handle);

// Blocking send of a byte array
// Input: handle; serial context handle
// Input: pData; pointer to data to send
// Input: numBytes; the number of bytes to send
// Returns: true if data transmitted OK, false otherwise.
extern bool serialBlockingSendByteArray(const HANDLE handle, const uint8_t *pData, const uint32_t numBytes);

#endif /* SERIAL_H_ */
