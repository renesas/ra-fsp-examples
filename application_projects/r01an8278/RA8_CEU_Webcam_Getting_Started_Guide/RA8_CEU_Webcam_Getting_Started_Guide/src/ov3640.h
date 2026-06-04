/***********************************************************************************************************************
 * File Name    : ov3640.h
 * Description  : Description  : Contains macros, data structures and functions used setup OV3640 camera.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
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

/* Functions declarations */
fsp_err_t ov3640_open (sensor_reg_t const *p_array);
fsp_err_t ov3640_set_resolution (sensor_reg_t const *p_array);

#endif /* OV3640_H_ */
