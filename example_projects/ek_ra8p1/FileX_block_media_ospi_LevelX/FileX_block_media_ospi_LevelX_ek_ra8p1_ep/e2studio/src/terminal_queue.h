/***********************************************************************************************************************
 * File Name    : terminal_queue.h
 * Description  : Contains macros, data structures, and functions for the terminal queue transport used in the EP.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 ***********************************************************************************************************************/

#ifndef TERMINAL_QUEUE_H_
#define TERMINAL_QUEUE_H_

#include <stdint.h>

/* Macro for the reset value */
#define RESET_VALUE             (0x00)

/* Macro for the application/terminal thread sleep tick */
#define TERMINAL_SLEEP_TICK      (1U)

/* Macros for converting size of bytes to string */
#define THOUSAND                 (1000UL)
#define MILLION                  (1000000UL)
#define BILLION                  (1000000000UL)
#define STR_UL64_MAX_LEN         (16U)

/* Macro for the terminal input buffer size */
#define TERM_BUFFER_SIZE         (64U)

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
    TERMINAL_OUTPUT_SECTOR_INFO,
    TERMINAL_OUTPUT_APP_MEDIA_VOLUME_INFO,
    TERMINAL_OUTPUT_APP_ENTRY_INFO,
    TERMINAL_OUTPUT_APP_DIR_PROPERTY
} terminal_event_t;

/* Enumerate for the menu screen */
typedef enum e_menu
{
    MENU_MAIN = 0,
    MENU_FLASH_MEDIA,
    MENU_DIR,
    MENU_FILE,
    MENU_SECTOR,
    MENU_MAX
} menu_t;

/* Macros for terminal queue transport in the RTOS project */
#if (BSP_CFG_RTOS != 0U)
#if (BSP_CFG_RTOS == 1U)
/* Macro for the terminal byte pool size */
#define TERM_BYTE_POOL_SIZE     (4096U)
#endif /* BSP_CFG_RTOS == 1U */

/* Macros for the terminal queue sizes */
#define TERM_OUTPUT_QUEUE_SIZE  (100U)
#define TERM_INPUT_QUEUE_SIZE   (100U)
#endif /* BSP_CFG_RTOS != 0U */

/* Structure for exchanging information between application threads and the terminal thread */
#if (BSP_CFG_RTOS != 0U)
typedef struct st_terminal_msg
{
    uint32_t id;
    uint32_t size;
    uint32_t time;
    char msg[];
} terminal_msg_t;
#endif /* BSP_CFG_RTOS != 0U */

/* Function declarations */
#if (BSP_CFG_RTOS != 0U)
void term_framework_init_check(void);
UINT term_framework_init(void);
UINT term_get_input_queue(char * p_msg, uint32_t * p_size, uint32_t wait);
UINT term_get_output_queue(void ** pp_msg_st, uint32_t wait);
UINT term_send_input_queue(uint32_t id, void * const p_data, uint32_t size);
UINT term_send_output_queue(uint32_t id, void * const p_data, uint32_t size);
#endif /* BSP_CFG_RTOS != 0U */
UINT get_tx_flags(ULONG requested_flags, ULONG *actual_flags_ptr);
UINT set_tx_flags(ULONG requested_flags);
void thread_sleep(ULONG timer_ticks);
void thread_suspend(void);

#endif /* TERMINAL_QUEUE_H_ */
