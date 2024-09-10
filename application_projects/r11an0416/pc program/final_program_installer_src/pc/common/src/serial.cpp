/***********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/*
*  Windows serial port driver.
*/


#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <tchar.h>

#include "serial.h"
#include "framedProtocolHost.h"

// Debug output control - define to enable debugging to STDOUT
//#define SERIAL_DEBUG
// And this one for per character debug (voluminous!)
//#define EXTRA_SERIAL_DEBUG

/*
* Private data
*/

typedef struct SerialContext_tag {
    // If this port structure is in use
    int inUse;

    // Port library handle
    HANDLE hComm;

    // Port name string
    TCHAR portName[20];

    // Receive thread handle & ID
    HANDLE hThread;
    DWORD  threadId;

    // Receive thread shutdown event handle
    HANDLE hShutdown;

    // Transmit overlapped structure
    OVERLAPPED osWrite;

    // Framed protocol connetion handle
    HANDLE fpHandle;

} SerialContext;

// The maximum number of open ports this serial module supports
// N.B: each open port with consume one thread
#define MAX_OPEN_PORTS 10

// The post context data
static SerialContext serialContext[MAX_OPEN_PORTS] = { 0 };

/*
* Private functions
*/

#ifdef EXTRA_SERIAL_DEBUG
static void dumpPacket(const char *msg, const uint8_t *pData, const uint32_t numBytes)
{
    uint32_t i;
    // Don't dump all of a large packet
    uint32_t dumpBytes = (numBytes < 50) ? numBytes : 50;
    fprintf(stdout, "Serial %s (%d bytes): ", msg, numBytes);
    for (i = 0; i < dumpBytes; i++)
    {
        fprintf(stdout, "0x%02X ", pData[i]);
    }
    fprintf(stdout, "%s\n", (numBytes > dumpBytes ? "..." : ""));
}
#endif

// Log a Windows error message
static void logError(SerialContext *pContext, DWORD errorCode)
{
    // Only log real errors
    if (errorCode != 0)
    {
        LPVOID lpMsgBuf;

        FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM |
            FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL,
            errorCode,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR)&lpMsgBuf,
            0, NULL);

        fprintf(stderr, "Error[%s]: code %d - %s", pContext->portName, errorCode, (LPCTSTR)lpMsgBuf);

        LocalFree(lpMsgBuf);
    }
}

// Allocate a serial context, returning NULL if none available
static SerialContext *allocateContext(void)
{
    SerialContext *pContext = NULL;
    int i = 0;
    while ((i < MAX_OPEN_PORTS) && (NULL == pContext))
    {
        if (!serialContext[i].inUse)
        {
            pContext = &serialContext[i];
            memset(pContext, 0, sizeof(pContext));
            pContext->inUse = TRUE;
            memset(&pContext->osWrite, 0, sizeof(OVERLAPPED));
            // Create this write operation's OVERLAPPED structure's hEvent.
            pContext->osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
            if (!pContext->osWrite.hEvent)
            {
                fprintf(stderr, "Failed to create write overlapped event handle\n");
            }
            pContext->fpHandle = INVALID_HANDLE_VALUE;
        }
        else
        {
            i++;
        }
    }
    return pContext;
}

// Free a serial context
static void freeContext(SerialContext *pContext)
{
    pContext->inUse = FALSE;
    CloseHandle(pContext->osWrite.hEvent);
}

// Get a serial context from a handle - returns NULL if it doesn't look like an active context
static SerialContext *getContext(HANDLE handle)
{
    SerialContext *pContext = (SerialContext *)handle;
    if (// Address range check
        ((pContext >= &serialContext[0]) && (pContext <= &serialContext[MAX_OPEN_PORTS - 1])) &&
        // Context check
        pContext->inUse && (NULL != pContext->hComm))
    {
        return pContext;
    }
    else
    {
        fprintf(stderr, "Invalid serial context %p\n", handle);
        return NULL;
    }
}

// Receive thread function
static DWORD WINAPI receiveThread(LPVOID lpParam)
{
    SerialContext *pContext = (SerialContext *)lpParam;

    OVERLAPPED osReader = { 0 };

#ifdef SERIAL_DEBUG
    fprintf(stdout, "RX thread started for port %ls\n", pContext->portName);
#endif

    // Create the overlapped event.
    osReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (NULL == osReader.hEvent)
    {
        fprintf(stderr, "Failed to create read overlapped event handle\n");
    }
    else
    {
        // Multiple handles array for waiting on (thread shutdown + port) events
        HANDLE waitHandles[2] = { pContext->hShutdown, osReader.hEvent };

        uint8_t rxByte;
        DWORD dwRead;
        BOOL threadRunning = TRUE;

        while (threadRunning)
        {
            // Issue read operation.
            if (!ReadFile(pContext->hComm, &rxByte, 1, &dwRead, &osReader))
            {
                // The expectation here is that ReadFile is indicating a delayed read operation has been started
                DWORD errorCode = GetLastError();
                if (ERROR_IO_PENDING == errorCode)
                {
                    // Wait for either thread or serial port event
                    DWORD dwRes = WaitForMultipleObjects(_countof(waitHandles), waitHandles, FALSE, INFINITE);
                    switch (dwRes)
                    {
                        // Thread signalled
                    case WAIT_OBJECT_0:
#ifdef SERIAL_DEBUG
                        fprintf(stdout, "%ls receive thread shutdown signalled\n", pContext->portName);
#endif
                        threadRunning = FALSE;
                        break;

                        // Read completed.
                    case (WAIT_OBJECT_0 + 1):
#ifdef EXTRA_SERIAL_DEBUG
                        fprintf(stdout, "%s receive thread read data signalled\n", pContext->portName);
#endif
                        if (!GetOverlappedResult(pContext->hComm, &osReader, &dwRead, FALSE))
                        {
                            // Error in communications
                            fprintf(stderr, "Failed to read serial port %s (delayed)\n", pContext->portName);
                            logError(pContext, GetLastError());
                            threadRunning = FALSE;
                        }
                        else
                        {
                            // Read completed successfully.
                            if (dwRead)
                            {
                                fpReceiveByte(pContext->fpHandle, rxByte);
                            }
                        }
                        break;

                    default:
                        // An error has occurred
                        fprintf(stderr, "Error waiting for event on port %s\n", pContext->portName);
                        threadRunning = FALSE;
                        break;
                    }
                }
                else
                {
                    // Something else went wrong with the ReadFile call.
                    logError(pContext, errorCode);
                    threadRunning = FALSE;
                }
            }
            else
            {
                // read completed immediately
                fpReceiveByte(pContext->fpHandle, rxByte);
            }
        }

        CloseHandle(osReader.hEvent);
        CloseHandle(pContext->hShutdown);
    }

    return 0;
}

/*
* Public API
*/

// Open the given serial port, returning Handle on success,
// Input: fpHandle; the framed protocol handle to use when submitting rx data
// Input: portNumber; the serial port number to open
// Input: baudrate; the baudrate to configure (will always use 8 bits, no parity, 1 stop bit)
// Output: byte of received data
// Returns: the serial context handle if open succeeeded, INVALID_HANDLE_VALUE otherwise.
HANDLE serialOpenCommPort(const HANDLE fpHandle, const uint8_t portNumber, const uint32_t baudrate)
{
    HANDLE retVal = INVALID_HANDLE_VALUE;
    SerialContext *pContext = allocateContext();
    if (pContext)
    {
        _stprintf_s(pContext->portName, _countof(pContext->portName), _TEXT("\\\\.\\COM%d"), portNumber);

        pContext->fpHandle = fpHandle;

        pContext->hComm = CreateFile(pContext->portName,
            GENERIC_READ | GENERIC_WRITE,
            0,
            0,
            OPEN_EXISTING,
            FILE_FLAG_OVERLAPPED,
            0);

        if (pContext->hComm == INVALID_HANDLE_VALUE)
        {
            fprintf(stderr, "Fatal error opening %s\n", pContext->portName);
            logError(pContext, GetLastError());
            freeContext(pContext);
            pContext = NULL;
        }
        else
        {
            DCB dcb = { 0 };
            COMMTIMEOUTS timeouts = { 0 };
            TCHAR dcbString[100];

            // Build a DCB control string
            _stprintf_s(dcbString, _countof(dcbString), _TEXT("baud=%d parity=N data=8 stop=1"), baudrate);

            // And from that build the DCB
            if (!BuildCommDCB(dcbString, &dcb))
            {
                // Failed to build DCB
                fprintf(stderr, "Failed to build DCB for comm port %s\n", pContext->portName);
                serialCloseCommPort(pContext->hComm);
                pContext = NULL;
            }
            else
            {
                // Update timeouts - no timeout for RX or TX since this code is completely event driven
                timeouts.ReadIntervalTimeout = 0;
                timeouts.ReadTotalTimeoutMultiplier = 0;
                timeouts.ReadTotalTimeoutConstant = 0;
                timeouts.WriteTotalTimeoutMultiplier = 0;
                timeouts.WriteTotalTimeoutConstant = 0;

                if (!SetCommState(pContext->hComm, &dcb) || !SetCommTimeouts(pContext->hComm, &timeouts))
                {
                    // Error in SetCommState. Possibly a problem with the communications
                    // port handle or a problem with the DCB structure itself.
                    fprintf(stderr, "Failed to update DCB/Timeouts for comm port %s\n", pContext->portName);
                    serialCloseCommPort(pContext->hComm);
                    pContext = NULL;
                }
                else
                {
                    // Comm port configured, now the receive thread can be started

                    // Purge comm port
                    PurgeComm(pContext->hComm, (PURGE_RXCLEAR | PURGE_TXCLEAR));

                    // Create an event that can be used to cleanly shutdown this thread
                    pContext->hShutdown = CreateEvent(NULL, TRUE, FALSE, NULL);

                    // And create the thread
                    pContext->hThread = CreateThread(
                        NULL,                   // default security attributes
                        0,                      // use default stack size
                        receiveThread,          // receive thread function
                        pContext,               // argument to thread function
                        0,                      // use default creation flags
                        &pContext->threadId);   // returns the thread identifier

                                                // If CreateThread fails, its probably game over for the application
                    if ((NULL == pContext->hShutdown) || (NULL == pContext->hThread))
                    {
                        fprintf(stderr, "Fatal error creating receive thread for %s\n", pContext->portName);
                        serialCloseCommPort(pContext->hComm);
                        pContext = NULL;
                    }
                }
            }
        }

        if (pContext != NULL)
        {
            retVal = (HANDLE *)pContext;
        }
    }
    return retVal;
}

// Close the serial port
void serialCloseCommPort(const HANDLE handle)
{
    SerialContext *pContext = getContext(handle);
    if (pContext)
    {
        // Terminate the RX thread
        if (!SetEvent(pContext->hShutdown))
        {
            fprintf(stderr, "Failed to set shutdown event for comm port %s\n", pContext->portName);
        }
        if (!CloseHandle(pContext->hThread))
        {
            fprintf(stderr, "Failed to close thread handle for comm port %s\n", pContext->portName);
        }

        // And close the port
        if (!CloseHandle(pContext->hComm))
        {
            fprintf(stderr, "Failed to close comm port %s\n", pContext->portName);
        }

        freeContext(pContext);
    }
}

// Update the baudrate
// Input: handle; serial context handle
// Input: baudrate; the baudrate to configure (will always use 8 bits, no parity, 1 stop bit)
// Returns: true if byte read, false otherwise
bool serialUpdateBaudrate(const HANDLE handle, const uint32_t baudrate)
{
    bool success = false;

    SerialContext *pContext = getContext(handle);
    if (pContext)
    {
        DCB dcb = { 0 };

        // Read current serial port DCB
        if (GetCommState(pContext->hComm, &dcb))
        {
            // Update baudrate and set
            dcb.BaudRate = baudrate;
            if (SetCommState(pContext->hComm, &dcb))
            {
                success = true;
            }
            else
            {
                fprintf(stderr, "Failed to update DCB for comm port %s\n", pContext->portName);
            }
        }
        else
        {
            fprintf(stderr, "Failed to read DCB for comm port %s\n", pContext->portName);
        }
    }

    return success;
}

// Blocking send of a byte array
bool serialBlockingSendByteArray(const HANDLE handle, const uint8_t *pData, const uint32_t numBytes)
{
    bool success = false;

    SerialContext *pContext = getContext(handle);
    if (pContext)
    {
        DWORD dwWritten;

        // Issue write.
        if (!WriteFile(pContext->hComm, pData, numBytes, &dwWritten, &pContext->osWrite))
        {
            if (ERROR_IO_PENDING == GetLastError())
            {
                // Write is pending, wait indefinitely for it to complete.
                if (!GetOverlappedResult(pContext->hComm, &pContext->osWrite, &dwWritten, TRUE))
                {
                    fprintf(stderr, "Failed to write to serial port (delayed)\n");
                }
#ifdef EXTRA_SERIAL_DEBUG
                else
                {
                    dumpPacket("wrote", pData, numBytes);
                }
#endif
            }
            else
            {
                // WriteFile failed, but isn't delayed.
                fprintf(stderr, "Failed immediate write to serial port\n");
            }
        }
#ifdef EXTRA_SERIAL_DEBUG
        else
        {
            fprintf(stdout, "%ls wrote %d bytes immediately\n", pContext->portName, numBytes);
        }
#endif
        success = true;
    }

    return success;
}

