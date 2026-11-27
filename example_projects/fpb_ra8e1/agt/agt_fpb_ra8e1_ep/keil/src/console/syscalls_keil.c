/***********************************************************************************************************************
* File Name    : syscalls_keil.c
* Description  : Wires Keil C library I/O retarget hooks to UART console (Keil MDK-ARM / Arm Compiler 6).
*                Retargets fputc() and fgetc() for printf/scanf.
*
* Toolchain    : Keil MDK-ARM with Arm Compiler 6.24 or later
* Usage        : Include this file when building with Keil.
*
* How it works
* ------------
*  The Keil C library routes standard I/O through:
*    fputc(ch, stream) — called by printf / puts / putchar / fprintf
*    fgetc(stream)     — called by scanf / fgets / getchar
*
*  stdout/stderr route to uart_console_write_byte().
*  stdin routes to uart_console_read().
*
*  CRLF handling: fputc inserts '\r' before every '\n' automatically,
*  so user code only needs '\n' in format strings.
***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include "uart_console.h"

#if !defined(__ARMCC_VERSION) || (__ARMCC_VERSION < 6000000)
#error "This file is intended for Keil Arm Compiler 6 only."
#endif

/* Disable semihosting for Arm Compiler 6 */
__asm(".global __use_no_semihosting\n");

/***********************************************************************************************************************
* @brief     fputc — called by printf / puts / putchar / fprintf.
* @param[in] ch         Character to write.
* @param[in] stream     Output stream.
* @retval    Written character.
*
* CRLF conversion: '\r' is inserted before every '\n' automatically so user code only needs '\n' in format strings.
***********************************************************************************************************************/
int fputc(int ch, FILE *stream)
{
    (void)stream;

    if (ch == '\n')
    {
        uart_console_write_byte((uint8_t)'\r');
    }

    uart_console_write_byte((uint8_t)ch);

    return ch;
}

/***********************************************************************************************************************
* @brief     fgetc — called by scanf / fgets / getchar.
* @param[in] stream     Input stream.
* @retval    Received character.
*
* Blocks until at least one character or a line terminator (CR/LF) is received.
*
* Two cases:
*   1. uart_console_has_char() — drain one byte for scanf/fgets per-character path.
*   2. line complete, buffer empty — synthesise '\n' so fgets/scanf terminate correctly,
*      then clear the line flag via uart_console_read(NULL, 0).
***********************************************************************************************************************/
int fgetc(FILE *stream)
{
    char ch = 0;

    (void)stream;

    /* Block until at least one char or line terminator received */
    while (!uart_console_has_char() && !uart_console_has_line())
    {
        /* Do nothing */
    }

    if (uart_console_has_char())
    {
        (void)uart_console_read(&ch, 1U);
        return (int)ch;
    }
    else
    {
        /* Buffer empty, line complete — synthesise '\n' and clear the line flag */
        uart_console_read(NULL, 0U);    /* Trigger clear_rx_events_if_empty() */
        return (int)'\n';
    }
}

/***********************************************************************************************************************
* Minimal no-semihosting stubs — required to satisfy Keil C library.
***********************************************************************************************************************/

/***********************************************************************************************************************
* @brief     _ttywrch — low-level character output used by Keil library.
* @param[in] ch         Character to write.
***********************************************************************************************************************/
void _ttywrch(int ch)
{
    uart_console_write_byte((uint8_t)ch);
}

/***********************************************************************************************************************
* @brief     _sys_exit — called by the C library on program exit.
* @param[in] return_code    Exit code.
***********************************************************************************************************************/
void _sys_exit(int return_code)
{
    (void)return_code;

    while (1)
    {
        /* Do nothing */
    }
}
