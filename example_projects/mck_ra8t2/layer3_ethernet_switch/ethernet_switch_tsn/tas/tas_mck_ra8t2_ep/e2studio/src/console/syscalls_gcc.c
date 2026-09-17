/***********************************************************************************************************************
 * File Name    : syscalls_gcc.c
 * Description  : Wires newlib syscall stubs to UART console (GCC arm-none-eabi toolchain).
 *                Overrides _write() and _read() — resolved by newlib for printf/scanf.
 *
 * Toolchain    : GCC arm-none-eabi + newlib (or newlib-nano)
 * Usage        : Include this file instead of syscalls_llvm.c when building with GCC.
 *
 * How it works
 * ------------
 *  newlib routes printf/scanf through low-level POSIX-style syscall stubs:
 *    _write(fd, buf, count) — called for each output burst  (printf, puts, putchar, fprintf)
 *    _read (fd, buf, count) — called for each input request (scanf, fgets, getchar)
 *
 *  Both stdout (fd=1) and stderr (fd=2) route to uart_console_write_byte().
 *  stdin  (fd=0) routes to uart_console_read().
 *
 *  CRLF handling: _write inserts '\r' before every '\n' automatically,
 *  so user code only needs '\n' in format strings.
 *
 * Linker flags required
 * ---------------------
 *  newlib-nano (recommended for bare-metal RA):
 *    --specs=nosys.specs   — provides weak default stubs (overridden here)
 *    --specs=nano.specs    — links newlib-nano (smaller printf/scanf)
 *    -u _printf_float      — add if %f support needed in newlib-nano
 *    -u _scanf_float       — add if %f input  support needed in newlib-nano
 *
 *  Full newlib:
 *    --specs=nosys.specs   — only this flag needed; float already included
 *
 * Other stubs
 * -----------
 *  Minimal stubs for _close, _fstat, _isatty, _lseek are provided below.
 *  These satisfy the linker without pulling in full filesystem support.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2024 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 ***********************************************************************************************************************/
 
#if defined(__GNUC__) && !defined(__clang__)

#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include "uart_console.h"

/***********************************************************************************************************************
 * _write — called by printf / puts / putchar / fprintf / fputs
 *
 * @param fd     File descriptor: 1 = stdout, 2 = stderr (both routed to UART TX).
 * @param buf    Pointer to output bytes.
 * @param count  Number of bytes to write.
 * @return       Number of bytes written (always == count on success).
 *
 * CRLF conversion: '\r' is inserted before every '\n' automatically so user
 * code only needs '\n' in format strings.
 **********************************************************************************************************************/
int _write(int fd, char *buf, int count)
{
    (void)fd;   /* stdout and stderr both go to UART — no fd distinction needed */

    for (int i = 0; i < count; i++)
    {
        if (buf[i] == '\n')
        {
            uart_console_write_byte((uint8_t)'\r');
        }
        uart_console_write_byte((uint8_t)buf[i]);
    }

    return count;
}

/***********************************************************************************************************************
 * _read — called by scanf / fgets / getchar
 *
 * @param fd     File descriptor: 0 = stdin.
 * @param buf    Destination buffer.
 * @param count  Maximum bytes to read.
 * @return       Number of bytes actually read (>= 1).
 *
 * Blocks until at least one character OR a line terminator (CR/LF) is received.
 *
 * Two cases:
 *   1. uart_console_has_char() — drain one byte for scanf/fgets per-character path.
 *   2. line complete, buffer empty — synthesise '\n' so fgets/scanf terminate
 *      correctly, then clear the line flag via uart_console_read(NULL, 0).
 **********************************************************************************************************************/
int _read(int fd, char *buf, int count)
{
    (void)fd;

    /* Block until at least one char OR line terminator received */
    while (!uart_console_has_char() && !uart_console_has_line()) { ; }

    if (uart_console_has_char())
    {
        /* Normal path — drain bytes up to count */
        return (int)uart_console_read(buf, (uint32_t)count);
    }
    else
    {
        /* Buffer empty, line complete — synthesise '\n' and clear the line flag */
        uart_console_read(NULL, 0U);    /* triggers clear_rx_events_if_empty() */
        buf[0] = '\n';
        return 1;
    }
}

/***********************************************************************************************************************
 * Minimal syscall stubs — required by newlib linker, not functionally used.
 **********************************************************************************************************************/

int _close(int fd)
{
    (void)fd;
    return -1;
}

int _fstat(int fd, struct stat *st)
{
    (void)fd;
    st->st_mode = S_IFCHR;  /* character device — tells newlib this is a terminal */
    return 0;
}

int _isatty(int fd)
{
    (void)fd;
    return 1;   /* always a TTY — enables line-buffered mode in newlib */
}

int _lseek(int fd, int offset, int whence)
{
    (void)fd;
    (void)offset;
    (void)whence;
    return -1;
}

#endif /* defined(__GNUC__) && !defined(__clang__) */
