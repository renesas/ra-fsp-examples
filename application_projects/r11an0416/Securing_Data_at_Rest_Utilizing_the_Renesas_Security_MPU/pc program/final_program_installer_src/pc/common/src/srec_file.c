/***********************************************************************************************************************
* Copyright (c) 2015 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/*
 *  SREC parser
 */


#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "srec.h"
#include "srec_file.h"

/*
 * Private data
 */

// Amount of extra buffer size to allocate in the reader
#define SIZE_OF_MODULE_SECURITY_TAIL 256

/*
 * Private functions
 */

// Generator callback for the writer
static bool generatorCallback(const char *pSrec, void *pUserParams)
{
    FILE *fp = (FILE *)pUserParams;
    return (fprintf(fp, "%s", pSrec) == strlen(pSrec)) ? true : false;
}

/*
 * Public API
 */

 /*
  * Reading a S Record file
  */

 // Read a S record file.
 //
 // N.B: this function will malloc pBuffer and on successful return, the ownership
 //      of pBuffer is transferred to the caller who will be responsible for free'ing it.
 //      If the file read fails, this function will free pBuffer and the caller need
 //      not take any action with pImage.
 //
 // Input: pFilename; name of file to read
 // Output: pImage; pointer to SrecReaderImage structure that will be filled in on success
 // Returns: true if read OK, false otherwise (and no action required on pImage).
bool srecReadFile(const char *pFilename, SrecReaderImage *pImage)
{
    bool success = false;

    pImage->pBuffer = NULL;

    // Do a first pass to work out buffer size and address range
    HANDLE hReader = srecCreateParser(NULL, 0, 0);
    if (INVALID_HANDLE_VALUE != hReader)
    {
        // Parse the file
        FILE *pFile;
        if (!fopen_s(&pFile, pFilename, "r"))
        {
            char line[1024];
            bool parseOk = true;
            while (parseOk && fgets(line, _countof(line), pFile))
            {
                parseOk = srecParse(hReader, line);
            }

            if (!feof(pFile))
            {
                fprintf(stderr, "Failed to read file %s on 1st pass\n", pFilename);
                parseOk = false;
            }
            fclose(pFile);

            if (parseOk && 
                srecGetLowestAddress(hReader, &pImage->lowestAddr) &&
                srecGetHighestAddress(hReader, &pImage->highestAddr))
            {
                // Done!
                success = true;
            }
            else
            {
                fprintf(stderr, "Failed to read srec address range\n");
            }
        }
        else
        {
            fprintf(stderr, "Failed to open %s\n", pFilename);
        }

        // Done with parser
        srecDestroyParser(hReader);
    }

    if (success)
    {
        success = false;

        // Now we know the dimensions of the buffer to allocate, do the second pass
        pImage->bufferSize = pImage->highestAddr - pImage->lowestAddr + 1 + SIZE_OF_MODULE_SECURITY_TAIL;
        pImage->pBuffer = (uint8_t *)malloc(pImage->bufferSize);
        // Fill with 0xFF, the memory erased state for handling non-contiguous records
        memset(pImage->pBuffer, 0xFF, pImage->bufferSize);

        HANDLE hReader = srecCreateParser(pImage->pBuffer, pImage->bufferSize, pImage->lowestAddr);
        if (INVALID_HANDLE_VALUE != hReader)
        {
            // Parse the file
            FILE *pFile;
            if (!fopen_s(&pFile, pFilename, "r"))
            {
                char line[1024];
                bool parseOk = true;
                while (parseOk && fgets(line, _countof(line), pFile))
                {
                    parseOk = srecParse(hReader, line);
                }
                if (!feof(pFile))
                {
                    fprintf(stderr, "Failed to read file %s on 2nd pass\n", pFilename);
                    parseOk = false;
                }
                fclose(pFile);

                if (parseOk && (srecGetHighestAddress(hReader, &pImage->highestAddr)))
                {
                    // Done!
                    success = true;
                }
            }

            // Done with parser
            srecDestroyParser(hReader);
        }
    }

    if (!success)
    {
        if (pImage->pBuffer)
        {
            free(pImage->pBuffer);
        }
        pImage->pBuffer = NULL;
        pImage->bufferSize = 0;
        pImage->lowestAddr = 0;
        pImage->highestAddr = 0;
    }

    return success;
}

/*
 * Writing a S Record file
 */

// Opens a S record file for writing.
// Input: pFilename; name of file to open
// Returns: the srec writer handle if open succeeeded, INVALID_HANDLE_VALUE otherwise.
HANDLE srecWriteOpenFile(const char *pFilename)
{
    FILE *fp;
    return !fopen_s(&fp, pFilename, "w+") ? (HANDLE)fp : INVALID_HANDLE_VALUE;
}

// Writes one comment line to the S record file, prepending the line with '//'.
// Input: handle; the writer handle
// Input: pLine; the null terminated string to write
// Input: bufferBaseAddress; the target address to be associated with the start of the buffer
// Returns: true if written OK, false otherwise (handle will no longer be valid on failure)
bool srecWriteCommentString(const HANDLE handle, const char *pLine)
{
    bool success = false;

    if (fprintf(handle, "// %s\n", pLine) > 0)
    {
        success = true;
    }
    else
    {
        fclose(handle);
    }

    return success;
}

// Writes the SrecImage and then closes the file.
// Input: handle; the writer handle
// Input: image; SrecWriterImage structure defining the data to write
// Returns: true if written OK, false otherwise (handle will no longer be valid in either case)
bool srecWriteSrecImageAndClose(const HANDLE handle, const SrecWriterImage image)
{
    bool success = false;

    // Choose address width depending on largest address required
    uint32_t maxAddress = image.lowestAddr + image.numBytes;
    SrecAddressWidth addrWidth = WIDTH_32_BIT;
    if (maxAddress < 0x10000)
    {
        addrWidth = WIDTH_16_BIT;
    }
    else if (maxAddress < 0x1000000)
    {
        addrWidth = WIDTH_24_BIT;
    }

    HANDLE hGen = srecCreateGenerator(generatorCallback, handle, addrWidth);
    if (INVALID_HANDLE_VALUE != hGen)
    {
        if (srecGenerateData(hGen, image.pBuffer, image.numBytes, image.lowestAddr))
        {
            if (EOF != fclose(handle))
            {
                success = true;
            }
        }
    }

    return success;
}

