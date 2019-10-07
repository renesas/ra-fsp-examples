

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
 *  SREC parser
 */


#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "srec.h"

/*
 * Private data
 */

// The longest possible S record line is ASCII hex encoded: 
//   header + byte count + max 255 bytes [address+data+checksum] + [CR]LF + null termination
#define MAX_LINE_LENGTH  517

// SREC parser context
typedef struct SrecParserContext_tag {

    // The data buffer into which parsed record data is placed
    uint8_t *pBuffer;
    uint32_t bufferSize;

    // The address associated with the buffer
    uint32_t bufferStartAddress;

    // The lowest and highest offsets that have been written to the buffer
    uint32_t lowestOffset;
    uint32_t highestOffset;

    // Optional start address record
    bool     startAddressAvailable;
    uint32_t startAddress;

    // Optional header
    bool  headerAvailable;
    char *pHeader;

} SrecParserContext;

// SREC generator context
typedef struct SrecGeneratorContext_tag {

    // The user supplied callback for generated S records.
    SrecGeneratorCallback pGeneratorCallback;

    // Pointer to optional user supplied callback parameters
    void *pUserParams;

    // Address width
    SrecAddressWidth addressWidth;

    // Maximum byte count value to use
    uint8_t maxByteCount;

    // Buffer to contain one line of S record 
    char line[MAX_LINE_LENGTH];

    // Buffer write position
    uint32_t writePos;

} SrecGeneratorContext;


// Parsed S record
typedef struct SRecord_tag {

    // The type (0..9) or (255) for a line to be skipped (e.g. comment line)
    uint8_t type;

    // The address in host byte order
    uint32_t address;

    // Data (limited by 8 bit length field)
    uint8_t data[255];
    uint8_t dataLength;

} SRecord;

// Type to address field length (number of chars)
uint8_t typeToAddressChars[10] = {   4,   4,   6,   8,   0,   4,   6,   8,   6,   4 };
// Type to address field length (number of bytes)
uint8_t typeToAddressBytes[10] = {   2,   2,   3,   4,   0,   2,   3,   4,   3,   2 };
// Type to maximum length of data field (number of bytes) [255 - address width - checksum]
uint8_t typeToMaxDataBytes[10] = { 252, 252, 251, 250,   0, 252, 251, 250, 251, 252 };

/*
 * Private functions
 */

// Read a hex encoded single digit from the SREC string, returning true if parsed OK, false otherwise
static bool readDigit(char *pStr, uint8_t *pDigit)
{
    bool success = true;

    if ((*pStr >= '0') && (*pStr <= '9'))
    {
        *pDigit = *pStr - '0';
    }
    else if ((*pStr >= 'A') && (*pStr <= 'F'))
    {
        *pDigit = *pStr - 'A' + 10;
    }
    else if ((*pStr >= 'a') && (*pStr <= 'f'))
    {
        *pDigit = *pStr - 'a' + 10;
    }
    else
    {
        success = false;
    }

    return success;
}

// Read a hex encoded byte from the SREC string, accumulating the checksum.
// Returns true if parsed OK, false otherwise
static bool readByte(char *pStr, uint8_t *pByte, uint8_t *pCheckSum)
{
    bool success = false;
    uint8_t digit[2];

    if (readDigit(pStr, &digit[0]) && readDigit(pStr + 1, &digit[1]))
    {
        *pByte = (digit[0] << 4) + digit[1];
        *pCheckSum += *pByte;
        success = true;
    }

    return success;
}

// Parse a srec record, filling in the SRecord structure.
// Returns true if record parsed OK, false otherwise
static bool parseRecord(const char *recordStr, SRecord *pSRecord)
{
    bool success = false;
    char *pRecord = (char *)recordStr;
    uint8_t checkSum = 0;

    // Is is a blank line (new line and/or line feed)?
    if (strlen(pRecord) < 3)
    {
        pSRecord->type = 255; // to skip this record
        success = true;
    }
    // Is it a comment line (beginning with '//')
    else if (strncmp(pRecord, "//", 2) == 0)
    {
        pSRecord->type = 255; // to skip this record
        success = true;
    }
    else if (strlen(pRecord) >= 10)
    {
        // All records start with "Sxbc" where x=type (0..9) and bc=hex encoded byte count
        // and followed by a minimum 4 digit address field and 2 digit checksum

        int type, byteCount;
        int res = sscanf_s(pRecord, "S%01d%02X", &type, &byteCount);
        if ((2 == res) && (type < 10) && (byteCount <= 255))
        {
            success = true;
            pRecord += 4;
            checkSum = byteCount;
            pSRecord->type = (uint8_t)type;

            // Parse address field
            pSRecord->address = 0;
            int shift = typeToAddressBytes[type] - 1;
            while (success && (shift >= 0))
            {
                uint8_t byte;
                if (readByte(pRecord, &byte, &checkSum))
                {
                    pSRecord->address += byte << (8 * shift);
                    pRecord += 2;
                }
                else
                {
                    success = false;
                }

                shift--;
            }

            if (success)
            {
                // Now extract the data
                pSRecord->dataLength = byteCount - typeToAddressBytes[type] - 1;
                int i = 0;
                while(success && (i <pSRecord->dataLength))
                {
                    if (readByte(pRecord, &pSRecord->data[i], &checkSum))
                    {
                        pRecord += 2;
                    }
                    else
                    {
                        success = false;
                    }

                    i++;
                }

                // And finally the checksum
                if (success)
                {
                    uint8_t calculatedSum = checkSum;
                    uint8_t recordSum;
                    if (!readByte(pRecord, &recordSum, &checkSum) || ((calculatedSum ^ 0xFF) !=  recordSum))
                    {
                        printf("Expected checksum=0x%x, actual checksum=0x%x\n", recordSum, calculatedSum ^ 0xFF);
                        success = false;
                    }
                }
            }
        }
    }

    return success;
}

// Attempt to store the SRecord in the buffer.
// Returns true if stored OK, false otherwise
static bool storeData(SrecParserContext *pContext, SRecord record)
{
    bool success = false;
    uint32_t recordOffset = 0;

    // Its valid for the buffer to be NULL if the record is being parsed to
    // extract the metadata only.
    if (pContext->pBuffer)
    {
        // Is the record address and length within range?
        if (record.address >= pContext->bufferStartAddress)
        {
            recordOffset = record.address - pContext->bufferStartAddress;

            if ((recordOffset + record.dataLength) <= pContext->bufferSize)
            {
                // Store the data
                memcpy(pContext->pBuffer + recordOffset, record.data, record.dataLength);
                success = true;
            }
        }
    }
    else
    {
        // just collecting the info
        recordOffset = record.address - pContext->bufferStartAddress;
        success = true;
    }

    if (success)
    {
        // Update address range info
        if (recordOffset < pContext->lowestOffset)
        {
            pContext->lowestOffset = recordOffset;
        }
        if ((recordOffset + record.dataLength - 1) > pContext->highestOffset)
        {
            pContext->highestOffset = recordOffset + record.dataLength - 1;
        }
    }

    return success;
}

// Store the header
static bool storeHeader(SrecParserContext *pContext, SRecord record)
{
    // Header should be a null terminated string
    if (record.dataLength > 0)
    {
        // Data may not be null terminated - allow for one if necessary
        pContext->pHeader = (char *)malloc(record.dataLength + 1);
        memcpy(pContext->pHeader, record.data, record.dataLength);
        // Ensure null termination
        pContext->pHeader[record.dataLength] = 0;
        pContext->headerAvailable = true;
    }

    return pContext->headerAvailable = true;
}

// Store the start address
static bool storeStartAddress(SrecParserContext *pContext, SRecord record)
{
    pContext->startAddressAvailable = true;
    pContext->startAddress = record.address;
    return true;
}

static const char toHex[] = { "0123456789ABCDEF" };

// Append a byte in ASCII hex to the generator buffer
// Returns byte value for checksum accumulation.
static uint8_t appendByte(SrecGeneratorContext *pContext, const uint8_t byte)
{
    pContext->line[pContext->writePos++] = toHex[(byte >> 4) & 0x0F];
    pContext->line[pContext->writePos++] = toHex[(byte & 0x0F)];
    return byte;
}

// Write a complete S record to the generator context and call the generator callback with the result
// For records with no data field, set pData to NULL and dataLength to 0.
// Returns true if written OK, false otherwise
static bool writeSRecord(SrecGeneratorContext *pContext, const uint8_t type, const uint32_t address, const uint8_t *pData, const uint8_t dataLength)
{
    bool success = false;

    if ((type <= 9) && (dataLength <= typeToMaxDataBytes[type]))
    {
        // Address within range? 
        SrecAddressWidth width = (type == 0) ? WIDTH_16_BIT : pContext->addressWidth;
        switch (width)
        {
            case WIDTH_16_BIT:
                success = (address <= 0xFFFF) ? true : false;
                break;

            case WIDTH_24_BIT:
                success = (address <= 0xFFFFFF) ? true : false;
                break;

            case WIDTH_32_BIT:
                success = (address <= 0xFFFFFFFF) ? true : false;
                break;

            default:
                break;
        }

        pContext->writePos = 0;
        pContext->line[pContext->writePos++] = 'S';
        pContext->line[pContext->writePos++] = '0' + type;

        // Byte count: address bytes + data + checksum
        uint8_t checksum = appendByte(pContext, typeToAddressBytes[type] + dataLength + 1);

        switch (typeToAddressBytes[type])
        {
            case 2:
                checksum += appendByte(pContext, (address >> 8) & 0xFF);
                checksum += appendByte(pContext, address & 0xFF);
                break;

            case 3:
                checksum += appendByte(pContext, (address >> 16) & 0xFF);
                checksum += appendByte(pContext, (address >> 8) & 0xFF);
                checksum += appendByte(pContext, address & 0xFF);
                break;

            case 4:
                checksum += appendByte(pContext, (address >> 24) & 0xFF);
                checksum += appendByte(pContext, (address >> 16) & 0xFF);
                checksum += appendByte(pContext, (address >> 8) & 0xFF);
                checksum += appendByte(pContext, address & 0xFF);
                break;

            default:
                success = false;
                break;
        }

        if (success)
        {
            if (pData && (dataLength > 0))
            {
                // Add data
                for (int i = 0; i < dataLength; i++)
                {
                    checksum += appendByte(pContext, pData[i]);
                }
            }

            // Add checksum, end of line and null termination
            appendByte(pContext, checksum ^ 0xFF);
            pContext->line[pContext->writePos++] = '\n';
            pContext->line[pContext->writePos] = 0;

            // Pass result back via the callback
            success = pContext->pGeneratorCallback(pContext->line, pContext->pUserParams);
        }
    }

    return success;
}



/*
 * Public API
 */

/*
 * Parsing S Records
 */

// Create a srec read parser. If pBuffer==NULL then when the file is parsed the metadata will
// be extracted (lowest/highest addresses etc) but the data will not be saved.
// Input: pBuffer; pointer to the buffer into which the parsed data will be placed
// Input: bufferSize; the length of the buffer
// Input: bufferBaseAddress; the target address to be associated with the start of the buffer
// Returns: the srec parser handle if open succeeeded, INVALID_HANDLE_VALUE otherwise.
HANDLE srecCreateParser(const uint8_t *pBuffer, const uint32_t bufferSize, const uint32_t bufferBaseAddress)
{
    SrecParserContext *pContext = (SrecParserContext *)malloc(sizeof(SrecParserContext));

    // Save buffer context
    pContext->pBuffer = (uint8_t *)pBuffer;
    pContext->bufferSize = bufferSize;
    pContext->bufferStartAddress = bufferBaseAddress;

    // Initialise internal parser context
    if (pBuffer)
    {
        pContext->lowestOffset = bufferSize;
    }
    else
    {
        // Information gathering exercise only
        pContext->lowestOffset = -1;
    }
    pContext->highestOffset = 0;

    // Optional records
    pContext->startAddressAvailable = false;
    pContext->startAddress = 0;

    pContext->headerAvailable = false;
    pContext->pHeader = NULL;

    return pContext;
}

// Parse a null terminated SREC line.
// Input: handle; srec parser handle
// Input: pLine; pointer to the null terminated SREC string
// Returns: true if SREC line parsed OK, false otherwise
bool srecParse(const HANDLE handle, const char *pLine)
{
    bool success = false;
    SrecParserContext *pContext = (SrecParserContext *)handle;
    SRecord record;

    if (parseRecord(pLine, &record))
    {
        switch (record.type)
        {
            case 0:
                success = storeHeader(pContext, record);
                break;

            case 1:
                success = storeData(pContext, record);
                break;

            case 2:
                success = storeData(pContext, record);
                break;

            case 3:
                success = storeData(pContext, record);
                break;

            case 7:
                success = storeStartAddress(pContext, record);
                break;

            case 8:
                success = storeStartAddress(pContext, record);
                break;

            case 9:
                success = storeStartAddress(pContext, record);
                break;

            case 255:
                // A line to be skipped (e.g. a comment line)
                success = true;
                break;

            default:
                fprintf(stderr, "Unsupported SREC type %d\n", record.type);
                break;
        }
    }

    return success;
}


// Getters for access to parsed records. These getters use the following style:
// - For types that support NULL, the getter will return the value or NULL if not available.
// - For types that do not support NULL, the getter will be passed a pointer to the variable to hold 
//   the result and return a bool indicating if the value is available. 

// Header (memory is transferred to the caller and must be freed when no longer required)
char *srecGetHeader(const HANDLE handle)
{
    SrecParserContext *pContext = (SrecParserContext *)handle;
    char *pHeader = NULL;

    if (pContext->headerAvailable)
    {
        uint32_t headerLength = (uint32_t)strlen(pContext->pHeader) + 1; // allow for null termination
        pHeader = (char *)malloc(headerLength);
        memcpy(pHeader, pContext->pHeader, headerLength);
    }

    return pHeader;
}

// Start address.
bool srecGetStartAddress(const HANDLE handle, uint32_t *pStartAddress)
{
    bool success = false;
    SrecParserContext *pContext = (SrecParserContext *)handle;

    if (pContext->startAddressAvailable)
    {
       *pStartAddress = pContext->startAddress;
        success = true;
    }

    return success;
}

// Lowest buffer offset parsed.
bool srecGetLowestOffset(const HANDLE handle, uint32_t *pLowestOffset)
{
    bool success = false;
    SrecParserContext *pContext = (SrecParserContext *)handle;

    // Any data parsed?
    if (pContext->lowestOffset < pContext->highestOffset)
    {
       *pLowestOffset = pContext->lowestOffset;
        success = true;
    }

    return success;
}

// Highest buffer offset parsed.
bool srecGetHighestOffset(const HANDLE handle, uint32_t *pHighestOffset)
{
    bool success = false;
    SrecParserContext *pContext = (SrecParserContext *)handle;

    // Any data parsed?
    if (pContext->lowestOffset < pContext->highestOffset)
    {
       *pHighestOffset = pContext->highestOffset;
        success = true;
    }

    return success;
}

// Lowest address parsed (equivalent to bufferBaseAddress + lowestOffset).
bool srecGetLowestAddress(const HANDLE handle, uint32_t *pLowestAddress)
{
    bool success = false;
    SrecParserContext *pContext = (SrecParserContext *)handle;

    // Any data parsed?
    if (pContext->lowestOffset < pContext->highestOffset)
    {
       *pLowestAddress = pContext->bufferStartAddress + pContext->lowestOffset;
        success = true;
    }

    return success;
}

// Highest address parsed (equivalent to bufferBaseAddress + highestOffset).
bool srecGetHighestAddress(const HANDLE handle, uint32_t *pHighestAddress)
{
    bool success = false;
    SrecParserContext *pContext = (SrecParserContext *)handle;

    // Any data parsed?
    if (pContext->lowestOffset < pContext->highestOffset)
    {
       *pHighestAddress = pContext->bufferStartAddress + pContext->highestOffset;
        success = true;
    }

    return success;
}

// Destroy a srec parser
// Input: handle; srec parser handle
void srecDestroyParser(const HANDLE handle)
{
    SrecParserContext *pContext = (SrecParserContext *)handle;
    if (pContext->pHeader)
    {
        free(pContext->pHeader);
    }
    free(pContext);
}

/*
 * Creating S Records
 */

// Create a srec generator.
// Input: pGeneratorCallback; user suplied callback for generated S records
// Input: pUserParams; pointer to user supplied parameter(s). Can be null if no user context required.
// Input: addressWidth; the width of the address fields used when generating S records.
// Returns: the srec generator handle if open succeeeded, INVALID_HANDLE_VALUE otherwise.
HANDLE srecCreateGenerator(SrecGeneratorCallback pGeneratorCallback, void *pUserParams, const SrecAddressWidth addressWidth)
{
    SrecGeneratorContext *pContext = (SrecGeneratorContext *)malloc(sizeof(SrecGeneratorContext));

    pContext->pGeneratorCallback = pGeneratorCallback;
    pContext->pUserParams = pUserParams;
    pContext->addressWidth = addressWidth;
    pContext->maxByteCount = 255;
    memset(pContext->line, 0, sizeof(pContext->line));

    return pContext;
}

// Optionally set the maximum data S record byte count value. This can be used to limit the
// data S record length for interoperability if required. If not set, the srec generator will
// create data records up to the maximum 255 byte count value.
// Input: handle; srec generator handle
// Input: maxbyteCount; maximum byte count value to use
void srecGeneratorSetMaxByteCount(const HANDLE handle, const uint8_t maxByteCount)
{
    SrecGeneratorContext *pContext = (SrecGeneratorContext *)handle;
    pContext->maxByteCount = maxByteCount;
}

// Generate a header record. This is used to start the S records.
// Input: handle; srec generator handle
// Input: pStr; pointer to the header string to encode
// Returns: true if S record generated OK, false otherwise.
bool srecGenerateHeader(const HANDLE handle, const char *pStr)
{
    SrecGeneratorContext *pContext = (SrecGeneratorContext *)handle;
    bool success = false;

    // Header always uses a 16bit address field of zero.
    if (strlen(pStr) <= typeToMaxDataBytes[0])
    {
        success = writeSRecord(pContext, 0, 0, (const uint8_t *)pStr, (const uint8_t)strlen(pStr));
    }

    return success;
}

// Given a block of memory, create S records of that blocks data. Each line (line+null terminated)
// is returned via the SrecWriterCallback.
// Input: handle; srec generator handle
// Input: pBuffer; pointer to the buffer conatining the source data
// Input: bufferSize; the length of the buffer
// Input: bufferBaseAddress; the target address to be associated with the start of the buffer
// Returns: true if S records generated OK, false otherwise.
bool srecGenerateData(const HANDLE handle, const uint8_t *pBuffer, const uint32_t numBytes, const uint32_t bufferBaseAddress)
{
    SrecGeneratorContext *pContext = (SrecGeneratorContext *)handle;
    bool success = false;

    // Type
    uint8_t type = 255; // Linter
    switch (pContext->addressWidth)
    {
        case WIDTH_16_BIT:
            type = 1;
            success = true;
            break;

        case WIDTH_24_BIT:
            type = 2;
            success = true;
            break;

        case WIDTH_32_BIT:
            type = 3;
            success = true;
            break;

        default:
            break;
    }

    if (success)
    {
        // Allow for capped maxByteCount less than 255
        uint32_t maxDataBytes;
        if (typeToMaxDataBytes[type] > (255 - pContext->maxByteCount))
        {
            maxDataBytes = typeToMaxDataBytes[type] - (255 - pContext->maxByteCount);
        }
        else
        {
            maxDataBytes = 1; // Enforce sensible minimum
        }

        // Write out the data S records
        uint32_t bytesRemaining = numBytes;
        uint32_t offset = 0;

        while (success && (bytesRemaining > 0))
        {
            uint32_t bytesToEncode = (bytesRemaining > maxDataBytes) ? maxDataBytes : bytesRemaining;
            success = writeSRecord(pContext, type, bufferBaseAddress + offset, pBuffer + offset, bytesToEncode);
            bytesRemaining -= bytesToEncode;
            offset += bytesToEncode;
        }
    }

    return success;
}

// Generate a start address record. This is used to terminate the S records.
// Input: handle; srec generator handle
// Input: startAddress; the start address
// Returns: true if S record generated OK, false otherwise.
bool srecGenerateStartAddress(const HANDLE handle, const uint32_t startAddress)
{
    SrecGeneratorContext *pContext = (SrecGeneratorContext *)handle;
    bool success = false;

    switch (pContext->addressWidth)
    {
        case WIDTH_16_BIT:
            success = writeSRecord(pContext, 9, startAddress, NULL, 0);
            break;

        case WIDTH_24_BIT:
            success = writeSRecord(pContext, 8, startAddress, NULL, 0);
            break;

        case WIDTH_32_BIT:
            success = writeSRecord(pContext, 7, startAddress, NULL, 0);
            break;

        default:
            break;
    }

    return success;
}

// Destroy a srec generator
// Input: handle; srec generator handle
void srecDestroyGenerator(const HANDLE handle)
{
    SrecGeneratorContext *pContext = (SrecGeneratorContext *)handle;
    free(pContext);
}

