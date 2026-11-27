/***********************************************************************************************************************
* File Name    : syscalls_iar.c
* Description  : Wires IAR DLIB I/O retarget hooks to UART console (IAR EWARM toolchain).
*                Overrides __write() and __read() — resolved by DLIB for printf/scanf.
*
* Toolchain    : IAR Embedded Workbench for ARM (EWARM) + DLIB
* Usage        : Include this file when building with IAR.
*
* How it works
* ------------
*  IAR DLIB routes printf/scanf through low-level I/O retarget functions:
*    __write(handle, buf, size) — called for each output burst  (printf, puts, putchar, fprintf)
*    __read (handle, buf, size) — called for each input request (scanf, fgets, getchar)
*
*  Both stdout (handle=1) and stderr (handle=2) route to uart_console_write_byte().
*  stdin  (handle=0) routes to uart_console_read().
*
*  CRLF handling: __write inserts '\r' before every '\n' automatically,
*  so user code only needs '\n' in format strings.
*
* IAR project settings required
* ------------------------------
*  General Options → Library Configuration:
*    - Library: "Normal DLIB" or "Full DLIB"
*    - stdout/stderr:   set to "Via __write"  (not "Via semihosting")
*    - stdin:           set to "Via __read"   (not "Via semihosting")
*
*  Alternatively, in .icf or via pragma:
*    No special linker flags needed — IAR resolves __write/__read by normal
*    symbol override (define them and they replace the weak DLIB defaults).
*
* Other stubs
* -----------
*  __close, __lseek, __seek are provided as minimal stubs.
*  These satisfy DLIB without pulling in full filesystem support.
***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include <LowLevelIOInterface.h>   /* IAR DLIB: _LLIO_STDIN, _LLIO_STDOUT, _LLIO_STDERR, _LLIO_ERROR */
#include <stddef.h>
#include "uart_console.h"
 
/***********************************************************************************************************************
* @brief     __write — called by printf / puts / putchar / fprintf / fputs.
* @param[in] handle     I/O handle: _LLIO_STDOUT (1) or _LLIO_STDERR (2) — both routed to UART TX.
* @param[in] buf        Pointer to output bytes.
* @param[in] size       Number of bytes to write.
* @retval    Number of bytes written (== size on success), zero if buf is NULL or size is 0.
*
* CRLF conversion: '\r' is inserted before every '\n' automatically so user code only needs '\n' in format strings.
***********************************************************************************************************************/
size_t __write(int handle, const unsigned char *buf, size_t size)
{
    /* Discard writes to stderr/stdout if buf is NULL (flush request) */
    if ((NULL == buf) || (0U == size))
    {
        return 0;
    }
 
    /* stdout and stderr both go to UART — no handle distinction needed */
    (void)handle;
 
    for (size_t i = 0; i < size; i++)
    {
        if ('\n' == buf[i])
        {
            uart_console_write_byte((uint8_t)'\r');
        }
        uart_console_write_byte((uint8_t)buf[i]);
    }
 
    return size;
}
 
/***********************************************************************************************************************
* @brief      __read — called by scanf / fgets / getchar.
* @param[in]  handle    I/O handle: _LLIO_STDIN (0).
* @param[out] buf       Destination buffer.
* @param[in]  size      Maximum bytes to read.
* @retval     Number of bytes actually read (>= 1), or 0 on invalid input.
*
* Blocks until at least one character OR a line terminator (CR/LF) is received.
*
* Two cases:
*   1. uart_console_has_char() — drain one byte for scanf/fgets per-character path.
*   2. line complete, buffer empty — synthesise '\n' so fgets/scanf terminate correctly,
*   then clear the line flag via uart_console_read(NULL, 0).
***********************************************************************************************************************/
size_t __read(int handle, unsigned char *buf, size_t size)
{
    (void)handle;

    /* Ignore invalid input (NULL buffer or zero size) */
    if ((NULL == buf) || (0U == size))
    {
        return 0;
    }

    /* Block until at least one char or line terminator received */
    while (!uart_console_has_char() && !uart_console_has_line())
    {
        /* Do nothing */
    }
 
    if (uart_console_has_char())
    {
        /* Normal path — drain bytes up to size */
        return (size_t)uart_console_read((char *)buf, (uint32_t)size);
    }
    else
    {
        /* Buffer empty, line complete — synthesise '\n' and clear the line flag */
        uart_console_read(NULL, 0U);    /* Trigger clear_rx_events_if_empty() */
        buf[0] = '\n';
        return 1U;
    }
}
 
/***********************************************************************************************************************
* Minimal DLIB stubs — required to satisfy linker, not functionally used.
***********************************************************************************************************************/

/***********************************************************************************************************************
* @brief     __close — called by fclose and low-level close operations.
* @param[in] handle         I/O handle to close.
* @retval    0              Success (not used in this implementation).
* @retval    _LLIO_ERROR    Always returned as close is not supported.
***********************************************************************************************************************/
int __close(int handle)
{
    (void)handle;
    return _LLIO_ERROR;
}

/***********************************************************************************************************************
* @brief     __lseek — called by fseek / ftell and low-level file positioning operations.
* @param[in] handle         I/O handle.
* @param[in] offset         Offset value.
* @param[in] whence         Origin for offset (SEEK_SET, SEEK_CUR, SEEK_END).
* @retval    >= 0           New file position (not used in this implementation).
* @retval    _LLIO_ERROR    Always returned as seek is not supported.
***********************************************************************************************************************/
long __lseek(int handle, long offset, int whence)
{
    (void)handle;
    (void)offset;
    (void)whence;
    return _LLIO_ERROR;
}
