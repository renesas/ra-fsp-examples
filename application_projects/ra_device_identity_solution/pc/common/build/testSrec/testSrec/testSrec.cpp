

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
 *  SREC test application
 */


#include <Windows.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include "stdafx.h"

#include "srec.h"
#include "srec_file.h"

// Test buffer size - 4MByte, equivalent to the chip
#define BUFFER_SIZE 0x400000

// The base address of the test buffer
#define BUFFER_BASE_ADDRESS 0x0

// Buffer in which to store the S records
typedef struct SrecBuffer_tag {

    // Size of buffer
    uint32_t bufferSize;

    // Pointer to start of buffer
    char *pBuffer;

    // Write position in the buffer
    uint32_t writePos;

} SrecBuffer;

// Generator callback for testGenerationAndParse()
bool testAndParseGeneratorCallback(const char *pSrec, void *pUserParams)
{
    bool success = false;
    SrecBuffer *pSrecBuffer = (SrecBuffer *)pUserParams;
    int srecLength = (int)strlen(pSrec);

    if ((pSrecBuffer->writePos + srecLength) <= pSrecBuffer->bufferSize)
    {
        memcpy(pSrecBuffer->pBuffer + pSrecBuffer->writePos, pSrec, srecLength);
        pSrecBuffer->writePos += srecLength;
        success = true;
    }
    else
    {
        fprintf(stdout, "=> error srec buffer overflow\n");
    }

    return success;
}

// Generate S Records then parse and compare memory block
#define TEST_BLOCK_SIZE    1000
bool testGenerationAndParse(const SrecAddressWidth addressWidth, const uint32_t blockAddress, const uint32_t startAddress, const uint8_t maxByteCount)
{
    bool success = false;
    int i;

    uint8_t addressWidthToSize[NUM_ADDRESS_WIDTHS] = { 16, 24, 32 };

    fprintf(stdout, "Testing address width %d, max byte count %d:\n", addressWidthToSize[addressWidth], maxByteCount);

    // Source and destination buffers
    uint8_t *pSrc  = (uint8_t *)malloc(TEST_BLOCK_SIZE);
    uint8_t *pDest = (uint8_t *)malloc(TEST_BLOCK_SIZE);

    // Seed the source with a pattern, destination with the inverse
    for (i = 0; i < TEST_BLOCK_SIZE; i++)
    {
        pSrc[i]  = rand() & 0xFF;
        pDest[i] = pSrc[i] ^ 0xFF;
    }

    // Buffer in which to store the S records
    SrecBuffer srecBuffer;
    srecBuffer.bufferSize = 4 * TEST_BLOCK_SIZE; // Crude allocation enough for S records
    srecBuffer.pBuffer  = (char *)malloc(srecBuffer.bufferSize);
    srecBuffer.writePos = 0;

    // Create a generator and build SRECs for the source block
    HANDLE hSrec = srecCreateGenerator(testAndParseGeneratorCallback, &srecBuffer, addressWidth);
    if (INVALID_HANDLE_VALUE != hSrec)
    {
        // Only set max byte count if its not 255, so it can be tested unset
        if (255 != maxByteCount)
        {
            srecGeneratorSetMaxByteCount(hSrec, maxByteCount);
        }
        success = srecGenerateHeader(hSrec, "testGenerationAndParse");
        if (success)
        {
            success = srecGenerateData(hSrec, pSrc, TEST_BLOCK_SIZE, blockAddress);
            if (success)
            {
                success = srecGenerateStartAddress(hSrec, startAddress);
                if (!success)
                {
                    fprintf(stdout, "=> error generating start address\n");
                }
            }
            else
            {
                fprintf(stdout, "=> error generating data\n");
            }
        }
        else
        {
            fprintf(stdout, "=> error generating header\n");
        }

        srecDestroyGenerator(hSrec);
    }
    else
    {
        fprintf(stdout, "=> error creating generator\n");
    }

    // Now create a parser and read into destination buffer
    if (success)
    {
        hSrec = srecCreateParser(pDest, TEST_BLOCK_SIZE, blockAddress);
        if (INVALID_HANDLE_VALUE != hSrec)
        {
            uint32_t parsedStartAddress, lowestOffset, highestOffset;
            char *pHeader = NULL;

            bool parseOk = true;
            char *pLastChar = srecBuffer.pBuffer + srecBuffer.writePos;
            char *pNextSrec = NULL;
            char *pSrec = strtok_s(srecBuffer.pBuffer, "\n", &pNextSrec);
            while (parseOk && (NULL != pSrec) && (pNextSrec <= pLastChar))
            {
                parseOk = srecParse(hSrec, pSrec);
                if (parseOk)
                {
                    pSrec = strtok_s(NULL, "\n", &pNextSrec);
                }
                else
                {
                    success = false;
                    fprintf(stdout, "=> error parsing S record: %s\n", pSrec);
                }
            }

            if (parseOk)
            {
                // Parsing completed, extract the info we need
                pHeader = srecGetHeader(hSrec);
                if (pHeader && srecGetLowestOffset(hSrec, &lowestOffset) && srecGetHighestOffset(hSrec, &highestOffset) && srecGetStartAddress(hSrec, &parsedStartAddress))
                {
                    // Check that the parsed parameters match the original
                    if (parsedStartAddress != startAddress)
                    {
                        success = false;
                        fprintf(stdout, "=> error parsed startAddress 0x%X != startAddress 0x%X\n", parsedStartAddress, startAddress);
                    }
                    else if (lowestOffset != 0)
                    {
                        success = false;
                        fprintf(stdout, "=> error parsed lowestOffset 0x%X != lowestOffset 0x%X\n", lowestOffset, 0);
                    }
                    else if (highestOffset != TEST_BLOCK_SIZE - 1)
                    {
                        success = false;
                        fprintf(stdout, "=> error parsed highestOffset 0x%X != highestOffset 0x%X\n", highestOffset, TEST_BLOCK_SIZE - 1);
                    }
                }
                else
                {
                    success = false;
                    fprintf(stdout, "=> error extracting required info\n");
                }
            }

            if (pHeader)
            {
                free(pHeader);
            }

            // Done with parser
            srecDestroyParser(hSrec);
        }
        else
        {
            fprintf(stdout, "=> error creating parser\n");
        }
    }

    // Test passed if source and destination blocks now have the same content
    if (success)
    {
        success = (memcmp(pSrc, pDest, TEST_BLOCK_SIZE) == 0) ? true : false;
        if (!success)
        {
            fprintf(stdout, "=> error: source and destination data does not match\n");
        }
    }

    free(srecBuffer.pBuffer);
    free(pSrc);
    free(pDest);

    if (success)
    {
        fprintf(stdout, "=> Pass\n");
    }
    else
    {
        fprintf(stdout, "=> Fail\n");
    }

    return success;
}

// Test reading an external file
bool testReadSrecFile(const char *filename)
{
    bool success = false;

    fprintf(stdout, "Testing reading external srec file:\n");

    // Allocate a 4MByte buffer - large enough for our test data
    uint8_t *pBuffer = (uint8_t *)malloc(BUFFER_SIZE);

    // Parse the test data file (not generated by the srec code)
    HANDLE hSrec = srecCreateParser(pBuffer, BUFFER_SIZE, BUFFER_BASE_ADDRESS);
    if (hSrec)
    {
        uint32_t startAddress, lowestOffset, highestOffset;
        char *pHeader = NULL;

        // Parse the test file
        FILE *pFile;
        if (!fopen_s(&pFile, filename, "r"))
        {
            char line[1024];
            bool parseOk = true;
            while (parseOk && fgets(line, _countof(line), pFile))
            {
                parseOk = srecParse(hSrec, line);
            }

            if (!feof(pFile))
            {
                fprintf(stderr, "Failed to read file %s\n", filename);
                parseOk = false;
            }
            fclose(pFile);

            if (parseOk)
            {
                // Parsing completed, extract the expected info
                pHeader = srecGetHeader(hSrec);
                if (pHeader && srecGetLowestOffset(hSrec, &lowestOffset) && srecGetHighestOffset(hSrec, &highestOffset) && srecGetStartAddress(hSrec, &startAddress))
                {
                    success = true;
                }
            }
        }

        // Done with parser
        srecDestroyParser(hSrec);

        // TODO validate extracted info against the test file

        if (pHeader)
        {
            free(pHeader);
        }
    }

    if (success)
    {
        fprintf(stdout, "=> Pass\n");
    }
    else
    {
        fprintf(stdout, "=> Fail\n");
    }

    return success;
}

// Test the srec_file higher level API
#define TEST_DATA_SIZE 0x10000
#define TEST_START_ADDR 0x1234
bool testSrecFileApi(const char *filename)
{
    bool success = false;

    fprintf(stdout, "Testing srec file API:\n");

    // Build a block of test data
    uint8_t *pTestData = (uint8_t *)calloc(0x10000, sizeof(uint8_t));
    // Fill with a random data pattern
    for (int i = 0; i < TEST_DATA_SIZE; i++)
    {
        pTestData[i] = rand() & 0xFF;
    }

    // Create a SREC writer and save the test data to a file.
    HANDLE hWriter = srecWriteOpenFile(filename);
    if (INVALID_HANDLE_VALUE != hWriter)
    {
        // write a comment to the file
        if (srecWriteCommentString(hWriter, "This is a test data file"))
        {
            // followed by the test data
            SrecWriterImage wrImage = { pTestData, TEST_DATA_SIZE, TEST_START_ADDR };
            success = srecWriteSrecImageAndClose(hWriter, wrImage);
        }
    }

    // If write phase worked, try to read the file back and compare the data read
    if (success)
    {
        success = false;

        SrecReaderImage rdImage;
        if (srecReadFile(filename, &rdImage))
        {
            uint32_t imageSize = rdImage.highestAddr - rdImage.lowestAddr + 1;
            if (rdImage.bufferSize >= (uint32_t)TEST_DATA_SIZE)
            {
                if (imageSize == (uint32_t)TEST_DATA_SIZE)
                {
                    if (rdImage.lowestAddr == (uint32_t)TEST_START_ADDR)
                    {
                        if (!memcmp(pTestData, rdImage.pBuffer, TEST_DATA_SIZE))
                        {
                            success = true;
                        }
                        else
                        {
                            fprintf(stderr, "Read image data mismatch\n");
                        }
                    }
                    else
                    {
                        fprintf(stderr, "Incorrect read image start address: 0x%X\n", rdImage.lowestAddr);
                    }
                }
                else
                {
                    fprintf(stderr, "Incorrect read image size: 0x%X\n", imageSize);
                }
            }
            else
            {
                fprintf(stderr, "Incorrect read buffer size: 0x%X\n", rdImage.bufferSize);
            }

            free(rdImage.pBuffer);
        }
        else
        {
            fprintf(stderr, "Failed to read file %s\n", filename);
        }
    }

    free(pTestData);

    if (success)
    {
        fprintf(stdout, "=> Pass\n");
    }
    else
    {
        fprintf(stdout, "=> Fail\n");
    }

    return success;
}

int main(const int argc, const char *argv[])
{
    bool success = false;
    
    if (argc == 3)
    {
        // First, test local generation and reading back again
        if (testGenerationAndParse(WIDTH_16_BIT, 0x1234,     0x5678, 255) &&
            testGenerationAndParse(WIDTH_24_BIT, 0x123456,   0x56789A, 255) &&
            testGenerationAndParse(WIDTH_32_BIT, 0x12345678, 0x56789ABC, 255) &&
            // And with smaller record size (16 bytes data => 19 byte count for 16 bit address)
            testGenerationAndParse(WIDTH_16_BIT, 0x1234, 0x5678, 19) &&
            testGenerationAndParse(WIDTH_24_BIT, 0x123456, 0x56789A, 20) &&
            testGenerationAndParse(WIDTH_32_BIT, 0x12345678, 0x56789ABC, 21))
        {
            // Now test reading an external file
            success = testReadSrecFile(argv[1]);
            if (success)
            {
                // Test the srec_file higher level API
                success = testSrecFileApi(argv[2]);
            }
        }
    }
    else
    {
        fprintf(stderr, "Usage:\n");
        fprintf(stderr, "   %s <input SREC file> <name of file to generate for srec_file API tests>\n", argv[0]);
    }

    return success ? 0 : -1;
}
