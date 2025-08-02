/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/**********************************************************************************************************************
 * File Name    : touch_GT911.c
 * Version      : 1.0
 * Description  : Implementation of application interface and system support for the GT111 Capacitive touch
 *                controller on a LCD panel.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 01.01.2025 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <string.h>
#include <stdio.h>

#include "r_typedefs.h"
#include "board_hw_cfg.h"
#include "common_utils.h"
#include "common_init.h"
#include "board_cfg.h"
#include "FreeRTOS.h"
#include "board_i2c_master.h"
#include "touch_GT911.h"
#include "jlink_console.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
static fsp_err_t get_product_id(i2c_master_ctrl_t * p_api_ctr, char *target);
static bool_t s_display_detected = false;
bool_t g_display_set = false;

#ifdef DUMP_GT911_REGS
uint8_t g_read_config[184];
#endif

#define SET_I2C_ADDRESS_0x14    (0)
#define SET_I2C_ADDRESS_0x5D    (1)


void reset_gt911(uint8_t set_address);

uint8_t g_gt911_i2c_address;


/**********************************************************************************************************************
 * Function Name: display_ts_detected
 * Description  : Helper Fuinction to inform the system that the display is configured and enabled.
 * Arguments    : None.
 * Return Value : Detection state.
 *********************************************************************************************************************/
bool_t display_ts_detected(void)
{
    while (false == g_display_set)
    {
        vTaskDelay(100);
    }
    return (s_display_detected);
}
/**********************************************************************************************************************
 End of function display_ts_detected
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: touch_irq_callback
 * Description  : Interrupt callback for configured channel, gives semaphore to OS indicating and callback event has
 *                occurred, this semaphore will be monitored by interested threads.
 * Argument     : p_args - not used required by I/F.
 * Return Value : NONE
 *********************************************************************************************************************/
void touch_irq_callback (external_irq_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);
    BaseType_t xHigherPriorityTaskWoken;
    BaseType_t xResult;

    xResult = xSemaphoreGiveFromISR(g_irq_binary_semaphore, &xHigherPriorityTaskWoken);

    if (pdFAIL != xResult)
    {
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}
/**********************************************************************************************************************
 End of function touch_irq_callback
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: reset_gt911
 * Description  : Helper Function to reset touch controller
 * Arguments    : set_address - device address for touch controller
 * Return Value : None.
 *********************************************************************************************************************/
void reset_gt911(uint8_t set_address)
{
    ioport_cfg_options_t set_irq_pin_level;

    if (SET_I2C_ADDRESS_0x5D == set_address)
    {
        set_irq_pin_level = IOPORT_CFG_PORT_OUTPUT_LOW;
        g_gt911_i2c_address = GT_911_I2C_ADDRESS_0x5D;
    }
    else
    {
        set_irq_pin_level = IOPORT_CFG_PORT_OUTPUT_HIGH;
        g_gt911_i2c_address = GT_911_I2C_ADDRESS_0x14;
    }

    // set P510 IRQ pin to output
    R_IOPORT_PortDirectionSet(g_ioport.p_ctrl, BSP_IO_PORT_05, 1, 1 << 10);
    R_IOPORT_PinCfg(g_ioport.p_ctrl, BSP_IO_PORT_05_PIN_10, (uint32_t) IOPORT_CFG_DRIVE_MID | (uint32_t) IOPORT_CFG_PORT_DIRECTION_OUTPUT | (uint32_t) set_irq_pin_level);

    vTaskDelay(10);

    // reset the LCD
    R_IOPORT_PinWrite(g_ioport.p_ctrl, BSP_IO_PORT_10_PIN_01, BSP_IO_LEVEL_LOW);
    vTaskDelay(10);

    R_IOPORT_PinWrite(g_ioport.p_ctrl, BSP_IO_PORT_10_PIN_01, BSP_IO_LEVEL_HIGH);
    vTaskDelay(10);

    // set the P510 IRQ pin to IRQ function
    R_IOPORT_PinCfg(g_ioport.p_ctrl, BSP_IO_PORT_05_PIN_10, (uint32_t) IOPORT_CFG_IRQ_ENABLE | (uint32_t) IOPORT_CFG_PORT_DIRECTION_INPUT);
}
/**********************************************************************************************************************
 End of function reset_gt911
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: get_product_id
 * Description  : Helper Function to reset touch controller
 * Arguments    : set_address - device address for touch controller
 *                target      - id read from controller (on valid when return FSP_SUCCESS)
 * Return Value : Request status.
 *                FSP_SUCCESS          - operation completed target is valid response
 *                FSP_ERR_INVALID_MODE - controller not available/detected
 *                see R_IIC_MASTER_Write & R_IIC_MASTER_Read for additional values.
 *********************************************************************************************************************/
fsp_err_t get_product_id(i2c_master_ctrl_t * p_api_ctrl, char *target)
{
    fsp_err_t err;

    err = rd_sensor_multi_reg16_8(p_api_ctrl, GT911_REG_PRODUCT_ID, (uint8_t *)target, 4);

    return (err);
}
/**********************************************************************************************************************
 End of function get_product_id
 *********************************************************************************************************************/

fsp_err_t enable_ts(i2c_master_ctrl_t * p_api_i2c_ctrl, external_irq_ctrl_t * p_api_irq_ctrl)
{

    fsp_err_t err;
    char product_id[5];

    reset_gt911(GT_911_I2C_ADDRESS);

    err = R_IIC_MASTER_SlaveAddressSet(p_api_i2c_ctrl, g_gt911_i2c_address, I2C_MASTER_ADDR_MODE_7BIT);

    if (FSP_SUCCESS == err)
    {
        err = get_product_id(p_api_i2c_ctrl, &product_id[0]);

        if (FSP_SUCCESS == err)
        {
            if (product_id[0] != '9')
            {
                /* Product ID should be 9xx */
                err = FSP_ERR_ASSERTION;
                s_display_detected = false;
                g_display_set = true;
            }
            else
            {
                s_display_detected = true;
                g_display_set = true;
            }
        }
    }

    if (FSP_SUCCESS == err)
    {
        err = wr_sensor_reg16_8(p_api_i2c_ctrl, GT911_REG_COMMAND, 0x00U);
        if (FSP_SUCCESS == err)
        {
            {
                err = R_ICU_ExternalIrqEnable(p_api_irq_ctrl);
            }
        }
    }

    return err;
}
