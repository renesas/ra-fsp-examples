

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

#include <windows.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "framedProtocolHost.h"
#include "framedProtocolCommon.h"
#include "serial.h"

/**************************************************************
* Private data
**************************************************************/
//Types of Comm protocol connection
typedef enum SpConnectionType_tag {
	SERIAL,
	INVALID_CONNECTION_TYPE
}FpConnectionType;

//Context data for serial connections
typedef struct SpSerialContext_tag {
	//Serial port number
	uint8_t portNumber;
}FpSerialContext;

// A single RX buffer structure
typedef struct RxBufferEntry_tag {
	// Receive data buffer
	uint8_t *pData;
	uint32_t bufferSize;
	uint32_t bytesInBuffer;
} RxBufferEntry;

typedef struct FpContext_tag {
	//If this context is in use
	bool inUse;

	//Handle for this context
	HANDLE handle;

	//Type of connection
	FpConnectionType type;

	// Connection specific context data
	union {
		FpSerialContext serial;
	} u;

	// Transport layer connection handle
	HANDLE hComm;

	// Transport layer callback to send data
	FpBlockingDataTransmit pBlockingTransmitFn;

	// Receive data buffer - single buffer entry for now
	RxBufferEntry *pRxBufferEntry;

	// Critical section object for managing access to the RX buffer between threads
	CRITICAL_SECTION rxBufferCriticalSection;

	//Receive data available event handle
	HANDLE hRxDataEvent;

	// In framing mode (normal apart from connection negotiation phase)?
	bool isFramed;

	// Complete receive frame (only in framing mode)
	// Only one frame deap since thats all the current protocol requires.
	RxBufferEntry *pRxFrameEntry;
}FpContext;


// Critical section object for managing creation and destroying context between threads
static CRITICAL_SECTION fpContextAllocCriticalSection;

//The serial protocol connection context data
static FpContext fpContext[MAX_FP_CONNECTIONS] = { 0 };

// Flag to trap uninitilised module
static bool fpInitialised = false;

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

// Allocate an RX buffer entry - enough for the largest framed packet
static RxBufferEntry *allocateRxBufferEntry(void)
{
	RxBufferEntry *pBuff = (RxBufferEntry *)malloc(sizeof(RxBufferEntry));

	pBuff->pData = (uint8_t *)malloc(MAX_LEN_DATA_PACKET);
	pBuff->bufferSize = MAX_LEN_DATA_PACKET;
	pBuff->bytesInBuffer = 0;

	return pBuff;
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

//Build a Generate Key command
static uint32_t buildGenKeyCommand(const uint8_t *pBuffer, const uint32_t bufferLength)
{
	return fpcBuildMessage(pBuffer, bufferLength, START_OF_HEADER, WRAPPER_KEY_REQUEST, NULL, 0);
}

// Build WRAPPED_KEY_CERT_RESP message
static uint32_t buildSendDevCertCommand(const uint8_t *pBuffer, const uint32_t bufferLength, const uint8_t *theData, uint16_t dataLen)
{
	return fpcBuildMessage(pBuffer, bufferLength, START_OF_HEADER, WRAPPED_KEY_CERT_PROGRAM, theData, dataLen);
}

//Returns true if expected response received, false otherwise
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
						success = true;
					else
						fprintf(stderr, "Error code 0x%02X for command 0x%02X\n", error, pCommand[3]);
				}

				free(pResp);
			}
			else
				fprintf(stderr, "No response for command 0x%02x\n", pCommand[3]);
		}
		else
			fprintf(stderr, "Failed to send command 0x%02X\n", pCommand[3]);
	}

	return success;
}

/*
Send a command and wait a short time for an acknowledgement with payload data.
Returns pointer to malloc'd payload data if expected response received, NULL otherwise
*/
static uint8_t *sendCommandAndWaitAckWithPayload(const HANDLE handle, const uint8_t *pCommand, const uint32_t commandLength, uint32_t *pPayloadLength, const uint32_t timeoutmS = 2000)
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
						//Its a valid ACK for the command, now extract the payload if present
						uint8_t *pLen = &pResp[1];
						uint16_t length = READ16BE(pLen);

						//length includes 1 byte response code
						if (length > 1)
						{
							*pPayloadLength = length - 1;
							pPayload = (uint8_t *)malloc(*pPayloadLength);
							memcpy(pPayload, &pResp[4], *pPayloadLength);
						}
					}
					else
					{
						fprintf(stderr, "Error code 0x%02X for command 0x%02X\n", error, pCommand[3]);
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
				fprintf(stderr, "No response for command 0x%02X\n", pCommand[3]);
			}
		}
		else
		{
			fprintf(stderr, "Failed to send command 0x%02X\n", pCommand[3]);
		}
	}

	return pPayload;
}

// Build WRAPPED_KEY_CHALLENGE_RESP message
static uint32_t buildSendChallengeResponseMessage(const uint8_t *pBuffer, const uint32_t bufferLength, const uint8_t *theData, uint16_t dataLen)
{
	return fpcBuildMessage(pBuffer, bufferLength, START_OF_HEADER, WRAPPED_KEY_CHALLENGE_RESP, theData, dataLen);
}

uint8_t* fpSendChallengeResponseMessage(const HANDLE handle, const uint8_t *theHash, const uint16_t data_len)
{
	bool success = true;
	uint8_t buffer[MAX_LEN_DATA_PACKET];
	uint32_t payloadLen;
	uint32_t commandLen = buildSendChallengeResponseMessage(buffer, sizeof(buffer), theHash, data_len);
	uint8_t *payload = NULL;
	if (commandLen > 0)
	{
		payload = sendCommandAndWaitAckWithPayload(handle, buffer, commandLen, &payloadLen);
	}
	return payload;
}

uint8_t* fpGetDeviceKey(const HANDLE handle, uint32_t *pKeyLength)
{
	uint8_t *pKey = NULL;
	*pKeyLength = 0;
	uint8_t buffer[LEN_GEN_KEY_CMD];

	uint32_t commandLen = buildGenKeyCommand(buffer, sizeof(buffer));
	if (commandLen > 0)
	{
		pKey = sendCommandAndWaitAckWithPayload(handle, buffer, commandLen, pKeyLength);
	}

	return pKey;
}

bool fpSendDeviceCert(const HANDLE handle, const uint8_t *theCert, const uint16_t data_len)
{
	bool retVal = false;
	uint8_t buffer[MAX_LEN_DATA_PACKET];
	uint32_t commandLen = buildSendDevCertCommand(buffer, sizeof(buffer), theCert, data_len);
	if (commandLen > 0)
	{
		retVal = sendCommandAndWaitAck(handle, buffer, commandLen);
	}

	return retVal;
}

void fpInitialize(void)
{
	InitializeCriticalSection(&fpContextAllocCriticalSection);
	fpInitialised = true;
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

//Create the Rx buffer
static void createRxBuffer(FpContext *pContext)
{
	// Currently a single buffer but designed to be easily upgraded to a buffer stack
	pContext->pRxBufferEntry = allocateRxBufferEntry();
	// Sharing of the RX buffer between user thread and RX thread is controlled by critical section mutex
	InitializeCriticalSection(&pContext->rxBufferCriticalSection);
	// No RX frames yet
	pContext->pRxFrameEntry = NULL;
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
	LeaveCriticalSection(&fpContextAllocCriticalSection);
}

//Allocate a context, returning NULL if none available
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

HANDLE OpenSerialPort(const uint8_t portNumber, const uint32_t baudrate)
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

			pContext->hComm = serialOpenCommPort(pContext->handle, portNumber, baudrate);
			if (INVALID_HANDLE_VALUE == pContext->hComm)
			{
				fprintf(stderr, "Failed to open serial port at %d baud\n", baudrate);
				freeContext(pContext);
				pContext = NULL;
			}

			if(pContext)
				handle = pContext->handle;
		}
		else
		{
			fprintf(stderr, "Failed to allocate Serial Comms protocol context\n");
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

						//Minimum frame size is 6 bytes
						if (pBuff->bytesInBuffer >= MIN_FRAME_SIZE)
						{
							uint8_t *pLen = &pBuff->pData[1];
							uint16_t length = (uint16_t)READ16BE(pLen);
							if ((length > MAX_FRAME_LENGTH) || (length == 0))
							{
								//Abor frame, start resync
								pBuff->bytesInBuffer = 0;
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
