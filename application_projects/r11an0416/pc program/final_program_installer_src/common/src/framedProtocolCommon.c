/***********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/*
 *  Common code for the framed serial protocol between PC and embedded code.
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "framedProtocolCommon.h"

/*
 * Private functions
 */


/*
 * Public API
 */

// Calculate a checksum over the given byte array
// Input: pointer to buffer over which teh checksum is to be calculated
// Input: number of bytes in the buffer
// Returns: the calculated checksum
uint8_t fpcCalculateChecksum(const uint8_t *pData, const uint16_t numBytes)
{
    uint8_t checksum = 0;
    // Checksum is a one byte two's complement of the array sum
    for (uint32_t i = 0; i<numBytes; i++)
    {
        checksum = (uint8_t) (checksum + *pData++);
    }
    return (uint8_t) ((checksum ^ 0xFF) + 1);
}

// Build a message of the given startCode, opcode and optional data
// Function signature for serial transmit callback
// Input: pointer to buffer into which the message will be compiled
// Input: length of the buffer
// Input: the message start code to be used
// Input: the message command code to use
// Input: pointer to optional message payload data (NULL if none to be sent)
// Input: number of bytes of payload data to send (0..MAX_PACKET_PAYLOAD)
// Returns size of command or zero on failure.
uint16_t fpcBuildMessage(const uint8_t *pBuffer, const uint16_t bufferLength, const uint8_t startCode, const CommandCode opCode, const uint8_t *pData, const uint16_t numBytes)
{
    // Minimum message length
    uint32_t packetLength = 6;
    if (pData && (numBytes > 0))
    {
        packetLength += numBytes;
    }

    if ((packetLength <= bufferLength) && (numBytes <= MAX_PACKET_PAYLOAD))
    {
        uint8_t *pBuff = (uint8_t *)(uintptr_t)pBuffer;
        // Header
        *pBuff++  = startCode;
        *pBuff++ = (uint8_t) ((1 + numBytes) >> 8);
        *pBuff++ = (uint8_t) ((1 + numBytes) & 0xff);
        *pBuff++ = opCode;
        // optional payload
        if (numBytes > 0)
        {
            memcpy(pBuff, pData, numBytes);
            pBuff += numBytes;
        }
        // Checksum - (length bytes+opCode+payload) 2's complement
        *pBuff++ = fpcCalculateChecksum(pBuffer+1, (uint16_t)(numBytes + 3));
        // Tail
        *pBuff = END_OF_PACKET;
    }
    else
    {
        packetLength = 0;
    }

    return (uint16_t)packetLength;
}
