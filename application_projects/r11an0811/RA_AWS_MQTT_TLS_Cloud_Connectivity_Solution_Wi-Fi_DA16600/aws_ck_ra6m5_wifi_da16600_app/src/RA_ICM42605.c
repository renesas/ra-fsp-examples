/***********************************************************************************************************************
 * File Name    : RA_ICM42605.c
 * Description  : Contains data structures and function definitions for ICM42605 sensor
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#include "icm.h"
#include "ICM42605.h"
#include "RmcI2C.h"
#include "user_choice.h"
#include "usr_data.h"
#include "r_typedefs.h"

icm_acc_data_t g_corraccraw;
icm_gyr_data_t g_gyrosval;
icm_mag_data_t g_magValue;
usr_icm_data_t g_icm_data;

float g_accelbias[3] = {0};
float g_gyrobias[3] = {0};
float Ares = 0;
float Gres = 0;
uint8_t g_newicm42605data = 0;
uint8_t g_icm42605status = 0;

float g_acl_val[3] = {0};
float g_gyr_val[3] = {0};
float g_tempr_val = 0;

uint8_t g_data_ready = 0;

/*******************************************************************************************************************//**
 * @brief       Setup for ICM42605
 * @param[in]   None
 * @retval      RIIC_SUCCESS        ICM-42605 Sensor setup successful
 * @retval      Any other error     ICM-42605 Sensor setup failed
 ***********************************************************************************************************************/
int8_t ICM42605_setup()
{
    int8_t  err = RIIC_SUCCESS;
    uint8_t chip_id = 0;
    uint8_t int_status = 0;

    err =  getChipID(&chip_id);

    if((DEVICE_ID != chip_id) && (RIIC_SUCCESS != err))
    {
        APP_DBG_PRINT("\r\nICM42605 Sensor get ChipID failed \r\n");
        return ERROR;
    }

    err = ICM42605_reset();
    if(RIIC_SUCCESS != err)
    {
        APP_DBG_PRINT("\r\nICM42605 Sensor reset failed \r\n");
        return err;
    }

    err = ICM42605_apex_featur_init();
    if(RIIC_SUCCESS != err)
    {
        APP_DBG_PRINT("\r\nICM42605 Sensor APEX feature initial failed \r\n");
        return err;
    }

    err = ICM42605_offsetBias(g_accelbias, g_gyrobias);
    if(RIIC_SUCCESS != err)
    {
        APP_DBG_PRINT("\r\nICM42605 Sensor get offsetBias failed \r\n");
        return err;
    }

    err = ICM42605_status(&int_status);
    if(RIIC_SUCCESS != err)
    {
        APP_DBG_PRINT("\r\nICM42605 Sensor read INT_STATUS failed \r\n");
        return err;
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief   Set sensor values to global variables to push to message queue.
 * @param[in]   float *aVal : Pointer to the ICM accelerometer Sensor Data
 * @param[in]   float *gyVal : Pointer the ICM gyroscope Sensor Data
 * @param[in]   float *gyVal : Pointer the ICM manometer Sensor Data
 * @retval      None
 ***********************************************************************************************************************/
void display_data(float *aVal, float *gyVal, float tmpVal)
{
    UNUSED_VARIABLE(tmpVal);
    if(1 == g_data_ready)
    {
        g_data_ready = 0;
        g_corraccraw.x = aVal[0];
        g_corraccraw.y = aVal[1];
        g_corraccraw.z = aVal[2];

        g_gyrosval.x = gyVal[0];
        g_gyrosval.y = gyVal[1];
        g_gyrosval.z = gyVal[2];
    }
}

/*******************************************************************************************************************//**
 * @brief   Get sensor values
 * @param[in]   None
 * @retval      RIIC_SUCCESS        ICM42605 Sensor get xyz values successful
 * @retval      Any other error     ICM42605 Sensor get xyz values failed    
 ***********************************************************************************************************************/
int8_t get_xyz_values()
{
    int8_t  err = RIIC_SUCCESS;
    int16_t a_reg[3] = {0};
    int16_t g_reg[3] = {0};
    int16_t tmpr_reg = 0;

    /* INT1 cleared on status read */
    err = ICM42605_status(&g_icm42605status);
    if(RIIC_SUCCESS != err)
    {
      return err;
    }

    /* for x,y,z values of accel and gyro */
    if(g_icm42605status & 0x08)
    {
        g_data_ready = 1;
        err = ICM42605_readData(a_reg, g_reg, &tmpr_reg);
        if(RIIC_SUCCESS != err)
        {
          return err;
        }

        /* get actual g value, this depends on scale being set */
        g_acl_val[0] = (float)a_reg[0]*Ares - g_accelbias[0];
        g_acl_val[1] = (float)a_reg[1]*Ares - g_accelbias[1];
        g_acl_val[2] = (float)a_reg[2]*Ares - g_accelbias[2];

        /* get actual gyro value, this depends on scale being set */
        g_gyr_val[0] = (float)g_reg[0]*Gres - g_gyrobias[0];
        g_gyr_val[1] = (float)g_reg[1]*Gres - g_gyrobias[1];
        g_gyr_val[2] = (float)g_reg[2]*Gres - g_gyrobias[2];

        g_tempr_val = ((float)tmpr_reg) / 132.48f + 25.0f; // Gyro chip temperature in degrees Centigrade
    }

    return err;

}


/*******************************************************************************************************************//**
 * @brief   icm42605_loop to get sensor values
 * @param[in]   None
 * @retval      RIIC_SUCCESS        ICM42605 Sensor get xyz values successful
 * @retval      Any other error     ICM42605 Sensor get xyz values failed 
 ***********************************************************************************************************************/
int8_t icm42605_loop()
{
    int8_t  err = RIIC_SUCCESS;

    err = get_xyz_values();
    if(FSP_SUCCESS != err)
    {
        APP_DBG_PRINT("\r\nICM42605 Sensor get xyz values failed \r\n");
        return err;
    }

    display_data(g_acl_val, g_gyr_val, g_tempr_val);
    
    /* Update value for icm_data variable */
    g_icm_data.acc_data.x = g_corraccraw.x;
    g_icm_data.acc_data.y = g_corraccraw.y;
    g_icm_data.acc_data.z = g_corraccraw.z;
    g_icm_data.gyr_data.x = g_gyrosval.x;
    g_icm_data.gyr_data.y = g_gyrosval.y;
    g_icm_data.gyr_data.z = g_gyrosval.z;
    g_icm_data.mag_data.x = g_magValue.x;
    g_icm_data.mag_data.y = g_magValue.y;
    g_icm_data.mag_data.z = g_magValue.z;

    xQueueOverwrite(g_icm_queue, &g_icm_data);

    return err;
}

/*******************************************************************************************************************//**
 * @brief   ICM Callback function
 * @param[in]   p_args
 * @retval      None
 ***********************************************************************************************************************/
void ICM_42605_Callback1(external_irq_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED (p_args);
    g_newicm42605data = 1;
}

/*******************************************************************************************************************//**
 * @brief   ICM Callback function
 * @param[in]   p_args
 * @retval      None
 ***********************************************************************************************************************/
void ICM_42605_Callback2(external_irq_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED (p_args);
}


/*******************************************************************************************************************//**
 * @brief   Set up ICM sensor
 * @param[in]   None
 * @retval      None
 ***********************************************************************************************************************/
void icm_setup(void)
{
    int8_t  err = RIIC_SUCCESS;

    err = ICM42605_setup();
    if(RIIC_SUCCESS != err)
    {
        APP_DBG_PRINT("\r\nICM-42605 Sensor setup failed \r\n");
        APP_ERR_TRAP(err);
    }
}

