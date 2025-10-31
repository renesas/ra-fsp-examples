/***********************************************************************************************************************
 * File Name    : camera_sensor.h
 * Description  : Contains macros, data structures and functions used setup camera sensors.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef CAMERA_SENSOR_H_
#define CAMERA_SENSOR_H_

#include "user_config.h"
#include "i2c_control.h"

#define CONFIG_TABLE_END_DETECT     (0xFFFF)
#define REQUEST_SOFTWARE_WAIT       (0xAAAA) /* Please ensure if this value is not listed in command list of your camera. */

#define SYS_CTRL0_REG               (0x3008)
#define SYS_CTRL0_SW_PWDN           (0x42)
#define SYS_CTRL0_SW_PWUP           (0x02)
#define SYS_CTRL0_SW_RST            (0x82)

#define SYS_RESET00_REG             (0x3000)
#define SYS_RESET02_REG             (0x3002)
#define SYS_CLK_ENABLE00_REG        (0x3004)
#define SYS_CLK_ENABLE02_REG        (0x3006)
#define IO_MIPI_CTRL00_REG          (0x300e)
#define SYSTEM_CONTROL1_REG         (0x302e)
#define SCCB_SYS_CTRL1_REG          (0x3103)
#define TIMING_TC_REG20_REG         (0x3820)
#define TIMING_TC_REG21_REG         (0x3821)
#define HZ5060_CTRL00_REG           (0x3c00)
#define HZ5060_CTRL01_REG           (0x3c01)
#define ISP_CTRL01_REG              (0x5001)
#define PRE_ISP_TEST_SET1           (0x503d)

#define SC_PLL_CTRL0_REG            (0x3034)
#define SC_PLL_CTRL1_REG            (0x3035)
#define SC_PLL_CTRL2_REG            (0x3036)
#define SC_PLL_CTRL3_REG            (0x3037)
#define SYS_ROOT_DIV_REG            (0x3108)
#define PCLK_PERIOD_REG             (0x4837)

#define AEC_REAL_GAIN_H_REG         (0x350a)
#define AEC_REAL_GAIN_L_REG         (0x350b)
#define AEC_PK_MANUAL_REG           (0x3503)
#define AEC_CTRL0F_REG              (0x3a0f)
#define AEC_CTRL10_REG              (0x3a10)
#define AEC_CTRL11_REG              (0x3a11)
#define AEC_CTRL1B_REG              (0x3a1b)
#define AEC_CTRL1E_REG              (0x3a1e)
#define AEC_CTRL1F_REG              (0x3a1f)

#define BLC_CTRL01_REG              (0x4001)
#define BLC_CTRL04_REG              (0x4004)
#define BLC_CTRL05_REG              (0x4005)

#define FMT_CTRL00_REG              (0x4300)
#define FMT_MUX_CTRL_REG            (0x501f)

#define VFIFO_CTRL0C_REG            (0x460C)
#define PCLK_DIV_REG                (0x3824)

#define AWB_CTRL00_REG              (0x5180)
#define AWB_CTRL01_REG              (0x5181)
#define AWB_CTRL02_REG              (0x5182)
#define AWB_CTRL03_REG              (0x5183)
#define AWB_CTRL04_REG              (0x5184)
#define AWB_CTRL05_REG              (0x5185)
#define AWB_CTRL17_REG              (0x5191)
#define AWB_CTRL18_REG              (0x5192)
#define AWB_CTRL19_REG              (0x5193)
#define AWB_CTRL20_REG              (0x5194)
#define AWB_CTRL21_REG              (0x5195)
#define AWB_CTRL22_REG              (0x5196)
#define AWB_CTRL23_REG              (0x5197)
#define AWB_CTRL30_REG              (0x519e)

#define AWB_MAN_CTRL                (0x3406)
#define AWB_R_GAIN_H                (0x3400)
#define AWB_R_GAIN_L                (0x3401)
#define AWB_G_GAIN_H                (0x3402)
#define AWB_G_GAIN_L                (0x3403)
#define AWB_B_GAIN_H                (0x3404)
#define AWB_B_GAIN_L                (0x3405)

#define TIMING_X_INC                (0x3814)
#define TIMING_Y_INC                (0x3815)
#define TIMING_VS                   (0x3803)
#define TIMING_DVPHO_H              (0x3808)
#define TIMING_DVPHO_L              (0x3809)
#define TIMING_DVPVO_H              (0x380a)
#define TIMING_DVPVO_L              (0x380b)
#define TIMING_HTS_H                (0x380c)
#define TIMING_HTS_L                (0x380d)
#define TIMING_VTS_H                (0x380e)
#define TIMING_VTS_L                (0x380f)

#define SDE_CTRL0_REG               (0x5580)
#define SDE_CTRL1_REG               (0x5581)
#define SDE_CTRL2_REG               (0x5582)
#define SDE_CTRL3_REG               (0x5583)
#define SDE_CTRL4_REG               (0x5584)
#define SDE_CTRL5_REG               (0x5585)
#define SDE_CTRL6_REG               (0x5586)
#define SDE_CTRL7_REG               (0x5587)
#define SDE_CTRL8_REG               (0x5588)
#define SDE_CTRL9_REG               (0x5589)
#define SDE_CTRL10_REG              (0x558a)
#define SDE_CTRL11_REG              (0x558b)

#define DEFAULT_MIPI_CHANNEL        (0)

/* Camera Register Value */
typedef struct camera_sensor_reg {
    uint16_t reg;
    uint8_t val;
} sensor_reg_t;

/* Camera Power State */
typedef enum
{
    CAM_POWER_ON     = BSP_IO_LEVEL_LOW,
    CAM_POWER_OFF    = BSP_IO_LEVEL_HIGH,
} camera_power_t;

/* Camera registers, information is in the DS */
#define REG_CAM_I2C_SLAVE_ADDR               (0x3C)
#define REG_CAM_RESET_ADDRESS                (0x3008)
#define REG_CAM_RESET_VALUE                  (0x80)

#if(defined BOARD_RA8P1_EK)
#define PIN_CAM_RESET                        (BSP_IO_PORT_07_PIN_09)
#endif

/* Camera test pattern */
#define MATCH_RATE_MIN                       (99.0F)
#define MATCH_RATE_STRING_LEN_MAX            (10U)

/* OV5640 Power-down pin is active LOW */
#define CAMERA_RST_PIN_SET(x) do{ \
    R_BSP_PinAccessEnable();  \
    R_BSP_PinWrite(PIN_CAM_RESET, x);  \
    R_BSP_PinAccessDisable(); \
}while(0)

/* MACRO for image resolution */
typedef enum {
    RES_1024x600 = 1,
    RES_VGA,
    RES_QVGA,
    RES_MAX
}e_image_resolution;

typedef struct {
    uint16_t width;
    uint16_t height;
} camera_config_t;

/* Camera input image size */
#define CAMERA_IMAGE_WIDTH          (1024U)
#define CAMERA_IMAGE_HEIGHT         (600U)

#define MIPI_XCLK_HZ                (24000000)
#define PLCK_MAX_HZ                 (96000000)

/* Functions declarations */
fsp_err_t camera_open (void);
fsp_err_t camera_stream_on(void);
fsp_err_t camera_stream_off(void);
fsp_err_t camera_write_array (sensor_reg_t const * p_array);

#endif /* CAMERA_SENSOR_H_ */
