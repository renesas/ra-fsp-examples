/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#include <Windows.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include "stdafx.h"

#include "framedProtocolHost.h"
#include "flashR7FA6M3AH3.h"
#include "srec.h"
#include "memoryMap.h"
//#include "secureKernelBuildId.h"

// Maximum number of SREC files to load
#define MAX_SREC_FILES 5

/*
 * Private data
 */


/*
 * Private functions
 */

// Allocate a buffer large enough to hold the code flash records for the given chip
static uint8_t *allocateBuffer(const FlashDescription *pFlashDesc, uint32_t &bufferSize, uint32_t &bufferBaseAddress)
{
    // Size is determined by the start address of the first record and the end address of the last
    bufferSize = pFlashDesc->pFlashRecords[pFlashDesc->numFlashRecords - 1].endAddr - pFlashDesc->pFlashRecords[0].startAddr + 1;
    bufferBaseAddress = pFlashDesc->pFlashRecords[0].startAddr;
    return (uint8_t *)malloc(bufferSize);
}

// Input: filename; name of the SREC file
// Input: pBuffer; pointer to buffer
// Input: bufferSize; number of bytes in the buffer the buffer
// Input: bufferBaseAddress; address associated with the base of the buffer
// Output: lowestAddress; lowest address in the buffer that has been written to
// Output: highestAddress; highest address in the buffer that has been written to
// Returns true on succes, false otherwise.
static bool parseSrec(const char *filename, const uint8_t *pBuffer, const uint32_t bufferSize, const uint32_t bufferBaseAddress, uint32_t &lowestAddress, uint32_t &highestAddress)
{
    bool success = false;
    HANDLE hSrec = INVALID_HANDLE_VALUE;

    if (pBuffer)
    {
        // Create a SREC parser
        hSrec = srecCreateParser(pBuffer, bufferSize, bufferBaseAddress);

        // Read the file and parse
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
                // Parsing completed, find the address range
                if (srecGetLowestAddress(hSrec, &lowestAddress) && srecGetHighestAddress(hSrec, &highestAddress))
                {
                    success = true;
                }
            }
        }

        // Done with parser
        srecDestroyParser(hSrec);
    }

    return success;
}

// Try to disable the chip flash access controls so that the
// kernel can be programmed.
// Input: handle; an open flash programming connection
// Returns: true if operation completed OK, false otherwise
static bool disableFlashAccessWindow(const HANDLE handle)
{
    bool success = false;

    // Read the flash access window register addresses
    uint32_t start, end;
    if (fpGetFlashAccessWindow(handle, &start, &end))
    {
        if (start == end)
        {
            // Access window is disabled
            fprintf(stdout, "Flash ready for access\n");
            success = true;
        }
        else
        {
            fprintf(stdout, "Disabling flash access window (0x%X..0x%X)\n", start, end);
            // Access window is enabled - access is only possible if FSPR is not set
            bool isProtected;
            if (fpGetFlashProtection(handle, &isProtected))
            {
                if (isProtected)
                {
                    fprintf(stderr, "Flash protection is active - unable to install kernel\n");
                }
                else
                {
                    // Reset the flash access window register to disable protection
                    if (fpResetFlashAccessWindowRegister(handle))
                    {
                        fprintf(stdout, "Flash access window is now disabled\n");
                        success = true;
                    }
                    else
                    {
                        fprintf(stderr, "Failed to disable the flash access window\n");
                    }
                }
            }
            else
            {
                fprintf(stderr, "Failed to read flash protection register\n");
            }
        }
    }
    else
    {
        fprintf(stderr, "Failed to read flash access window registers\n");
    }

    return success;
}

// Erase all code flash and program with the parsed SREC file
// Input: pFlashDesc; pointer to chip flash description record
// Input: filenames; array of SREC filenames
// Input: numFiles; the number of SREC files to process
// Input: portNumber; the serial port number to use
// Returns: true if operation completed OK, false otherwise
static bool eraseFlashAndProgram(const FlashDescription *pFlashDesc, const char *filenames[], const uint8_t numFiles, const uint8_t portNumber)
{
    bool success = false;

    // Allocate a buffer and retrieve addressing info from the flash description records
    uint32_t bufferBaseAddress, bufferSize;
    uint8_t *pBuffer = allocateBuffer(pFlashDesc, bufferSize, bufferBaseAddress);

    if (pBuffer)
    {
        // Parse the SREC files first - only erase the flash if the parsing is successful
        uint32_t lowestAddress[MAX_SREC_FILES], highestAddress[MAX_SREC_FILES];
        bool parseOk = true;
        int i = 0;
        while (parseOk && (i < numFiles))
        {
            parseOk = parseSrec(filenames[i], pBuffer, bufferSize, bufferBaseAddress, lowestAddress[i], highestAddress[i]);
            if (parseOk)
            {
                fprintf(stdout, "Parsed SREC file: %s (0x%X..0x%X)\n", filenames[i], lowestAddress[i], highestAddress[i]);
            }
            else
            {
                fprintf(stderr, "Failed to parse SREC file: %s\n", filenames[i]);
            }

            i++;
        }

        // Only if all the files parsed OK will the flash be erased
        if (parseOk)
        {
            fprintf(stdout, "Initialising flash programming connection to port COM%d\n", portNumber);
            HANDLE handle = fpOpenFramedConnection(portNumber, 1000000, true);
            if (INVALID_HANDLE_VALUE != handle)
            {
                fprintf(stdout, "Established flash programming connection\n");

                // Can the flash access window be disabled to allow kernel programming?
                if (disableFlashAccessWindow(handle))
                {
                    // Try to erase the flash
                    fprintf(stdout, "Erasing flash\n");
                    if (fpEraseAll(handle, pFlashDesc))
                    {
                        fprintf(stdout, "Flash erased OK\n");

                        // Write each section of the buffer corresponding to a parsed file
                        success = true;
                        i = 0;
                        while (success && (i < numFiles))
                        {
                            uint32_t bufferOffset = lowestAddress[i] - bufferBaseAddress;
                            uint32_t numBytes = highestAddress[i] - lowestAddress[i] + 1;


                            // The memory map has been rearranged for the Evaluation so that 1stRx is within the FAW protected region of the flash.
                            // This means that the kernel image contains (a) kernal [which appears before 1st Rx] and (b) the main MT [which appears after 1strx]
                            // So the flashing of the kernel, flashes the pages containing the 1stRx.
                            // Hence, we need to erase them again before flashing the kernel.
                            if (i > 0)
                            {
                                fprintf(stdout, "Erase flash for %s region start=0x%x, end=0x%x\n", filenames[i], lowestAddress[i], highestAddress[i]);
                                success = fpErase(handle, pFlashDesc, lowestAddress[i], highestAddress[i]+1);
                                if (success)
                                {
                                    fprintf(stdout, "Flash region erased OK\n");
                                }
                                else
                                {
                                    fprintf(stderr, "Failed to erase flash for %s region\n", filenames[i]);
                                }
                            }

                            if (success)
                            {
                                fprintf(stdout, "Writing flash for %s region (bufferOffset=0x%X, 0x%X bytes)\n", filenames[i], bufferOffset, numBytes);
                                success = fpWrite(handle, pFlashDesc, lowestAddress[i], (pBuffer + bufferOffset), numBytes);
                                if (success)
                                {
                                    fprintf(stdout, "Flash region written OK\n");
                                }
                                else
                                {
                                    fprintf(stderr, "Failed to write flash for %s region\n", filenames[i]);
                                }
                            }
                            i++;
                        }
                        
                        if (success)
                        {
/* MOD REA START - Define this macro to enable flash access window */
#ifdef ENABLE_FLASH_ACCESS_WINDOW
                            success = false;
                            // Enable the flash access window and configure FSPR if ACTIVATE_THE_FSPR is defined
                            if (fpSetFlashAccessWindowAndFlashProtection(handle, FAW_START, FAW_END))
                            {
                                uint32_t start, end;
                                if (fpGetFlashAccessWindow(handle, &start, &end))
                                fprintf(stdout, "Flash Access Window configured for start=0x%x, end=0x%x\n", start, end);
                                success = true;
#ifdef ACTIVATE_THE_FSPR
                                // Check the FSPR configuration status
                                success = false;
                                bool isProtected;
                                if (fpGetFlashProtection(handle, &isProtected))
                                {
                                    if (isProtected)
                                    {
                                        fprintf(stderr, "Flash access window is now LOCKED\nCompleted OK\n");
                                        success = true;
                                    }
                                    else
                                    {
                                        fprintf(stderr, "Failed to lock the flash access window\n");
                                    }

                                else
                                {
                                    fprintf(stderr, "Failed to read flash protection register\n");
                                }
#else
                                fprintf(stdout, "Completed OK\n");
#endif
                            }
                            else
                            {
                                fprintf(stderr, "Failed to configure the flash access window\n");
                            }
#endif
                            /* MOD REA END */
                        }
                    else
                    {
                        fprintf(stderr, "Failed to write flash\n");
                    }
                    }
                else
                {
                    fprintf(stderr, "Failed to erase flash\n");
                }
                }
            }
        else
        {
            fprintf(stderr, "Failed to create flash programming connection\n");
        }
        }

        free(pBuffer);
    }
    else
    {
        fprintf(stderr, "Failed to allocate buffer\n");
    }

    return success;
}

/*
 * Main entry point
 */

int main(const int argc, const char *argv[])
{
    int retVal = -1;

  
    if ((argc > 2)  && (argc < (MAX_SREC_FILES + 3)))
    {
        // Initialise framed protocol module
        fpInitialise();

        if (eraseFlashAndProgram(&flashR7FA6M3AH3, &argv[2], argc - 2, (uint8_t)atoi(argv[1])))
        {
            retVal = 0;
        }
    }
    else
    {
        fprintf(stderr, "Usage:\n");
        fprintf(stderr, "   %s <serial port number> <kernel SREC file> [<additional SREC files>]\n", argv[0]);

    }

    return retVal;
}

