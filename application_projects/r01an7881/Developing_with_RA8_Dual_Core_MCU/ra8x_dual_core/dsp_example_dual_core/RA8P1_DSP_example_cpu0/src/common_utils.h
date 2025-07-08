/***********************************************************************************************************************
 * File Name    : common_utils.h
 * Description  : Contains macros and functions used common.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef COMMON_UTILS_H_
#define COMMON_UTILS_H_

/* generic headers */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ipc_squeue.h"
#include "operation_thread.h"

#define NULL_CHAR               ('\0')

#define LVL_ERR                 (1u)       /* error conditions */

#define MODULE_CLOSE            (0U)

#define SERIAL_LOG

/* MENU Banner */
#define LOG_INFO             "\r\n========================================\r\n"\
                             "CMSIS-DSP FFT Example - Result Log\r\n"\
                             "----------------------------------------\r\n"\
                             "FFT Size           : %lu\r\n"\
                             "Sampling Rate      : %lu Hz\r\n"\
                             "Frequency Res.     : %lu Hz\r\n"\
                             "Peak Frequency Bin : %lu\r\n"\
                             "Peak Frequency     : %.2f Hz\r\n"\
                             "Peak Magnitude     : %.2f\n\r\n"\
                             "Reference Bin      : %lu\r\n"\
                             "Test Status        : %s\r\n"\
                             "CPU Cycles Used    : %lu cycles (DWT)\r\n"\
                             "Execution Time     : %.2f (us)\r\n"\
                             "========================================\r\n"

#define APP_PRINT(fn_, ...)             (send_to_print((fn_), ##__VA_ARGS__))

#define APP_CLEAR_ET_SCREEN             (send_to_print("\033[2J"))

#define APP_ERR_PRINT(fn_, ...) ({\
    if(LVL_ERR)\
    {\
        send_to_print("\r\n[ERR] In Function: %s(), %s", __FUNCTION__, (fn_), ##__VA_ARGS__);\
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
        send_to_print("\r\nReturned Error Code: 0x%x  \r\n", (err));\
        __asm("BKPT #0\n");\
    }\
})

/* Function declaration */
fsp_err_t send_to_print(char *format, ...);
#endif /* COMMON_UTILS_H_ */
