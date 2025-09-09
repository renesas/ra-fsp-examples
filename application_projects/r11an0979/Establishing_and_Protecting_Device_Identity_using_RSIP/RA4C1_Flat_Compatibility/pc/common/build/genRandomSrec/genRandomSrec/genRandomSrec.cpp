

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


#include <Windows.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <tchar.h>
#include "stdafx.h"

#include "srec.h"
#include "srec_file.h"

int main(const int argc, const char *argv[])
{
    bool success = false;

    if (argc == 4)
    {
        const char *pFilename = argv[3];

        // Accept numerical input in either decimal or hex (preceeded by 0x)
        // setbase(0) switches into auto dec/hex parsing which is quite neat!
        std::istringstream lenStrm(argv[1]);
        int numBytes;
        lenStrm >> std::setbase(0) >> numBytes;
        std::istringstream addrStrm(argv[2]);
        int address;
        addrStrm >> std::setbase(0) >> address;

        if ((numBytes > 0) && (address > 0))
        {
            // Attempt to open the output file
            HANDLE handle = srecWriteOpenFile(pFilename);
            if (INVALID_HANDLE_VALUE != handle)
            {
                SrecWriterImage image;
                image.numBytes = numBytes;
                image.lowestAddr = address;
                image.pBuffer = (uint8_t *)malloc(image.numBytes);

                // Fill with random data
                fprintf(stdout, "Creating random data\n");
                for (int i = 0; i < image.numBytes; i++)
                {
                    image.pBuffer[i] = rand() & 0xFF;
                }

                // Write SREC file
                if (srecWriteSrecImageAndClose(handle, image))
                {
                    fprintf(stdout, "SREC file %s created\n", pFilename);
                }
                else
                {
                    fprintf(stderr, "Failed to write output file %s\n", pFilename);
                }

                free(image.pBuffer);
            }
            else
            {
                fprintf(stderr, "Failed to open output file %s\n", pFilename);
            }
        }
        else
        {
            fprintf(stderr, "Failed to parse numerical input. numBytes=%d, address=%d\n", numBytes, address);
        }
    }
    else
    {
        fprintf(stderr, "Usage:\n");
        fprintf(stderr, "   %s <length of data> <start address> <output srec filename>\n", argv[0]);
    }

    return success ? 0 : -1;
}

