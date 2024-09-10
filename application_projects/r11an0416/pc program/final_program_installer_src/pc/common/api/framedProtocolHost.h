/***********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/*
*  Framed protocol support for the host
*/


#ifndef FRAMED_PROTOCOL_HOST_H_
#define FRAMED_PROTOCOL_HOST_H_

#include <stdint.h>
#include "FlashDescription.h"

#ifndef HANDLE
typedef void* HANDLE;
#endif

/**************************************************************
* Module initialisation
**************************************************************/

// Initialise the framed protocol module
extern void fpInitialise(void);

/**************************************************************
* Transport specific application API
**************************************************************/

// Open a framed serial connection to the chip.
// Input: portNumber; the number of the serial port to open
// Input: baudRate; the baudrate to negotiate for the framed connection.
// Input: doCommsSettingPhase; true if the 'hidden flash' comms setting phase and baudrate negotiation is required
// Returning framed protocol Handle on success, INVALID_HANDLE_VALUE if failed.
extern HANDLE fpOpenFramedConnection(const uint8_t portNumber, const uint32_t baudrate, const bool doCommsSettingPhase);

// Close a framed serial connection
// Input: handle; the framed protocol handle
extern void fpCloseCommPort(const HANDLE handle);


/**************************************************************
* Application API for all transports
**************************************************************/

// Get an array of active connection handles.
// N.B: returned array is the responsibility of the caller and must be freed when no longer required.
// Output: pNumHandles; the number of handles in the returned array
// Returning pointer to malloc'd handle array, NULL if no active connections.
extern HANDLE *fpGetConnections(uint32_t *pNumHandles);

// Get a char array description of the connection. User supplies the char array into which the null terminated
// description will be written. A empty string will be returned if the handle is not valid. 
// Input: handle; the framed protocol handle to use
// Input: pointer to char array
// Input: length of char array in which to write the description
extern void fpGetHandleDescription(const HANDLE handle, char * const pStr, const uint8_t sizeOfStr);

// Erase all the flash memory
// Input: handle; the framed protocol handle to use
// Input: pFlashDesc; pointer to the description of the available flash regions in the target chip
// Returns true on success, false otherwise
extern bool fpEraseAll(const HANDLE handle, const FlashDescription *pFlashDesc);

// Erase a region of flash
// Input: handle; the framed protocol handle to use
// Input: pFlashDesc; pointer to the description of the available flash regions in the target chip
// Input: startAddr; the start address of the area to be erased
// Input: startAddr; the end address of the area to be erased
// Returns true on success, false otherwise
extern bool fpErase(const HANDLE handle, const FlashDescription *pFlashDesc, const uint32_t startAddr, const uint32_t endAddr);

// Program a region of flash
// Input: handle; the framed protocol handle to use
// Input: pFlashDesc; pointer to the description of the available flash regions in the target chip
// Input: startAddr; the address in chip memory from which to write the data
// Input: pData; pointer to the data to write into flash
// Input: numBytes; the number of bytes of data to write
// Returns true on success, false otherwise
extern bool fpWrite(const HANDLE handle, const FlashDescription *pFlashDesc, const uint32_t startAddr, const uint8_t *pData, const uint32_t numBytes);

// Request device certificate
// Input: handle; the framed protocol handle to use
// Output: pCertificateLength; pointer to storage for certificate length.
// Returns Pointer to device certificate on success, NULL otherwise
// N.B: caller must free the returned memory!
extern uint8_t *fpGetDeviceCertificate(const HANDLE handle, uint32_t *pCertificateLength, const bool suppressLogging);

// Request device Key
// Input: handle; the framed protocol handle to use
// Output: pKeyLength; pointer to storage for key length.
// Returns Pointer to device certificate on success, NULL otherwise
// N.B: caller must free the returned memory!
extern uint8_t *fpGetDeviceKey(const HANDLE handle, uint32_t *pKeyLength, const bool suppressLogging);

// Request device OEM Cert
// Input: handle; the framed protocol handle to use
// Output: 
// Returns Pointer to device certificate on success, NULL otherwise
// N.B: caller must free the returned memory!
extern uint8_t *fpGetOEMCert(const HANDLE handle, uint32_t *oemCertLength, bool suppressDevCertErrors);

// Request device OEM Cert
// Input: handle; the framed protocol handle to use
// Output: 
// Returns Pointer to device certificate on success, NULL otherwise
// N.B: caller must free the returned memory!
extern uint8_t *fpSendChallengeResponseMessage(const HANDLE handle, const uint8_t *theHash, const uint16_t data_len, bool suppressDevCertErrors);

// Send OEM Cert to device
// Input: handle; the framed protocol handle to use
//        theCert pointer to OEMCertificate
// Output: status
// Returns Pointer to module version list on success, NULL otherwise
// N.B: caller must free the returned memory!
extern bool fpSendDeviceCert(const HANDLE handle, const uint8_t *theCert, const uint16_t data_len, bool suppressDevCertErrors);

// Request module version list
// Input: handle; the framed protocol handle to use
// Output: pVersionListLength; pointer to storage for module version list length.
// Returns Pointer to module version list on success, NULL otherwise
// N.B: caller must free the returned memory!
extern uint8_t *fpGetModuleVersionList(const HANDLE handle, uint32_t *pVersionListLength);

// Send apply image command to the target
// Input: handle; the framed protocol handle to use
// Returns true on success, false otherwise
extern bool fpApplyImage(const HANDLE handle);

// Read the current flash access window configuration
// Input: handle; the framed protocol handle to use
// Output: start; the first address in the access window
// Output: end; the last address IN the access window
// Returns true on success, false otherwise
extern bool fpGetFlashAccessWindow(const HANDLE handle, uint32_t *pStart, uint32_t *pEnd);

// Request the flash access window be configured
// Input: handle; the framed protocol handle to use
// Input: start; the first address In the access window
// Input: end; the last address IN the access window
// Returns true on success, false otherwise
extern bool fpSetFlashAccessWindow(const HANDLE handle, const uint32_t start, const uint32_t end);

// Request the flash access window and FSPR configured
// Input: handle; the framed protocol handle to use
// Input: start; the first address In the access window
// Input: end; the last address IN the access window
// Returns true on success, false otherwise
extern bool fpSetFlashAccessWindowAndFlashProtection(const HANDLE handle, const uint32_t start, const uint32_t end);

// Request the flash access window register be reset (sets FAWS, FAWE and BTFLG to all 1's)
// Input: handle; the framed protocol handle to use
// Returns true on success, false otherwise
extern bool fpResetFlashAccessWindowRegister(const HANDLE handle);

// Read the FSPR flash protection flag
// Input: handle; the framed protocol handle to use
// Output: pIsProtected, if true the FSPR is active (0), else FSPR has not yet been set (1)
// Returns true on success, false otherwise
extern bool fpGetFlashProtection(const HANDLE handle, bool *pIsProtected);

// Set the FSPR flash protection flag (FSPR=>0)
// **** WARNING: THIS WILL LOCK THE FLASH PROTECTION REGISTERS AND CANNOT BE UNDONE ****
// Input: handle; the framed protocol handle to use
// Returns true on success, false otherwise
extern bool fpSetFlashProtection(const HANDLE handle);
// ^^^^^^ YOU HAVE BEEN WARNED ^^^^^^

// Type for the 128bit OSIS register
typedef struct FpOsis_tag {
	// [0] is the lowest 8 bits, [15] is highest 8 bits
	uint8_t reg[16];
} FpOsis;

// Read the current OSIS register value
// Input: handle; the framed protocol handle to use
// Output: pOsis; the OSIS register value structure into which the register value will be written
// Returns true on success, false otherwise
extern bool fpGetOsis(const HANDLE handle, FpOsis *pOsis);

// Set the OSIS register
// Input: handle; the framed protocol handle to use
// Input: pOsis; the value to write to the OSIS register
// Returns true on success, false otherwise
extern bool fpSetOsis(const HANDLE handle, const FpOsis *pOsis);

// Poll with timeout for a pull request
// Input: handle; the framed protocol handle to use
// Input: timeoutMs; the timeout in mS, zero for no timeout
// Returns true on pull received, false otherwise
extern bool fpPollForPullRequest(const HANDLE handle, const uint16_t timeoutMs);

/**************************************************************
* Transport layer interface API
**************************************************************/

// Implemented by transport layer:

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

// A TCP/IP client has connected
// Input: hComm; the TCP connection handle
// Input: pRemoteAddr; the remote client IP address
// Returns: the framed protocol handle to use or INVALID_HANDLE_VALUE on failure to accept the new connection
extern HANDLE fpTcpClientConnected(const HANDLE hComm, const char *pRemoteAddr);

// A TCP/IP client has disconnected
// Input: handle; the framed protocol handle to the connection which has been terminated
extern void fpTcpClientDisconnected(const HANDLE handle);

#endif /* FRAMED_PROTOCOL_HOST_H_ */
