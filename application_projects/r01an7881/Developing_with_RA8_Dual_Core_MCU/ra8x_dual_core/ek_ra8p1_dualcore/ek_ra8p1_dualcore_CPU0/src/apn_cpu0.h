/***********************************************************************************************************************
 * File Name    : apn_cpu0.h
 * Description  : This file contains the declarations for main application,
 *
 * This header file is intended for use with the corresponding implementation
 * in rtc.c.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef APN_CPU0_H_
#define APN_CPU0_H_
#include <stdio.h>
#include "hal_data.h"

/* Coordinate pair for terminal layout positioning */
typedef struct { int x; int y; } pos_t;

#define BUFFER_SIZE_UP                  (2048)  //Size of share memory for output from CPU0 to CPU1
#define BUFFER_SIZE_DOWN                (64)    //Size of share memory for input from CPU1 to CPU0
#define RESET_VALUE                     (0x00)
#define SET_FLAG                        (1)
#define RESET_FLAG                      (0)

/* Application position layout */
#define LOG_POS                         ((pos_t){11,1})  // Log start position: row 10, column 1
#define TSN_STATUS_POS                  ((pos_t){10,1})  // ADC_TSN driver status position start: row 11, column 1
#define DATE_POS                        ((pos_t){1,30})  // Display Date start position: row 1, column 30
#define TIME_POS                        ((pos_t){2,30})  // Display Time start position: row 2, column 30
#define TEMP_POS                        ((pos_t){3,1})   // Display real-time Temperature start position: row 3, column 1

#define APP_ERR_ASSERTION               (1)

#define APP_CLEAR_SCREEN                (ipc_log_send("\033[2J\033[H"))

#define APP_PRINT_INFO(pos,fn_, ...)    (ipc_log_send("\033[s\033[%d;%dH\033[2K"fn_"\033[u",(pos).x,(pos).y, ##__VA_ARGS__))

#define APP_ERR_PRINT(fn_, ...)         (ipc_log_send("\033[%d;%dH\033[2K[ERR] In Function: %s(), %s",LOG_POS.x,LOG_POS.y, __FUNCTION__, (fn_), ##__VA_ARGS__))

#define APP_ERR_RETURN(err, fn_, ...)   ({\
    if(err){\
        APP_ERR_PRINT((fn_), ##__VA_ARGS__);\
        return (err);\
    }\
})

#define APP_ERR_TRAP(err)           ({\
    if(err){\
        __asm("BKPT #0\n");\
    }\
})

#define APP_ASSERT(a)                                   \
    {                                                   \
        if ((a))                                        \
        {                                               \
            (void) 0;                  /* Do nothing */ \
        }                                               \
        else                                            \
        {                                               \
            APP_ERR_PRINT("\r\nInvalid parameter");     \
            APP_ERR_TRAP(APP_ERR_ASSERTION);            \
        }                                               \
    }

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

typedef enum e_tsn_request
{
    STOP_SCAN_TSN = 0,
    SCAN_TSN
}tsn_request_t;

/***********************************************************************************************************************
 * User-defined APIs
 **********************************************************************************************************************/
fsp_err_t smem_output_busy(void);
fsp_err_t ipc_log_send(char *format, ...);
void      apn0_entry(void);

#endif /* APN_CPU0_H_ */
