/***********************************************************************************************************************
 * File Name    : common_utils.h
 * Description  : Contains macros, data structures and functions commonly used in the EP
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef COMMON_UTILS_H_
#define COMMON_UTILS_H_

/* Generic headers */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hal_data.h"
/* SEGGER RTT and error related headers */
#include "SEGGER_RTT/SEGGER_RTT.h"

#define BIT_SHIFT_8     (8u)
#define SIZE_64         (64u)

#define LVL_ERR         (1u)       /* Error conditions */
#define LVL_DEBUG       (3u)       /* Debug-level messages */

#define LOG_LEVEL       (LVL_ERR)  /* To see the debug messages, LOG_LEVEL should be set to LVL_DEBUG */

#define RESET_VALUE     (0x00)

#define EP_VERSION      ("1.0")
#define MODULE_NAME     "USBX_HMSC"
#define BANNER_INFO     "\r\n********************************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                           *"\
                        "\r\n*   Example Project Version %s                                                *"\
                        "\r\n*   Flex Software Pack Version  %d.%d.%d                                          *"\
                        "\r\n********************************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and" \
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" module\r\n"

#define SEGGER_INDEX            (0)

#define APP_PRINT(fn_, ...)     (SEGGER_RTT_printf (SEGGER_INDEX,(fn_), ##__VA_ARGS__))

#define APP_ERR_PRINT(fn_, ...) ({\
                                if(LVL_ERR)\
                                SEGGER_RTT_printf (SEGGER_INDEX, "[ERR] In Function: %s(), %s",\
                                __FUNCTION__,(fn_),##__VA_ARGS__);\
                                })

#define APP_ERR_TRAP(err)       ({\
                                if((err)) {\
                                SEGGER_RTT_printf(SEGGER_INDEX, "\r\nReturned Error Code: 0x%x  \r\n", (err));\
                                __asm("BKPT #0\n");} /* Trap upon the error */\
                                })

#define APP_READ(read_data)     (SEGGER_RTT_Read (SEGGER_INDEX, (read_data), sizeof(read_data)))

#define APP_CHECK_DATA          (SEGGER_RTT_HasKey())

#define APP_DBG_PRINT(fn_, ...) ({\
                                if(LOG_LEVEL >= LVL_DEBUG)\
                                SEGGER_RTT_printf (SEGGER_INDEX, "[DBG] In Function: %s(), %s",\
                                __FUNCTION__,(fn_),##__VA_ARGS__);\
                                })

/* Macros for RTT RTOS */
#define AZURE_RTOS  (1)
#define FREERTOS    (2)

#if (BSP_CFG_RTOS == AZURE_RTOS)
#include "tx_api.h"
/* RTOS - data structures and macros */

/* Change or modify this based on your application requirement */
#define BYTE_POOL_SIZE      ((ULONG)2048U)
#define TIME_STAMP_MSG_SIZE (35U)

/* As per SEGGER_RTT_conf.h size */
#define IP_MSG_MAX_SIZE     (BUFFER_SIZE_DOWN-1)
#ifndef BYTE_POOL_SIZE
#error "byte allocated size is essential to use RTT-rtos aware framework"
#else
/* ONLY 50% of POOL memory can be used for output buffer
 * for example - 256 bytes  is pool allocated
 * and SEGGER defined BUFFER_SIZE_UP macro is 1024
 *
 * hence only 128 bytes can be used for output buffer maximum
 * so formula made here is
 * 1) (1024/256) = 4
 * 2)  4*32 = 128 */
#define OP_MSG_MAX_SIZE     ((UINT)( (UINT)(BUFFER_SIZE_UP/BYTE_POOL_SIZE) * (UINT)32U ))
#endif

typedef enum event_id
{
    RTT_INPUT_MESSAGE_TYPE_STR_QUEUED,
    RTT_OUTPUT_MESSAGE_BANNER,
    RTT_OUTPUT_MESSAGE_APP_INFO_STR,
    RTT_OUTPUT_MESSAGE_APP_ERR_STR,
    RTT_OUTPUT_MESSAGE_APP_ERR_TRAP,
    RTT_OUTPUT_MESSAGE_APP_VAL,
    RTT_OUTPUT_READ_DATA,
    RTT_OUTPUT_MESSAGE_APP_PRINT_MENU,
    RTT_OUTPUT_MESSAGE_APP_PRINT_MEDIA_SPACE,
    RTT_OUTPUT_MESSAGE_APP_PRINT_VOLUME_SPACE,
    RTT_OUTPUT_MESSAGE_APP_PRINT_FILE_ENTRIES
}event_id_t;

/* Used to exchange information between application thread and RTT thread */
typedef  __PACKED_STRUCT  rtt_user_display_data
{
    event_id_t msg_id;
    uint32_t msg_data_size;
    uint32_t msg_time;
    CHAR  p_msg[];
}rtt_msg_t;

/* Signature of function passed to tx_thread_info_get */
typedef struct
{
    TX_THREAD*   thread_ptr;
    CHAR *      name;
    UINT        state;
    ULONG       run_count;
    UINT        priority;
    UINT        preemption_threshold;
    ULONG       time_slice;
    TX_THREAD * next_thread;
    TX_THREAD * suspended_thread;
}thread_info_t;

/* Variables */
extern TX_QUEUE g_rtt_op_data_queue;
extern TX_QUEUE g_rtt_ip_data_queue;
extern TX_BYTE_POOL   g_byte_pool;

/* Function definitions */
UINT app_rtt_print_data(event_id_t id, uint32_t size, void * const p_data);
UINT check_for_RTT_user_IP(uint8_t * p_get_data);
UINT rtt_framework_init(void);
void rtt_thread_init_check(void);
void rtt_thread_service_cleanup(void);

#endif /* RTOS aware RTT data structure and function */

#endif /* COMMON_UTILS_H_ */
