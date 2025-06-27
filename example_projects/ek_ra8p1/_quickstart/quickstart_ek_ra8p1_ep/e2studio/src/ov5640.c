/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/**********************************************************************************************************************
 * File Name    : ov5640.c
 * Version      : 1.0
 * Description  : Implementation of application interface and system support for the FT5316 Capacitive touch
 *                controller on a LCD panel.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 01.01.2025 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "bsp_api.h"
#include "hal_data.h"
#include "hal_data.h"


#include "camera_thread.h"
#include "common_init.h"
#include "common_utils.h"
#include "board_i2c_master.h"
#include "board_cfg_switch.h"
#include "ov5640_cfg.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
/* OV5640 Power-down pin is active LOW */
#define OV5640_RST_PIN_SET(x) {do{ \
    R_BSP_PinAccessEnable();  \
    R_BSP_PinWrite(OV5640_CAM_RESET, (x));  \
    R_BSP_PinAccessDisable(); \
}while(0);}

/* OV5640 Power-down pin is active HIGH */
#define OV5640_PWDN_PIN_SET(x) {do{ \
    R_BSP_PinAccessEnable();  \
    R_BSP_PinWrite(OV5640_CAM_PWR_ON, (x));  \
    R_BSP_PinAccessDisable();  \
}while(0);}

/* in line delay function caller */
#define delay_ms(x)     (R_BSP_SoftwareDelay((x), BSP_DELAY_UNITS_MILLISECONDS))

/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: ov5640_write_reg
 * Description  : Write a register onthe OV5640 device
 * Return Value : NONE
 *********************************************************************************************************************/
void ov5640_write_reg(uint16_t reg, uint8_t dat)
{
    uint8_t temp      = dat;
    uint8_t read_back = 0;

    R_IIC_MASTER_SlaveAddressSet(&g_board_i2c_master_ctrl, OV5640_I2C_SLAVE_ADDR, I2C_MASTER_ADDR_MODE_7BIT);

    wr_sensor_reg16_8(&g_board_i2c_master_ctrl, reg, temp);

    read_back = ov5640_read_reg(reg);
    if(read_back != dat)
    {
//        __BKPT(0);
    }
    return;
}
/**********************************************************************************************************************
 End of function ov5640_write_reg
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: ov5640_read_reg
 * Description  : Read a register on the OV5640 device
 * Arguments    : Register to read
 * Return Value : Read result
 *********************************************************************************************************************/
uint8_t ov5640_read_reg(uint16_t reg)
{
    uint8_t dat = 0;

        R_IIC_MASTER_SlaveAddressSet(&g_board_i2c_master_ctrl, OV5640_I2C_SLAVE_ADDR, I2C_MASTER_ADDR_MODE_7BIT);
        rd_sensor_reg16_8(&g_board_i2c_master_ctrl, reg, &dat);
    return dat;
}
/**********************************************************************************************************************
 End of function ov5640_read_reg
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: ov5640_hw_init
 * Description  : Toggle the hardware reset line to set the Camera into Reset and Power Down
 * Arguments    : NONE
 * Return Value : NONE
 *********************************************************************************************************************/
void ov5640_hw_init(void)
{
    /* Enter reset and power-down state */
    
    /* Reset pin is Active LOW */
    OV5640_RST_PIN_SET(BSP_IO_LEVEL_LOW); 

    /* Power Down pin is Active HIGH */
    OV5640_PWDN_PIN_SET(BSP_IO_LEVEL_HIGH);
}
/**********************************************************************************************************************
 End of function ov5640_hw_init
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: ov5640_exit_power_down
 * Description  : Exit the power down state leaving the devie reset and ready to operate.
 * Arguments    : NONE
 * Return Value : NONE
 *********************************************************************************************************************/
void ov5640_exit_power_down(void)
{
    /* Reset pin is Active LOW */
    OV5640_RST_PIN_SET(BSP_IO_LEVEL_LOW);
    delay_ms(20);

    /* Power Down pin is Active HIGH */
    OV5640_PWDN_PIN_SET(BSP_IO_LEVEL_LOW);
    delay_ms(50);

    /* Reset pin is Active LOW */
    OV5640_RST_PIN_SET(BSP_IO_LEVEL_HIGH);
    delay_ms(20);
}
/**********************************************************************************************************************
 End of function ov5640_exit_power_down
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: ov5640_hw_reset
 * Description  : Toggle the Reset pin with a 20mS delay for valid reset and settling time after reset
 * Arguments    : NONE
 * Return Value : NONE
 *********************************************************************************************************************/
void ov5640_hw_reset(void)
{
    /* Reset pin is Active LOW */
    OV5640_RST_PIN_SET(BSP_IO_LEVEL_LOW);
    delay_ms(20);

    /* Reset pin is Active LOW */
    OV5640_RST_PIN_SET(BSP_IO_LEVEL_HIGH);
    delay_ms(20);
}
/**********************************************************************************************************************
 End of function ov5640_hw_reset
 *********************************************************************************************************************/

/**
* @brief OV5640 module software reset
* @param None
* @retval None
*/
/**********************************************************************************************************************
 * Function Name: ov5640_sw_reset
 * Description  : Issue the software reset command via the command interface and wait for valid period to elapse before
 *              : continuing execution. Note this is not RTOS thread friendly, delay may be much longer.
 * Arguments    : NONE
 * Return Value : NONE
 *********************************************************************************************************************/
void ov5640_sw_reset(void)
{
    uint8_t reg3103;

    reg3103  = ov5640_read_reg(0x3103);
    reg3103 &= (uint8_t) ~(0x01U << 1);
    ov5640_write_reg(0x3103, reg3103);
    ov5640_write_reg(0x3008, 0x82);
    delay_ms(300);
}
/**********************************************************************************************************************
 End of function ov5640_sw_reset
 *********************************************************************************************************************/

/* The folowing functions are provided for reference but are currently unused in this example code. The functions have
   not been verified. */
#if (0)

/**********************************************************************************************************************
 * Function Name: ov5640_get_chip_id
 * Description  : Read the Camera ID register to verify the correct device is attached.
 * Arguments    : NONE
 * Return Value : Register read value
 *********************************************************************************************************************/
static uint16_t ov5640_get_chip_id(void)
{
    uint16_t chip_id;

    chip_id = ov5640_read_reg(0x300A) << 8;
    chip_id |= ov5640_read_reg(0x300B);

    return chip_id;
}
/**********************************************************************************************************************
 End of function ov5640_get_chip_id
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: ov5640_led_on
 * Description  : Turn on the Camera Module LED
 * Arguments    : NONE
 * Return Value : NONE
 *********************************************************************************************************************/
void ov5640_led_on(void)
{
    ov5640_write_reg(0x3016, 0x02);
    ov5640_write_reg(0x301C, 0x02);
    ov5640_write_reg(0x3019, 0x02);
}
/**********************************************************************************************************************
 End of function ov5640_led_on
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: ov5640_led_off
 * Description  : Turn off the Camera Module LED
 * Arguments    : NONE
 * Return Value : NONE
 *********************************************************************************************************************/
void ov5640_led_off(void)
{
    ov5640_write_reg(0x3016, 0x02);
    ov5640_write_reg(0x301C, 0x02);
    ov5640_write_reg(0x3019, 0x00);
}
/**********************************************************************************************************************
 End of function ov5640_led_off
 *********************************************************************************************************************/
#endif

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
