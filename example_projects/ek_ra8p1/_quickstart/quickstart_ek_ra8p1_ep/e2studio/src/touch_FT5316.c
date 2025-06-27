/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/**********************************************************************************************************************
 * File Name    : touch_FT5316.c
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
#include <stdarg.h>

#include "r_typedefs.h"
#include "hal_data.h"
#include "board_hw_cfg.h"
#include "common_init.h"
#include "common_utils.h"
#include "touch_FT5316.h"
#include "r_i2c_master_api.h"
#include "board_i2c_master.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define R_BYTE_UNINITALIZED (0xFF)

/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Private (static) variables and functions
 *********************************************************************************************************************/
static bool_t s_touch_panel_int = false;
static bool_t s_irq19_valid = false;

uint8_t g_ft5316_i2c_address = FT5316_SLAVE_ADDR;

void touch_panel_reset(void)
{
    R_IOPORT_PinWrite(g_ioport.p_ctrl, BSP_IO_PORT_06_PIN_06, BSP_IO_LEVEL_HIGH);
    vTaskDelay (10);
    R_IOPORT_PinWrite(g_ioport.p_ctrl, BSP_IO_PORT_06_PIN_06, BSP_IO_LEVEL_LOW);
    vTaskDelay (10);
    R_IOPORT_PinWrite(g_ioport.p_ctrl, BSP_IO_PORT_06_PIN_06, BSP_IO_LEVEL_HIGH);
}

void touch_panel_configure(void)
{
}

/**********************************************************************************************************************
 * Function Name: read_ft5316_tp_status
 * Description  : Reads the touch panel to determine the number of touch points detected
 * Return Value : tp_detected: number of touch points detected
 *********************************************************************************************************************/
uint8_t read_ft5316_tp_status(void)
{
    /* This will read the status registers to determine how many touch points exist */
    uint8_t tp_detected = R_BYTE_UNINITALIZED;

    R_IIC_MASTER_SlaveAddressSet (&g_board_i2c_master_ctrl, g_ft5316_i2c_address, I2C_MASTER_ADDR_MODE_7BIT);

    read_reg8 (TD_STATUS, &tp_detected, &g_board_i2c_master_ctrl, 0x01);

    return (tp_detected);
}
/**********************************************************************************************************************
 End of function read_ft5316_tp_status
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: read_reg8
 * Description  : reads n bytes from the defined register to reg_val
 * Return Value : err: success state
 *********************************************************************************************************************/
fsp_err_t read_reg8(uint8_t reg_address, uint8_t *reg_val, i2c_master_ctrl_t *p_i2c_master_ctrl, uint8_t bytes)
{
    fsp_err_t err = FSP_SUCCESS;

    err = R_IIC_MASTER_Write (p_i2c_master_ctrl, &reg_address, bytes, true);
    err = board_i2c_master_wait_complete ();

    err = R_IIC_MASTER_Read (p_i2c_master_ctrl, reg_val, bytes, false);
    err = board_i2c_master_wait_complete ();

    return err;
}
/**********************************************************************************************************************
 End of function read_reg8
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: write_reg8
 * Description  : writes value to the defined register
 * Return Value : err: success state
 *********************************************************************************************************************/
fsp_err_t write_reg8(i2c_master_ctrl_t *p_i2c_master_ctrl, uint8_t reg_address, uint8_t value)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t data[2];

    data[0] = reg_address;
    data[1] = value;

    R_IIC_MASTER_Write (p_i2c_master_ctrl, data, 2, false);
    err = board_i2c_master_wait_complete ();

    return (err);
}
/**********************************************************************************************************************
 End of function write_reg8
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: irq19_tp_callback
 * Description  : touch panel interrupt callback
 * Return Value : p_args: type of interrupt
 *********************************************************************************************************************/
void irq19_tp_callback(external_irq_callback_args_t *p_args)
{
    UNUSED_PARAM(p_args);

    s_touch_panel_int = true;

    /* tell touch pad thread */
    BaseType_t xHigherPriorityTaskWoken;
    BaseType_t xResult;

    xResult = xSemaphoreGiveFromISR(g_irq_binary_semaphore, &xHigherPriorityTaskWoken);
    s_irq19_valid = true;

    if (pdFAIL != xResult)
    {
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}
/**********************************************************************************************************************
 End of function irq19_tp_callback
 *********************************************************************************************************************/

