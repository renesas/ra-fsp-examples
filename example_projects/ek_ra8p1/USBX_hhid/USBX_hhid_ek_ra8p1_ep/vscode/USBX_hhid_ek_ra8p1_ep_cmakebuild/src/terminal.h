/***********************************************************************************************************************
 * File Name    : terminal.h
 * Description  : Contains macros, data structures, and functions used in terminal.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef TERMINAL_H_
#define TERMINAL_H_

/* Generic headers */
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "hal_data.h"
#include "terminal_thread.h"
#include "usbx_hhid_ep.h"
#include "usbx_hhid_example_app.h"
#include "console/uart_console.h"
#if (BSP_CFG_RTOS == 1)
#include "tx_api.h"

/* Macro for the terminal byte pool size. It must hold, at once, the largest message of the EP and a few short ones */
#define TERM_BYTE_POOL_SIZE  (2048U)

/* Macros to print info, error and trap the error */
#define PRINT_INFO_STR(str)  ({term_print_data(TERM_OP_MSG_INFO_STR, sizeof(str), (str));})
#define PRINT_ERR_STR(str)   ({term_print_data(TERM_OP_MSG_ERR_STR, sizeof(str), (str));})
#define ERROR_TRAP(err)      ({term_print_data(TERM_OP_MSG_ERR_TRAP, sizeof(UINT *), &(err));})

typedef enum event_id
{
    TERM_OP_MSG_BANNER,
    TERM_OP_MSG_INFO_STR,
    TERM_OP_MSG_ERR_STR,
    TERM_OP_MSG_ERR_TRAP,
    TERM_OP_MSG_KEY_PRESS,
    TERM_OP_MSG_LEFT_BUTTON_PRESS,
    TERM_OP_MSG_RIGHT_BUTTON_PRESS,
    TERM_OP_MSG_MOUSE_POS,
} event_id_t;

/* Structure for exchanging information between application threads and the terminal thread */
typedef struct term_msg
{
    event_id_t msg_id;
    CHAR msg_data[];
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
