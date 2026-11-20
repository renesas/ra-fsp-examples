/***********************************************************************************************************************
 * File Name    : terminal.h
 * Description  : Contains macros, variables, data structures, and functions used in terminal.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef TERMINAL_H_
#define TERMINAL_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "tx_port.h"
#include "filex_sdram_ep.h"
#include "terminal_thread.h"
#include "console/uart_console.h"

/* Macros for the terminal buffer size */
#define TERM_BUFFER_SIZE                (16U)

/* Macros for terminal functionality in the RTOS project */
#if (BSP_CFG_RTOS != 0U)
#if (BSP_CFG_RTOS == 1U)
#define TERM_BYTE_POOL_SIZE             (4096U)
#endif /* BSP_CFG_RTOS == 1U */
#define TERM_OUTPUT_QUEUE_SIZE          (100U)
#define TERM_INPUT_QUEUE_SIZE           (100U)
#endif /* BSP_CFG_RTOS != 0U */

/* Enumerate for the terminal event */
typedef enum terminal_event
{
    TERMINAL_INPUT_MESSAGE,
    TERMINAL_OUTPUT_EP_BANNER,
    TERMINAL_OUTPUT_EP_INFO,
    TERMINAL_OUTPUT_APP_INFO_STR,
    TERMINAL_OUTPUT_APP_ERR_STR,
    TERMINAL_OUTPUT_APP_ERR_TRAP,
    TERMINAL_OUTPUT_APP_MENU,
    TERMINAL_OUTPUT_APP_MEDIA_PROPERTY,
    TERMINAL_OUTPUT_APP_ENTRY_INFO,
    TERMINAL_OUTPUT_APP_DIR_PROPERTY
} terminal_event_t;

/* Structure for exchanging information between application threads and the terminal thread */
#if (BSP_CFG_RTOS != 0U)
typedef struct terminal_msg
{
    terminal_event_t id;
    uint32_t size;
    uint32_t time;
    CHAR msg[];
} terminal_msg_t;
#endif /* BSP_CFG_RTOS != 0U */

/* External variable */
extern TX_THREAD terminal_thread;

/* Function declarations */
#if (BSP_CFG_RTOS != 0U)
uint32_t term_get_input_queue(char * p_msg, uint32_t * p_size, uint32_t wait);
uint32_t term_send_output_queue(terminal_event_t id, void * const p_data, uint32_t size);
#endif /* BSP_CFG_RTOS != 0U */
void terminal_thread_entry_run(void);
UINT terminal_thread_init_check(void);

#endif /* TERMINAL_H_ */
