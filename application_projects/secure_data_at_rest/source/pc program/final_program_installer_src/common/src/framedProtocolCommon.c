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
