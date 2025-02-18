

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

// testPullRequest.cpp : Defines the entry point for the console application.
//

#include <Windows.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <map>
#include <memory>
#include "stdafx.h"

#include "framedProtocolHost.h"
#include "moduleInfo.h"



int main(const int argc, const char *argv[])
{
    if (argc <2)
    {
        fprintf(stderr, "Usage:\n");
        fprintf(stderr, "   %s <serial port number> [additional serial port numbers]\n", argv[0]);
        return -1;
    }

    // Initialise framed protocol module
    fpInitialise();

    // Try to open the serial framed protocol connections
    for (int i = 1; i < argc; i++)
    {
        uint8_t portNumber = (uint8_t)atoi(argv[i]);
        fprintf(stdout, "Initialising connection to port COM%d...", portNumber);
        HANDLE handle = fpOpenFramedConnection(portNumber, 1000000, false);
        if (INVALID_HANDLE_VALUE != handle)
        {
            fprintf(stdout, "OK\n");
        }
        else
        {
            fprintf(stderr, "FAILED\n");
        }
    }

    // Start the listening TCP/IP server listening on all interfaces
    char port[] = "27015";
    if (fpStartTcpServer(NULL, port))
    {
        fprintf(stdout, "TCP/IP server listening on port %s\n", port);
    }
    else
    {
        fprintf(stderr, "Failed to start TCP/IP server\n");
    }

    // Poll for pull request on all the open ports
    while (true)
    {
        uint32_t numHandles;

        // Get a list of all currently active connection handles
        HANDLE *pHandles = fpGetConnections(&numHandles);
        if (pHandles)
        {
            for (uint32_t i = 0; i < numHandles; i++)
            {
                if (fpPollForPullRequest(pHandles[i], 100))
                {
                    char desc[20];
                    fpGetHandleDescription(pHandles[i], desc, sizeof(desc));
                    fprintf(stdout, "Pull request received from %s\n", desc);
                    uint32_t versionListLength;
                    ModuleVersionInfo *pVersionInfo = (ModuleVersionInfo*)fpGetModuleVersionList(pHandles[i], &versionListLength);
                    if (NULL != pVersionInfo)
                    {
                        fprintf(stdout, "Module version list:\n");
                        for (uint32_t i = 0; i < pVersionInfo->numModules; i++)
                        {
                            fprintf(stdout, "  module=%d, version=%d\n", (int)pVersionInfo->info[i].module, (int)pVersionInfo->info[i].version);
                        }
                    }
                    else
                    {
                        fprintf(stderr, "Failed to retrieve module version list\n");
                    }
                }
            }

            free(pHandles);
        }
        else
        {
            // Don't spin continuously when no handles available
            Sleep(100);
        }
    }

    return 0;
}

