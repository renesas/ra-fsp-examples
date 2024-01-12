/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * File Name    : arducam_port.c
 * Version      : .
 * Description  : .
 *********************************************************************************************************************/

#include <stdarg.h>

#include "r_typedefs.h"
#include "hal_data.h"
#include "fsp_common_api.h"
#include "arducam.h"

/* uncomment DEBUG_I2C to show event details */

// #define DEBUG_I2C (1)

#define TX_WAIT_FOREVER    (0xFFFFFFFF)
#define RESET_VALUE        (0x00)

/* define link to your i2c driver HERE */
static iic_master_instance_ctrl_t * p_api_ctrl = &g_i2c_master1_ctrl;
static volatile i2c_master_event_t  i2c_event  = I2C_MASTER_EVENT_ABORTED;

byte rdSensorReg16_Multi(uint16_t regID, uint8_t * regDat, uint32_t len);

void             tx_semaphore_ceiling_put(bool_t * store);
bool_t           tx_semaphore_get(bool_t * store, uint32_t delay);
static fsp_err_t validate_i2c_event(void);

fsp_err_t i2c_cam_cb_wait(void);

extern void touch_i2c_callback(i2c_master_callback_args_t * p_args);

/* Called from touch i2c isr routine */

/**********************************************************************************************************************
 * Function Name: g_i2c_master1_cb
 * Description  : .
 * Argument     : p_args
 * Return Value : .
 *********************************************************************************************************************/
void g_i2c_master1_cb (i2c_master_callback_args_t * p_args)
{
    i2c_event = p_args->event;

    touch_i2c_callback(p_args);
}

/**********************************************************************************************************************
 * End of function g_i2c_master1_cb
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: i2c_cam_cb_wait
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
fsp_err_t i2c_cam_cb_wait (void)
{
    fsp_err_t ret = FSP_SUCCESS;
    fsp_err_t err = validate_i2c_event();

    /* handle error */
    if (FSP_ERR_TRANSFER_ABORTED == err)
    {
#ifdef DEBUG_I2C
        printf_colour("** i2c_cam_cb_wait Device ID command, I2C write failed ** \r\n");
#endif
        ret = FSP_ERR_TRANSFER_ABORTED;
    }

    return ret;
}

/**********************************************************************************************************************
 * End of function i2c_cam_cb_wait
 *********************************************************************************************************************/

#ifdef DEBUG_I2C
static char_t s_event_as_str[][64] =
{
    "not set",
    "transfer was aborted",
    "receive operation was completed successfully",
    "transmit operation was completed successfully",
};
#endif

/**********************************************************************************************************************
 * Function Name: validate_i2c_event
 * Description  : .
 * Return Value : .
 *********************************************************************************************************************/
static fsp_err_t validate_i2c_event (void)
{
    uint16_t local_time_out = UINT16_MAX;

    /* resetting call back event capture variable */
    i2c_event = (i2c_master_event_t) RESET_VALUE;

    do
    {
        /* This is to avoid infinite loop */
        --local_time_out;

        if (RESET_VALUE == local_time_out)
        {
// printf_colour("** validate_i2c_event local_time_out ** \r\n");
            return FSP_ERR_TRANSFER_ABORTED;
        }
    } while (i2c_event == RESET_VALUE);

#ifdef DEBUG_I2C
    printf_colour("** validate_i2c_event i2c_event [%s] ** \r\n", s_event_as_str[i2c_event]);
#endif

    if (i2c_event != I2C_MASTER_EVENT_ABORTED)
    {
        i2c_event = (i2c_master_event_t) RESET_VALUE; // Make sure this is always Reset before return

// printf_colour("** validate_i2c_event FSP_SUCCESS  Happy Dog ** \r\n");
        return FSP_SUCCESS;
    }

    i2c_event = (i2c_master_event_t) RESET_VALUE; // Make sure this is always Reset before return
    return FSP_ERR_TRANSFER_ABORTED;
}

/**********************************************************************************************************************
 * End of function validate_i2c_event
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: wrSensorReg8_8
 * Description  : .
 * Arguments    : regID
 *              : regDat
 * Return Value : .
 *********************************************************************************************************************/
byte wrSensorReg8_8 (int regID, int regDat)
{
    fsp_err_t err;

    uint8_t data[2] = {(uint8_t) regID, (uint8_t) regDat};

    err = R_IIC_MASTER_Write(p_api_ctrl, data, 2, false);

    if (FSP_SUCCESS == err)
    {
        err = i2c_cam_cb_wait();
    }

#if USE_DEBUG_BREAKPOINTS
    if (err)
    {
        __BKPT(0);
    }
#endif

    return (FSP_SUCCESS == err) ? 0 : 1;
}

/**********************************************************************************************************************
 * End of function wrSensorReg8_8
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: wrSensorReg16_8
 * Description  : .
 * Arguments    : regID
 *              : regDat
 * Return Value : .
 *********************************************************************************************************************/
byte wrSensorReg16_8 (int regID, int regDat)
{
    fsp_err_t err;

    uint8_t data[3] = {(uint8_t) (regID >> 8), (uint8_t) regID, (uint8_t) regDat};

    err = R_IIC_MASTER_Write(p_api_ctrl, data, 3, false);

    if (FSP_SUCCESS == err)
    {
        err = i2c_cam_cb_wait();
    }

#if USE_DEBUG_BREAKPOINTS
    if (err)
    {
        __BKPT(0);
    }
#endif

    return (FSP_SUCCESS == err) ? 0 : 1;
}

/**********************************************************************************************************************
 * End of function wrSensorReg16_8
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: rdSensorReg16_8
 * Description  : .
 * Arguments    : regID
 *              : regDat
 * Return Value : .
 *********************************************************************************************************************/
byte rdSensorReg16_8 (uint16_t regID, uint8_t * regDat)
{
    fsp_err_t err;

    uint8_t data[2] = {(uint8_t) (regID >> 8), (uint8_t) regID};

    err = R_IIC_MASTER_Write(p_api_ctrl, data, 2, true);
    if (FSP_SUCCESS != err)
    {
// printf_colour("** R_IIC_MASTER_Write API failed ** \r\n");
    }

    if (FSP_SUCCESS == err)
    {
        err = i2c_cam_cb_wait();
    }

    if (FSP_SUCCESS == err)
    {
        err = R_IIC_MASTER_Read(p_api_ctrl, regDat, 1, false);
    }

    if (FSP_SUCCESS == err)
    {
        err = i2c_cam_cb_wait();
    }

#if USE_DEBUG_BREAKPOINTS
    if (err)
    {
        __BKPT(0);
    }
#endif

    return (FSP_SUCCESS == err) ? 0 : 1;
}

/**********************************************************************************************************************
 * End of function rdSensorReg16_8
 *********************************************************************************************************************/

/**********************************************************************************************************************
 * Function Name: rdSensorReg16_Multi
 * Description  : .
 * Arguments    : regID
 *              : regDat
 *              : len
 * Return Value : .
 *********************************************************************************************************************/
byte rdSensorReg16_Multi (uint16_t regID, uint8_t * regDat, uint32_t len)
{
    fsp_err_t err;

    uint8_t data[2] = {(uint8_t) (regID >> 8), (uint8_t) regID};

    err = R_IIC_MASTER_Write(p_api_ctrl, data, 2, true);
    if (FSP_SUCCESS != err)
    {
// printf_colour("** R_IIC_MASTER_Write API failed ** \r\n");
    }

    if (FSP_SUCCESS == err)
    {
        err = i2c_cam_cb_wait();
    }

    if (FSP_SUCCESS == err)
    {
        err = R_IIC_MASTER_Read(p_api_ctrl, regDat, len, false);
    }

    if (FSP_SUCCESS == err)
    {
        err = i2c_cam_cb_wait();
    }

#if USE_DEBUG_BREAKPOINTS
    if (err)
    {
        __BKPT(0);
    }
#endif

    return (FSP_SUCCESS == err) ? 0 : 1;
}

/**********************************************************************************************************************
 * End of function rdSensorReg16_Multi
 *********************************************************************************************************************/
