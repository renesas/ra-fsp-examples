/***********************************************************************************************************************
 * File Name    : apn_cpu1.h
 * Description  : Contains macros and functions used common.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef APN_CPU1_H_
#define APN_CPU1_H_

/* generic headers */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hal_data.h"
#include "uart_control.h"

/* Used for cursor control in terminal */
typedef struct { int x; int y; } pos_t;

#define NULL_CHAR               ('\0')

/* ASCII in Hex value is CORE0. Define message from CORE0 */
#define CORE_0_MESSAGE            ((char *) 0x636F726530)

#define LVL_ERR      (1u)               /* error conditions */

#define ERR_INVALID_USER_INPUT  (-1)

/* User input option */
#define SET_DATE_TIME           (1)
#define SET_ALARM               (2)
#define START_STOP_ADC          (3)

#define SET_FLAG                (0x01)
#define RESET_FLAG              (0X00)

#define SET_VALUE               (0x01)
#define RESET_VALUE             (0x00)
#define MODULE_CLOSE            (0U)

#define BUFFER_SIZE_UP          (2048u)
#define BUFFER_SIZE_DOWN        (32u)

#define LOG_POS                 ((pos_t){11,1})  // row 10, column 1
#define TSN_STATUS_POS          ((pos_t){10,1})  // row 11, column 1
#define DATE_POS                ((pos_t){1,30})  // row 1, column 30
#define TIME_POS                ((pos_t){2,30})  // row 2, column 30
#define TEMP_POS                ((pos_t){3,1})   // row 3, column 1
#define BANNER_POS              ((pos_t){4,1})   // row 4, column 1

/* MENU Banner */
#define MENU_INFO              "1 :  Set RTC Calendar Date and Time.\r\n"\
                               "2 :  Set RTC alarm to toggle the LED at a specific second.\r\n"\
                               "3 :  Stop/Start Scan Temperature\r\n"\
                               "User Input :  \r\n"

#define BANNER_OPTION_1        "Setting RTC Date and Time \r\n"\
                               "\r\nEnter Date and Time in DD:MM:YYYY HH:MM:SS format."\
							   "\r\nSample Input: 25:06:2025 15:25:05 \r\n<< "

#define BANNER_OPTION_2        "Setting Calendar Alarm for RTC."\
                               "\r\nEnter seconds value (Format: SS) for which recursive alarm has to be set. "\
							   "\r\nPlease input seconds value in the range between 00 and 59.\r\nSample Input: 35\r\n"

#define SHOW_SCREEN(a)          {APP_CLEAR_BANNER;\
		                        if (MAIN_SCREEN == a)\
		                        {   APP_PRINT(BANNER_POS,MENU_INFO);\
		                        }\
								else if(RTC_INPUT_SCREEN == a) \
								{\
                                    APP_PRINT(BANNER_POS,BANNER_OPTION_1);\
								}\
								else if(SET_ALARM_SCREEN == a)\
                                {\
		                            APP_PRINT(BANNER_POS,BANNER_OPTION_2);\
                                }}
#define SEGGER_INDEX                    (0)


#define APP_PRINT(pos,fn_, ...)         (uart_write_message("\033[%d;%dH"fn_,(pos).x,(pos).y, ##__VA_ARGS__))

#define APP_PRINT_INFO(pos,fn_, ...)    (uart_write_message("\033[s\033[%d;%dH\033[2K"fn_"\033[u",(pos).x,(pos).y, ##__VA_ARGS__))

#define APP_CLEAR_SCREEN                (uart_write_message("\033[2J\033[H"))

#define APP_CLEAR_BANNER                (uart_write_message("\033[9;1H\033[1J"))

#define APP_ERR_PRINT(fn_, ...) ({\
    if(LVL_ERR)\
    {\
        uart_write_message("\033[%d;%dH\033[2K[ERR] In Function: %s(), %s", LOG_POS.x,LOG_POS.y,__FUNCTION__, (fn_), ##__VA_ARGS__);\
    }\
})

#define APP_ERR_RETURN(err, fn_, ...)   ({\
    if(err){\
        APP_ERR_PRINT((fn_), ##__VA_ARGS__);\
        return (err);\
    }\
})

#define APP_ERR_TRAP(err)           ({\
    if(err){\
        uart_write_message("\033[%d;%dH\033[2KReturned Error Code: 0x%x  \r\n", LOG_POS.x,LOG_POS.y, (err));\
        uart_deinit();\
        __asm("BKPT #0\n");\
    }\
})

typedef enum e_state_screen
{
    MAIN_SCREEN = 0,
    RTC_INPUT_SCREEN,
    SET_ALARM_SCREEN,
} statte_screen_t;

/***********************************************************************************************************************
 * User-structure define for share memory
 **********************************************************************************************************************/
typedef struct st_share_memory
{
    /* log data and length send from CPU0 to CPU1 */
    unsigned char buf_out[BUFFER_SIZE_UP];
    size_t length;

    /* Flag notify that RTC time set successfully */
    uint8_t valid_sequent;

    /* Input date time get from user on CPU1*/
    uint8_t time_buf[BUFFER_SIZE_DOWN];
}share_mem_t;

/***********************************************************************************************************************
 * User-defined APIs
 **********************************************************************************************************************/
fsp_err_t smem_input_busy(void);
fsp_err_t smem_output_busy(void);
fsp_err_t ipc_init(const ipc_instance_t *ipc0, const ipc_instance_t *ipc1);
void      apn1_entry(void);

#endif /* APN_CPU1_H_ */
