/***********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

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
