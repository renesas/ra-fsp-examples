

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
 *  Windows TCP/IP server for the framed protocol
 */


#ifndef TCP_SERVER_H_
#define TCP_SERVER_H_

#include <stdint.h>

#ifndef HANDLE
    typedef void* HANDLE;
#endif

// Initialise the TCP server module
extern void tcpServerInitialise(void);

// Start a listening TCP server. Only one such server instance is supported.
// Input: pAddr; ASCII string of IP address to bind to or NULL if the should bind to all available interfaces
// Input: pServiceNameOrPort; ASCII string of service name (e.g. 'http') or TCP/IP port number to listen on for incoming connections
// Returning true on success, false otherwise.
extern bool tcpServerStartServer(const char *pAddr, const char *pServiceNameOrPort);

// Shutdown the TCP server
extern void tcpServerStopServer(void);

// Blocking send of a byte array
// Input: handle; TCP connection handle
// Input: pData; pointer to data to send
// Input: numBytes; the number of bytes to send
// Returns: true if data transmitted OK, false otherwise.
extern bool tcpBlockingSendByteArray(const HANDLE handle, const uint8_t *pData, const uint32_t numBytes);

#endif /* TCP_SERVER_H_ */
