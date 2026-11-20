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
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "tx_port.h"
#include "hal_data.h"
#include "terminal_thread.h"
#include "netx_crypto_ep.h"
#include "console/uart_console.h"

/* Macros for the input/output buffer size */
#define IN_BUFF_SIZE                (16U)
#define OUT_BUFF_SIZE               (2048U)

/* Macros for RTOS type */
#define AZURE_RTOS                  (1U)
#define FREERTOS                    (2U)

#if (BSP_CFG_RTOS == AZURE_RTOS)
#include "tx_api.h"

/* Change or modify this based on your application requirement */
#ifndef BYTE_POOL_SIZE
#define BYTE_POOL_SIZE              ((ULONG)256U)
#endif /* BYTE_POOL_SIZE */

/* Macro for timestamp string buffer size */
#define TIME_STAMP_MSG_SIZE         (35U)

/* Macro for queue storage size in bytes */
#define TERM_QUEUE_STORAGE_SIZE     (20U)

/* Maximum input string length excluding NULL terminator */
#define IP_MSG_MAX_SIZE             (IN_BUFF_SIZE - 1)

/* Maximum output message size allowed for a terminal output message */
#define OP_MSG_MAX_SIZE             ((UINT)((OUT_BUFF_SIZE / BYTE_POOL_SIZE) * 32U))
#endif /* BSP_CFG_RTOS == AZURE_RTOS */

/* Enumeration of terminal event identifiers */
typedef enum event_id
{
    TERM_INPUT_MESSAGE_TYPE_STR_QUEUED,
    TERM_OUTPUT_MESSAGE_BANNER,
    TERM_OUTPUT_MESSAGE_APP_INFO_STR,
    TERM_OUTPUT_MESSAGE_APP_ERR_STR,
    TERM_OUTPUT_MESSAGE_APP_ERR_TRAP,
    TERM_OUTPUT_MESSAGE_APP_PRINT_MENU
} event_id_t;

/* Structure used to exchange information between the application thread and terminal thread */
typedef __PACKED_STRUCT term_user_display_data
{
    event_id_t msg_id;
    uint32_t msg_data_size;
    uint32_t msg_time;
    CHAR p_msg[];
} term_msg_t;

/* Structure used to store thread information returned by tx_thread_info_get() */
typedef struct
{
    TX_THREAD * thread_ptr;
    CHAR * name;
    UINT state;
    ULONG run_count;
    UINT priority;
    UINT preemption_threshold;
    ULONG time_slice;
    TX_THREAD * next_thread;
    TX_THREAD * suspended_thread;
} thread_info_t;

/* External variable */
extern TX_THREAD terminal_thread;

/* Function declarations */
VOID terminal_thread_entry_run(VOID);
VOID app_term_print_data(event_id_t id, uint32_t size, const VOID * p_data);
UINT check_for_term_user_ip(uint8_t * p_get_data);
VOID term_thread_init_check(VOID);

#endif /* TERMINAL_H_ */
