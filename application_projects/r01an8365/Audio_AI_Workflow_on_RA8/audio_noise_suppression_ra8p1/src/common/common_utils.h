/***********************************************************************************************************************
 * File Name    : common_utils.h
 * Description  : Contains macros, data structures, and functions commonly used in the project.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef COMMON_COMMON_UTILS_H_
#define COMMON_COMMON_UTILS_H_

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "hal_data.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
/* Macros for the terminal interface */
#include <SEGGER/SEGGER_RTT/SEGGER_RTT.h>
#define SEGGER_INDEX                  (0)
#define TERM_BUFFER_SIZE              (BUFFER_SIZE_DOWN)
#define TERM_INIT()                   (0)
#define TERM_PRINTF(fmt, ...)         (SEGGER_RTT_printf(SEGGER_INDEX, (fmt), ##__VA_ARGS__))
#define TERM_READ(buf, len)           (SEGGER_RTT_Read(SEGGER_INDEX, (buf), (len)))
#define TERM_HAS_DATA()               (SEGGER_RTT_HasData(SEGGER_INDEX))
#define TERM_HAS_KEY()                (SEGGER_RTT_HasKey())
#define TERM_DEINIT()                 /* No de-initialization needed for SEGGER RTT */

/* Macros commonly used */
#define LVL_ERR                         (1U)       /* Error conditions */
#define RESET_VALUE                     (0x00)
#define NULL_CHAR                       ('\0')
#define MODULE_CLOSE                    (0U)

#define APP_PRINT(fn_, ...)             (TERM_PRINTF((fn_), ##__VA_ARGS__))

#if LVL_ERR
  #define APP_ERR_PRINT(fn_, ...)       (APP_PRINT("\r\n[ERR] In Function: %s(), %s", __FUNCTION__, \
                                                   (fn_), ##__VA_ARGS__))
#else
  #define APP_ERR_PRINT(fn_, ...)
#endif /* LVL_ERR */

#define APP_ERR_RET(con, err, fn_, ...) ({\
                                        if (con)\
                                        {\
                                        APP_ERR_PRINT((fn_), ##__VA_ARGS__); \
                                        return (err); \
                                        }\
                                        })

#define ERROR_TRAP                      ({ \
                                        __asm("BKPT #0\n"); \
                                        })

/* Trap upon the error */
#define APP_ERR_TRAP(err)               ({\
                                        if(err)\
                                        {\
                                        APP_PRINT("\r\nReturned Error Code: 0x%x  \r\n", (err));\
                                        TERM_DEINIT(); \
                                        ERROR_TRAP; \
                                        }\
                                        })

#define APP_READ(buf, len)              (TERM_READ(buf, len))

#define APP_CHECK_DATA                  (TERM_HAS_DATA())

#define APP_CHECK_KEY                   (TERM_HAS_KEY())

#endif /* COMMON_COMMON_UTILS_H_ */
