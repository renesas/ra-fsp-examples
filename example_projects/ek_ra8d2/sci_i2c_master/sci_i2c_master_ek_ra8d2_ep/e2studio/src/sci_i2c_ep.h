/***********************************************************************************************************************
 * File Name    : sci_i2c_ep.h
 * Description  : Contains data structures and functions used in sci_i2c_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2023 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef SCI_I2C_EP_H_
#define SCI_I2C_EP_H_

#include <stdio.h>
#include <stdbool.h>
#include "hal_data.h"

/* Macro for the reset value */
#define RESET_VALUE                             (0x00)

/* Macros for sensor sensitivity */
#define SENSOR_SENSITIVITY                      (0.0039F)
#define SENSOR_RESET_VALUE                      (0.0F)

/* Macros for sensor configuration data */
#define SENSOR_ID_DATA                          (0xE5)
#define SENSOR_BW_RATE_DATA                     (0x04)
#define SENSOR_POWER_CTL_DATA                   (0x08)
#define SENSOR_INT_ENABLE_DATA                  (0x80)
#define SENSOR_INT_SOURCE_DATA                  (0x80)

/* Macros for sensor register address */
#define SENSOR_ID_REG                           (0x00)
#define SENSOR_BW_RATE_REG                      (0x2C)
#define SENSOR_POWER_CTL_REG                    (0x2D)
#define SENSOR_INT_ENABLE_REG                   (0x2E)
#define SENSOR_INT_SOURCE_REG                   (0x30)
#define SENSOR_AXIS_DATA_REG                    (0x32)
#define SENSOR_DATA_SIZE                        (6U)
#define X_AXIS_ADDR_OFFSET                      (0U)
#define Y_AXIS_ADDR_OFFSET                      (1U)
#define Z_AXIS_ADDR_OFFSET                      (2U)

/* Macros for data-ready timeout */
#define DATA_TIMEOUT_UNIT                       (BSP_DELAY_UNITS_MILLISECONDS)
#define DATA_TIMEOUT_DIV                        (10U)

/* Macros for I2C transfer */
#define I2C_ONE_BYTE                            (1U)
#define I2C_TWO_BYTE                            (2U)
#define I2C_DATA_BYTE                           (SENSOR_DATA_SIZE)
#define I2C_TIMEOUT_UNIT                        (BSP_DELAY_UNITS_MILLISECONDS)
#define I2C_TIMEOUT_DIV                         (1U)

/* Macros based on specific SCI version */
#if (BSP_PERIPHERAL_SCI_B_PRESENT)
#define MODULE_NAME                             "r_sci_b_i2c"
#define MODULE_PADDING                          "  "
#define SCI_I2C_OPEN(ctrl, cfg)                 (R_SCI_B_I2C_Open((ctrl), (cfg)))
#define SCI_I2C_WRITE(ctrl, src, len, rs)       (R_SCI_B_I2C_Write((ctrl), (src), (len), (rs)))
#define SCI_I2C_READ(ctrl, dest, len, rs)       (R_SCI_B_I2C_Read((ctrl), (dest), (len), (rs)))
#define SCI_I2C_CLOSE(ctrl)                     (R_SCI_B_I2C_Close(ctrl))
#elif (BSP_PERIPHERAL_SCI_PRESENT)
#define MODULE_NAME                             "r_sci_i2c"
#define MODULE_PADDING                          "    "
#define SCI_I2C_OPEN(ctrl, cfg)                 (R_SCI_I2C_Open((ctrl), (cfg)))
#define SCI_I2C_WRITE(ctrl, src, len, rs)       (R_SCI_I2C_Write((ctrl), (src), (len), (rs)))
#define SCI_I2C_READ(ctrl, dest, len, rs)       (R_SCI_I2C_Read((ctrl), (dest), (len), (rs)))
#define SCI_I2C_CLOSE(ctrl)                     (R_SCI_I2C_Close(ctrl))
#endif /* SCI version */

/* Extern variables - defined in sci_i2c_ep.c */
extern volatile i2c_master_event_t g_sci_i2c_event;
extern volatile bool g_b_sensor_ready_flag;

/* Public functions */
fsp_err_t sensor_init(void);
fsp_err_t sensor_get_axis_data(float * p_x_axis, float * p_y_axis, float * p_z_axis);
void sensor_deinit(void);

#endif /* SCI_I2C_EP_H_ */
