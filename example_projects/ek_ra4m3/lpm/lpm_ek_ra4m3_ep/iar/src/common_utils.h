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
#include "uart_terminal.h"
#include "lpm_ep.h"

/* Type define */

/* Macro definition */
#define LVL_ERR         (1u)           /* error conditions   */

#define RESET_VALUE     (0x00)

#define EP_VERSION      "1.1"
#define MODULE_NAME     "r_lpm"
#define BANNER_VER      "\r\n********************************************************************************"                                                          \
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME " Module                               *"                                                 \
                                                                              "\r\n*   Example Project Version %s                                                *"     \
                                                                              "\r\n*   Flex Software Pack Version  %d.%d.%d                                          *" \
                                                                              "\r\n********************************************************************************"

#define BANNER_INFO     "\r\nRefer to readme.txt file for more details on Example Project and"                                                                                      \
                        "\r\nFSP User's Manual for more information about "MODULE_NAME " driver.\r\n"                                                                               \
                                                                                       "\r\nThe Example Project demonstrates the basic functionality of LPM driver on Renesas \r\n" \
                                                                                       "RA MCUs based on FSP. On successful initialization of necessary hardware modules,\r\n"      \
                                                                                       "the MCU will automatically enter each LPM mode and then wait for a wake-up source\r\n"      \
                                                                                       "trigger, either manually by an external interrupt or automatically by timer.\r\n\n"

#define BANNER_RESET    "\r\n********************************************************************************" \
                        "\r\n*   MCU has been reset from Deep SW Standby mode by internal reset             *" \
                        "\r\n********************************************************************************\r\n"

#define TERMINAL_PRINT(fn_)              (uart_print((uint8_t *) (fn_)))

#define TERMINAL_ERR_RETURN(err, fn_)    ({                                           \
        if (err) {                                                                    \
            if (LVL_ERR) {                                                            \
                uart_error_info_print((uint8_t *) (__FUNCTION__), (uint8_t *) (fn_)); \
            }                                                                         \
            return (err);                                                             \
        }                                                                             \
    })

#define TERMINAL_ERR_HANDLE(err, fn_)    ({                                   \
        if (err) {                                                            \
            handle_error((err), (uint8_t *) __FUNCTION__, (uint8_t *) (fn_)); \
        }                                                                     \
    })

#define APP_ERR_TRAP(err)                ({ \
        if (err) {                          \
            __asm("BKPT #0\n");             \
        }                                   \
    })

#endif                                 /* COMMON_UTILS_H_ */
