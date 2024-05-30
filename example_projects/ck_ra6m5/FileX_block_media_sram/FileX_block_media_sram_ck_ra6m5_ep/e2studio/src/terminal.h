/***********************************************************************************************************************
 * File Name    : terminal.h
 * Description  : Contains data structures and functions used in serial.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2024 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#ifndef TERMINAL_H_
#define TERMINAL_H_

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "common_utils.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define ONE_SLEEP_TICK                          (1U)
#define TERMINAL_SLEEP_TICK                     (1U)

/* Macros for the terminal framework */
#define TERMINAL_BYTE_POOL_NAME     "terminal_byte_pool_name"
#define TERMINAL_BYTE_POOL_SIZE     (4096U)
#define TERMINAL_INPUT_ALIGN        (1U)

/* Macros for the SCI UART handle */
#define TX_MAX_SIZE                 (1024U)
#define RX_MAX_SIZE                 (16U)
#define CHAR_CR                     (0x0D)

/* Macro for main menu */
#define MENU_OPTION                 "\r\nMenu option:"\
                                    "\r\n  1. Format Media."\
                                    "\r\n  2. Create Directory."\
                                    "\r\n  3. Get Directory Properties."\
                                    "\r\n  4. Delete Directory."\
                                    "\r\n  5. Create Empty File."\
                                    "\r\n  6. Write To File."\
                                    "\r\n  7. Read From File."\
                                    "\r\n  8. Delete File."\
                                    "\r\n  User Input:  "

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/
/* Enumerate for the terminal event  */
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
    TERMINAL_OUTPUT_APP_MEDIA_VOLUME_INFO,
    TERMINAL_OUTPUT_APP_ENTRY_INFO,
    TERMINAL_OUTPUT_APP_DIR_PROPERTY
}terminal_event_t;

/* Enumerate for month values */
typedef enum e_month
{
    JANUARY    = 1,
    FEBRUARY   = 2,
    MARCH      = 3,
    APRIL      = 4,
    MAY        = 5,
    JUNE       = 6,
    JULY       = 7,
    AUGUST     = 8,
    SEPTEMBER  = 9,
    OCTOBER    = 10,
    NOVEMBER   = 11,
    DECEMBER   = 12,
}month_t;

/* Enumerate for the Menu screen */
typedef enum e_menu
{
    MENU_MAIN = 0,
    MENU_MEDIA,
    MENU_DIR,
    MENU_FILE,
    MENU_MAX
} menu_t;

/* Structure used to exchange information between application thread and terminal thread */
typedef struct terminal_msg
{
    terminal_event_t id;
    uint32_t size;
    uint32_t time;
    CHAR msg[];
}terminal_msg_t;

/* Structure to store date and time */
typedef struct st_time
{
    UINT month;
    UINT date;
    UINT year;
    UINT hour;
    UINT min;
    UINT sec;
}time_t;

/* Structure to store entry details */
typedef struct st_entry_info
{
    CHAR name[FX_MAX_LONG_NAME_LEN];
    UINT attr;
    ULONG size;
    time_t time;
}entry_info_t;

/* Structure to store directory property details */
typedef struct st_dir_property
{
    UINT file;
    UINT subdir;
    ULONG size;
}dir_property_t;

/***********************************************************************************************************************
 * Public function prototypes
 **********************************************************************************************************************/
#if (USE_VIRTUAL_COM == 1)
UINT serial_init(void);
void serial_deinit(void);
#endif /* USE_VIRTUAL_COM */
UINT terminal_framework_init(void);
UINT terminal_printf(char * format, ...);
UINT terminal_get_input_queue(uint8_t * p_data);
UINT terminal_send_output_queue(terminal_event_t id, uint32_t size, void * const p_data);
UINT terminal_input_handle(void);
UINT terminal_output_handle(void);
UINT terminal_thread_init_check(void);

#endif /* TERMINAL_H_ */
