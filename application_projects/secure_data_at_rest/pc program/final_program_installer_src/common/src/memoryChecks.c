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
