/***********************************************************************************************************************
 * File Name    : smbus_ep.c
 * Description  : Contains data structures and functions used in smbus_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2024 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "smbus_ep.h"

/* Private global variables */
static volatile rm_comms_callback_args_t smbus_callback_args;
static volatile rm_comms_smbus_error_t * smbus_error;

/* SMBus write/read parameters */
static rm_comms_write_read_params_t write_read_param =
{
    .p_src = NULL,
    .p_dest = NULL,
    .src_bytes = RESET_VALUE,
    .dest_bytes = RESET_VALUE,
};

/* Private functions */
static fsp_err_t smbus_init(void);
static fsp_err_t sensor_write_reg(uint8_t value1, uint8_t value2);
static fsp_err_t smbus_wait_event(void);
static fsp_err_t sensor_set_reg(void);

/*******************************************************************************************************************//**
 * @brief     This function initializes the required hardware modules for SMBus.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
static fsp_err_t smbus_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open IIC Master module to communicate with sensor */
    err = IIC_MASTER_OPEN(&g_i2c_master0_ctrl, &g_i2c_master0_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Open GPT module to handle timeout for SMBus standard */
    err = R_GPT_Open(&g_comms_smbus_rsc_ctrl0_timer_ctrl, &g_comms_smbus_rsc_ctrl0_timer_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Open ELC module to link IIC event to GPT */
    err = R_ELC_Open(&g_elc_ctrl, &g_elc_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Open SMBus Middleware driver */
    err = RM_COMMS_SMBUS_Open(&g_comms_smbus0_ctrl, &g_comms_smbus0_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function writes data to a sensor register via SMBus.
 * @param[in] value1                    First byte to write to the register.
 * @param[in] value2                    Second byte to write to the register.
 * @retval    FSP_SUCCESS               Written successfully.
 * @retval    Any other error codes     Failed to write.
 **********************************************************************************************************************/
static fsp_err_t sensor_write_reg(uint8_t value1, uint8_t value2)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t smbus_buf[SMBUS_THREE_BYTE] = {CFG_ADR, value1, value2};

    /* Reset FSP communication common event */
    smbus_callback_args.event = (rm_comms_event_t) SMBUS_RESET_EVENT;

    /* Reset SMBus specific error */
    smbus_error->smbus_event = RM_COMMS_SMBUS_NO_ERROR;

    /* Write input values to configuration register of sensor via SMBus */
    err = RM_COMMS_SMBUS_Write(&g_comms_smbus0_ctrl, smbus_buf, SMBUS_THREE_BYTE);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Wait SMBus event */
    err = smbus_wait_event();

    return err;
}

/*******************************************************************************************************************//**
 * @brief      This function reads data from a sensor register via SMBus.
 * @param[in]  adr                      Address of the selected register.
 * @param[out] p_value                  Pointer to store the register value.
 * @retval     FSP_SUCCESS              Read successfully.
 * @retval     Any other error codes    Failed to read.
 **********************************************************************************************************************/
fsp_err_t sensor_read_reg(uint8_t adr, uint8_t * p_value)
{
    fsp_err_t err = FSP_SUCCESS;

    write_read_param.p_src = &adr;
    write_read_param.p_dest = p_value;
    write_read_param.src_bytes = SMBUS_ONE_BYTE;
    write_read_param.dest_bytes = SMBUS_THREE_BYTE; /* 2 data bytes + 1 PEC byte */

    /* Reset FSP communication common event */
    smbus_callback_args.event = (rm_comms_event_t) SMBUS_RESET_EVENT;

    /* Reset SMBus specific error */
    smbus_error->smbus_event = RM_COMMS_SMBUS_NO_ERROR;

    /* Read data from register of sensor */
    err = RM_COMMS_SMBUS_WriteRead(&g_comms_smbus0_ctrl, write_read_param);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Wait SMBus transmission complete */
    err = smbus_wait_event();

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function waits until the SMBus transmission is complete, then translates any SMBus specific
 *            error into an FSP error code.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Completed successfully.
 * @retval    Any other error codes     Failed to complete.
 **********************************************************************************************************************/
static fsp_err_t smbus_wait_event(void)
{
    /* Wait until SMBus transmission complete */
    while (RM_COMMS_EVENT_OPERATION_COMPLETE != smbus_callback_args.event)
    {
        /* Filter SMBus specific error */
        if (RM_COMMS_EVENT_ERROR == smbus_callback_args.event)
        {
            switch (smbus_error->smbus_event)
            {
                case RM_COMMS_SMBUS_MEXT_TIMEOUT:
                {
                    /* Transmission time between each event exceeded 10 milliseconds */
                    return FSP_ERR_TIMEOUT;
                }

                case RM_COMMS_SMBUS_SEXT_TIMEOUT:
                {
                    /* Total transmission time exceeded 25 milliseconds */
                    return FSP_ERR_TIMEOUT;
                }

                case RM_COMMS_SMBUS_DATA_CORRUPT:
                {
                    /* PEC byte is incorrect */
                    return FSP_ERR_INVALID_DATA;
                }

                case RM_COMMS_SMBUS_MISC_ERROR:
                {
                    /* SMBus dependency modules failed */
                    return FSP_ERR_ASSERTION;
                }

                default:
                {
                    break;
                }
            }
        }
    }

    return FSP_SUCCESS;
}

/*******************************************************************************************************************//**
 * @brief     This function sets and verifies the sensor configuration register via SMBus.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Configured successfully.
 * @retval    Any other error codes     Failed to configure.
 **********************************************************************************************************************/
static fsp_err_t sensor_set_reg(void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t read_buff[CFG_DATA_SIZE] = {RESET_VALUE};

    /* Write data to register */
    err = sensor_write_reg(CFG_8BIT_HIGHER, CFG_8BIT_LOWER);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Read back data from register */
    err = sensor_read_reg(CFG_ADR, read_buff);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Verify the data read back matches the data written */
    if ((CFG_8BIT_HIGHER != read_buff[0]) || (CFG_8BIT_LOWER != read_buff[1]))
    {
        return FSP_ERR_ASSERTION;
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function initializes the necessary hardware and configures the sensor register.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t sensor_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize the necessary hardware to control the sensor */
    err = smbus_init();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Configure sensor registers */
    err = sensor_set_reg();

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function is the user-defined SMBus callback.
 * @param[in] p_args                    Pointer to the callback argument.
 * @retval    None.
 **********************************************************************************************************************/
void comms_smbus_callback(rm_comms_callback_args_t * p_args)
{
    if (NULL != p_args)
    {
        /* Get FSP communication common event */
        smbus_callback_args.event = p_args->event;

        /* Get SMBus specific error */
        smbus_error = (rm_comms_smbus_error_t *) p_args->p_instance_args;
    }
}

/*******************************************************************************************************************//**
 * @brief     This function closes the opened SMBus and its dependency modules.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void sensor_deinit(void)
{
    /* Close the opened IIC Master module */
    if (g_i2c_master0_ctrl.open)
    {
        IIC_MASTER_CLOSE(&g_i2c_master0_ctrl);
    }

    /* Close the opened GPT module */
    if (g_comms_smbus_rsc_ctrl0_timer_ctrl.open)
    {
        R_GPT_Close(&g_comms_smbus_rsc_ctrl0_timer_ctrl);
    }

    /* Close the opened ELC module */
    if (g_elc_ctrl.open)
    {
        R_ELC_Close(&g_elc_ctrl);
    }

    /* Close the opened SMBus */
    if (g_comms_smbus0_ctrl.open)
    {
        RM_COMMS_SMBUS_Close(&g_comms_smbus0_ctrl);
    }
}
