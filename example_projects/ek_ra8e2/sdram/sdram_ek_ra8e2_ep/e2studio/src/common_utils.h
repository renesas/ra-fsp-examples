/***********************************************************************************************************************
 * File Name    : common_utils.h
 * Description  : Contains macros and functions used common to the EP
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

#define NULL_CHAR               ('\0')

#define LVL_ERR                 (1u)       /* Error conditions */

#define RESET_VALUE             (0x00)
#define MODULE_CLOSE            (0U)

#define EP_VERSION              ("1.0")
#define MODULE_NAME             "SDRAM"
#define BANNER_INFO	            "\r\n******************************************************************"\
								"\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                 *"\
								"\r\n*   Example Project Version %s                                  *"\
								"\r\n*   Flex Software Pack Version  %d.%d.%d                            *"\
								"\r\n******************************************************************"\
								"\r\nRefer to readme.txt file for more details on Example Project and" \
        						"\r\nFSP User's Manual for more information about "MODULE_NAME" BSP\r\n"

#define SEGGER_INDEX            (0)

/* Macro when using virtual COM */
#if (USE_VIRTUAL_COM == 1)


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

static inline void UART_ERR_TRAP(void) {
    __asm("BKPT #0\n");
}


/* Macros when using J-link RTT Viewer */
#else

#define APP_PRINT(fn_, ...)             (SEGGER_RTT_printf (SEGGER_INDEX,(fn_), ##__VA_ARGS__))

#define APP_ERR_PRINT(fn_, ...) ({\
    if(LVL_ERR)\
    {\
        SEGGER_RTT_printf (SEGGER_INDEX, "[ERR] In Function: %s(), %s", __FUNCTION__, (fn_), ##__VA_ARGS__);\
    }\
})

#define APP_ERR_RETURN(err, fn_, ...)   ({\
    if(err){\
        APP_ERR_PRINT((fn_), ##__VA_ARGS__);\
        return (err);\
    }\
})

#define APP_ERR_TRAP(err)  ({\
    if(err) {\
        SEGGER_RTT_printf(SEGGER_INDEX, "\r\nReturned Error Code: 0x%x  \r\n", (err));\
        __asm("BKPT #0\n");\
    }\
})

/* Macro for read data from J-link RTT Viewer */
#define APP_READ(read_data)     (SEGGER_RTT_Read (SEGGER_INDEX, (read_data), sizeof(read_data)))
/* Macro for check has receive any data from J-link RTT Viewer */
#define APP_CHECK_DATA          (SEGGER_RTT_HasKey())

#endif /* USE_VIRTUAL_COM */
/* Macro for handle error */
#define APP_ERR_HANDLE(err, fn_)   ({\
    if(err){\
        handle_error((err), (uint8_t *)(fn_));\
    }\
})

#endif /* COMMON_UTILS_H_ */
