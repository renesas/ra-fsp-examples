/***********************************************************************************************************************
 * File Name    : common_utils.h
 * Description  : Contains macros, data structures and functions used  common to the EP
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
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
/* SEGGER RTT and error related headers */
#include "SEGGER_RTT/SEGGER_RTT.h"


#define BIT_SHIFT_8  (8u)
#define SIZE_64      (64u)

#define LVL_ERR      (1u)       /* error conditions   */

#define RESET_VALUE             (0x00)
#define EP_INFO  "\r\nThis Example Project demonstrates the basic functionalities of the MIPI DSI device driver\r\n"\
                  "on Renesas RA MCUs based on Renesas FSP. On successful initialization of MIPI DSI module,\r\n"\
                  "the EP will display 8-color bars on the MIPI LCD (external LCD connected to RA MCU) using\r\n"\
                  "the graphics and MPI-DSI modules in FSP. A user menu will be provided over the RTT Viewer.\r\n"\
                  "User can choose the time to enter Ultra-Low-Power State(ULPS) and touch the screen to exit\r\n"\
                  "this mode. Status information and error messages will be printed on Jlink RTT Viewer during\r\n"\
                  "the execution of the project.\r\n\n "\


#define EP_VERSION              ("1.1")
#define MODULE_NAME             "MIPI DSI"
#define BANNER_INFO             "\r\n********************************************************************************"\
                                "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                            *"\
                                "\r\n*   Example Project Version %s                                                *"\
                                "\r\n*   Flex Software Pack Version  %d.%d.%d                                          *"\
                                "\r\n********************************************************************************"\
                                "\r\nRefer to readme.txt file for more details on Example Project and" \
                                "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define SEGGER_INDEX            (0)

#define APP_PRINT(fn_, ...)     (SEGGER_RTT_printf (SEGGER_INDEX,(fn_), ##__VA_ARGS__))

#if(LVL_ERR)
    #define APP_ERR_PRINT_FUNC_NAME (APP_PRINT("[ERR] In Function: %s(), ", __FUNCTION__))

    #define APP_ERR_PRINT(fn_, ...) ({\
        APP_ERR_PRINT_FUNC_NAME;\
        APP_PRINT((fn_), ##__VA_ARGS__);\
    })

    #define APP_ERR_RETURN(err, fn_)   ({\
    fsp_err_t err_return = (err);\
    if(err_return)\
    {\
        APP_ERR_PRINT(fn_);\
        return err_return;\
    }\
})
#else
    #define APP_ERR_PRINT(fn_, ...)

    #define APP_ERR_RETURN(err, fn_)
#endif


#define APP_ERR_TRAP(err)   ({\
    fsp_err_t a = (err);\
    if(a)\
    {\
        SEGGER_RTT_printf(SEGGER_INDEX, "\r\nReturned Error Code: 0x%x \r\n", a);\
        __asm("BKPT #0\n");\
    }\
}) /* trap upon the error */

#define APP_READ(read_data)     (SEGGER_RTT_Read (SEGGER_INDEX, (read_data), BUFFER_SIZE_DOWN))

#define APP_CHECK_DATA          (SEGGER_RTT_HasKey())


#endif /* COMMON_UTILS_H_ */
