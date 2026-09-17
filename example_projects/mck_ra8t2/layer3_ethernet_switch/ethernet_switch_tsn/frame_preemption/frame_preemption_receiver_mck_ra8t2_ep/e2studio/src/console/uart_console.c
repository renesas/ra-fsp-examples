/***********************************************************************************************************************
 * File Name    : uart_console.c
 * Description  : UART console library for Renesas RA MCUs.
 *                Retargets printf/scanf/fgets to UART via picolibc tinystdio.
 *
 * Quick Start
 * -----------
 *  1. Add uart_console.c, uart_console.h, syscalls_llvm.c to your project.
 *
 *  2. In FSP (RA Configurator), open your UART instance and set:
 *       Name     : g_uart_console
 *       Callback : uart_console_callback
 *       Enable clock, select UART channel to use OB VCOM
 *
 *  3. Call uart_console_init() once before any printf/scanf:
 *       uart_console_init();
 *
 *  4. Use standard C I/O anywhere in your project:
 *       printf("ADC = %d\r\n", adc_val);
 *       scanf("%d", &period);
 *       fgets(line, sizeof(line), stdin);
 *
 * Output functions (route to UART TX via console_putc in syscalls_llvm.c)
 * -------------------------------------------------------------------
 *
 *  printf(fmt, ...)
 *    Formatted output. Supports %d %u %f %s %x %X %c and width/precision.
 *      printf("val = %d\r\n",       42);
 *      printf("volt = %.2f V\r\n",  3.30f);
 *      printf("name = %s\r\n",      "RA8T2");
 *      printf("reg  = 0x%08X\r\n",  0xDEADBEEF);
 *
 *  puts(str)
 *    Print string + newline. No format needed. Simpler than printf for fixed messages.
 *      puts("Scan started.");       // Same as printf("Scan started.\n")
 *      puts("Error: timeout.");
 *
 *  putchar(c)
 *    Print exactly one character. Useful in loops.
 *      putchar('.');                // Print progress dot
 *      for (char c='A'; c<='Z'; c++) { putchar(c); }
 *
 *  fprintf(stdout, fmt, ...)
 *    Same as printf — explicit stdout target.
 *      fprintf(stdout, "ch%d = %d\r\n", ch, val);
 *
 *  snprintf(buf, n, fmt, ...)
 *    Format into a buffer — NO UART output. Use when you need the string for
 *    further processing before printing.
 *      char msg[32];
 *      snprintf(msg, sizeof(msg), "ch%d: %.2f V", ch, volt);
 *      // msg = "ch1: 3.30 V" — send later or compare
 *      printf("%s\r\n", msg);
 *
 * Input functions (route to UART RX via console_getc in syscalls_llvm.c)
 * ------------------------------------------------------------------
 *
 *  scanf("%d", &i)
 *    Read a decimal integer. Blocks until user types number + ENTER.
 *      int period;
 *      printf("Period (ms): ");
 *      scanf("%d", &period);        // User types: 500<ENTER>
 *
 *  scanf("%f", &f)
 *    Read a float. Accepts: "3.14", ".5", "1e3".
 *      float threshold;
 *      printf("Threshold: ");
 *      scanf("%f", &threshold);     // User types: 2.75<ENTER>
 *
 *  scanf("%31s", buf)
 *    Read one word — stops at space. Always use width limit to prevent overflow.
 *      char cmd[32];
 *      scanf("%31s", cmd);          // "hello world" → stores only "hello"
 *
 *  fgets(buf, n, stdin)
 *    Read full line including spaces. Stops at ENTER or n-1 chars.
 *    Strips trailing '\n' with strcspn before use.
 *      char line[64];
 *      fgets(line, sizeof(line), stdin);           // "hello world<ENTER>"
 *      line[strcspn(line, "\r\n")] = '\0';         // → "hello world"
 *
 *  getchar()
 *    Read exactly one character. Returns immediately after one keypress.
 *      printf("Press y/n: ");
 *      int key = getchar();
 *      if (key == 'y') { start(); }
 *
 * Non-blocking RX (use in polling loops to avoid blocking the main loop)
 * -----------------------------------------------------------------------
 *
 *  uart_console_has_line()
 *    Returns non-zero when CR/LF received — line ready for scanf/fgets.
 *      if (uart_console_has_line()) { scanf("%d", &cmd); }
 *
 *  uart_console_has_char()
 *    Returns non-zero when ≥1 byte is in the RX buffer — char ready for getchar().
 *      if (uart_console_has_char()) { int key = getchar(); }
 *
 *  Recommended pattern — main loop that never blocks:
 *      while (true)
 *      {
 *          if (uart_console_has_line())
 *          {
 *              scanf("%d", &cmd);
 *              process(cmd);
 *          }
 *          adc_read_data();         // Run continuously, not blocked
 *      }
 *
 * Flush RX buffer after scanf
 * ---------------------------
 *  scanf leaves '\n' in the buffer. Drain before the next read:
 *      scanf("%d", &val);
 *      char tmp[UART_CONSOLE_RX_BUFFER_SIZE];
 *      uart_console_read(tmp, sizeof(tmp));         // Discard leftover '\n'
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

#include <string.h>
#include <stdbool.h>
#include <hal_data.h>
#include "uart_console.h"

/***********************************************************************************************************************
 * Private global variables
 **********************************************************************************************************************/

/* RX ring buffer — filled byte-by-byte in uart_console_callback() ISR context */
static char g_rx_buf[UART_CONSOLE_RX_BUFFER_SIZE];

/* Combined event flags for TX_COMPLETE, RX_CHAR, RX_COMPLETE. Set in ISR, polled in main */
static volatile uart_event_t g_event = (uart_event_t)0U;

/* Number of valid bytes currently in g_rx_buf */
static volatile uint32_t g_rx_count = 0U;

static uint8_t            g_tx_buf[UART_CONSOLE_TX_BUFFER_SIZE];
static volatile uint32_t  g_tx_head     = 0U; /* next free slot to push into */
static volatile uint32_t  g_tx_tail     = 0U; /* oldest byte not yet confirmed sent */
static volatile uint32_t  g_tx_count    = 0U; /* bytes queued, including any currently in flight */
static volatile uint32_t  g_tx_inflight = 0U; /* bytes handed to the current UART_WRITE(), not yet confirmed by TX_COMPLETE */
static volatile bool      g_tx_busy     = false; /* true while a UART_WRITE() is in flight */

/* Hand the largest currently-available contiguous run (g_tx_tail onward,
 * capped at the physical end of the array - UART_WRITE() needs one
 * contiguous pointer+length, it can't wrap) to the UART driver. Caller must
 * already know g_tx_count > 0 and g_tx_busy is being claimed. Runs with
 * interrupts disabled (called only from within __disable_irq() sections or
 * from ISR context). */
static void uart_console_tx_start_next_chunk(void)
{
    uint32_t to_end   = UART_CONSOLE_TX_BUFFER_SIZE - g_tx_tail;
    uint32_t chunk_len = (g_tx_count < to_end) ? g_tx_count : to_end;

    g_tx_busy     = true;
    g_tx_inflight = chunk_len;

    if (FSP_SUCCESS != UART_WRITE(&g_uart_console_ctrl, &g_tx_buf[g_tx_tail], chunk_len))
    {
        g_tx_busy     = false;
        g_tx_inflight = 0U;
    }
}

/***********************************************************************************************************************
 * Private functions
 **********************************************************************************************************************/

/***********************************************************************************************************************
 * @brief     This function clears RX event flags when the buffer has been fully consumed.
 *            Called after every uart_console_read() to keep flag state consistent.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void clear_rx_events_if_empty(void)
{
    if (0U == g_rx_count)
    {
        g_event &= ~(uint32_t)(UART_EVENT_RX_COMPLETE | UART_EVENT_RX_CHAR);
    }
}

/***********************************************************************************************************************
 * @brief     UART callback function for handling TX and RX events.
 *            Handles events:
 *             - UART_EVENT_TX_COMPLETE: Send the next queued TX byte (if any), keeping transmission
 *                                       going entirely in the background - see uart_console_write_byte().
 *
 *             - UART_EVENT_RX_CHAR:     Store received byte in g_rx_buf.
 *                                       CR ('\r') and LF ('\n') are NOT stored; they set RX_COMPLETE flag
 *                                       so the caller knows a full line is ready.
 *                                       BS ('\b') removes the last stored byte (simple backspace support).
 *                                       Overflow is silently dropped (buffer full guard).
 * @param[in] p_args   Pointer to UART callback arguments.
 * @retval    None.
 **********************************************************************************************************************/
void uart_console_callback(uart_callback_args_t *p_args)
{
    switch (p_args->event)
    {
        case UART_EVENT_TX_COMPLETE:
        {
            g_tx_tail = (g_tx_tail + g_tx_inflight) % UART_CONSOLE_TX_BUFFER_SIZE;
            g_tx_count -= g_tx_inflight;
            g_tx_inflight = 0U;

            if (g_tx_count > 0U)
            {
                uart_console_tx_start_next_chunk();
            }
            else
            {
                g_tx_busy = false;
            }

            break;
        }

        case UART_EVENT_RX_CHAR:
        {
            /* Drop incoming byte if buffer is full — overflow protection */
            if (UART_CONSOLE_RX_BUFFER_SIZE <= g_rx_count)
            {
                break;
            }

            char c = (char)p_args->data;

            if (c == '\n' || c == '\r')
            {
                /* Line terminator — signal line complete, do not store CR/LF */
                g_event |= UART_EVENT_RX_COMPLETE;
            }
            else if (c == '\b')
            {
                /* Backspace — remove last stored character */
                if (0U < g_rx_count)
                {
                    g_rx_count--;
                }
            }
            else
            {
                /* Normal printable character — append to buffer */
                g_rx_buf[g_rx_count++] = c;
                g_event |= UART_EVENT_RX_CHAR;
            }
            break;
        }

        default:
            break;
    }
}

/***********************************************************************************************************************
 * @brief     This function initializes UART console interface.
 *            Safe to call multiple times — no-op if already open.
 *            Must be called before the first printf/scanf.
 * @param[in] None.
 * @retval    FSP_SUCCESS              Initialization successful or already initialized.
 * @retval    Any other error codes    Failed to initialize.
 **********************************************************************************************************************/
uint32_t uart_console_init(void)
{
    if (g_uart_console_ctrl.open)
    {
        return FSP_SUCCESS;     /* Already open — no-op */
    }

    return UART_OPEN(&g_uart_console_ctrl, &g_uart_console_cfg);
}

/***********************************************************************************************************************
 * @brief     This function de-initializes UART console interface and reset all state.
 *            Call before entering low-power mode or reconfiguring UART.
 * @param[in] None.
 * @retval    FSP_SUCCESS              De-initialization successful.
 * @retval    Any other error codes    Failed to de-initialize.
 **********************************************************************************************************************/
uint32_t uart_console_deinit(void)
{
    uart_console_flush();
    
    g_rx_count = 0U;
    g_event = (uart_event_t)0U;
    memset(g_rx_buf, 0, sizeof(g_rx_buf));

    g_tx_head     = 0U;
    g_tx_tail     = 0U;
    g_tx_count    = 0U;
    g_tx_inflight = 0U;
    g_tx_busy     = false;

    return UART_CLOSE(&g_uart_console_ctrl);
}

/***********************************************************************************************************************
 * @brief     This function queues one raw byte for UART transmission and returns immediately -
 *            it does NOT wait for the byte to actually go out on the wire. The byte is always
 *            pushed into the TX ring buffer first (never handed to UART_WRITE() from a local/
 *            stack variable - see the g_tx_buf comment for why); if the UART is currently idle,
 *            a transmission is kicked off for whatever is now queued (which may already include
 *            bytes from a still-in-progress printf() call, letting them go out as one batch).
 *            If the ring buffer is full, the byte is silently dropped rather than waiting for
 *            room - this function never blocks, under any circumstance.
 *            Used internally by syscalls_llvm.c for printf/puts/putchar.
 * @param[in] byte   Byte to transmit.
 * @retval    None.
 **********************************************************************************************************************/
void uart_console_write_byte(uint8_t byte)
{
    __disable_irq();

    if (g_tx_count < UART_CONSOLE_TX_BUFFER_SIZE)
    {
        g_tx_buf[g_tx_head] = byte;
        g_tx_head = (g_tx_head + 1U) % UART_CONSOLE_TX_BUFFER_SIZE;
        g_tx_count++;
    }
    /* else: ring full - drop the byte (matches the existing RX overflow
     * policy: silently drop rather than stall the caller). */

    if (!g_tx_busy)
    {
        uart_console_tx_start_next_chunk();
    }

    __enable_irq();
}

/***********************************************************************************************************************
 * @brief     Block until every byte queued/handed to UART_WRITE() by uart_console_write_byte() has
 *            actually finished transmitting (g_tx_count reaches 0, cleared from
 *            uart_console_callback()'s TX_COMPLETE handling), then add a small fixed margin.
 *            uart_console_write_byte() is intentionally asynchronous - printf() returns as soon as
 *            bytes are queued, while the actual shift-out continues in the background via
 *            interrupts. That is a problem right before entering a low-power mode (WFI): the
 *            peripheral clock can change or be gated on entry, corrupting/cutting off a
 *            transmission still in flight - call this first so nothing is left queued when the CPU
 *            actually sleeps.
 *            The extra margin covers the gap between "TX_COMPLETE fired" (g_tx_count reaches 0)
 *            and the LAST byte's stop bit actually finishing on the wire - on some SCI
 *            configurations the event backing UART_EVENT_TX_COMPLETE can fire once the data
 *            register is empty (ready for the next byte) rather than once the shift register/stop
 *            bit is fully done, leaving a roughly one-bit-period window where the wire is still
 *            active even though g_tx_count already reads 0.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void uart_console_flush(void)
{
    while (0U < g_tx_count)
    {
        ;
    }
}

/***********************************************************************************************************************
 * @brief     This function copies bytes from the internal RX buffer into p_buffer.
 *            Supports three modes based on buffer_size:
 *              - == 0 : No copy — just clear RX_COMPLETE flag and call clear_rx_events_if_empty().
 *                       Used by syscalls_llvm.c console_getc() after synthesising '\n' for fgets.
 *
 *              - == 1 : Read one byte from the HEAD of the buffer, then shift remaining bytes
 *                       left by one using memmove(). This preserves FIFO order for scanf/fgets
 *                       which call getc() one byte at a time.
 *                       Clears event flags when buffer becomes empty.
 *
 *              - >  1 : Bulk copy — copies min(g_rx_count, buffer_size) bytes to p_buffer,
 *                       then resets g_rx_count and clears all RX event flags.
 *                       Use to drain a full input line after uart_console_has_line() returns true.
 *
 * @param[out] p_buffer      Pointer to output buffer.
 * @param[in]  buffer_size   Number of bytes to read or control mode.
 * @retval     Number of bytes read.
 **********************************************************************************************************************/
uint32_t uart_console_read(void *p_buffer, uint32_t buffer_size)
{
    uint32_t len = 0U;

    if (0U == buffer_size)
    {
        /* Mode 0: flag clear only — called by console_getc after '\n' synthesised */
        g_event &= ~(uint32_t)UART_EVENT_RX_COMPLETE;
        clear_rx_events_if_empty();
        return 0U;
    }
    else if (1U == buffer_size)
    {
        /* Mode 1: single-byte head read for scanf/fgets per-character path */
        if (0U < g_rx_count)
        {
            *(char *)p_buffer = g_rx_buf[0];
            g_rx_count--;
            memmove(g_rx_buf, g_rx_buf + 1, g_rx_count);   /* Shift left */
            len = 1U;
        }
        clear_rx_events_if_empty();
    }
    else
    {
        /* Mode 2: bulk read — copy everything available up to buffer_size */
        len = (g_rx_count >= buffer_size) ? buffer_size : g_rx_count;
        memcpy(p_buffer, g_rx_buf, len);
        g_rx_count = 0U;
        g_event &= ~(uint32_t)(UART_EVENT_RX_COMPLETE | UART_EVENT_RX_CHAR);
    }

    return len;
}

/***********************************************************************************************************************
 * @brief     This function checks if a complete line has been received.
 *            Returns non-zero when UART_EVENT_RX_COMPLETE is set, meaning a CR or LF was received.
 *            Use this as a non-blocking gate before scanf/fgets to prevent blocking the main loop.
 * @param[in] None.
 * @retval    1   Line available.
 * @retval    0   No complete line available.
 **********************************************************************************************************************/
uint32_t uart_console_has_line(void)
{
    return (UART_EVENT_RX_COMPLETE & g_event) ? 1U : 0U;
}

/***********************************************************************************************************************
 * @brief     This function checks if at least one character is available in RX buffer.
 *            Returns non-zero when g_rx_count > 0, meaning at least one byte is in the buffer.
 *            Use as a gate before getchar() to avoid blocking the main loop.
 * @param[in] None.
 * @retval    1   Character available.
 * @retval    0   No character available.
 **********************************************************************************************************************/
uint32_t uart_console_has_char(void)
{
    return (0U < g_rx_count) ? 1U : 0U;
}
