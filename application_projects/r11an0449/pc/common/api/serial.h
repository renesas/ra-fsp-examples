/**********************************************************************************************************************
* Copyright (c) 2015 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/

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
