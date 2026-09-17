/*
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
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
#include "touch_ft5316.h"
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
bool_t g_s_touch_panel_int = false;
bool_t g_s_irq19_valid = false;

volatile uint8_t g_ft5316_i2c_address = FT5316_SLAVE_ADDR;

/**********************************************************************************************************************
 * Function Name: touch_panel_reset
 * Description  : Reset the touch panel
 * Return Value : .
 *********************************************************************************************************************/
void touch_panel_reset(void)
{
    R_IOPORT_PinWrite(g_ioport.p_ctrl, BSP_IO_PORT_06_PIN_06, BSP_IO_LEVEL_HIGH);
    vTaskDelay (250);
    R_IOPORT_PinWrite(g_ioport.p_ctrl, BSP_IO_PORT_06_PIN_06, BSP_IO_LEVEL_LOW);
    vTaskDelay (250);
    R_IOPORT_PinWrite(g_ioport.p_ctrl, BSP_IO_PORT_06_PIN_06, BSP_IO_LEVEL_HIGH);
}

/**********************************************************************************************************************
 * Function Name: touch_panel_configure
 * Description  : dummy function
 * Return Value : .
 *********************************************************************************************************************/
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


    rd_sensor_reg8_8 (TD_STATUS, &tp_detected);

    return (tp_detected);
}
/**********************************************************************************************************************
 End of function read_ft5316_tp_status
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: irq_tp_callback
 * Description  : touch panel interrupt callback
 * Return Value : p_args: type of interrupt
 *********************************************************************************************************************/
void irq_tp_callback(external_irq_callback_args_t *p_args)
{
    UNUSED_PARAM(p_args);

    g_s_touch_panel_int = true;

    /* tell touch pad thread */
    BaseType_t xHigherPriorityTaskWoken;
    BaseType_t xResult;

    xResult = xSemaphoreGiveFromISR(g_irq_binary_semaphore, &xHigherPriorityTaskWoken);
    g_s_irq19_valid = true;

    if (pdFAIL != xResult)
    {
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}
/**********************************************************************************************************************
 End of function irq_tp_callback
 *********************************************************************************************************************/

