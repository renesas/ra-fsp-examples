/***********************************************************************************************************************
 * File Name    : terminal.h
 * Description  : Contains macros, declarations of data structures and functions used in terminal.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef TERMINAL_H_
#define TERMINAL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "hal_data.h"
#include "tx_api.h"
#include "console/uart_console.h"

/* Macro for the reset value */
#define RESET_VALUE                 (0x00)

/* Macro for the NULL char value */
#define NULL_CHAR                   ('\0')

#define TERMINAL_QUEUE              "terminal_op_queue"
#define DHCPV6_SERVER_QUEUE         "dhcpv6_thread_input_queue"
#define BYTE_POOL_NAME              "terminal_processing_block"

/* Macros for the debug print gate. Set LOG_LEVEL to LVL_DEBUG to enable DBG_PRINT */
#define LVL_ERR                     (1U)
#define LVL_DEBUG                   (3U)
#define LOG_LEVEL                   (LVL_ERR)

#define DBG_PRINT(fn_)              ({\
                                    if (LOG_LEVEL >= LVL_DEBUG)\
                                    {\
                                        printf("[DBG] In Function: %s(), %s", __FUNCTION__, (fn_));\
                                    }\
                                    })

/* Macros to print info, error and trap the error */
#define PRINT_INFO_STR(str)         (app_terminal_print_data(TERMINAL_OUTPUT_MESSAGE_APP_INFO_STR, sizeof(str), (str)))
/* strlen is used instead of sizeof because str may be a const char * rather than a string literal */
#define PRINT_ERR_STR(str)          (app_terminal_print_data(TERMINAL_OUTPUT_MESSAGE_APP_ERR_STR,\
                                                             (strlen(str) + 1U), (void *)(str)))

/* Macro to queue the error code for the terminal thread. err must be an lvalue */
#define ERROR_TRAP(err)             (app_terminal_print_data(TERMINAL_OUTPUT_MESSAGE_APP_ERR_TRAP, sizeof(err), &(err)))

/* Macros for terminal RTOS */
#define AZURE_RTOS                  (1U)
#define FREERTOS                    (2U)

#if (BSP_CFG_RTOS == AZURE_RTOS)
/* RTOS - data structures and macros */

/* Change or modify this based on your application requirement */
#define BYTE_POOL_SIZE              ((ULONG)256U)
#define TIME_STAMP_MSG_SIZE         (35U)

/* Macro for the maximum size of a user input message */
#define IP_MSG_MAX_SIZE             (16U)

typedef enum event_id
{
    TERMINAL_INPUT_MESSAGE_TYPE_STR_QUEUED,
    TERMINAL_OUTPUT_MESSAGE_BANNER,
    TERMINAL_OUTPUT_MESSAGE_APP_INFO_STR,
    TERMINAL_OUTPUT_MESSAGE_APP_ERR_STR,
    TERMINAL_OUTPUT_MESSAGE_APP_ERR_TRAP,
    TERMINAL_OUTPUT_MESSAGE_APP_PRINT_MENU,
    TERMINAL_OUTPUT_MESSAGE_APP_PRINT_CLIENT_DATA,
}event_id_t;

/* Used to exchange information between application thread and terminal thread */
typedef __PACKED_STRUCT terminal_user_display_data
{
    event_id_t  msg_id;
    uint32_t    msg_data_size;
    uint32_t    msg_time;
    CHAR        p_msg[];
} terminal_msg_t;

/* Signature of function passed to tx_thread_info_get */
typedef struct
{
    TX_THREAD*  thread_ptr;
    CHAR *      name;
    UINT        state;
    ULONG       run_count;
    UINT        priority;
    UINT        preemption_threshold;
    ULONG       time_slice;
    TX_THREAD * next_thread;
    TX_THREAD * suspended_thread;
}thread_info_t;
#endif /* BSP_CFG_RTOS */

/* Extern variables */
extern TX_QUEUE g_terminal_op_data_queue;
extern TX_QUEUE g_terminal_ip_data_queue;
extern TX_BYTE_POOL g_byte_pool;
extern TX_THREAD terminal_thread;

/* Function declarations */
UINT terminal_framework_init(VOID);
UINT app_terminal_print_data(event_id_t id, uint32_t size, void * const p_data);
UINT memory_allocate_terminal(TX_BYTE_POOL * pool, terminal_msg_t ** p_buf, uint32_t size);
UINT check_for_terminal_user_IP(char * const p_get_data);
VOID terminal_thread_init_check(VOID);
VOID terminal_thread_service_cleanup(VOID);

#endif /* TERMINAL_H_ */
