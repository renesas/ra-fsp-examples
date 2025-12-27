/***********************************************************************************************************************
 * File Name    : camera_sensor.h
 * Description  : Contains macros, data structures and functions used setup camera sensors.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef CAMERA_SENSOR_H_
#define CAMERA_SENSOR_H_

#include "common_utils.h"

/* Camera devices definition */
#define CAMERA_OV3640                       (0U)
#define CAMERA_OV5640                       (1U)
#if defined(BOARD_RA8D1_EK) || defined(BOARD_RA8E1_FPB)
#define CAMERA_VERSION                      (CAMERA_OV3640)
#elif defined(BOARD_RA8P1_EK)
#define CAMERA_VERSION                      (CAMERA_OV5640)
#endif

/* Camera register value */
typedef struct camera_sensor_reg
{
    uint16_t reg;
    unsigned char val;
} sensor_reg_t;

/* Camera power state */
typedef enum
{
    CAM_POWER_ON  = BSP_IO_LEVEL_LOW,
    CAM_POWER_OFF = BSP_IO_LEVEL_HIGH,
} camera_power_t;

/* Camera registers, information is in the DS */
#define REG_CAM_PIDH                         (0x300A)
#define REG_CAM_PIDH_DEFAULT                 (0x36)
#define REG_CAM_PIDL                         (0x300B)
#define REG_CAM_PIDL_DEFAULT                 (0x4C)
#define REG_CAM_I2C_SLAVE_ADDR               (0x3C)
#define REG_CAM_I2C_SLAVE_ADDR_WRITE         (0x78)
#define REG_CAM_I2C_SLAVE_ADDR_READ          (0x79)

#if defined(BOARD_RA8D1_EK)
#define PIN_CAM_PWR_ON                       (BSP_IO_PORT_07_PIN_04)
#define PIN_CAM_RESET                        (BSP_IO_PORT_07_PIN_05)
#define REG_CAM_RESET_ADDRESS                (0x3012)
#define REG_CAM_RESET_VALUE                  (0x80)

#elif defined(BOARD_RA8E1_FPB)
#define PIN_CAM_PWR_ON                       (BSP_IO_PORT_04_PIN_14)
#define PIN_CAM_RESET                        (BSP_IO_PORT_04_PIN_15)
#define REG_CAM_RESET_ADDRESS                (0x3012)
#define REG_CAM_RESET_VALUE                  (0x80)

#elif defined(BOARD_RA8P1_EK)
#define PIN_CAM_PWR_ON                       (BSP_IO_PORT_FF_PIN_FF) /* CAM_PWR_ON is not assigned to
                                                                      * any pin by default in schematic */
#define PIN_CAM_RESET                        (BSP_IO_PORT_07_PIN_09)
#define REG_CAM_RESET_ADDRESS                (0x3008)
#define REG_CAM_RESET_VALUE                  (0x80)
#endif /* PIN_CAM DEFINE */

#define REG_CAM_I2C_TIMEOUT_UNIT             (10U)
#define REG_CAM_END_OF_ARRAY                 (0xFFFF)
#define REQUEST_SOFTWARE_WAIT                (0xAAAA)

/* Camera test pattern */
#define CAM_TEST_PATTERN                     (1U)
#define NUM_OF_COLOR                         (8U)

#define MATCH_RATE_MIN                       (99.0F)
#define MATCH_RATE_STRING_LEN_MAX            (10U)

/* Macros for image resolution */
#if defined (BOARD_RA8E1_FPB)
#define QVGA_WIDTH                           (320U)
#define QVGA_HEIGHT                          (240U)
#else
#define VGA_WIDTH                            (640U)
#define VGA_HEIGHT                           (480U)

#define SXGA_WIDTH                           (1280U)
#define SXGA_HEIGHT                          (960U)
#endif

#define CAMERA_ACTIVE_IMAGE_WIDTH            (SXGA_WIDTH)
#define CAMERA_ACTIVE_IMAGE_HEIGHT           (SXGA_HEIGHT)
#define BYTE_PER_PIXEL                       (2U)

/* Functions declarations */
fsp_err_t camera_open(void);
fsp_err_t camera_set_resolution(sensor_reg_t const *p_array);

#endif /* CAMERA_SENSOR_H_ */
