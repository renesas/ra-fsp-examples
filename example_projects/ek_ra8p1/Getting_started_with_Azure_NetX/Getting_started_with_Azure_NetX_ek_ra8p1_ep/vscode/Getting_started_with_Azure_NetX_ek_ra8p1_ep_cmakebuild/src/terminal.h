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
#include <string.h>
#include "terminal_thread.h"
#include "console/uart_console.h"

/* Macro for the reset value */
#define RESET_VALUE                     (0x00)

/* Macro for the null char */
#define NULL_CHAR                       ('\0')

#if (BSP_CFG_RTOS == 1)
#include "tx_api.h"

/* Macro for the maximum size of a user input message, including the null char */
#define TERM_IP_MAX_SIZE                (UART_CONSOLE_RX_BUFFER_SIZE + 1U)

/* Macro for the terminal byte pool size. It must hold, at once, the largest message of the EP and a few short ones */
#define TERM_BYTE_POOL_SIZE             (4096U)

/* Macros to print info and error messages and to trap the error */
#define PRINT_INFO_STR(str)             (term_print_data(TERM_OP_MSG_INFO_STR, sizeof(str), (str)))
#define PRINT_ERR_STR(str)              (term_print_data(TERM_OP_MSG_ERR_STR, (strlen(str) + 1U), (void *)(str)))
#define ERROR_TRAP(err)                 ({\
                                        UINT err_code_ = (UINT)(err);\
                                        term_print_data(TERM_OP_MSG_ERR_TRAP, sizeof(err_code_), &err_code_);\
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
    TERM_OP_MSG_NET_INFO,
    TERM_OP_MSG_TCP_SEND_DATA,
    TERM_OP_MSG_TCP_RECV_DATA,
    TERM_OP_MSG_UDP_SEND_DATA,
    TERM_OP_MSG_UDP_RECV_DATA,
    TERM_OP_MSG_DNS_HOST_ADDR,
    TERM_OP_MSG_DNS_HOST_NAME
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
