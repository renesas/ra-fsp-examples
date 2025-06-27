/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : board_i2c_master.c
 * Version      : 1.0
 * Description  : I2C Bus master driver, interface to all I2C devices.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 20.05.2025 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include <stdarg.h>

#include "r_typedefs.h"
#include "common_utils.h"
#include "common_init.h"
#include "board_cfg.h"
#include "FreeRTOS.h"
#include "board_i2c_master.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define I2C_TRANSFER_COMPLETE  (1<<0)
#define I2C_TRANSFER_ABORT     (1<<1)

#define I2C_TIMEOUT_MS         (1000/portTICK_PERIOD_MS)

/**********************************************************************************************************************
 Local Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: board_i2c_master_wait_complete
 * Description  : Process result of active I2C operation.
 * Argument     : NONE
 * Return Value : result of operation
 *                FSP_SUCCESS
 *                FSP_ERR_TRANSFER_ABORTED
 *********************************************************************************************************************/
fsp_err_t board_i2c_master_wait_complete(void)
{
    fsp_err_t ret = FSP_ERR_INVALID_MODE;
    EventBits_t uxBits;

    uxBits =  xEventGroupWaitBits(g_i2c_event_group,
                    I2C_TRANSFER_COMPLETE | I2C_TRANSFER_ABORT,
                    pdTRUE,     //Clear bits before returning
                    pdFALSE,    //Either bit will do
                    I2C_TIMEOUT_MS  );

    if ((I2C_TRANSFER_COMPLETE & uxBits) == I2C_TRANSFER_COMPLETE)
    {
        ret = FSP_SUCCESS;
    }
    else if ((I2C_TRANSFER_ABORT & uxBits) == I2C_TRANSFER_ABORT)
    {
        ret = FSP_ERR_ABORTED;
    }
    else
    {
        /* xEventGroupWaitBits() returned because of timeout */
        ret = FSP_ERR_TIMEOUT;
    }

    return ret;
}
/**********************************************************************************************************************
 End of function board_i2c_master_wait_complete
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: wr_sensor_reg8_8
 * Description  : Write to active i2c device specified by call to R_RIIC_MASTER_SlaveAddressSet
 * Arguments    : regID  - sub-address on the active i2c device
 *              : regDat - byte to write
 * Return Value : 0 - operation succeeded
 *                1 - internal error
 *********************************************************************************************************************/
fsp_err_t wr_sensor_reg8_8(int regID, int regDat)
{
    fsp_err_t err = FSP_ERR_INVALID_MODE;
    uint8_t data[2] = {(uint8_t) regID, (uint8_t) regDat};

    err = R_IIC_MASTER_Write(&g_board_i2c_master_ctrl, data, 2, false);

    if (FSP_SUCCESS == err)
    {
        err = board_i2c_master_wait_complete();
    }
#if USE_DEBUG_BREAKPOINTS
    if (err) __BKPT(0);
#endif

    return (err);
}
/**********************************************************************************************************************
 End of function wr_sensor_reg8_8
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: rd_sensor_reg8_8
 * Description  : Read to active i2c device specified by call to R_RIIC_MASTER_SlaveAddressSet
 * Arguments    : regID  - sub-address on the active i2c device
 *              : regDat - byte to read from device
 * Return Value : 0 - operation succeeded
 *                1 - internal error
 *********************************************************************************************************************/
fsp_err_t rd_sensor_reg8_8(uint16_t regID, uint8_t* regDat)
{
    fsp_err_t err = FSP_ERR_INVALID_MODE;
    uint8_t data[1] = {(uint8_t) regID};

    err = R_IIC_MASTER_Write(&g_board_i2c_master_ctrl, data, 1, true);

    if (FSP_SUCCESS == err)
    {
        err = board_i2c_master_wait_complete();
    }

    if (FSP_SUCCESS == err)
    {
        err = R_IIC_MASTER_Read(&g_board_i2c_master_ctrl, regDat, 1, false);
    }

    if (FSP_SUCCESS == err)
    {
        err = board_i2c_master_wait_complete();
    }

#if USE_DEBUG_BREAKPOINTS
    if (err) __BKPT(0);
#endif

    return FSP_SUCCESS == err ? 0 : 1;
}
/**********************************************************************************************************************
 End of function rd_sensor_reg8_8
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: wr_sensor_reg16_8
 * Description  : Write byte of data (specified in bytes) to active i2c target
 * Arguments    : p_api_ctrl - i2c_stack registered in FSP
 *              : regID      - sub address on target device
 *              : regDat     - data to write
 * Return Value : see board_i2c_master_wait_complete.
 *********************************************************************************************************************/
fsp_err_t wr_sensor_reg16_8(i2c_master_ctrl_t * p_api_ctrl, uint16_t regID, uint8_t regDat)
{
    fsp_err_t err = FSP_ERR_INVALID_MODE;

    uint8_t data[3] = {((uint8_t) ((regID&0xFF00)>>8)),((uint8_t) (regID & 0x00FF)), regDat};

    err = R_IIC_MASTER_Write(p_api_ctrl, data, 3, false);
    if (FSP_SUCCESS == err)
    {
        err = board_i2c_master_wait_complete();
    }

    return err;
}
/**********************************************************************************************************************
 End of function wr_sensor_reg16_8
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: rd_sensor_reg16_8
 * Description  : Read byte of data (specified in bytes) from active i2c target
 * Arguments    : p_api_ctrl - i2c_stack registered in FSP
 *              : regID      - sub address on target device
 *              : regDat     - data to read, specified by caller
 * Return Value : see board_i2c_master_wait_complete.
 *********************************************************************************************************************/
fsp_err_t rd_sensor_reg16_8(i2c_master_ctrl_t * p_api_ctrl, uint16_t regID, uint8_t* regDat)
{
    fsp_err_t err = FSP_ERR_INVALID_MODE;

    uint8_t data[2] = {((uint8_t) ((regID&0xFF00)>>8)),((uint8_t) (regID & 0x00FF))};

    err = R_IIC_MASTER_Write(p_api_ctrl, data, 2, true);
    if (FSP_SUCCESS == err)
    {
        err = board_i2c_master_wait_complete();
        if (FSP_SUCCESS == err)
        {
            err = R_IIC_MASTER_Read(p_api_ctrl, regDat, 1, false);
            if (FSP_SUCCESS == err)
            {
                err = board_i2c_master_wait_complete();
            }
        }
    }

    return err;
}
/**********************************************************************************************************************
 End of function rd_sensor_reg16
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: board_i2c_master_callback
 * Description  : Function called during I2C interrupt detection to trigger
 *                required post processing.
 * Argument     : p_args - contains but not limited to event code.
 * Return Value : NONE.
 *********************************************************************************************************************/
void board_i2c_master_callback(i2c_master_callback_args_t * p_args)
{
    BaseType_t xHigherPriorityTaskWoken;
    BaseType_t xResult = pdFAIL;

    /* xHigherPriorityTaskWoken must be initialised to pdFALSE. */
    xHigherPriorityTaskWoken = pdFALSE;

    if ((I2C_MASTER_EVENT_TX_COMPLETE == p_args->event) || (I2C_MASTER_EVENT_RX_COMPLETE == p_args->event))
    {
        xResult = xEventGroupSetBitsFromISR(g_i2c_event_group, I2C_TRANSFER_COMPLETE, &xHigherPriorityTaskWoken);
    }
    else if (I2C_MASTER_EVENT_ABORTED == p_args->event)
    {
        xResult = xEventGroupSetBitsFromISR(g_i2c_event_group, I2C_TRANSFER_ABORT, &xHigherPriorityTaskWoken);
    }
    else
    {
        /* should never get here. */
        ;
    }

    /* Was the message posted successfully? */
    if (pdFAIL != xResult)
    {
        /* If xHigherPriorityTaskWoken is now set to pdTRUE then a context
        switch should be requested.  The macro used is port specific and will
        be either portYIELD_FROM_ISR() or portEND_SWITCHING_ISR() - refer to
        the documentation page for the port being used. */
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }

    R_BSP_IrqStatusClear(R_FSP_CurrentIrqGet());
}
/**********************************************************************************************************************
 End of function board_i2c_master_callback
 *********************************************************************************************************************/
