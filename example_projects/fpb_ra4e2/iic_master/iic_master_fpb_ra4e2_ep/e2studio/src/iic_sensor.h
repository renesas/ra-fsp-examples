/***********************************************************************************************************************
 * File Name    : iic_sensor.h
 * Description  : Contains macros and function declarations used in iic_sensor.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef IIC_SENSOR_H_
#define IIC_SENSOR_H_

#include <stdbool.h>
#include <stddef.h>
#include "hal_data.h"

/* Transfer byte counts */
#define ONE_BYTE                                    (0x01)
#define TWO_BYTE                                    (0x02)

/* Sensor configuration */
#define MEASURE_PAYLOAD_SIZE                        (0x03)
#define SENSOR_READ_DELAY                           (0x03)
#define ENABLE_BIT                                  (0x08)
#define DATA_REGISTERS                              (0x06)

/* Internal accelerometer registers */
#define DEVICE_ID_REG                               (0x00)
#define DEVICE_SIGNATURE                            (0xE5)
#define POWER_CTL_REG                               (0x2D)
#define AXIS_DATA                                   (0x32)

/* Data size constants */
#define SENSOR_DATA_SIZE                            (0x06)
#define ADXL_DATA_SIZE                              (0x07)
#define BIT_SHIFT_8                                 (8u)

#define DEFAULT_AXIS_VALUE                          (0.0f)

#define RESET_VALUE                                 (0x00)

/* Macros for used functions based on IIC version */
#if (BSP_PERIPHERAL_IICA_PRESENT)
#define MODULE_NAME                                 "r_iica_master"
#define MODULE_PADDING                              " "
#define IIC_MASTER_OPEN(ctrl, cfg)                  (R_IICA_MASTER_Open((ctrl), (cfg)))
#define IIC_MASTER_WRITE(ctrl, src, len, restart)   (R_IICA_MASTER_Write((ctrl), (src), (len), (restart)))
#define IIC_MASTER_READ(ctrl, dest, len, restart)   (R_IICA_MASTER_Read((ctrl), (dest), (len), (restart)))
#define IIC_MASTER_CLOSE(ctrl)                      (R_IICA_MASTER_Close(ctrl))
#elif defined(BOARD_RA6T2_MCK) || defined(BOARD_RA4E2_FPB)
#define MODULE_NAME                                 "r_iic_b_master"
#define MODULE_PADDING                              ""
#define IIC_MASTER_OPEN(ctrl, cfg)                  (R_IIC_B_MASTER_Open((ctrl), (cfg)))
#define IIC_MASTER_WRITE(ctrl, src, len, restart)   (R_IIC_B_MASTER_Write((ctrl), (src), (len), (restart)))
#define IIC_MASTER_READ(ctrl, dest, len, restart)   (R_IIC_B_MASTER_Read((ctrl), (dest), (len), (restart)))
#define IIC_MASTER_CLOSE(ctrl)                      (R_IIC_B_MASTER_Close(ctrl))
#elif (BSP_PERIPHERAL_IIC_PRESENT)
#define MODULE_NAME                                 "r_iic_master"
#define MODULE_PADDING                              "  "
#define IIC_MASTER_OPEN(ctrl, cfg)                  (R_IIC_MASTER_Open((ctrl), (cfg)))
#define IIC_MASTER_WRITE(ctrl, src, len, restart)   (R_IIC_MASTER_Write((ctrl), (src), (len), (restart)))
#define IIC_MASTER_READ(ctrl, dest, len, restart)   (R_IIC_MASTER_Read((ctrl), (dest), (len), (restart)))
#define IIC_MASTER_CLOSE(ctrl)                      (R_IIC_MASTER_Close(ctrl))
#endif /* IIC version */

/* Function declarations */
fsp_err_t init_sensor(void);
fsp_err_t read_sensor_data(uint8_t * xyz_data);
void deinit_sensor(void);

#endif /* IIC_SENSOR_H_ */
