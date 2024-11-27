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

/* generic headers */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hal_data.h"
#if (USE_VIRTUAL_COM == 0)
#include "SEGGER_RTT/SEGGER_RTT.h"
#endif /* USE_VIRTUAL_COM == 0*/

#define NULL_CHAR               ('\0')
#define NEWLINE_CHAR            ('\n')
#define CARRIAGE_CHAR           ('\r')
#define SPACE_CHAR              (' ')

#define LVL_ERR                 (1u)       /* error conditions */

#define RESET_VALUE             (0x00)
#define MODULE_CLOSE            (0U)

#define EP_VERSION              ("1.0")
#define MODULE_NAME             "USB Multiple Host"
#define BANNER_INFO             "\r\n******************************************************************"\
                                "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module     *"\
                                "\r\n*   Example Project Version %s                                  *"\
                                "\r\n*   Flex Software Pack Version  %d.%d.%d                            *"\
                                "\r\n******************************************************************"\
                                "\r\nRefer to readme.txt file for more details on Example Project and" \
                                "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define KEY_SIZE_IN_BYTES        (64)
#define FALSE                    (0)
#define TRUE                     (1)
#define SEGGER_INDEX             (0)

void trim_new_line(char *str);

#if (USE_VIRTUAL_COM == 1)
#define BACKSPACE                (0x08)
#define CARRIAGE_RETURN          (0x0D)
#define MENU_EXIT_CRTL           (0x20)
#define BUFFER_SIZE_UP           (1024)
#define BUFFER_SIZE_DOWN         (32)
#define TRANSFER_LENGTH          (1024)

void jlink_console_init(void);
void Jlink_console_write(char *format, ...);
bool handle_virtual_com_input(bool is_single_key_input);
void uart_deinit(void);

#else
bool handle_rtt_segger_input(bool is_single_key_input);
#endif /* USE_VIRTUAL_COM == 1 */
/* Macro when using virtual COM */
#if (USE_VIRTUAL_COM == 1)

#define APP_PRINT(fn_, ...)                      \
    if (xSemaphoreTake(g_app_print_semaphore, portMAX_DELAY)) { \
        Jlink_console_write((fn_), ##__VA_ARGS__);        \
        xSemaphoreGive(g_app_print_semaphore);                  \
    }                                                     \

#define APP_ERR_PRINT(fn_, ...) ({\
    if(LVL_ERR)\
    {\
        Jlink_console_write("[ERR] In Function: %s(), %s", __FUNCTION__, (fn_), ##__VA_ARGS__);\
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
        Jlink_console_write("\r\nReturned Error Code: 0x%x  \r\n", (err));\
        uart_deinit();\
        __asm("BKPT #0\n");\
    }\
})

#define APP_GET_USER_INPUT(is_single_key_input)                           \
    ({                                                                    \
        bool input_status = false;                                        \
        if (xSemaphoreTake(g_user_input_semaphore, portMAX_DELAY)) {      \
            input_status = handle_virtual_com_input(is_single_key_input); \
            xSemaphoreGive(g_user_input_semaphore);                       \
            vTaskDelay (10);                                              \
        }                                                                 \
        input_status;                                                     \
    })

/* Macro when using J-link RTT Viewer */
#else

#define APP_PRINT(fn_, ...)                      \
    if (xSemaphoreTake(g_app_print_semaphore, portMAX_DELAY)) { \
    	SEGGER_RTT_printf (SEGGER_INDEX,(fn_), ##__VA_ARGS__);        \
        xSemaphoreGive(g_app_print_semaphore);                  \
    }

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
/* Macro for wait data from J-link RTT Viewer */
#define APP_WAIT_DATA           (SEGGER_RTT_WaitKey())

#define APP_GET_USER_INPUT(is_single_key_input)                           \
    ({                                                                    \
        bool input_status = false;                                        \
        if (xSemaphoreTake(g_user_input_semaphore, portMAX_DELAY)) {      \
            input_status = handle_rtt_segger_input(is_single_key_input);  \
            xSemaphoreGive(g_user_input_semaphore);                       \
            vTaskDelay (10);                                              \
        }                                                                 \
        input_status;                                                     \
    })

#endif /* USE_VIRTUAL_COM */

/* Macro for handle error */
#define APP_ERR_HANDLE(err, fn_)   ({\
    if(err){\
        handle_error((err), (uint8_t *)(fn_));\
    }\
})

#endif /* COMMON_UTILS_H_ */
