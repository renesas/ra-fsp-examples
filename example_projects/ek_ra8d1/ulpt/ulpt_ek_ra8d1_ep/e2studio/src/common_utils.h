/***********************************************************************************************************************
 * File Name    : common_utils.h
 * Description  : Contains macros, data structures and functions used  common to the EP
 ***********************************************************************************************************************/
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
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
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

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/
typedef char                    char_t;

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define BIT_SHIFT_8             (8U)
#define SIZE_64                 (64U)

#define LVL_ERR                 (1U)       /* error conditions   */

#define RESET_VALUE             (0x00)

#define EP_VERSION              ("1.0")
#define MODULE_NAME             "r_ulpt"
#define BANNER_INFO             "\r\n******************************************************************" \
		                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                *" \
                                "\r\n*   Example Project Version %s                                  *" \
                                "\r\n*   Flex Software Pack Version  %d.%d.%d                            *" \
                                "\r\n******************************************************************" \
                                "\r\nRefer to readme.txt file for more details on Example Project and" \
                                "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n" \

#define EP_INFO                 "\r\nThis Example Project demonstrates the functionality of ULPT in periodic mode and"\
		                        "\r\none-shot mode. The user selects the period for both one-shot and periodic timers"\
		                        "\r\nas 1 second, 2 seconds, or 3 seconds. After selecting the period, the ULPT1 starts"\
                                "\r\nin periodic mode and toggles the on-board LED when the ULPT1 underflow occurs."\
								"\r\nThe ULPT1 will stop after underflow three times, and the ULPT0 will start in "\
								"\r\none-shot mode after that. The MCU will be put into Deep SW Standby mode and"\
								"\r\nwait for the ULPT0 underflow to reset internally.\r\n"

#define BANNER_RESET            "\r\n******************************************************************"\
                                "\r\n* MCU has been reset from Deep SW Standby mode by internal reset *"\
								"\r\n******************************************************************\r\n"\
								"\r\nA sequence has been completed\r\n"\

#define SEGGER_INDEX                    (0U)

#define APP_PRINT(fn_, ...)             (SEGGER_RTT_printf (SEGGER_INDEX,(fn_), ##__VA_ARGS__))

#define APP_PRINT_WAIT                  ({while (SEGGER_RTT_HasDataUp(SEGGER_INDEX));})

#define APP_ERR_PRINT(fn_, ...)         ({\
    if(LVL_ERR)\
    {\
        SEGGER_RTT_printf (SEGGER_INDEX, "[ERR] In Function: %s(), %s", __FUNCTION__, (fn_), ##__VA_ARGS__);\
    }\
})

#define APP_ERR_RETURN(err, fn_, ...)  ({\
    if(err)\
    {\
        if(LVL_ERR)\
        {\
            SEGGER_RTT_printf (SEGGER_INDEX, "[ERR] In Function: %s(), %s", __FUNCTION__, (fn_), ##__VA_ARGS__);\
        }\
        return err;\
    }\
})

#define APP_ERR_TRAP(err)               ({\
    if(err)\
    {\
        SEGGER_RTT_printf(SEGGER_INDEX, "\r\nReturned Error Code: 0x%x  \r\n", err);\
        __asm("BKPT #0\n"); /* trap upon the error  */ \
    }\
})

#define APP_ERR_HANDLE(err, fn_)        (handle_error (err, __FUNCTION__, fn_))

#define APP_READ(read_data)             (SEGGER_RTT_Read (SEGGER_INDEX, read_data, sizeof(read_data)))

#define APP_CHECK_DATA                  (SEGGER_RTT_HasKey())

#endif /* COMMON_UTILS_H_ */
