/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "hal_data.h"
#include "board_i2c_devices.h"

/***************************************************************************************************************************
 * Macro definitions
 ***************************************************************************************************************************/

#define BSP_I2C_SLAVE_ADDR_SWITCH          (0x43)   //Slave address for SWITCH
#define RESET_VALUE                         (0x00)


/***************************************************************************************************************************
 * Private global variables and functions
 ***************************************************************************************************************************/
uint8_t wrSWReg16_8(int regID, int regDat);
uint8_t rdSWReg16_8(uint16_t regID, uint8_t* regDat);

static fsp_err_t i2c_sw_cb_wait(void);

static fsp_err_t validate_i2c_event(void);
static volatile i2c_master_event_t i2c_sw_event = I2C_MASTER_EVENT_ABORTED;


void g_board_i2c_master_cb(i2c_master_callback_args_t * p_args)
{
    i2c_sw_event = p_args->event;

    R_BSP_IrqStatusClear(R_FSP_CurrentIrqGet());
}

static fsp_err_t i2c_sw_cb_wait(void)
{
    fsp_err_t ret = FSP_SUCCESS;
    fsp_err_t err = validate_i2c_event();

    /* handle error */
    if(FSP_ERR_TRANSFER_ABORTED == err)
    {
#ifdef BOARD_DEVICES_DEBUG_I2C
        print_to_console("** i2c_cam_cb_wait Device ID command, I2C write failed ** \r\n");
#endif
        ret = FSP_ERR_TRANSFER_ABORTED;
    }

    return ret;
}
static fsp_err_t validate_i2c_event(void)
{
    uint16_t local_time_out = UINT16_MAX;

    /* resetting call back event capture variable */
    i2c_sw_event = (i2c_master_event_t)RESET_VALUE;

    do
    {
        /* This is to avoid infinite loop */
        --local_time_out;

        if(RESET_VALUE == local_time_out)
        {
#ifdef BOARD_DEVICES_DEBUG_I2C
            print_to_console("** validate_i2c_event local_time_out ** \r\n");
#endif
            return FSP_ERR_TRANSFER_ABORTED;
        }

    }while(i2c_sw_event == RESET_VALUE);

    if(i2c_sw_event != I2C_MASTER_EVENT_ABORTED)
    {
        // Make sure this is always Reset before return
        i2c_sw_event = (i2c_master_event_t)RESET_VALUE;
#ifdef BOARD_DEVICES_DEBUG_I2C
        print_to_console("** validate_i2c_event FSP_SUCCESS  ** \r\n");
#endif
        return FSP_SUCCESS;
    }

    i2c_sw_event = (i2c_master_event_t)RESET_VALUE; // Make sure this is always Reset before return
    return FSP_ERR_TRANSFER_ABORTED;
}


uint8_t wrSWReg16_8(int regID, int regDat)
{
    fsp_err_t err;

    uint8_t data[3] = {((uint8_t) ((regID&0xFF00)>>8)),((uint8_t) (regID & 0x00FF)), *((uint8_t *) regDat)};

    err = R_IIC_MASTER_Write(&g_i2c_master_board_ctrl, data, 3, false);

    if (FSP_SUCCESS == err)
    {
        err = i2c_sw_cb_wait();
    }
#if USE_DEBUG_BREAKPOINTS
    if (err) __BKPT(0);
#endif

    return FSP_SUCCESS == err ? 0 : 1;
}


uint8_t rdSWReg16_8(uint16_t regID, uint8_t* regDat)
{
    fsp_err_t err;

    uint8_t data[2] = {((uint8_t) ((regID&0xFF00)>>8)),((uint8_t) (regID & 0x00FF))};

    err = R_IIC_MASTER_Write(&g_i2c_master_board_ctrl, data, 2, true);

    if (FSP_SUCCESS == err)
    {
        err = i2c_sw_cb_wait();
    }

    if (FSP_SUCCESS == err)
    {
        err = R_IIC_MASTER_Read(&g_i2c_master_board_ctrl, regDat, 1, false);
    }

    if (FSP_SUCCESS == err)
    {
        err = i2c_sw_cb_wait();
    }

#if USE_DEBUG_BREAKPOINTS
    if (err) __BKPT(0);
#endif

    return FSP_SUCCESS == err ? 0 : 1;
}
