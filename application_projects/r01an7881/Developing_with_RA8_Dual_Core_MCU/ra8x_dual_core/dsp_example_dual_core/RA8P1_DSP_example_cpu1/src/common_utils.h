/***********************************************************************************************************************
 * File Name    : common_utils.h
 * Description  : Contains macros and functions used common to the EP
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
#include "uart_control.h"
#include "ipc_squeue.h"

/* Macro for ASCII character */
#define CR                      ('\r')
#define LF                      ('\n')

#define NULL_CHAR               ('\0')

#define LVL_ERR                 (1u)       /* error conditions */

#define SET_FLAG                (0x01)

#define MODULE_CLOSE            (0U)

#define SEGGER_INDEX            (0)


#define APP_PRINT(fn_, ...)             (uart_write_message(NULL, 0, (fn_), ##__VA_ARGS__))

#define APP_CLEAR_ET_SCREEN             (uart_write_message(NULL, 0, "\033[2J"))

#define APP_ERR_PRINT(fn_, ...) ({\
    if(LVL_ERR)\
    {\
        uart_write_message(NULL, 0, "\r\n[ERR] In Function: %s(), %s", __FUNCTION__, (fn_), ##__VA_ARGS__);\
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
        uart_write_message(NULL, 0, "\r\nReturned Error Code: 0x%x  \r\n", (err));\
        uart_deinit(&g_uart8);\
        __asm("BKPT #0\n");\
    }\
})


#endif /* COMMON_UTILS_H_ */
