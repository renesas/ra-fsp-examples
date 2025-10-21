/***********************************************************************************************************************
 * File Name    : common_utils.h
 * Description  : Contains macros, data structures and functions commonly used in the EP.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
 
#ifndef COMMON_UTILS_H_
#define COMMON_UTILS_H_

/* Generic headers */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "hal_data.h"

/* SEGGER RTT and error related headers */
#include "SEGGER_RTT/SEGGER_RTT.h"

#define BIT_SHIFT_8             (8u)
#define BIT_SHIFT_12            (12u)
#define BIT_SHIFT_15            (15u)

#define SIZE_64                 (64u)

#define LVL_ERR                 (1u)       /* Error conditions */

#define RESET_VALUE             (0x00)

/* Macros for voltage conversion and window threshold comparison of ADC results */
#ifndef BOARD_RA6T2_MCK
#define V_REF                   (3.3f)
#ifdef BOARD_RA2A1_EK
#define ADC_16_BIT              (32768u)
#define ADC_H_LMT               (24824u)
#define ADC_L_LMT               (9929u)
#else
#define ADC_12_BIT              (4096u)
#define ADC_H_LMT               (3103u)
#define ADC_L_LMT               (1241u)
#endif /* BOARD_RA2A1_EK */
#endif /* BOARD_RA6T2_MCK not defined */

#define EP_VERSION              ("1.0")

#if (BSP_PERIPHERAL_ADC_D_PRESENT)
#define MODULE_NAME             "r_adc_d"
#elif (BSP_PERIPHERAL_ADC_B_PRESENT)
#define MODULE_NAME             "r_adc_b"
#else
#define MODULE_NAME             "r_adc"
#endif /* BSP_PERIPHERAL_ADC_D_PRESENT */

#define BANNER_1                "\r\n******************************************************************"
#if (BSP_PERIPHERAL_ADC_PRESENT)
#define BANNER_2                "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                 *"
#else
#define BANNER_2                "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module               *"
#endif
#define BANNER_3                "\r\n*   Example Project Version %s                                  *"
#define BANNER_4                "\r\n*   Flex Software Pack Version %d.%d.%d                             *"
#define BANNER_5                "\r\n******************************************************************"
#define BANNER_6                "\r\nRefer to readme.txt file for more details on Example Project and"\
                                "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define SEGGER_INDEX            (0)

#define APP_PRINT(fn_, ...)     (SEGGER_RTT_printf(SEGGER_INDEX,(fn_), ##__VA_ARGS__))

#define APP_ERR_PRINT(fn_, ...) ({\
                                if(LVL_ERR)\
                                SEGGER_RTT_printf(SEGGER_INDEX, "[ERR] In Function: %s(), %s",\
                                __FUNCTION__,(fn_),##__VA_ARGS__);\
                                })

#define APP_ERR_TRAP(err)       ({\
                                if((err)) {\
                                SEGGER_RTT_printf(SEGGER_INDEX, "\r\nReturned Error Code: 0x%x  \r\n", (err));\
                                __asm("BKPT #0\n");} /* Trap upon the error */\
		                        })

#define APP_READ(read_data)     (SEGGER_RTT_Read(SEGGER_INDEX, (read_data), sizeof(read_data)))

#define APP_CHECK_DATA          (SEGGER_RTT_HasKey())

#endif /* COMMON_UTILS_H_ */
