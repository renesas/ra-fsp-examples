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

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "memoryChecks.h"
#include "memorymap.h"

// This function is called from the application.
// It must NOT access kernel local variables.
// It must NOT produce any tracing (since PMODs are owned by the app, not the kernel).
bool memoryChecksIsBufferInRam(const void *pBuf, uint32_t length)
{
    bool retval;

    if ((0 == length)
#if !defined _WIN32
        ||
        ((uint32_t)pBuf < APP_RAM_START_ADDR) ||
        ((uint32_t)pBuf >= RAM_END_ADDR) ||
        (length > APP_RAM_SIZE ) ||
        ((uint32_t)pBuf + length >= RAM_END_ADDR)
#endif
        )
    {
        retval = false;
    }
    else
    {
        retval = true;
    }
    return retval;
}

// This function is called from the application.
// It must NOT access kernel local variables.
// It must NOT produce any tracing (since PMODs are owned by the app, not the kernel).
bool memoryChecksIsBufferInRom(const void *pBuf, uint32_t length)
{
    bool retval;

    if ((0 == length)
#if !defined _WIN32
        ||
        //((uint32_t)pBuf < CODE_FLASH_START_ADDR) ||
        ((uint32_t)pBuf >= CODE_FLASH_END_ADDR) ||
        (length > CODE_FLASH_SIZE) ||
        ((uint32_t)pBuf + length >= CODE_FLASH_END_ADDR)
#endif
        )
    {
        retval = false;
    }
    else
    {
        retval = true;
    }
    return retval;
}

// This function is called from the application.
// It must NOT access kernel local variables.
// It must NOT produce any tracing (since PMODs are owned by the app, not the kernel).
bool memoryChecksIsBufferInMemory(const void *pBuf, uint32_t length)
{
    return (memoryChecksIsBufferInRom(pBuf, length) || memoryChecksIsBufferInRam(pBuf, length));
}
