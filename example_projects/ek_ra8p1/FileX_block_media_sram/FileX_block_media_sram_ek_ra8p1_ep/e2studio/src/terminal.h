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

#include "filex_sram_ep.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "tx_port.h"
#include "terminal_thread.h"
#include "console/uart_console.h"
#if (BSP_CFG_RTOS == 1)
#include "tx_api.h"

/* Macro for the maximum size of a user input message, including the null char */
#define TERM_IP_MAX_SIZE                (UART_CONSOLE_RX_BUFFER_SIZE + 1U)

/* Macro for the terminal byte pool size */
#define TERM_BYTE_POOL_SIZE             (4096U)

/* Macros for printing info, error and trapping the error */
#define PRINT_INFO_STR(str)             (term_print_data(TERM_OP_MSG_INFO_STR, sizeof(str), (str)))
#define PRINT_ENTRY_INFO(inf)           (term_print_data(TERM_OP_MSG_ENTRY_INFO, sizeof(entry_info_t), &(inf)))
#define PRINT_DIR_PROPERTY(pr)          (term_print_data(TERM_OP_MSG_DIR_PROPERTY, sizeof(dir_property_t), &(pr)))
#define TERM_ERR_TRAP(cond, err, str)   ({\
                                        if (cond)\
                                        {\
                                           term_print_data(TERM_OP_MSG_ERR_STR, sizeof(str), (str));\
                                           term_print_data(TERM_OP_MSG_ERR_TRAP, sizeof(err), &(err));\
                                           tx_thread_suspend(tx_thread_identify());\
                                        }\
                                        })

/* Enumeration for terminal events */
typedef enum event_id
{
    TERM_IP_MSG_STR,
    TERM_OP_MSG_BANNER,
    TERM_OP_MSG_INFO_STR,
    TERM_OP_MSG_ERR_STR,
    TERM_OP_MSG_ERR_TRAP,
    TERM_OP_MSG_MENU,
    TERM_OP_MSG_ENTRY_INFO,
    TERM_OP_MSG_DIR_PROPERTY
} event_id_t;

/* Structure for exchanging information between application threads and the terminal thread */
typedef struct term_msg
{
    event_id_t msg_id;
    uint32_t msg_data_size;
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
UINT term_ip_get(CHAR * const p_buf, uint32_t size);

#endif /* BSP_CFG_RTOS == 1 */
#endif /* TERMINAL_H_ */
