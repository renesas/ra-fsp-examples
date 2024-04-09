/***********************************************************************************************************************
 * File Name    : sau_i2c_master_ep.h
 * Description  : Contains data structures and functions used in sau_i2c_master_ep.c
 **********************************************************************************************************************/
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
 * Copyright (C) 2024 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#ifndef SAU_I2C_MASTER_EP_H_
#define SAU_I2C_MASTER_EP_H_

/* Macro for RTT Viewer */
#define RTT_DELAY_VALUE             (100U)

/* Macro for Accelerometer sensitivity g/LSB */
#define SENSOR_SENSITIVITY          (0.0039F)
#define SENSOR_RESET_VALUE          (0.0F)

/* Macro for Accelerometer configure data */
#define SENSOR_ID_DATA              (0xE5)
#define SENSOR_BW_RATE_DATA         (0x04)
#define SENSOR_POWER_CTL_DATA       (0x08)
#define SENSOR_INT_ENABLE_DATA      (0x80)
#define SENSOR_INT_SOURCE_DATA      (0x80)

/*  Macro for Accelerometer register address */
#define SENSOR_ID_REG               (0x00)
#define SENSOR_BW_RATE_REG          (0x2C)
#define SENSOR_POWER_CTL_REG        (0x2D)
#define SENSOR_INT_ENABLE_REG       (0x2E)
#define SENSOR_INT_SOURCE_REG       (0x30)
#define SENSOR_AXIS_DATA_REG        (0x32)
#define SENSOR_DATA_SIZE            (6U)
#define X_AXIS_ADDR_OFFSET          (0U)
#define Y_AXIS_ADDR_OFFSET          (1U)
#define Z_AXIS_ADDR_OFFSET          (2U)

/* Macro to wait for Accelerometer to activate new data is ready */
#define DATA_TIMEOUT_UNIT           (BSP_DELAY_UNITS_MILLISECONDS)
#define DATA_TIMEOUT_DIV            (10U)

/* Macro for I2C transfer */
#define I2C_ONE_BYTE                (1U)
#define I2C_TWO_BYTE                (2U)
#define I2C_DATA_BYTE               (SENSOR_DATA_SIZE)
#define I2C_TIMEOUT_UNIT            (BSP_DELAY_UNITS_MICROSECONDS)
#define I2C_TIMEOUT_DIV             (1U)
#define I2C_TIMEOUT_PERIOD          (8000U)

/* Macro for printing acceleration value to RTT */
#define SENSOR_STR_SIZE             (64U)

/* Public function declarations */
void sau_i2c_ep_entry(void);

#endif /* SAU_I2C_MASTER_EP_H_ */
