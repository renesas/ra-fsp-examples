/***********************************************************************************************************************
 * File Name    : common_utils.h
 * Description  : Contains macros, data structures and functions used  common to the EP
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
#include "hal_data.h"

/* SEGGER RTT and error related headers */
#include "SEGGER_RTT/SEGGER_RTT.h"


#define BIT_SHIFT_8     (8U)
#define SIZE_64         (64U)

#define LVL_ERR         (1U)       /* error conditions   */

#define RESET_VALUE     (0x00)
#define MODULE_CLOSE    (0U)


#define EP_VERSION      "1.1"
#define MODULE_NAME     "r_sau_i2c"
#define BANNER_INFO     "\r\n******************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module             *"\
                        "\r\n*   Example Project Version %s                                  *"\
                        "\r\n*   Flex Software Pack Version  %d.%d.%d                            *"\
                        "\r\n******************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and" \
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO         "\r\nThe Example Project demonstrates the basic functionality of the SAU I2C driver on Renesas"\
                        "\r\nRA MCUs based on FSP. The project initializes the SAU I2C master module with the standard"\
                        "\r\nrate of 100 kHz and communicates with the PmodACL Board for ADXL345. On power-up, after"\
                        "\r\ninitializing the SAU I2C master module and configuring the ADXL345 sensor, the RA MCU will"\
                        "\r\nread the axis acceleration data from the sensor and display it on the RTT Viewer every time"\
                        "\r\nnew data is detected by an external interrupt.\r\n\r\n"

#define SEGGER_INDEX            (0U)

#define APP_PRINT(fn_, ...)     (SEGGER_RTT_printf (SEGGER_INDEX,(fn_), ##__VA_ARGS__))

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

#define APP_READ(buf)           (SEGGER_RTT_Read (SEGGER_INDEX, (buf), BUFFER_SIZE_DOWN))

#define APP_CHECK_DATA          (SEGGER_RTT_HasKey())

#endif /* COMMON_UTILS_H_ */
