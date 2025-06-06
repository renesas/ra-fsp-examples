/***********************************************************************************************************************
 * File Name    : common_utils.h
 * Description  : Contains macros, data structures and functions used  common to the EP
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef COMMON_UTILS_H_
#define COMMON_UTILS_H_

/* Generic headers */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hal_data.h"

/* SEGGER RTT and error related headers */
#include "SEGGER_RTT/SEGGER_RTT.h"

#define BIT_SHIFT_8             (8U)
#define SIZE_64                 (64U)

#define LVL_ERR                 (1U)       /* Error conditions */

#define RESET_VALUE             (0x00)

#define EP_VERSION              "1.1"
/* Macro for the SCI_B version */
#if (BSP_FEATURE_SCI_VERSION == 2U)
#define MODULE_NAME             "r_sci_b_i2c"
#else

#define MODULE_NAME             "r_sci_i2c"
#endif

#define BANNER_1                "\r\n******************************************************************"
#if (BSP_FEATURE_SCI_VERSION == 2U)
#define BANNER_2                "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module           *"
#else
#define BANNER_2                "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module             *"
#endif
#define BANNER_3                "\r\n*   Example Project Version %s                                  *"
#define BANNER_4                "\r\n*   Flex Software Pack Version  %d.%d.%d                            *"
#define BANNER_5                "\r\n******************************************************************"
#define BANNER_6                "\r\nRefer to readme.txt file for more details on Example Project and"\
                                "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO                 "\r\nThe Example Project demonstrates the basic functionality"\
                                " of the SCI I2C driver on Renesas"\
								"\r\nRA MCUs based on FSP. This EP utilizes the PMOD ACL sensor as"\
								" an I2C slave device."\
								"\r\nUpon successful initialization, the MCU will read axis acceleration"\
								" from the PMOD ACL sensor"\
								"\r\nand display it on J-Link RTT Viewer every time new data is detected"\
								" by an external interrupt.\r\n\r\n"

#define SEGGER_INDEX            (0U)

#define APP_PRINT(fn_, ...)     (SEGGER_RTT_printf (SEGGER_INDEX,(fn_), ##__VA_ARGS__))

#define APP_PRINT_WAIT          ({while (SEGGER_RTT_HasDataUp(SEGGER_INDEX));})

#define APP_ERR_PRINT(fn_, ...) ({\
                                if(LVL_ERR)\
                                {\
    	                        SEGGER_RTT_printf (SEGGER_INDEX, "[ERR] In Function: %s(), %s",\
                                __FUNCTION__,(fn_),##__VA_ARGS__);\
                                }\
                                })

#define APP_ERR_RETURN(err, fn_, ...)   ({\
                                        if(err) {\
                                        APP_ERR_PRINT((fn_), ##__VA_ARGS__);\
                                        return (err);}\
                                        })

#define APP_ERR_TRAP(err)       ({\
                                if(err) {\
    	                        SEGGER_RTT_printf(SEGGER_INDEX, "\r\nReturned Error Code: 0x%x  \r\n", err);\
    	                        __asm("BKPT #0\n");}\
                                })

#define APP_READ(read_data)     (SEGGER_RTT_Read (SEGGER_INDEX, (read_data), sizeof(read_data)))

#define APP_CHECK_DATA          (SEGGER_RTT_HasKey())

#endif /* COMMON_UTILS_H_ */
