

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


// Stop Windows.h including the earlier Windows Sockest 1.1 headers
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <tchar.h>

#include "tcpServer.h"
#include "framedProtocolHost.h"
#include "framedProtocolCommon.h"

// Indicate to linker that the Windows socket library is required
#pragma comment(lib, "Ws2_32.lib")

// Debug output control - define to enable debugging to STDOUT
// #define TCP_SERVER_DEBUG

/**************************************************************
 * Private data
 **************************************************************/

// Context for listening server
typedef struct TcpServerContext_tag {
    // Listening server socket
    SOCKET listenSocket;

    // Thread handle & ID
    HANDLE hThread;
    DWORD  threadId;

} TcpServerContext;

static TcpServerContext tcpServerContext;

// Context for client connections
typedef struct TcpConnectionContext_tag {
    // If this connection context is in use
    int inUse;

    // Connection socket
    SOCKET socket;

    // Remote host name string
    char hostName[80];

    // Framed protocol connection handle
    HANDLE fpHandle;

    // Socket event
    WSAEVENT event;

} TcpConnectionContext;

// The maximum number of client connections this server supports
#define MAX_CLIENT_CONNECTIONS 10

// The post context data
static TcpConnectionContext connectionContext[MAX_CLIENT_CONNECTIONS] = { 0 };

bool tcpServerInitialised = false;

// For the server thread, need an array of active client socket contexts
static TcpConnectionContext *pActiveContexts[MAX_CLIENT_CONNECTIONS] = { NULL };


/**************************************************************
 * Private functions
 **************************************************************/

 // Log a Windows error message
static void logError(TcpConnectionContext *pContext, DWORD errorCode)
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

        fprintf(stderr, "Error[%s]: code %d - %s", pContext->hostName, errorCode, (LPCTSTR)lpMsgBuf);

        LocalFree(lpMsgBuf);
    }
}

// Allocate a connection context, returning NULL if none available
static TcpConnectionContext *allocateContext(SOCKET socket, struct sockaddr_in *pAddr)
{
    TcpConnectionContext *pContext = NULL;
    int i = 0;
    while ((i < MAX_CLIENT_CONNECTIONS) && (NULL == pContext))
    {
        if (!connectionContext[i].inUse)
        {
            pContext = &connectionContext[i];
            memset(pContext, 0, sizeof(pContext));
            pContext->inUse = TRUE;
            pContext->socket = socket;
            pContext->fpHandle = INVALID_HANDLE_VALUE;
            
            // Build hostname string from client address (avoiding widechars and extra Windows DLLs)
            snprintf(pContext->hostName, sizeof(pContext->hostName), "%d.%d.%d.%d:%d", 
                     pAddr->sin_addr.s_net, pAddr->sin_addr.s_host, pAddr->sin_addr.s_lh, pAddr->sin_addr.s_impno, 
                     pAddr->sin_port);

            // Setup the socket event
            pContext->event = WSACreateEvent();
            WSAEventSelect(pContext->socket, pContext->event, (FD_READ | FD_CLOSE));

            // Signal the new connection to the new framed protocol layer and store fpHandle
            pContext->fpHandle = fpTcpClientConnected((HANDLE)pContext, pContext->hostName);
        }
        else
        {
            i++;
        }
    }
    return pContext;
}

// Free a connection context
static void freeContext(TcpConnectionContext *pContext)
{
    // Disconnect from the framed protocol
    fpTcpClientDisconnected(pContext->fpHandle);

    if (INVALID_SOCKET != pContext->socket)
    {
        int iResult = shutdown(pContext->socket, SD_SEND);
        if (iResult == SOCKET_ERROR) 
        {
            fprintf(stderr, "TcpServer: client socket shutdown failed with error: %d\n", WSAGetLastError());
        }
        WSACloseEvent(pContext->event);
        closesocket(pContext->socket);
        pContext->socket = INVALID_SOCKET;
    }

    pContext->inUse = FALSE;
}

// Get a connection context from a handle - returns NULL if it doesn't look like an active context
static TcpConnectionContext *getContext(HANDLE handle)
{
    TcpConnectionContext *pContext = (TcpConnectionContext *)handle;
    if (// Address range check
        ((pContext >= &connectionContext[0]) && (pContext <= &connectionContext[MAX_CLIENT_CONNECTIONS - 1])) &&
        // Context check
        pContext->inUse)
    {
        return pContext;
    }
    else
    {
        fprintf(stderr, "Invalid connection context %p\n", handle);
        return NULL;
    }
}

// Server thread function
static DWORD WINAPI serverThread(LPVOID lpParam)
{
    bool threadRunning = true;

#ifdef TCP_SERVER_DEBUG
    fprintf(stdout, "TCP server thread started\n");
#endif

    // Create an event for the listening socket
    WSAEVENT listenEvent = WSACreateEvent();

    // Associate event types FD_ACCEPT and FD_CLOSE
    // with the listening socket and its event
    WSAEventSelect(tcpServerContext.listenSocket, listenEvent, FD_ACCEPT | FD_CLOSE);

    // Start the socket listening
    if (SOCKET_ERROR == listen(tcpServerContext.listenSocket, MAX_CLIENT_CONNECTIONS))
    {
        fprintf(stderr, "TcpServer: Error at listen(): %ld\n", WSAGetLastError());
        threadRunning = false;
    }
    else
    {
#ifdef TCP_SERVER_DEBUG
        fprintf(stdout, "TCP server thread listening\n");
#endif

        // Server thread loop
        while (threadRunning)
        {
            // Build array of active sockets to wait on. The first one is always the listening socket
            WSAEVENT eventArray[WSA_MAXIMUM_WAIT_EVENTS];
            int numEvents = 0;
            eventArray[numEvents++] = listenEvent;
            int numActiveContexts = 0;
            for (int i = 0; i < MAX_CLIENT_CONNECTIONS; i++)
            {
                if (connectionContext[i].inUse)
                {
                    pActiveContexts[numActiveContexts++] = &connectionContext[i];
                    eventArray[numEvents++] = connectionContext[i].event;
                }
            }

            // Wait for network events on all sockets
            DWORD index = WSAWaitForMultipleEvents(numEvents, eventArray, FALSE, WSA_INFINITE, FALSE);
            index = index - WSA_WAIT_EVENT_0;

            // Iterate through all events to see if more than one is signaled
            for (int eventArrayIndex = index; eventArrayIndex < numEvents; eventArrayIndex++)
            {
                index = WSAWaitForMultipleEvents(1, &eventArray[eventArrayIndex], TRUE, 0, FALSE);
                if ((index == WSA_WAIT_FAILED) || (index == WSA_WAIT_TIMEOUT))
                { 
                    // Not this socket
                    continue;
                }
                else
                {
                    WSANETWORKEVENTS networkEvents;
                    if (0 == eventArrayIndex)
                    {
                        // Server socket signalled
                        WSAEnumNetworkEvents(tcpServerContext.listenSocket, eventArray[0], &networkEvents);

                        // Check for FD_ACCEPT messages
                        if (networkEvents.lNetworkEvents & FD_ACCEPT)
                        {
                            if (networkEvents.iErrorCode[FD_ACCEPT_BIT] != 0)
                            {
                                fprintf(stderr, "TcpServer: FD_ACCEPT failed with error %d\n", networkEvents.iErrorCode[FD_ACCEPT_BIT]);
                            }
                            else
                            {
                                // Accept a new connection
                                struct sockaddr client;
                                int cLen = sizeof(struct sockaddr);
                                SOCKET acceptSocket = accept(tcpServerContext.listenSocket, &client, &cLen);
                                // Allocate a new connection context
                                TcpConnectionContext *pContext = allocateContext(acceptSocket, (sockaddr_in *)&client);
                                if (!pContext)
                                {
                                    fprintf(stderr, "TcpServer: rejecting connection; too many connections\n");
                                    closesocket(acceptSocket);
                                }
#ifdef TCP_SERVER_DEBUG
                                else
                                {
                                    fprintf(stdout, "TcpServer: accepted a new socket\n");
                                }
#endif
                            }
                        }

                        // Server socket has been closed
                        if (networkEvents.lNetworkEvents & FD_CLOSE)
                        {
#ifdef TCP_SERVER_DEBUG
                            fprintf(stdout, "TcpServer: listening socket closed\n");
#endif
                            if (networkEvents.iErrorCode[FD_CLOSE_BIT] != 0)
                            {
                                fprintf(stderr, "TcpServer: FD_CLOSE failed with error %d\n", networkEvents.iErrorCode[FD_CLOSE_BIT]);
                            }
                            threadRunning = false;
                        }
                    }
                    else
                    {
                        // One of the client sockets
                        WSAEnumNetworkEvents(pActiveContexts[eventArrayIndex - 1]->socket, eventArray[eventArrayIndex], &networkEvents);

                        if (networkEvents.lNetworkEvents & FD_READ)
                        {
#ifdef TCP_SERVER_DEBUG
                            fprintf(stdout, "TcpServer: client socket read event\n");
#endif
                            // Blocking read of socket now we know data is available
                            char buffer[MAX_LEN_DATA_PACKET];
                            int numBytes = recv(pActiveContexts[eventArrayIndex - 1]->socket, buffer, sizeof(buffer), 0);
                            if (numBytes > 0)
                            {
                                // Pass to framed protocol - this isn't as bad as it looks because packet parsing has to been
                                // done on a byte by byte basis at some point, so best to do it at transport stream interface
                                for (int i = 0; i < numBytes; i++)
                                {
                                    fpReceiveByte(pActiveContexts[eventArrayIndex - 1]->fpHandle, buffer[i]);
                                }
                            }
                            else if (0 == numBytes)
                            {
                                // Socket has closed
                                freeContext(pActiveContexts[eventArrayIndex - 1]);
                            }
                            else if (SOCKET_ERROR == numBytes)
                            {
                                logError(pActiveContexts[eventArrayIndex - 1], WSAGetLastError());
                            }
                        }

                        if (networkEvents.lNetworkEvents & FD_CLOSE)
                        {
#ifdef TCP_SERVER_DEBUG
                            fprintf(stdout, "TcpServer: client socket closed\n");
#endif
                            if (networkEvents.iErrorCode[FD_CLOSE_BIT] != 0)
                            {
                                fprintf(stderr, "TcpServer: client FD_CLOSE failed with error %d\n", networkEvents.iErrorCode[FD_CLOSE_BIT]);
                            }
                            freeContext(pActiveContexts[eventArrayIndex - 1]);
                        }
                    }
                }
            }
        }
    }

    WSACloseEvent(listenEvent);

#ifdef TCP_SERVER_DEBUG
    fprintf(stdout, "TCP server thread exiting\n");
#endif

    return 0;
}

/**************************************************************
 * Public API
 **************************************************************/

// Initialise the TCP server module
void tcpServerInitialise(void)
{
    // Initialise winsock
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) 
    {
        fprintf(stderr, "TcpServer: WSAStartup failed: %d\n", iResult);
    }
    else
    {
        memset(&tcpServerContext, 0, sizeof(tcpServerContext));
        tcpServerContext.listenSocket = INVALID_SOCKET;
        tcpServerInitialised = true;
    }
}

// Start a listening TCP server. Only one such server instance is supported.
// Input: pAddr; ASCII string of IP address to bind to or NULL if the should bind to all available interfaces
// Input: pServiceNameOrPort; ASCII string of service name (e.g. 'http') or TCP/IP port number to listen on for incoming connections
// Returning true on success, false otherwise.
bool tcpServerStartServer(const char *pAddr, const char *pServiceNameOrPort)
{
    bool success = false;

    if (tcpServerInitialised)
    {
        // Create server listening socket and bind address+port
        struct addrinfo *result = NULL, *ptr = NULL, hints;

        ZeroMemory(&hints, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        hints.ai_flags = AI_PASSIVE;

        // Resolve the local address and port to be used by the server
        int iResult = getaddrinfo(pAddr, pServiceNameOrPort, &hints, &result);
        if (iResult != 0) 
        {
            fprintf(stderr, "TcpServer: getaddrinfo failed: %d\n", iResult);
        }
        else
        {
            // Create the listening socket
            tcpServerContext.listenSocket = socket(
                result->ai_family,
                result->ai_socktype,
                result->ai_protocol);

            if (INVALID_SOCKET == tcpServerContext.listenSocket)
            {
                fprintf(stderr, "TcpServer: Error at socket(): %ld\n", WSAGetLastError());
                freeaddrinfo(result);
            }
            else
            {
                // Bind the TCP listening socket
                iResult = bind(tcpServerContext.listenSocket, result->ai_addr, (int)result->ai_addrlen);
                if (iResult == SOCKET_ERROR) 
                {
                    fprintf(stderr, "TcpServer: bind failed with error: %d\n", WSAGetLastError());
                    closesocket(tcpServerContext.listenSocket);
                    tcpServerContext.listenSocket = INVALID_SOCKET;
                }
                else
                {
                    success = true;
                }
            }

            freeaddrinfo(result);
        }

        // If successful, launch server socket thread
        if (success)
        {
            // And create the thread
            tcpServerContext.hThread = CreateThread(
                NULL,                          // default security attributes
                0,                             // use default stack size
                serverThread,                  // thread function
                NULL,                          // argument to thread function
                0,                             // use default creation flags
                &tcpServerContext.threadId);   // returns the thread identifier

            if (NULL == tcpServerContext.hThread)
            {
                fprintf(stderr, "TcpServer: error creating server thread\n");
                success = false;
            }
        }
    }

    return success;
}

// Shutdown the TCP server
void tcpServerStopServer(void)
{
    if (tcpServerInitialised)
    {
        // Close client sockets
        for (int i = 0; i < MAX_CLIENT_CONNECTIONS; i++)
        {
            if (connectionContext[i].inUse)
            {
                freeContext(&connectionContext[i]);
            }
        }

        // CLose server socket - the server thread will detect this and shutdown
        if (INVALID_SOCKET != tcpServerContext.listenSocket)
        {
            closesocket(tcpServerContext.listenSocket);
        }
    }
}

// Blocking send of a byte array
// Input: handle; TCP connection handle
// Input: pData; pointer to data to send
// Input: numBytes; the number of bytes to send
// Returns: true if data transmitted OK, false otherwise.
bool tcpBlockingSendByteArray(const HANDLE handle, const uint8_t *pData, const uint32_t numBytes)
{
    bool success = false;

    TcpConnectionContext *pContext = getContext(handle);
    if (pContext)
    {
        // Blocking send on the socket
        int sentBytes = send(pContext->socket, (char *)pData, numBytes, 0);
        if (sentBytes == numBytes)
        {
            success = true;
        }
        else if (SOCKET_ERROR == sentBytes)
        {
            logError(pContext, WSAGetLastError());
        }
    }

    return success;
}
