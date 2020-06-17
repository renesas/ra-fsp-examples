
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

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
