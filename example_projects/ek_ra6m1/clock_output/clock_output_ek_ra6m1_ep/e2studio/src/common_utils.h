/***********************************************************************************************************************
 * File Name    : common_utils.h
 * Description  : Contains macros, data structures and functions used  common to the EP
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
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
#if (USE_VIRTUAL_COM == 1)
#include "SERIAL_TERM/serial_terminal.h"
#else
#include "SEGGER_RTT/SEGGER_RTT.h"
#endif /* USE_VIRTUAL_COM */

#define LVL_ERR         (1U)           /* Error conditions   */
#define LVL_DEBUG       (3U)           /* Debug-level messages */

#define LOG_LEVEL       (LVL_ERR)      /* To See the Debug Messages, LOG_LEVEL should be set to LVL_DEBUG */

#define RESET_VALUE     (0x00)
#define NULL_CHAR       ('\0')

#define MODULE_CLOSE    (0U)

#define EP_VERSION      ("1.0")

#define MODULE_NAME     "Multiple Clocks Output"

#define BANNER_INFO     "\r\n*********************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module   *"\
                        "\r\n*   Example Project Version %s                                     *"\
                        "\r\n*   Flex Software Pack Version  %d.%d.%d                               *"\
                        "\r\n*********************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and" \
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" module\r\n"

/* Macros when using virtual COM */
#if (USE_VIRTUAL_COM == 1)
/* Macro for error LED */
#define ERR_LED                         (0U)

#define APP_PRINT(fn_, ...)             (serial_write_message((fn_), ##__VA_ARGS__))

#define APP_ERR_PRINT(fn_, ...) ({\
    if(LVL_ERR)\
    {\
        serial_write_message("[ERR] In Function: %s(), %s", __FUNCTION__, (fn_), ##__VA_ARGS__);\
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
        serial_write_message("\r\nReturned Error Code: 0x%x  \r\n", (err));\
        uart_deinit();\
        __asm("BKPT #0\n");\
    }\
})

/* Macros when using J-link RTT Viewer */
#else
#define SEGGER_INDEX            (0)

/* Define APP_PRINT */
#define APP_PRINT(fn_, ...)     (SEGGER_RTT_printf (SEGGER_INDEX,(fn_), ##__VA_ARGS__))

/* Define APP_ERR_PRINT */
#define APP_ERR_PRINT(fn_, ...) ({\
    if(LVL_ERR)\
    {\
        SEGGER_RTT_printf (SEGGER_INDEX, "[ERR] In Function: %s(), %s", __FUNCTION__, (fn_), ##__VA_ARGS__);\
    }\
})

/* Define APP_ERR_RETURN */
#define APP_ERR_RETURN(err, fn_, ...)   ({\
    if(err){\
        APP_ERR_PRINT((fn_), ##__VA_ARGS__);\
        return (err);\
    }\
})

/* Define APP_ERR_TRAP */
#define APP_ERR_TRAP(err)  ({\
    if(err) {\
        SEGGER_RTT_printf(SEGGER_INDEX, "\r\nReturned Error Code: 0x%x  \r\n", (err));\
        __asm("BKPT #0\n");\
    }\
})

/* Define APP_READ */
#define APP_READ(buf)           (SEGGER_RTT_Read (SEGGER_INDEX, (buf), BUFFER_SIZE_DOWN))

/* Define APP_CHECK_DATA */
#define APP_CHECK_DATA          (SEGGER_RTT_HasKey())

/* Define APP_DBG_PRINT */
#define APP_DBG_PRINT(fn_, ...) ({\
    if(LOG_LEVEL >= LVL_DEBUG)\
        SEGGER_RTT_printf (SEGGER_INDEX, "[DBG] In Function: %s(), %s",__FUNCTION__,(fn_),##__VA_ARGS__);\
})
#endif /* USE_VIRTUAL_COM */
/* Macro for handle error */
#define APP_ERR_HANDLE(err, fn_)   ({\
    if(err){\
        handle_error((err), (uint8_t *)(fn_));\
    }\
})

#endif /* COMMON_UTILS_H_ */
