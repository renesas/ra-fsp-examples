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
#include <string.h>
#include "terminal_thread.h"
#include "console/uart_console.h"

#if (BSP_CFG_RTOS == 1)
#include "tx_api.h"

/* Macro for the terminal byte pool size */
#define TERM_BYTE_POOL_SIZE             (4096U)

/* Enumeration for terminal output events */
typedef enum event_id
{
    TERM_OP_MSG_BANNER,
    TERM_OP_MSG_INFO_STR,
    TERM_OP_MSG_ERR_STR,
    TERM_OP_MSG_ERR_TRAP
} event_id_t;

/* Structure for exchanging information between application threads and the terminal thread */
typedef struct term_msg
{
    event_id_t msg_id;
    CHAR p_msg[];
} term_msg_t;

/* Structure for terminal thread information */
typedef struct
{
    TX_THREAD * thread_ptr;
    UINT state;
    ULONG run_count;
} thread_info_t;

/* External variable */
extern TX_THREAD terminal_thread;

/* Function declarations */
void term_thread_entry_run(void);
void term_thread_init_check(void);
UINT term_print_data(event_id_t id, uint32_t size, void * const p_data);

#endif /* BSP_CFG_RTOS == 1 */

#endif /* TERMINAL_H_ */
