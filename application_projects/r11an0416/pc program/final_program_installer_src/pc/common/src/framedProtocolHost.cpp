/***********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/*
*  Framed protocol support for the host
*/

#include <windows.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "serial.h"
#include "framedProtocolCommon.h"
#include "framedProtocolHost.h"
#include "memoryMap.h"

// Debug output control - define to enable debugging to STDOUT
//#define FP_DEBUG
//#define EXTRA_FP_DEBUG

// 20 seconds is long enough to erase the entire FLASH.
// 60 seconds is long enough for QSPI. 
#ifdef ENABLE_QSPI_FLASH_SUPPORT
#define FLASH_ERASE_TIMEOUT 60000
#else
#define FLASH_ERASE_TIMEOUT 20000
#endif

/**************************************************************
* Private data
**************************************************************/

// A single RX buffer structure
typedef struct RxBufferEntry_tag {
	// Receive data buffer
	uint8_t *pData;
	uint32_t bufferSize;
	uint32_t bytesInBuffer;

} RxBufferEntry;

// Types of framed protocol connection
typedef enum FpConnectionType_tag {
	SERIAL,
	TCP_CLIENT,

	INVALID_CONNECTION_TYPE
} FpConnectionType;

// Context data for serial connections
typedef struct FpSerialContext_tag {

	// Serial port number
	uint8_t portNumber;

} FpSerialContext;

// Context data for TCP/IP connections
typedef struct FpTcpIpContext_tag {

	// Remote address
	char remoteAddress[80];

} FpTcpIpContext;

// Context data for each framed protocol connection
typedef struct FpContext_tag {
	// If this context is in use
	bool inUse;

	// The handle for this context
	HANDLE handle;

	// Type of connection
	FpConnectionType type;

	// Connection specific context data
	union {
		FpSerialContext serial;
		FpTcpIpContext  tcpIp;
	} u;

	// Transport layer connection handle
	HANDLE hComm;

	// Transport layer callback to send data
	FpBlockingDataTransmit pBlockingTransmitFn;

	// Receive data buffer - single buffer entry for now
	RxBufferEntry *pRxBufferEntry;
	// Critical section object for managing access to the RX buffer between threads
	CRITICAL_SECTION rxBufferCriticalSection;

	// Receive data available event handle
	HANDLE hRxDataEvent;

	// In framing mode (normal apart from connection negotiation phase)?
	bool isFramed;

	// Complete receive frame (only in framing mode)
	// Only one frame deap since thats all the current protocol requires.
	RxBufferEntry *pRxFrameEntry;

} FpContext;

// The maximum number of connections supported
#define MAX_FP_CONNECTIONS 10

// The framed protocol connection context data
static FpContext fpContext[MAX_FP_CONNECTIONS] = { 0 };

// Critical section object for managing creation and destroying context between threads
static CRITICAL_SECTION fpContextAllocCriticalSection;

// Flag to trap uninitilised module
static bool fpInitialised = false;

/**************************************************************
* Private functions
**************************************************************/

#ifdef FP_DEBUG
static void dumpPacket(const char *type, const uint8_t *pData, const uint32_t numBytes)
{
	uint32_t i;
	// Don't dump all of a large packet
	uint32_t dumpBytes = (numBytes < 50) ? numBytes : 50;
	fprintf(stdout, "FP %s (%d bytes): ", type, numBytes);
	for (i = 0; i < dumpBytes; i++)
	{
		fprintf(stdout, "0x%02X ", pData[i]);
	}
	fprintf(stdout, "%s\n", (numBytes > dumpBytes ? "..." : ""));
}
#endif

// Allocate an RX buffer entry - enough for the largest framed packet
static RxBufferEntry *allocateRxBufferEntry(void)
{
	RxBufferEntry *pBuff = (RxBufferEntry *)malloc(sizeof(RxBufferEntry));

	pBuff->pData = (uint8_t *)malloc(MAX_LEN_DATA_PACKET);
	pBuff->bufferSize = MAX_LEN_DATA_PACKET;
	pBuff->bytesInBuffer = 0;

	return pBuff;
}

// Free an RX buffer entry
static void freeRxBufferEntry(RxBufferEntry *pBuff)
{
	if (pBuff)
	{
		if (pBuff->pData)
		{
			free(pBuff->pData);
		}
		free(pBuff);
	}
}

// Create the RX buffer
static void createRxBuffer(FpContext *pContext)
{
	// Currently a single buffer but designed to be easily upgraded to a buffer stack
	pContext->pRxBufferEntry = allocateRxBufferEntry();
	// Sharing of the RX buffer between user thread and RX thread is controlled by critical section mutex
	InitializeCriticalSection(&pContext->rxBufferCriticalSection);
	// No RX frames yet
	pContext->pRxFrameEntry = NULL;
}

// Pop an RX buffer entry - returns NULL if no rx data available
static RxBufferEntry *popRxBuffer(FpContext *pContext)
{
	RxBufferEntry *pBuff = NULL;

	EnterCriticalSection(&pContext->rxBufferCriticalSection);
	// In framing mode?
	if (pContext->isFramed)
	{
		// Anything on the frame queue?
		if (pContext->pRxFrameEntry && pContext->pRxFrameEntry->bytesInBuffer)
		{
			pBuff = pContext->pRxFrameEntry;
			pContext->pRxFrameEntry = NULL;
		}
	}
	// Otherwise just return any receive data
	else
	{
		if (pContext->pRxBufferEntry && pContext->pRxBufferEntry->bytesInBuffer)
		{
			pBuff = pContext->pRxBufferEntry;
			pContext->pRxBufferEntry = allocateRxBufferEntry();
		}
	}
	LeaveCriticalSection(&pContext->rxBufferCriticalSection);

	return pBuff;
}

// Destroy the RX buffer
static void destroyRxBuffer(FpContext *pContext)
{
	if (pContext->pRxBufferEntry)
	{
		freeRxBufferEntry(pContext->pRxBufferEntry);
		pContext->pRxBufferEntry = NULL;
	}
	if (pContext->pRxFrameEntry)
	{
		freeRxBufferEntry(pContext->pRxFrameEntry);
		pContext->pRxFrameEntry = NULL;
	}
	DeleteCriticalSection(&pContext->rxBufferCriticalSection);
}

// Allocate a context, returning NULL if none available
static FpContext *allocateContext(const FpConnectionType type)
{
	FpContext *pContext = NULL;
	int i = 0;
	// Connection contexts may be allocated or free from multiple threads
	EnterCriticalSection(&fpContextAllocCriticalSection);
	while ((i < MAX_FP_CONNECTIONS) && (NULL == pContext))
	{
		if (!fpContext[i].inUse)
		{
			pContext = &fpContext[i];
			memset(pContext, 0, sizeof(pContext));
			pContext->inUse = true;
			pContext->handle = (HANDLE *)pContext;
			pContext->type = type;
			createRxBuffer(pContext);
			// Create the RX data available event (auto-reset)
			pContext->hRxDataEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
			if (!pContext->hRxDataEvent)
			{
				fprintf(stderr, "Failed to create rx data event handle\n");
			}
			// Normal operation is in framed mode
			pContext->isFramed = true;
		}
		else
		{
			i++;
		}
	}
	LeaveCriticalSection(&fpContextAllocCriticalSection);
	return pContext;
}

// Free a context
static void freeContext(FpContext *pContext)
{
	// Connection contexts may be allocated or free from multiple threads
	EnterCriticalSection(&fpContextAllocCriticalSection);
	pContext->inUse = false;
	CloseHandle(pContext->hRxDataEvent);
	destroyRxBuffer(pContext);
	LeaveCriticalSection(&fpContextAllocCriticalSection);
}

// Get a context from a handle - returns NULL if it doesn't look like an active context
static FpContext *getContext(HANDLE handle)
{
	FpContext *pContext = (FpContext *)handle;
	if (// Address range check
		((pContext >= &fpContext[0]) && (pContext <= &fpContext[MAX_FP_CONNECTIONS - 1])) &&
		// Context check
		pContext->inUse && (INVALID_CONNECTION_TYPE != pContext->type))
	{
		return pContext;
	}
	else
	{
		fprintf(stderr, "Invalid framed protocol context %p\n", handle);
		return NULL;
	}
}

// Blocking (with timeout) read of a single byte
// Input: handle; serial context handle
// Input: timeoutms; the timeout period in mS
// Output: byte of received data
// Returns: true if byte read, false otherwise
static bool blockingReadByte(const HANDLE handle, const uint32_t timeoutms, uint8_t &byte)
{
	bool success = false;
	FpContext *pContext = getContext(handle);
	if (pContext)
	{
		DWORD waitResult = WaitForSingleObject(pContext->hRxDataEvent, timeoutms);
		switch (waitResult)
		{
		case WAIT_OBJECT_0:
		{
			// data available
			RxBufferEntry *pBuff = popRxBuffer(pContext);
			if (pBuff)
			{
				byte = *pBuff->pData;
				freeRxBufferEntry(pBuff);
				success = true;
			}
		} break;

		default:
			break;
		}

	}
	return success;
}

// Blocking (with timeout) read of a byte array
// Input: handle; serial context handle
// Input: timeoutms; the timeout period in mS
// Output: number of bytes in returned data
// Returns:
//  - a malloc'd buffer containing the data
//  - NULL if no data ready
// N.B: the caller takes responsibility for managing any returned buffer
static uint8_t *blockingReadBytes(const HANDLE handle, const uint32_t timeoutms, uint32_t &numBytes)
{
	uint8_t *pData = NULL;
	FpContext *pContext = getContext(handle);
	if (pContext)
	{
		DWORD waitResult = WaitForSingleObject(pContext->hRxDataEvent, timeoutms);
		switch (waitResult)
		{
		case WAIT_OBJECT_0:
		{
			// data available
			RxBufferEntry *pBuff = popRxBuffer(pContext);
			if (pBuff)
			{
				pData = pBuff->pData;
				numBytes = pBuff->bytesInBuffer;
				// We are taking ownership of the data
				pBuff->pData = NULL;
				freeRxBufferEntry(pBuff);
			}
		} break;

		default:
			break;
		}

	}
	return pData;
}

// Send a byte and wait a short time for the expected response
// Returns true if expected response received, false otherwise
static bool sendAndCheckReponse(const HANDLE handle, const uint8_t txByte, const uint8_t expectedByte)
{
	bool success = false;

	FpContext *pContext = getContext(handle);
	if (pContext)
	{
		if (pContext->pBlockingTransmitFn(pContext->hComm, &txByte, 1))
		{
			// 100mS timeout
			uint8_t rxByte;
			if (blockingReadByte(handle, 100, rxByte))
			{
				if (rxByte == expectedByte)
				{
					success = true;
				}
			}
		}
	}

	return success;
}

// Handle a basic response data packet
// Returns true if the packet is a reponse for the given opCode, false otherwise
static bool handleAcknowledgement(const uint8_t *pData, const uint32_t numBytes, const uint8_t opcode, uint8_t &status)
{
	bool valid = false;

	// A data packets are a minimum of 7 bytes long and begins with SOD
	if ((numBytes >= 7) && (START_OF_DATA == *pData))
	{
		uint8_t rxCode = pData[3];
		if ((rxCode & 0x7F) == opcode)
		{
			// Top bit indicates error - set status to status byte
			if (rxCode & 0x80)
			{
				status = pData[4];
			}
			else
			{
				status = 0;
			}
			valid = true;
		}
	}

#ifdef FP_DEBUG
	dumpPacket("ACK", pData, numBytes);
#endif

	return valid;
}

// Send a command and wait a short time for an acknowledgement
// Returns true if expected response received, false otherwise
static bool sendCommandAndWaitAck(const HANDLE handle, const uint8_t *pCommand, const uint32_t commandLength, const uint32_t timeoutmS = 2000)
{
	bool success = false;
	FpContext *pContext = getContext(handle);
	if (pContext)
	{
		if (pContext->pBlockingTransmitFn(pContext->hComm, pCommand, commandLength))
		{
			uint32_t numBytes;
			uint8_t *pResp = blockingReadBytes(handle, timeoutmS, numBytes);
			if (pResp)
			{
				uint8_t error;
				if (handleAcknowledgement(pResp, numBytes, pCommand[3], error))
				{
					if (0 == error)
					{
						success = true;
					}
					else
					{
						fprintf(stderr, "Error code 0x%02X for command 0x%02X\n", error, pCommand[3]);
					}
				}

				free(pResp);
			}
			else
			{
				fprintf(stderr, "No response for command 0x%02X\n", pCommand[3]);
			}
		}
		else
		{
			fprintf(stderr, "Failed to send command 0x%02X\n", pCommand[3]);
		}
	}

	return success;
}

// Send a command and wait a short time for an acknowledgement with payload data
// Returns pointer to malloc'd payload data if expected response received, NULL otherwise
// N.B: caller must free the payload memory!
static uint8_t * sendCommandAndWaitAckWithPayload(const HANDLE handle, const uint8_t *pCommand, const uint32_t commandLength, uint32_t *pPayloadLength, const bool suppressLogging, const uint32_t timeoutmS = 2000)
{
	uint8_t *pPayload = NULL;

	FpContext *pContext = getContext(handle);
	if (pContext)
	{
		uint32_t numBytes = 0;
		*pPayloadLength = 0;

		if (pContext->pBlockingTransmitFn(pContext->hComm, pCommand, commandLength))
		{
			uint8_t *pResp = blockingReadBytes(handle, timeoutmS, numBytes);
			if (pResp)
			{
				uint8_t error;
				if (handleAcknowledgement(pResp, numBytes, pCommand[3], error))
				{
					if (0 == error)
					{
						// Its a valid ack for the command, now extract the payload if present
						uint8_t *pLen = &pResp[1];
						uint16_t length = READ16BE(pLen);
						// Length includes the 1 byte response code
						if (length > 1)
						{
							*pPayloadLength = length - 1;
							pPayload = (uint8_t *)malloc(*pPayloadLength);
							memcpy(pPayload, &pResp[4], *pPayloadLength);
						}
					}
					else
					{
						if (!suppressLogging)
						{
							fprintf(stderr, "Error code 0x%02X for command 0x%02X\n", error, pCommand[3]);
						}
					}
				}
				else
				{
					fprintf(stdout, "sendCommandAndWaitAckWithPayload: no response received \n");
				}

				free(pResp);
			}
			else
			{
				if (!suppressLogging)
				{
					fprintf(stderr, "No response for command 0x%02X\n", pCommand[3]);
				}
			}
		}
		else
		{
			if (!suppressLogging)
			{
				fprintf(stderr, "Failed to send command 0x%02X\n", pCommand[3]);
			}
		}
	}

	return pPayload;
}

// Build a baudrate command
static uint32_t buildBaudRateCommand(const uint8_t *pBuffer, const uint32_t bufferLength, const uint32_t baudrate)
{
	uint8_t payload[4];
	WRITE32BE(&payload[0], baudrate);
	return fpcBuildMessage(pBuffer, bufferLength, START_OF_HEADER, BAUDRATE, payload, sizeof(payload));
}

// Build an ID authentication command
static uint32_t buildIdAuthenticationCommand(const uint8_t *pBuffer, const uint32_t bufferLength, const uint8_t *pIdc)
{
	return fpcBuildMessage(pBuffer, bufferLength, START_OF_HEADER, ID_AUTHENTICATION, pIdc, 16);
}

// Build an erase command
static uint32_t buildEraseCommand(const uint8_t *pBuffer, const uint32_t bufferLength, const uint32_t startAddr, const uint32_t endAddr)
{
	uint8_t payload[8];
	WRITE32BE(&payload[0], startAddr);
	WRITE32BE(&payload[4], endAddr);
	return fpcBuildMessage(pBuffer, bufferLength, START_OF_HEADER, ERASE, payload, sizeof(payload));
}

// Build a write command
static uint32_t buildWriteCommand(const uint8_t *pBuffer, const uint32_t bufferLength, const uint32_t startAddr, const uint32_t endAddr)
{
	uint8_t payload[8];
	WRITE32BE(&payload[0], startAddr);
	WRITE32BE(&payload[4], endAddr);
	return fpcBuildMessage(pBuffer, bufferLength, START_OF_HEADER, WRITE, payload, sizeof(payload));
}

// Build a read command
static uint32_t buildReadCommand(const uint8_t *pBuffer, const uint32_t bufferLength, const uint32_t startAddr, const uint32_t endAddr)
{
	uint8_t payload[8];
	WRITE32BE(&payload[0], startAddr);
	WRITE32BE(&payload[4], endAddr);
	return fpcBuildMessage(pBuffer, bufferLength, START_OF_HEADER, READ, payload, sizeof(payload));
}

// Build a certificate request command
static uint32_t buildCertificateRequestCommand(const uint8_t *pBuffer, const uint32_t bufferLength)
{
	return fpcBuildMessage(pBuffer, bufferLength, START_OF_HEADER, CERTIFICATE_REQUEST, NULL, 0);
}

// Build a certificate request command
static uint32_t buildDeviceKeyRequestCommand(const uint8_t *pBuffer, const uint32_t bufferLength)
{
	return fpcBuildMessage(pBuffer, bufferLength, START_OF_HEADER, WRAPPED_KEY_REQUEST, NULL, 0);
}

// Build a certificate request command
static uint32_t buildGetOEMCertCommand(const uint8_t *pBuffer, const uint32_t bufferLength)
{
	return fpcBuildMessage(pBuffer, bufferLength, START_OF_HEADER, WRAPPED_KEY_CERT_REQUEST, NULL, 0);
}

// Build WRAPPED_KEY_CERT_RESP message
static uint32_t buildSendOEMCertCommand(const uint8_t *pBuffer, const uint32_t bufferLength, const uint8_t *theData, uint16_t dataLen)
{
	return fpcBuildMessage(pBuffer, bufferLength, START_OF_HEADER, WRAPPED_KEY_CERT_PROGRAM, theData, dataLen);
}
// Build WRAPPED_KEY_CHALLENGE_RESP message
static uint32_t buildSendChallengeResponseMessage(const uint8_t *pBuffer, const uint32_t bufferLength, const uint8_t *theData, uint16_t dataLen)
{
	return fpcBuildMessage(pBuffer, bufferLength, START_OF_HEADER, WRAPPED_KEY_CHALLENGE_RESP, theData, dataLen);
}
// Build a module version list request command
static uint32_t buildModuleVersionListRequestCommand(const uint8_t *pBuffer, const uint32_t bufferLength)
{
	return fpcBuildMessage(pBuffer, bufferLength, START_OF_HEADER, MODULE_VERSION_LIST_REQUEST, NULL, 0);
}

// Build an apply image command
static uint32_t buildApplyImageCommand(const uint8_t *pBuffer, const uint32_t bufferLength)
{
	return fpcBuildMessage(pBuffer, bufferLength, START_OF_HEADER, APPLY_IMAGE, NULL, 0);
}

// Build a data packet
static uint32_t buildDataPacket(const uint8_t *pBuffer, const uint32_t bufferLength, const uint8_t *pData, const uint32_t dataLength, const CommandCode resCode)
{
	uint32_t packetLength = 0;

	// Max data packet payload is 1024 bytes
	if (dataLength <= 1024)
	{
		packetLength = fpcBuildMessage(pBuffer, bufferLength, START_OF_DATA, resCode, pData, dataLength);
	}

	return packetLength;
}

// Write a block of data to code flash. This assumes that addresses have been validated and all the
// data fits within the given single flash region. Tail padding will be applied to ensure writes are
// always a multiple of the write block size.
static bool writeFlashRegion(const HANDLE handle, const FlashDescriptionRecord *pFlashRecord, const uint32_t startAddr, const uint8_t *pData, const uint32_t numBytes)
{
	bool success = false;

	// Maximum number of bytes to send in one packet (chunkSize = 1..1024 max)
	// Must be a multiple of the write access unit!
	uint32_t chunkSize = pFlashRecord->writeBlockSize * (1024 / pFlashRecord->writeBlockSize);

	// The end address must be aligned with the end of a write block, so round up if necessary and we will pad the data later
	uint32_t endAddr = startAddr + numBytes - 1;
	uint32_t sourceEndAddr = endAddr;
	if ((endAddr + 1) % pFlashRecord->writeBlockSize)
	{
		// Doesn't nicely fit the write block size, round up
		endAddr = (((endAddr / pFlashRecord->writeBlockSize) + 1) * pFlashRecord->writeBlockSize) - 1;
#ifdef FP_DEBUG
		fprintf(stdout, "Rounding up endAddr from 0x%X to 0x%X\n", sourceEndAddr, endAddr);
#endif
	}

	// Send the write command
	uint8_t buffer[LEN_WRITE_CMD];
	uint32_t commandLen = buildWriteCommand(buffer, sizeof(buffer), startAddr, endAddr);
	if (commandLen > 0)
	{
		if (sendCommandAndWaitAck(handle, buffer, commandLen))
		{
			uint8_t *pDataPkt = (uint8_t *)malloc(MAX_LEN_DATA_PACKET);

			// Write mode engaged, now send the data in packets of chunkSize
			success = true;
			uint32_t bytesSent = 0;
			while (success && (bytesSent < numBytes))
			{
				// No padding required?
				if ((numBytes - bytesSent) >= chunkSize)
				{
					commandLen = buildDataPacket(pDataPkt, MAX_LEN_DATA_PACKET, pData + bytesSent, chunkSize, WRITE);
					success = sendCommandAndWaitAck(handle, pDataPkt, commandLen);
				}
				else
				{
					// Create a padded buffer up to the nearest whole write access unit
					uint32_t bytesRemaining = sourceEndAddr - (startAddr + bytesSent) + 1;
					uint32_t paddedSize = (bytesRemaining % pFlashRecord->writeBlockSize) == 0 ? bytesRemaining : ((bytesRemaining / pFlashRecord->writeBlockSize) + 1) * pFlashRecord->writeBlockSize;
#ifdef FP_DEBUG
					fprintf(stdout, "%d bytes remaining, padding to %d\n", bytesRemaining, paddedSize);
#endif
					uint8_t *pPadded = (uint8_t *)malloc(paddedSize);
					// Pad with 0xFF to keep unused bytes 'erased'
					memset(pPadded, 0xFF, paddedSize);
					memcpy(pPadded, pData + bytesSent, bytesRemaining);
					// And send it
					commandLen = buildDataPacket(pDataPkt, MAX_LEN_DATA_PACKET, pPadded, paddedSize, WRITE);
					success = sendCommandAndWaitAck(handle, pDataPkt, commandLen);
					free(pPadded);
				}
				bytesSent += chunkSize;
			}

			free(pDataPkt);
		}
		else
		{
			fprintf(stderr, "Write command failed\n");
		}
	}

	return success;
}

/**************************************************************
* Module initialisation
**************************************************************/

// Initialise the framed protocol module
void fpInitialise(void)
{
	InitializeCriticalSection(&fpContextAllocCriticalSection);

	fpInitialised = true;
}

/**************************************************************
* Transport specific application API
**************************************************************/

// Open a framed serial connection to the chip.
// Input: portNumber; the number of the serial port to open
// Input: baudRate; the baudrate to negotiate for the framed connection.
// Input: doCommsSettingPhase; true if the 'hidden flash' comms setting phase and baudrate negotiation is required
// Returning framed protocol Handle on success, INVALID_HANDLE_VALUE if failed.
HANDLE fpOpenFramedConnection(const uint8_t portNumber, const uint32_t baudrate, const bool doCommsSettingPhase)
{
	HANDLE handle = INVALID_HANDLE_VALUE;

	if (fpInitialised)
	{
		FpContext *pContext = allocateContext(SERIAL);
		if (pContext)
		{
			// Setup serial transmit function
			pContext->pBlockingTransmitFn = &serialBlockingSendByteArray;
			// Store serial specific context
			pContext->u.serial.portNumber = portNumber;

			if (doCommsSettingPhase)
			{
				// Switch to unframed mode
				pContext->isFramed = false;

				// Connections always start at 9600 baud unframed
				pContext->hComm = serialOpenCommPort(pContext->handle, portNumber, 9600);
				if (INVALID_HANDLE_VALUE != pContext->hComm)
				{
					bool success = false;

					// Try to exchange 0x00's a number of times
					bool zeroReceived = false;
					int i = 0;
					while (!zeroReceived && (i++ < 100))
					{
						zeroReceived = sendAndCheckReponse(pContext->handle, 0x00, 0x00);
					}

					if (zeroReceived)
					{
						// Now the "comms setting phase"
						if (sendAndCheckReponse(pContext->handle, 0x55, 0xC3))
						{
							// From now on, all messages are framed
							pContext->isFramed = true;

							if (9600 != baudrate)
							{
								// Negotiate the new baudrate
								uint8_t baudRateCommand[LEN_BAUDRATE_CMD];
								uint32_t commandLen = buildBaudRateCommand(baudRateCommand, sizeof(baudRateCommand), baudrate);
								if (commandLen > 0)
								{
									if (sendCommandAndWaitAck(pContext->handle, baudRateCommand, commandLen))
									{
										// Update the serial port
										if (serialUpdateBaudrate(pContext->hComm, baudrate))
										{
											success = true;
										}
										else
										{
											fprintf(stderr, "Failed to set baudrate %d\n", baudrate);
										}
									}
									else
									{
										fprintf(stderr, "Failed to negotiate baudrate %d\n", baudrate);
									}
								}
							}
							else
							{
								// All done
								success = true;
							}
						}
					}

					if (!success)
					{
						// Tidyup on failure
						serialCloseCommPort(pContext->hComm);
						freeContext(pContext);
						pContext = NULL;
					}
				}
			}
			else
			{
				// Just go straight to the required baud rate
				pContext->hComm = serialOpenCommPort(pContext->handle, portNumber, baudrate);
				if (INVALID_HANDLE_VALUE == pContext->hComm)
				{
					fprintf(stderr, "Failed to open serial port at %d baud\n", baudrate);
					freeContext(pContext);
					pContext = NULL;
				}
			}
		}
		else
		{
			fprintf(stderr, "Failed to allocate framed protocol context\n");
		}

		if (pContext != NULL)
		{
			handle = pContext->handle;
		}
	}
	else
	{
		fprintf(stderr, "Framed protocol module has not been initialised!\n");
	}

	return handle;
}

// Close a framed serial connection
// Input: handle; the framed protocol handle
void fpCloseCommPort(const HANDLE handle)
{
	FpContext *pContext = getContext(handle);
	if (pContext && (SERIAL == pContext->type))
	{
		serialCloseCommPort(pContext->hComm);
		freeContext(pContext);
	}
}



/**************************************************************
* Application API for all transports
**************************************************************/

// Get an array of active connection handles.
// N.B: returned array is the responsibility of the caller and must be freed when no longer required.
// Output: pNumHandles; the number of handles in the returned array
// Returning pointer to malloc'd handle array, NULL if no active connections.
HANDLE *fpGetConnections(uint32_t *pNumHandles)
{
	HANDLE *pHandles = NULL;

	uint32_t numHandles = 0;
	pHandles = (HANDLE *)malloc(MAX_FP_CONNECTIONS * sizeof(HANDLE));

	for (int i = 0; i < MAX_FP_CONNECTIONS; i++)
	{
		if (fpContext[i].inUse)
		{
			pHandles[numHandles] = fpContext[i].handle;
			numHandles++;
		}
	}

	if (numHandles == 0)
	{
		free(pHandles);
		pHandles = NULL;
	}

	*pNumHandles = numHandles;

	return pHandles;
}

// Get a char array description of the connection. User supplies the char array into which the null terminated
// description will be written. A empty string will be returned if the handle is not valid. 
// Input: handle; the framed protocol handle to use
// Input: length of char array in which to write the description
// Input: pointer to char array
void fpGetHandleDescription(const HANDLE handle, char * const pStr, const uint8_t sizeOfStr)
{
	if (sizeOfStr > 0)
	{
		FpContext *pContext = getContext(handle);
		if (pContext)
		{
			switch (pContext->type)
			{
			case SERIAL:
				sprintf_s(pStr, sizeOfStr, "COM%d", pContext->u.serial.portNumber);
				break;

			case TCP_CLIENT:
				sprintf_s(pStr, sizeOfStr, "%s", pContext->u.tcpIp.remoteAddress);
				break;

			default:
				sprintf_s(pStr, sizeOfStr, "Unknown connection type %d", pContext->type);
				break;
			}
		}
		else
		{
			pStr[0] = 0;
		}
	}
}

// Erase all the flash memory
// Input: handle; the framed protocol handle to use
// Input: pFlashDesc; pointer to the description of the available flash regions in the target chip
// Returns true on success, false otherwise
bool fpEraseAll(const HANDLE handle, const FlashDescription *pFlashDesc)
{
	// Erase all the flash regions
	int i;
	int regionsErased = 0;
	for (i = 0; i < pFlashDesc->numFlashRecords; i++)
	{
		uint8_t buffer[LEN_ERASE_CMD];
		uint32_t commandLen = buildEraseCommand(buffer, sizeof(buffer), pFlashDesc->pFlashRecords[i].startAddr, pFlashDesc->pFlashRecords[i].endAddr);
		if (commandLen > 0)
		{
			// Extended timeout for erase required - 20secs is enough for a full chip erase
			if (sendCommandAndWaitAck(handle, buffer, commandLen, 20000))
			{
				regionsErased++;
			}
		}
	}

	return (regionsErased == pFlashDesc->numFlashRecords) ? true : false;
}

// Erase a region of flash
// Input: handle; the framed protocol handle to use
// Input: pFlashDesc; pointer to the description of the available flash regions in the target chip
// Input: startAddr; the start address of the area to be erased
// Input: startAddr; the end address of the area to be erased
// Returns true on success, false otherwise
bool fpErase(const HANDLE handle, const FlashDescription *pFlashDesc, const uint32_t startAddr, const uint32_t endAddr)
{
	bool success = false;

	// Find the flash description record(s) that contain the start & end addresses
	int startingFlashDescription = -1;
	int endingFlashDescription = -1;
	int i = 0;
	while ((i < pFlashDesc->numFlashRecords) && ((-1 == startingFlashDescription) || (-1 == endingFlashDescription)))
	{
		if ((startAddr >= pFlashDesc->pFlashRecords[i].startAddr) && (startAddr <= pFlashDesc->pFlashRecords[i].endAddr))
		{
			startingFlashDescription = i;
		}

		if ((endAddr >= pFlashDesc->pFlashRecords[i].startAddr) && (endAddr <= pFlashDesc->pFlashRecords[i].endAddr))
		{
			endingFlashDescription = i;
		}

		i++;
	}

	// Start and end addresses look OK?
	if ((startingFlashDescription >= 0) && (endingFlashDescription >= 0))
	{
		success = true;

		i = startingFlashDescription;
		while (success &&  i <= endingFlashDescription)
		{
			uint8_t buffer[LEN_ERASE_CMD];
			uint32_t regionStartAddr;
			uint32_t regionEndAddr;
			if (i == startingFlashDescription)
			{
				regionStartAddr = startAddr;
			}
			else
			{
				regionStartAddr = pFlashDesc->pFlashRecords[i].startAddr;
			}
			if (i == endingFlashDescription)
			{
				regionEndAddr = endAddr;
			}
			else
			{
				regionEndAddr = pFlashDesc->pFlashRecords[i].endAddr;
			}

			uint32_t commandLen = buildEraseCommand(buffer, sizeof(buffer), regionStartAddr, regionEndAddr);
			if (commandLen > 0)
			{
				// Extended timeout for erase required - 20secs is enough for a full chip erase (60 for QSPI)
				success = sendCommandAndWaitAck(handle, buffer, commandLen, FLASH_ERASE_TIMEOUT);
			}
			else
			{
				success = false;
			}

			i++;
		}
	}

	return success;
}

// Program a region of flash
// Input: handle; the framed protocol handle to use
// Input: pFlashDesc; pointer to the description of the available flash regions in the target chip
// Input: startAddr; the address in chip memory from which to write the data
// Input: pData; pointer to the data to write into flash
// Input: numBytes; the number of bytes of data to write
// Returns true on success, false otherwise
bool fpWrite(const HANDLE handle, const FlashDescription *pFlashDesc, const uint32_t startAddr, const uint8_t *pData, const uint32_t numBytes)
{
	bool success = false;
	int i;

	if (numBytes > 0)
	{
		// Find the flash description record(s) that contain the start & end addresses
		uint32_t endAddr = startAddr + numBytes - 1;
		int startingFlashDescription = -1;
		int endingFlashDescription = -1;
		i = 0;
		while ((i < pFlashDesc->numFlashRecords) && ((-1 == startingFlashDescription) || (-1 == endingFlashDescription)))
		{
		//	fprintf(stdout, "fpWrite i=%d, startingFlashDescription is %d, endingFlashDescription is %d\n", i, startingFlashDescription, endingFlashDescription);

			if ((startAddr >= pFlashDesc->pFlashRecords[i].startAddr) && (startAddr <= pFlashDesc->pFlashRecords[i].endAddr))
			{
				startingFlashDescription = i;
			}

			if ((endAddr >= pFlashDesc->pFlashRecords[i].startAddr) && (endAddr <= pFlashDesc->pFlashRecords[i].endAddr))
			{
				endingFlashDescription = i;
			}
		//	fprintf(stdout, "fpWrite2 i=%d, startingFlashDescription is %d, endingFlashDescription is %d\n", i, startingFlashDescription, endingFlashDescription);
			i++;
		}

		// Start and end addresses look OK?
		if ((startingFlashDescription >= 0) && (endingFlashDescription >= 0))
		{
			int currentFlashDescription = startingFlashDescription;
			uint32_t bytesSent = 0;
			uint32_t regionStartAddr = startAddr;
			success = true;
			while (success && (bytesSent < numBytes))
			{
				// This takes into account that we may not be starting at the begining of a region
				uint32_t availableRegionSize = pFlashDesc->pFlashRecords[currentFlashDescription].endAddr - regionStartAddr + 1;

				// How many bytes can we send in this region?
				uint32_t bytesToSendInRegion = (numBytes - bytesSent) > availableRegionSize ? availableRegionSize : numBytes - bytesSent;

				success = writeFlashRegion(handle, &pFlashDesc->pFlashRecords[currentFlashDescription], regionStartAddr, pData + bytesSent, bytesToSendInRegion);

				bytesSent += bytesToSendInRegion;

				// Another region to process?
				if (bytesSent < numBytes)
				{
					currentFlashDescription++;
					// Now we start at the beginning of the next region
					regionStartAddr = pFlashDesc->pFlashRecords[currentFlashDescription].startAddr;
				}
			}
		}
	}

	return success;
}

// Request device certificate
// Input: handle; the framed protocol handle to use
// Output: pCertificateLength; pointer to storage for certificate length.
// Returns Pointer to device certificate on success, NULL otherwise
// N.B: caller must free the returned memory!
uint8_t *fpGetDeviceCertificate(const HANDLE handle, uint32_t *pCertificateLength, bool suppressLogging)
{
	uint8_t *pCert = NULL;
	*pCertificateLength = 0;

	uint8_t buffer[LEN_CERTIFICATE_CMD];
	uint32_t commandLen = buildCertificateRequestCommand(buffer, sizeof(buffer));
	if (commandLen > 0)
	{
		pCert = sendCommandAndWaitAckWithPayload(handle, buffer, commandLen, pCertificateLength, suppressLogging);
	}

	return pCert;
}
// Request device Key
// Input: handle; the framed protocol handle to use
// Output: pKeyLength; pointer to storage for Key length.
// Returns Pointer to device Key on success, NULL otherwise
// N.B: caller must free the returned memory!
uint8_t *fpGetDeviceKey(const HANDLE handle, uint32_t *pKeyLength, bool suppressLogging)
{
	uint8_t *pKey = NULL;
	*pKeyLength = 0;

	uint8_t buffer[LEN_DEVICEKEY_CMD];
	uint32_t commandLen = buildDeviceKeyRequestCommand(buffer, sizeof(buffer));
	if (commandLen > 0)
	{
		pKey = sendCommandAndWaitAckWithPayload(handle, buffer, commandLen, pKeyLength, suppressLogging);
	}

	return pKey;
}

uint8_t *fpGetOEMCert(const HANDLE handle, uint32_t *oemCertLength, bool suppressDevCertErrors)
{
	uint8_t *pOemCert = NULL;
	*oemCertLength = 0;

	uint8_t buffer[LEN_CERTIFICATE_CMD];
	uint32_t commandLen = buildGetOEMCertCommand(buffer, LEN_CERTIFICATE_CMD);

	if (commandLen > 0)
	{
		pOemCert = sendCommandAndWaitAckWithPayload(handle, buffer, commandLen, oemCertLength, suppressDevCertErrors);
	}

	return pOemCert;
}

uint8_t *fpSendChallengeResponseMessage(const HANDLE handle, const uint8_t *theHash, const uint16_t data_len, bool suppressDevCertErrors)
{
	bool success = true;
	uint8_t buffer[MAX_LEN_DATA_PACKET];
	uint32_t payloadLen;
	uint32_t commandLen = buildSendChallengeResponseMessage(buffer, sizeof(buffer), theHash, data_len);
	uint8_t *payload = NULL;
	if (commandLen > 0)
	{
		payload = sendCommandAndWaitAckWithPayload(handle, buffer, commandLen, &payloadLen, suppressDevCertErrors);
	}
	return payload;
}

bool fpSendDeviceCert(const HANDLE handle, const uint8_t *theCert, const uint16_t data_len, bool suppressDevCertErrors)
{
	bool success = false;
	uint8_t buffer[MAX_LEN_DATA_PACKET];
	uint32_t commandLen = buildSendOEMCertCommand(buffer, sizeof(buffer), theCert, data_len);
	if (commandLen > 0)
	{
		success = sendCommandAndWaitAck(handle, buffer, commandLen);
	}

	return success;
}

// Request module version list
// Input: handle; the framed protocol handle to use
// Output: pVersionListLength; pointer to storage for module version list length.
// Returns Pointer to module version list on success, NULL otherwise
// N.B: caller must free the returned memory!
uint8_t *fpGetModuleVersionList(const HANDLE handle, uint32_t *pVersionListLength)
{
	uint8_t *pList = NULL;
	*pVersionListLength = 0;

	uint8_t buffer[LEN_VERSION_LIST_CMD];
	uint32_t commandLen = buildModuleVersionListRequestCommand(buffer, sizeof(buffer));
	if (commandLen > 0)
	{
		pList = sendCommandAndWaitAckWithPayload(handle, buffer, commandLen, pVersionListLength, false);
	}

	return pList;
}

// Send apply image command to the target
// Input: handle; the framed protocol handle to use
// Returns true on success, false otherwise
bool fpApplyImage(const HANDLE handle)
{
	bool success = false;
	uint8_t buffer[LEN_APPLY_IMAGE_CMD];
	uint32_t commandLen = buildApplyImageCommand(buffer, sizeof(buffer));
	if (commandLen > 0)
	{
		success = sendCommandAndWaitAck(handle, buffer, commandLen);
	}

	return success;
}

// Read the current flash access window configuration
// Input: handle; the framed protocol handle to use
// Output: start; the first address in the access window
// Output: end; the last address in the access window
// Returns true on success, false otherwise
bool fpGetFlashAccessWindow(const HANDLE handle, uint32_t *pStart, uint32_t *pEnd)
{
	bool success = false;

	// Send the read command
	uint8_t buffer[LEN_READ_CMD];
	uint32_t commandLen = buildReadCommand(buffer, sizeof(buffer), FAW_CONFIG_ADDR_START + 4, FAW_CONFIG_ADDR_START + 7);
	if (commandLen > 0)
	{
		uint8_t *pReg = NULL;
		uint32_t readLen;
		pReg = sendCommandAndWaitAckWithPayload(handle, buffer, commandLen, &readLen, false);
		if (pReg && (4 == readLen))
		{
			// Extract and scale register values
			*pStart = (pReg[0] << 13) + ((pReg[1] & 0x03) << 21);
			// pEnd is the last address IN the window, so subtract 1 from FAWE)
			*pEnd = (pReg[2] << 13) + ((pReg[3] & 0x03) << 21) - 1;
			success = true;
		}
	}

	return success;
}

// Request the flash access window be configured
// Input: handle; the framed protocol handle to use
// Input: start; the first address in the access window
// Input: end; the last address in the access window
// Returns true on success, false otherwise
bool fpSetFlashAccessWindow(const HANDLE handle, const uint32_t start, const uint32_t end)
{
	bool success = false;

	// Build fields for the flash access window register
	// Don't exchange first 8K area (BTFLG == 1)
	uint8_t btflg = 0x80;
	// FAWS
	uint16_t faws = ((start >> 13) & 0x03FF);
	// FAWE (end is the last address IN the window, so add 1 to get FAWE)
	uint16_t fawe = (((end + 1) >> 13) & 0x03FF);

	//fprintf(stdout, "Flash Access Window start=0x%x, end=0x%x\n", faws, fawe);
	uint8_t buffer[LEN_WRITE_CMD];
	uint32_t commandLen = buildWriteCommand(buffer, sizeof(buffer), FAW_CONFIG_ADDR_START, FAW_CONFIG_ADDR_END);
	if (commandLen > 0)
	{
		if (sendCommandAndWaitAck(handle, buffer, commandLen))
		{
			uint8_t *pDataPkt = (uint8_t *)malloc(MAX_LEN_DATA_PACKET);

			// Write mode engaged, now send the data

			// The access unit for the config area is 16byte blocks
			uint8_t data[16];
			// Default unused bits to 1's
			memset(data, 0xFF, sizeof(data));
			// FAW registers in the 4th-8th bytes of the block, little endian and unused bits to 1
			data[4] = faws & 0xFF;
			data[5] = ((faws >> 8) & 0x03) | 0xF8; ///******** WARNING _ FSPR BIT ALSO WRITTEN HERE..DO NOT CHAGE 0xF8 VALUE*****************
			data[6] = fawe & 0xFF;
			data[7] = ((fawe >> 8) & 0x03) | btflg | 0x78;
			commandLen = buildDataPacket(pDataPkt, MAX_LEN_DATA_PACKET, data, sizeof(data), WRITE);
			success = sendCommandAndWaitAck(handle, pDataPkt, commandLen);

			free(pDataPkt);
		}
	}

    return success;
}

// Request the flash access window and FSPR configured
// Input: handle; the framed protocol handle to use
// Input: start; the first address in the access window
// Input: end; the last address in the access window
// Returns true on success, false otherwise
bool fpSetFlashAccessWindowAndFlashProtection(const HANDLE handle, const uint32_t start, const uint32_t end)
{
    bool success = false;

    // Build fields for the flash access window register
    // Don't exchange first 8K area (BTFLG == 1)
    uint8_t btflg = 0x80;
    // FAWS
    uint16_t faws = ((start >> 13) & 0x03FF);
    // FAWE (end is the last address IN the window, so add 1 to get FAWE)
    uint16_t fawe = (((end + 1) >> 13) & 0x03FF);

    //fprintf(stdout, "Flash Access Window start=0x%x, end=0x%x\n", faws, fawe);
    uint8_t buffer[LEN_WRITE_CMD];
    uint32_t commandLen = buildWriteCommand(buffer, sizeof(buffer), FAW_CONFIG_ADDR_START, FAW_CONFIG_ADDR_END);
    if (commandLen > 0)
    {
        if (sendCommandAndWaitAck(handle, buffer, commandLen))
        {
            uint8_t *pDataPkt = (uint8_t *)malloc(MAX_LEN_DATA_PACKET);

            // Write mode engaged, now send the data

            // The access unit for the config area is 16byte blocks
            uint8_t data[16];
            // Default unused bits to 1's
            memset(data, 0xFF, sizeof(data));
            // FAW registers in the 4th-8th bytes of the block, little endian and unused bits to 1
            data[4] = faws & 0xFF;
#ifdef ACTIVATE_THE_FSPR
            data[5] = ((faws >> 8) & 0x03) | 0x78; ///******** WARNING Set the FSPR: **** WARNING: THIS CANNOT BE UNDONE!*****************
#else
            data[5] = ((faws >> 8) & 0x03) | 0xF8; ///******** WARNING _ FSPR BIT ALSO WRITTEN HERE..DO NOT CHAGE 0xF8 VALUE *********************
#endif
            data[6] = fawe & 0xFF;
            data[7] = ((fawe >> 8) & 0x03) | btflg | 0x78;
            commandLen = buildDataPacket(pDataPkt, MAX_LEN_DATA_PACKET, data, sizeof(data), WRITE);
            success = sendCommandAndWaitAck(handle, pDataPkt, commandLen);

            free(pDataPkt);
        }
    }

    return success;
}

// Request the flash access window register be reset (sets FAWS, FAWE and BTFLG to all 1's)
// Input: handle; the framed protocol handle to use
// Returns true on success, false otherwise
// **** WARNING: MAKE SURE YOU WRITE ALL 1's . THIS ALSO WRITES TO FLASH PROTECTION REGISTER(FSPR)
// IF ALL 0's ARE WRITTEN, THIS WILL LOCK THE FLASH PROTECTION REGISTERS AND CANNOT BE UNDONE ****
// ^^^^^^ YOU HAVE BEEN WARNED ^^^^^^
bool fpResetFlashAccessWindowRegister(const HANDLE handle)
{
	bool success = false;

	uint8_t buffer[LEN_WRITE_CMD];
	uint32_t commandLen = buildWriteCommand(buffer, sizeof(buffer), FAW_CONFIG_ADDR_START, FAW_CONFIG_ADDR_END);
	if (commandLen > 0)
	{
		if (sendCommandAndWaitAck(handle, buffer, commandLen))
		{
			uint8_t *pDataPkt = (uint8_t *)malloc(MAX_LEN_DATA_PACKET);

			// Write mode engaged, now send the data - all 1's to disable FAWS, FAWE and BTFLG , ***FSPR***
			uint8_t data[16];
			memset(data, 0xFF, sizeof(data));
			commandLen = buildDataPacket(pDataPkt, MAX_LEN_DATA_PACKET, data, sizeof(data), WRITE);
			success = sendCommandAndWaitAck(handle, pDataPkt, commandLen);

			free(pDataPkt);
		}
	}

	return success;
}

// Read the FSPR flash protection flag
// Input: handle; the framed protocol handle to use
// Output: pIsProtected, if true the FSPR is active (0), else FSPR has not yet been set (1)
// Returns true on success, false otherwise
bool fpGetFlashProtection(const HANDLE handle, bool *pIsProtected)
{
	bool success = false;

	// Send the read command
	uint8_t buffer[LEN_READ_CMD];
	// Just read the top byte of the register
	uint32_t commandLen = buildReadCommand(buffer, sizeof(buffer), FSPR_CONFIG_ADDR_START + 5, FSPR_CONFIG_ADDR_START + 5);
	if (commandLen > 0)
	{
		uint8_t *pReg = NULL;
		uint32_t readLen;
		pReg = sendCommandAndWaitAckWithPayload(handle, buffer, commandLen, &readLen, false);
		if (pReg && (1 == readLen))
		{
			if (*pReg & 0x80)
			{
				*pIsProtected = false;
			}
			else
			{
				*pIsProtected = true;
			}
			success = true;
		}
	}

	return success;
}

// Set the FSPR flash protection flag (FSPR=>0)
// **** WARNING: THIS WILL LOCK THE FLASH PROTECTION REGISTERS AND CANNOT BE UNDONE ****
// Input: handle; the framed protocol handle to use
// Returns true on success, false otherwise
// ^^^^^^ YOU HAVE BEEN WARNED ^^^^^^
bool fpSetFlashProtection(const HANDLE handle)
{
	bool success = false;

	uint8_t buffer[LEN_WRITE_CMD];
	uint32_t commandLen = buildWriteCommand(buffer, sizeof(buffer), FSPR_CONFIG_ADDR_START, FSPR_CONFIG_ADDR_END);
	if (commandLen > 0)
	{
		if (sendCommandAndWaitAck(handle, buffer, commandLen))
		{
			uint8_t *pDataPkt = (uint8_t *)malloc(MAX_LEN_DATA_PACKET);

			// Write mode engaged, now send the data

			// The access unit for the config area is 16byte blocks
			uint8_t data[16];
			// Default unused bits to 1's
			memset(data, 0xFF, sizeof(data));
			// FSPR is in 5th byte of the block, unused bits to 1
			data[5] = 0x7F; // Set FSPR to zero to enable

			commandLen = buildDataPacket(pDataPkt, MAX_LEN_DATA_PACKET, data, sizeof(data), WRITE);
			success = sendCommandAndWaitAck(handle, pDataPkt, commandLen);

			free(pDataPkt);
		}
	}

	return success;
}

// Read the current OSIS register value
// Input: handle; the framed protocol handle to use
// Output: pOsis; user supplied OSIS structure into which the register value will be written
// Returns true on success, false otherwise
bool fpGetOsis(const HANDLE handle, FpOsis *pOsis)
{
	bool success = false;

	// Send the read command
	uint8_t buffer[LEN_READ_CMD];
	uint32_t commandLen = buildReadCommand(buffer, sizeof(buffer), OSIS_CONFIG_AREA_START, OSIS_CONFIG_AREA_END);
	if (commandLen > 0)
	{
		uint8_t *pReg = NULL;
		uint32_t readLen;
		pReg = sendCommandAndWaitAckWithPayload(handle, buffer, commandLen, &readLen, false);
		if (pReg && (16 == readLen))
		{
			memcpy(pOsis->reg, pReg, 16);
			success = true;
		}
	}

	return success;
}

// Set the OSIS register
// Input: handle; the framed protocol handle to use
// Input: pOsis; the value to write to the OSIS register
// Returns true on success, false otherwise
bool fpSetOsis(const HANDLE handle, const FpOsis *pOsis)
{
	bool success = false;

	uint8_t buffer[LEN_WRITE_CMD];
	uint32_t commandLen = buildWriteCommand(buffer, sizeof(buffer), OSIS_CONFIG_AREA_START, OSIS_CONFIG_AREA_END);
	if (commandLen > 0)
	{
		if (sendCommandAndWaitAck(handle, buffer, commandLen))
		{
			uint8_t *pDataPkt = (uint8_t *)malloc(MAX_LEN_DATA_PACKET);

			// Write mode engaged, now send the 16 byte register data

			commandLen = buildDataPacket(pDataPkt, MAX_LEN_DATA_PACKET, pOsis->reg, 16, WRITE);
			success = sendCommandAndWaitAck(handle, pDataPkt, commandLen);

			free(pDataPkt);
		}
	}

	return success;
}

// Poll with timeout for a pull request
// Input: handle; the framed protocol handle to use
// Input: timeoutMs; the timeout in mS, zero for no timeout
// Returns true on pull received, false otherwise
bool fpPollForPullRequest(const HANDLE handle, const uint16_t timeoutMs)
{
	bool pullRequested = false;

	uint32_t numBytes;
	uint8_t *pResp = blockingReadBytes(handle, timeoutMs, numBytes);
	if (pResp)
	{
		// The Pull Request message is defined to match the same structure as
		// responses to all messages, so the handleAcknowledgement() function
		// can be similarly used to validate the message structure.
		uint8_t error;
		if (handleAcknowledgement(pResp, numBytes, PULL_REQUEST, error))
		{
			if (0 == error)
			{
				pullRequested = true;
			}
		}

		free(pResp);
	}

	return pullRequested;
}

/**************************************************************
* Transport layer API
**************************************************************/

static void appendToBuffer(RxBufferEntry *pBuff, uint8_t byte)
{
	// Append new byte to buffer
	// Enough room to append to the current RX buffer?
	if (pBuff->bufferSize > pBuff->bytesInBuffer)
	{
#ifdef EXTRA_FP_DEBUG
		fprintf(stdout, "Appending byte 0x%02X\n", byte);
#endif
		pBuff->pData[pBuff->bytesInBuffer++] = byte;
	}
	else
	{
		fprintf(stderr, "RX overflow, discarding data\n");
	}
}

// Handle a received byte of data. This function is thread safe.
void fpReceiveByte(const HANDLE handle, const uint8_t byte)
{
	FpContext *pContext = getContext(handle);
	if (pContext)
	{
		bool newData = false;
		EnterCriticalSection(&pContext->rxBufferCriticalSection);
		if (pContext->pRxBufferEntry)
		{
			RxBufferEntry *pBuff = (pContext->pRxBufferEntry);

			// In framing mode?
			if (pContext->isFramed)
			{
				if (0 == pBuff->bytesInBuffer)
				{
					// Looking for the start of a packet - discard anything else
					if ((START_OF_HEADER == byte) || (START_OF_DATA == byte))
					{
						appendToBuffer(pBuff, byte);
					}
				}
				else
				{
					// Receiving a packet
					if (pBuff->bytesInBuffer < pBuff->bufferSize)
					{
						appendToBuffer(pBuff, byte);

						// Minimum frame size is 6 bytes
						if (pBuff->bytesInBuffer >= 6)
						{
							// Read length field
							uint8_t *pLen = &pBuff->pData[1];
							uint16_t length = (uint16_t)READ16BE(pLen);
							if ((length > MAX_FRAME_LENGTH) || (length == 0))
							{
								// Abort frame, start resync
								pBuff->bytesInBuffer = 0;
#ifdef EXTRA_FP_DEBUG
								fprintf(stdout, "Aborting bad frame\n");
#endif
							}
							else
							{
								// Length OK - have we received enough data to finish the frame?
								// (length doesn't include SOH, 2LEN bytes, SUM and ETX)
								if (pBuff->bytesInBuffer == (length + 5))
								{
									// If checksum is good and final byte is an end of packet, frame is received, otherwise sync is lost
									// Checksum is over (2LEN bytes + opCode + payload)
									uint8_t checkSum = fpcCalculateChecksum(pLen, (uint16_t)(length + 2));
									if ((checkSum == pLen[length + 2]) && (END_OF_PACKET == pLen[length + 3]))
									{
#ifdef EXTRA_FP_DEBUG
										fprintf(stdout, "Completed frame\n");
#endif
										// Discard any frame that has not yet been received
										if (pContext->pRxFrameEntry)
										{
											freeRxBufferEntry(pContext->pRxFrameEntry);
										}
										// Move the current RX buffer over to the frame entry
										pContext->pRxFrameEntry = pContext->pRxBufferEntry;
										// And allocate a new RX buffer
										pContext->pRxBufferEntry = allocateRxBufferEntry();
										newData = true;
									}
								}
							}
						}
					}
					else
					{
						// Lost sync - start looking for start of frame again
						pBuff->bytesInBuffer = 0;
#ifdef EXTRA_FP_DEBUG
						fprintf(stdout, "Packet too large, resyncing\n");
#endif
					}
				}
			}
			else
			{
				// Unframed, so signal every new byte
				appendToBuffer(pBuff, byte);
				newData = true;
			}
		}
		LeaveCriticalSection(&pContext->rxBufferCriticalSection);

		if (newData)
		{
			// Signal new data event
			SetEvent(pContext->hRxDataEvent);
		}
	}
}



