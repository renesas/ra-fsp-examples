/***********************************************************************************************************************
 * File Name    : smbus_ep.h
 * Description  : Contains macros, variables, and functions used in smbus_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2024 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef SMBUS_EP_H_
#define SMBUS_EP_H_

#include <stdbool.h>
#include <stddef.h>
#include "hal_data.h"

/* Macro for the reset value */
#define RESET_VALUE                     (0x00)

/* Macros for text color */
#define CTRL_RESET                      "\x1B[0m"
#define CTRL_TEXT_BRIGHT_GREEN          "\x1B[1;32m"

/* Macro for SMBus event reset value */
#define SMBUS_RESET_EVENT               (255U)

/* Macros for SMBus transfer byte counts */
#define SMBUS_THREE_BYTE                (3U)
#define SMBUS_ONE_BYTE                  (1U)

/* Macros for sensor register address */
#define CFG_ADR                         (0x01)
#define DATA_ADR                        (0x00)

/* Macros for configuration register write value. Set resolution as 12-bits and enable PEC */
#define CFG_8BIT_HIGHER                 (0x00)
#define CFG_8BIT_LOWER                  (0x68)
#define CFG_DATA_SIZE                   (3U)
#define TEMP_DATA_SIZE                  (3U)

/* Macros for manipulating and printing the temperature value */
#define FLOAT_RESET_VALUE               (0.0F)
#define TEMPERATURE_INDEX               (0U)
#define FLOAT_DIVISOR                   (16.0F)
#define SHIFT_FOUR_BITS                 (4U)
#define DEGREE_SYMBOL                   "\xB0"
#define PRINT_DELAY                     (2U)

/* Macros for on-board LED */
#if defined (BOARD_RA4W1_EK) || defined (BOARD_RA4T1_MCK) || defined (BOARD_RA6T2_MCK) || defined (BOARD_RA6T3_MCK) ||\
    defined (BOARD_RA8T1_MCK) || defined (BOARD_RA6T1_RSSK) || defined (BOARD_RA8T2_MCK)
#define LED_ON                          (BSP_IO_LEVEL_LOW)
#define LED_OFF                         (BSP_IO_LEVEL_HIGH)
#else
#define LED_ON                          (BSP_IO_LEVEL_HIGH)
#define LED_OFF                         (BSP_IO_LEVEL_LOW)
#endif /* Specific board */

/* Macros based on IIC version */
#if (BSP_PERIPHERAL_IIC_PRESENT)
#define IIC_MASTER_OPEN(ctrl, cfg)      (R_IIC_MASTER_Open((ctrl), (cfg)))
#define IIC_MASTER_CLOSE(ctrl)          (R_IIC_MASTER_Close(ctrl))
#elif (BSP_PERIPHERAL_IIC_B_PRESENT)
#define IIC_MASTER_OPEN(ctrl, cfg)      (R_IIC_B_MASTER_Open((ctrl), (cfg)))
#define IIC_MASTER_CLOSE(ctrl)          (R_IIC_B_MASTER_Close(ctrl))
#endif /* IIC version */

/* Extern variable - used for on-board LED */
extern bsp_leds_t g_bsp_leds;

/* Function declarations */
fsp_err_t sensor_init(void);
fsp_err_t sensor_read_reg(uint8_t adr, uint8_t * p_value);
void sensor_deinit(void);

#endif /* SMBUS_EP_H_ */
