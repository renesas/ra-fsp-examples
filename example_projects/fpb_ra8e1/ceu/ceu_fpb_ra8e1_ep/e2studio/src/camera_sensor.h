/***********************************************************************************************************************
 * File Name    : camera_sensor.h
 * Description  : Contains macros, data structures, and functions used in camera_sensor.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2023 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef CAMERA_SENSOR_H_
#define CAMERA_SENSOR_H_

#include "ceu_ep.h"

/* Macros for camera devices */
#define CAMERA_OV3640                       (0U)
#define CAMERA_OV5640                       (1U)
#if defined (BOARD_RA8D1_EK) || defined (BOARD_RA8E1_FPB)
#define CAMERA_VERSION                      (CAMERA_OV3640)
#elif defined (BOARD_RA8P1_EK) || defined (BOARD_RA8D2_EK)
#define CAMERA_VERSION                      (CAMERA_OV5640)
#endif /* Specific board */

/* Macros for camera power and reset control */
#if defined (BOARD_RA8D1_EK)
#define PIN_CAM_PWR_ON                      (BSP_IO_PORT_07_PIN_04)
#define PIN_CAM_RESET                       (BSP_IO_PORT_07_PIN_05)
#define REG_CAM_RESET_ADDRESS               (0x3012)
#define REG_CAM_RESET_VALUE                 (0x80)
#elif defined (BOARD_RA8E1_FPB)
#define PIN_CAM_PWR_ON                      (BSP_IO_PORT_04_PIN_14)
#define PIN_CAM_RESET                       (BSP_IO_PORT_04_PIN_15)
#define REG_CAM_RESET_ADDRESS               (0x3012)
#define REG_CAM_RESET_VALUE                 (0x80)
#elif defined (BOARD_RA8P1_EK) || defined (BOARD_RA8D2_EK)
/* CAM_PWR_ON is not assigned to any pin by default in schematic */
#define PIN_CAM_PWR_ON                      (BSP_IO_PORT_FF_PIN_FF)
#define PIN_CAM_RESET                       (BSP_IO_PORT_07_PIN_09)
#define REG_CAM_RESET_ADDRESS               (0x3008)
#define REG_CAM_RESET_VALUE                 (0x80)
#endif /* Specific board */

/* Macro for I2C timeout */
#define CAM_I2C_TIMEOUT_DELAY               (10U)

/* Macros for special register values */
#define REG_CAM_END_OF_ARRAY                (0xFFFF)
#define REQUEST_SOFTWARE_WAIT               (0xAAAA)

/* Macros for camera test pattern */
#define CAM_TEST_PATTERN                    (0U)
#if defined (BOARD_RA8E1_FPB)
#define MATCH_RATE_MIN                      (70.0F)
#else
#define MATCH_RATE_MIN                      (99.0F)
#endif /* CAM_TEST_PATTERN */

/* Macros for image resolution */
#if defined (BOARD_RA8E1_FPB)
#define QVGA_WIDTH                          (320U)
#define QVGA_HEIGHT                         (240U)
#else
#define VGA_WIDTH                           (640U)
#define VGA_HEIGHT                          (480U)
#define SXGA_WIDTH                          (1280U)
#define SXGA_HEIGHT                         (960U)
#endif /* BOARD_RA8E1_FPB */

#if (CAMERA_VERSION == CAMERA_OV5640)
/* Macros for active image dimensions */
#define CAMERA_ACTIVE_IMAGE_WIDTH           (SXGA_WIDTH)
#define CAMERA_ACTIVE_IMAGE_HEIGHT          (SXGA_HEIGHT)
#endif /* CAMERA_OV5640 */

/* Macro for bytes per pixel */
#define BYTE_PER_PIXEL                      (2U)

/* Camera register configuration */
typedef struct camera_sensor_reg
{
    uint16_t reg;
    unsigned char val;
} sensor_reg_t;

/* Camera power state */
typedef enum
{
    CAM_POWER_ON = BSP_IO_LEVEL_LOW,
    CAM_POWER_OFF = BSP_IO_LEVEL_HIGH,
} camera_power_t;

/* External variables - Defined in camera_sensor.c */
#if defined (BOARD_RA8E1_FPB)
extern const sensor_reg_t g_cam_qvga[];
#else
extern const sensor_reg_t g_cam_sxga[];
extern const sensor_reg_t g_cam_vga[];
#endif /* BOARD_RA8E1_FPB */

#if (CAM_TEST_PATTERN)
#if defined (BOARD_RA8E1_FPB)
extern const uint32_t g_cam_qvga_color_one_line[];
#else
extern const uint32_t g_cam_vga_color_one_line[];
extern const uint32_t g_cam_sxga_color_one_line[];
#endif /* BOARD_RA8E1_FPB */
#endif /* CAM_TEST_PATTERN */

/* Functions declarations */
fsp_err_t camera_open(void);
fsp_err_t camera_set_resolution(sensor_reg_t const * p_array);

#endif /* CAMERA_SENSOR_H_ */
