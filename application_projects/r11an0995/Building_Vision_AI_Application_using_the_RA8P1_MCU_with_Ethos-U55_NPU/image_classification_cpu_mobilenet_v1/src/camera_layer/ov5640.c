/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "bsp_api.h"
#include "hal_data.h"
#include "hal_data.h"
#include "camera_thread.h"
#include "board_i2c_devices.h"
#include "ov5640_cfg.h"

/***************************************************************************************************************************
 * Macro definitions
 ***************************************************************************************************************************/
/* OV5640 Power-down pin is active LOW */
#define OV5640_RST_PIN_SET(x) do{ \
    R_BSP_PinAccessEnable();  \
    R_BSP_PinWrite(OV5640_CAM_RESET, x);  \
    R_BSP_PinAccessDisable(); \
}while(0)


#define delay_ms(x)  R_BSP_SoftwareDelay(x, BSP_DELAY_UNITS_MILLISECONDS)



/**
 * @brief       OV5640 register programming
 * @param       reg: register address
 *              dat: register data 
 * @retval      none
 */
void ov5640_write_reg(uint16_t reg, uint8_t dat)
{

    uint8_t temp[4] = {0};
    temp[0] = dat;
    R_IIC_MASTER_SlaveAddressSet(&g_i2c_master_board_ctrl, OV5640_I2C_SLAVE_ADDR, I2C_MASTER_ADDR_MODE_7BIT);

    wrSWReg16_8(reg, (int) &temp[0]);

    uint8_t read_back = ov5640_read_reg(reg);
    if(read_back != dat)
    {
     //   __BKPT(0);
    }
}

/**
 * @brief       OV5640 register read
 * @param       reg: register address
 * @retval      register data
 */
uint8_t ov5640_read_reg(uint16_t reg)
{
    uint8_t dat = 0;

        R_IIC_MASTER_SlaveAddressSet(&g_i2c_master_board_ctrl, OV5640_I2C_SLAVE_ADDR, I2C_MASTER_ADDR_MODE_7BIT);
        rdSWReg16_8(reg, &dat);
    return dat;
}

/**
 * @brief       OV5640 initialization
 * @param       none
 * @retval      none
 */
void ov5640_hw_init(void)
{
    // Enter reset and power-down state
    OV5640_RST_PIN_SET(BSP_IO_LEVEL_LOW); // Active LOW


}

/**
 * @brief       OV5640 power down
 * @param       none
 * @retval      none
 */
void ov5640_exit_power_down(void)
{
    OV5640_RST_PIN_SET(BSP_IO_LEVEL_LOW); // Active LOW
    delay_ms(20);
 
    OV5640_RST_PIN_SET(BSP_IO_LEVEL_HIGH); // Active LOW
    delay_ms(20);
}

/**
 * @brief       OV5640 hardware reset
 * @param       none
 * @retval      none
 */
void ov5640_hw_reset(void)
{
    OV5640_RST_PIN_SET(BSP_IO_LEVEL_LOW); // Active HIGH
    delay_ms(20);
    OV5640_RST_PIN_SET(BSP_IO_LEVEL_HIGH); // Active LOW
    delay_ms(20);
}


/**
 * @brief       OV5640 software reset
 * @param       none
 * @retval      none
 */
void ov5640_sw_reset(void)
{
    uint8_t reg3103;

    reg3103 = ov5640_read_reg(0x3103);
    reg3103 &= (uint8_t) ~(0x01U << 1);
    ov5640_write_reg(0x3103, reg3103);
    ov5640_write_reg(0x3008, 0x82);
    delay_ms(300);
}





