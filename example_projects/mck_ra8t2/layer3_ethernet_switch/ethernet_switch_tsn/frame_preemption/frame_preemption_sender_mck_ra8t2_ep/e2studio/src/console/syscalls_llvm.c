/***********************************************************************************************************************
 * File Name    : syscalls_llvm.c
 * Description  : Wires picolibc tinystdio to UART console (LLVM ATfE toolchain).
 *                Defines stdout/stdin/stderr as FILE* — resolved by linker for printf/scanf.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/* Mirrors the guard in syscalls_gcc.c: this file targets picolibc, which is
 * what the clang_arm (LLVM ATfE) toolchain uses. Guarding by __clang__ keeps
 * this safely inert if the same source tree is ever built with real GCC
 * (arm-none-eabi-gcc + newlib), where syscalls_gcc.c is the active one
 * instead - without needing IDE-side "Exclude from Build" bookkeeping. */
#if defined(__clang__)

#include <stdio.h>
#include "uart_console.h"

/***********************************************************************************************************************
 * Picolibc FILE callbacks
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * @brief     Output a character to UART console (used by printf/puts).
 *            This function automatically inserts '\r' before '\n' to ensure proper CRLF
 *            formatting for terminals requiring carriage return + line feed.
 * @param[in] c   Character to output.
 * @param[in] f   File stream (unused).
 * @retval    Output character cast to unsigned char.
 **********************************************************************************************************************/
static int console_putc(char c, FILE *f)
{
    (void)f;
    if (c == '\n')
    {
        uart_console_write_byte((uint8_t)'\r');
    }
    uart_console_write_byte((uint8_t)c);
    return (unsigned char)c;
}

/***********************************************************************************************************************
 * @brief     This function reads a character from UART console (used by scanf/fgets/getchar).
 *            Handles two cases based on g_rx_count:
 *             - > 0  : Chars waiting → drain one by one via uart_console_read.
 *             - == 0 : Buffer empty but line complete (CR/LF received, all chars consumed)
 *                                     → synthesise '\n' so fgets/scanf terminate correctly.
 *                                     → clear line flag so next read starts fresh.
 * @param[in] f   File stream (unused).
 * @retval    Input character cast to unsigned char.
 **********************************************************************************************************************/
static int console_getc(FILE *f)
{
    (void)f;

    /* Block until at least one char OR line terminator received */
    while (!uart_console_has_char() && !uart_console_has_line()) { ; }

    if (uart_console_has_char())
    {
        /* Normal path — drain one byte from head of RX buffer */
        char c = 0;
        uart_console_read(&c, 1U);
        return (unsigned char)c;
    }
    else
    {
        /* Buffer empty, line complete — all chars already consumed by previous getc calls.
         * Return '\n' so fgets/scanf know the line has ended, then clear the flag. */
        uart_console_read(NULL, 0U);    /* Trigger clear_rx_events_if_empty() */
        return (unsigned char)'\n';
    }
}

/***********************************************************************************************************************
 * stdout / stdin / stderr
 * Picolibc tinystdio (armv8.1m variant) resolves these as extern FILE*.
 **********************************************************************************************************************/
static FILE g_stdin_file = FDEV_SETUP_STREAM(NULL, console_getc, NULL, _FDEV_SETUP_READ);
static FILE g_stdout_file = FDEV_SETUP_STREAM(console_putc, NULL, NULL, _FDEV_SETUP_WRITE);

FILE *const stdin = &g_stdin_file;
FILE *const stdout = &g_stdout_file;
FILE *const stderr = &g_stdout_file;

#endif /* defined(__clang__) */
