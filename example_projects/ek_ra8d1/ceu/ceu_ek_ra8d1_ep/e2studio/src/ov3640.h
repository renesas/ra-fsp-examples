/***********************************************************************************************************************
 * File Name    : ov3640.h
 * Description  : Description  : Contains macros, data structures and functions used setup OV3640 camera.
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
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#ifndef OV3640_H_
#define OV3640_H_

#include "common_utils.h"

/* OV3640 Register Value */
typedef struct ov3640_sensor_reg {
    uint16_t reg;
    uint8_t val;
} sensor_reg_t;

/* OV3640 Power State */
typedef enum
{
    OV3640_POWER_ON     = BSP_IO_LEVEL_LOW,
    OV3640_POWER_OFF    = BSP_IO_LEVEL_HIGH,
} ov3640_power_t;

/* OV3640 registers, information is in the DS */
#define OV3640_PIDH                         (0x300A)
#define OV3640_PIDH_DEFAULT                 (0x36)
#define OV3640_PIDL                         (0x300B)
#define OV3640_PIDL_DEFAULT                 (0x4C)
#define OV3640_I2C_SLAVE_ADDR               (0x3C)
#define OV3640_I2C_SLAVE_ADDR_WRITE         (0x78)
#define OV3640_I2C_SLAVE_ADDR_READ          (0x79)

#define OV3640_CAM_PWR_ON                   (BSP_IO_PORT_07_PIN_04)
#define OV3640_CAM_RESET                    (BSP_IO_PORT_07_PIN_05)

#define OV3640_RESET_ADDRESS                (0x3012)
#define OV3640_RESET_VALUE                  (0x80)
#define OV3640_I2C_TIMEOUT_UNIT             (10)
#define OV3640_END_OF_ARRAY                 (0xFFFF)

/* OV3640 test pattern */
#define OV3640_TEST_PATTERN                 (1U)
#define NUM_OF_COLOR                        (8U)
#define COLOR_ONE                           (0xFF82FF82)
#define COLOR_TWO                           (0xFF91FF04)
#define COLOR_THREE                         (0xFE04FEDD)
#define COLOR_FOUR                          (0xEF04EF04)
#define COLOR_FIVE                          (0x79FA79FA)
#define COLOR_SIX                           (0x69FA6927)
#define COLOR_SEVEN                         (0x3A743AFA)
#define COLOR_EIGHT                         (0x2B822B82)
#define MATCH_RATE_MIN                      (99.0F)
#define MATCH_RATE_STRING_LEN_MAX           (10U)

/* Functions declarations */
fsp_err_t ov3640_open (void);
fsp_err_t ov3640_set_resolution (sensor_reg_t const *p_array);

#endif /* OV3640_H_ */
