/***********************************************************************************************************************
 * File Name    : uart_console.h
 * Description  : UART console library for Renesas RA MCUs.
 *                Retargets printf/scanf/fgets to UART via picolibc tinystdio.
 *
 * Quick Start
 * -----------
 *  1. Add uart_console.c, uart_console.h, syscalls_llvm.c to your project.
 *
 *  2. In FSP (RA Configurator), open your UART instance and set:
 *       Callback: uart_console_callback
 *
 *  3. Call uart_console_init() once before any printf/scanf:
 *       uart_console_init();
 *
 *  4. Use standard C I/O anywhere in your project:
 *       printf("ADC = %d\r\n", adc_val);
 *       scanf("%d", &period);
 *       fgets(line, sizeof(line), stdin);
 *
 * Output functions (all route to UART TX via console_putc in syscalls_llvm.c)
 * -----------------------------------------------------------------------
 *   printf(fmt, ...)           — Formatted output.
 *   puts(str)                  — Print string + newline.
 *   putchar(c)                 — Print single character.
 *   fprintf(stdout, fmt, ...)  — Same as printf.
 *   snprintf(buf, n, fmt, ...) — Format into buffer only, no UART output.
 *
 * Input functions (all route to UART RX via console_getc in syscalls_llvm.c)
 * ----------------------------------------------------------------------
 *   scanf("%d", &i)            — Read integer, blocks until ENTER.
 *   scanf("%f", &f)            — Read float,   blocks until ENTER.
 *   scanf("%31s", buf)         — Read one word, stops at space.
 *   fgets(buf, n, stdin)       — Read full line including spaces.
 *   getchar()                  — Read one character, no ENTER needed.
 *
 * Non-blocking RX (use in polling loops to avoid blocking the main loop)
 * -----------------------------------------------------------------------
 *   uart_console_has_line()    — True when CR/LF received (line ready for scanf/fgets).
 *   uart_console_has_char()    — True when ≥1 byte in buffer (char ready for getchar).
 *
 * Recommended pattern for non-blocking input:
 *   if (uart_console_has_line())    { scanf("%d", &val); }
 *   if (uart_console_has_char())    { int c = getchar(); }
 *
 * UART instance
 * -------------
 *   By default this library uses the FSP-generated instance named g_uart_console.
 *   To use a different instance, change g_uart_console_ctrl / g_uart_console_cfg
 *   in uart_console.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef UART_CONSOLE_H_
#define UART_CONSOLE_H_

#include <stdint.h>

/***********************************************************************************************************************
 * Configuration — adjust per board/project
 **********************************************************************************************************************/

/* TX ring buffer size in bytes - bounds how much can be queued while a
 * transmission is already in flight. uart_console_write_byte() never blocks:
 * if this fills up, new bytes are silently dropped (same overflow policy as
 * the RX buffer) rather than waiting for room. Increase if long printf lines
 * get truncated. */
#define UART_CONSOLE_TX_BUFFER_SIZE     (1024U)

/* RX ring buffer size in bytes. Increase if long input lines overflow. */
#define UART_CONSOLE_RX_BUFFER_SIZE     (64U)

/* Macros for used functions based on specific UART version */
#if (BSP_PERIPHERAL_SCI_PRESENT)
#define UART_OPEN(ctrl, cfg)            (R_SCI_UART_Open((ctrl), (cfg)))
#define UART_WRITE(ctrl, src, bytes)    (R_SCI_UART_Write((ctrl), (src), (bytes)))
#define UART_CLOSE(ctrl)                (R_SCI_UART_Close(ctrl))
#elif (BSP_PERIPHERAL_SCI_B_PRESENT)
#define UART_OPEN(ctrl, cfg)            (R_SCI_B_UART_Open((ctrl), (cfg)))
#define UART_WRITE(ctrl, src, bytes)    (R_SCI_B_UART_Write((ctrl), (src), (bytes)))
#define UART_CLOSE(ctrl)                (R_SCI_B_UART_Close(ctrl))
#elif (BSP_PERIPHERAL_UARTA_PRESENT)
#define UART_OPEN(ctrl, cfg)            (R_UARTA_Open((ctrl), (cfg)))
#define UART_WRITE(ctrl, src, bytes)    (R_UARTA_Write((ctrl), (src), (bytes)))
#define UART_CLOSE(ctrl)                (R_UARTA_Close(ctrl))
#endif /* UART version */

/***********************************************************************************************************************
 * Lifecycle
 **********************************************************************************************************************/
uint32_t uart_console_init(void);
uint32_t uart_console_deinit(void);

/***********************************************************************************************************************
 * Output
 **********************************************************************************************************************/
void uart_console_write_byte(uint8_t byte);
void uart_console_flush(void);

/***********************************************************************************************************************
 * Input
 **********************************************************************************************************************/
uint32_t uart_console_read(void *p_buffer, uint32_t buffer_size);
uint32_t uart_console_has_line(void);
uint32_t uart_console_has_char(void);

#endif /* UART_CONSOLE_H_ */
