/**********************************************************************************************************************
 * File Name    : smbus_ep.c
 * Description  : Contains data structures and functions
 *********************************************************************************************************************/
/**********************************************************************************************************************
* Copyright (c) 2024 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
**********************************************************************************************************************/

#include "smbus_ep.h"

/* Exported global variable */
extern bsp_leds_t g_bsp_leds;

/* Private global variables */
static volatile rm_comms_callback_args_t smbus_callback_args;
static volatile rm_comms_smbus_error_t * smbus_error;

/* SMBus WriteRead parameters */
static rm_comms_write_read_params_t write_read_param =
{
 .p_src = NULL,
 .p_dest = NULL,
 .src_bytes = RESET_VALUE,
 .dest_bytes = RESET_VALUE,
};

/* Private function declarations */
static fsp_err_t smbus_init(void);
static fsp_err_t sensor_write_reg(uint8_t value1, uint8_t value2);
static fsp_err_t sensor_read_reg(uint8_t adr, uint8_t * p_value);
static fsp_err_t smbus_wait_event(void);
static fsp_err_t sensor_set_reg(void);
static fsp_err_t sensor_init(void);
static void sensor_deinit(void);
static void handle_error(fsp_err_t err, uint8_t * err_str);
static void led_error_state_set(led_power_t state);

/***********************************************************************************************************************
 * @brief      This function is used to start the SMBus example operation.
 * @param[IN]  None
 * @retval     None
 **********************************************************************************************************************/
void smbus_entry(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version          = { RESET_VALUE };
    uint8_t temp_val[TEMP_DATA_SIZE]    = { RESET_VALUE };
    char    temp_buff[BUFF_SIZE]        = { RESET_VALUE };
    float   temperature                 = FLOAT_RESET_VALUE;

    /* Turn off all LEDs on board at start up */
    for (int i =0; i < g_bsp_leds.led_count; i++)
    {
        R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[i], (bsp_io_level_t)LED_POWER_OFF);
    }

    /* Initialize terminal */
    err = TERM_INIT();
    if (FSP_SUCCESS != err)
    {
        /* Turn on error LED to indicate an error has occurred */
        led_error_state_set(LED_POWER_ON);

        /* Error trap */
        ERROR_TRAP;
    }


    /* Version get API for FSP information */
    R_FSP_VersionGet(&version);

    /* Print the EP banner on the terminal */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor,
              version.version_id_b.patch);

    /* Print the EP information on the terminal */
    APP_PRINT(EP_INFO);

    /* Initialize MAX31875 temperature sensor */
    err = sensor_init();
    APP_ERR_HANDLE(err, "**sensor_init failed**\r\n");

    /* Main loop */
    while (true)
    {
        /* Clear temperature buffer */
        memset(temp_buff, RESET_VALUE, sizeof(temp_buff));

        /* Read raw data from sensor */
        err = sensor_read_reg(DATA_ADR, temp_val);
        APP_ERR_HANDLE(err, "**sensor_read_reg failed**\r\n");

        /* Convert the temperature reading to degree before printing */
        temperature = (float) ((int16_t) (((uint16_t) temp_val[TEMPERATURE_INDEX] << SHIFT_FOUR_BITS)
                    | ((uint8_t) temp_val[TEMPERATURE_INDEX + 1] >> SHIFT_FOUR_BITS))) / FLOAT_DIVISOR;

        /* Convert float to string to print temperature values */
        snprintf(temp_buff, sizeof(temp_buff), "%.04f%s", temperature, DEGREE_SYMBOL);
        APP_PRINT("Temperature: %sC\r\n", temp_buff);

        /* Delay to display temperature values on terminal */
        R_BSP_SoftwareDelay(PRINT_DELAY, BSP_DELAY_UNITS_SECONDS);
    }
}
/***********************************************************************************************************************
* End of function smbus_entry
***********************************************************************************************************************/

/***********************************************************************************************************************
 * @brief      This function initializes the essential hardware components required for SMBus.
 * @param[IN]  None
 * @retval     FSP_SUCCESS    Upon successful operation.
 * @retval     Any other error code apart from FSP_SUCCESS means the operation failed.
 **********************************************************************************************************************/
static fsp_err_t smbus_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open IIC master module to communicate with sensor */
#if defined (USE_IIC_MODULE)
    err = R_IIC_MASTER_Open(&g_i2c_master0_ctrl, &g_i2c_master0_cfg);
    APP_ERR_RET(err != FSP_SUCCESS, err, "**R_IIC_MASTER_Open API failed**\r\n");
#elif defined (USE_IIC_B_MODULE)
    err = R_IIC_B_MASTER_Open(&g_i2c_master0_ctrl, &g_i2c_master0_cfg);
    APP_ERR_RET(err != FSP_SUCCESS,err, "**R_IIC_B_MASTER_Open API failed**\r\n");
#endif

    /* Open GPT module to handle timeout for SMBus standard */
    err = R_GPT_Open(&g_comms_smbus_rsc_ctrl0_timer_ctrl, &g_comms_smbus_rsc_ctrl0_timer_cfg);
    APP_ERR_RET(err != FSP_SUCCESS,err, "**R_GPT_Open API failed**\r\n");

    /* Open ELC module to link IIC event to GPT */
    err = R_ELC_Open(&g_elc_ctrl, &g_elc_cfg);
    APP_ERR_RET(err != FSP_SUCCESS,err, "**R_ELC_Open API failed**\r\n");

    /* Open SMBus Middleware driver */
    err = RM_COMMS_SMBUS_Open(&g_comms_smbus0_ctrl, &g_comms_smbus0_cfg);
    APP_ERR_RET(err != FSP_SUCCESS,err, "**RM_COMMS_SMBUS_Open API failed**\r\n");

    return err;
}
/***********************************************************************************************************************
* End of function smbus_init
***********************************************************************************************************************/

/***********************************************************************************************************************
 * @brief       This function writes data to sensor register via SMBus.
 * @param[IN]   value1, value 2     Values to write to register.
 * @retval      FSP_SUCCESS         Upon successful operation.
 * @retval      Any other error code apart from FSP_SUCCESS means the operation failed.
 **********************************************************************************************************************/
static fsp_err_t sensor_write_reg(uint8_t value1, uint8_t value2)
{
    fsp_err_t   err                     = FSP_SUCCESS;
    uint8_t     smbus_buf [SMBUS_THREE_BYTE]  = {CFG_ADR, value1, value2};

    /* Reset FSP communication common event */
    smbus_callback_args.event = (rm_comms_event_t)SMBUS_RESET_EVENT;

    /* Reset SMBus specific error */
    smbus_error->smbus_event = RM_COMMS_SMBUS_NO_ERROR;

    /* Write input values to configuration register of sensor via SMBus  */
    err = RM_COMMS_SMBUS_Write(&g_comms_smbus0_ctrl, smbus_buf, SMBUS_THREE_BYTE);
    APP_ERR_RET(err != FSP_SUCCESS,err, "**RM_COMMS_SMBUS_Write API failed**\r\n");

    /* Wait SMBus event */
    err = smbus_wait_event();
    APP_ERR_RET(err != FSP_SUCCESS,err, "smbus_wait_event failed\r\n");

    return err;
}
/***********************************************************************************************************************
* End of function sensor_write_reg
***********************************************************************************************************************/

/***********************************************************************************************************************
 * @brief       This function reads data from a sensor register via SMBUS.
 * @param[IN]   adr             Address of the selected register.
 * @param[IN]   p_value         Pointer to store register value.
 * @retval      FSP_SUCCESS     Upon successful operation.
 * @retval      Any other error code apart from FSP_SUCCESS means the operation failed.
 **********************************************************************************************************************/
static fsp_err_t sensor_read_reg(uint8_t adr , uint8_t * p_value)
{
    fsp_err_t   err         = FSP_SUCCESS;

    write_read_param.p_src = &adr;
    write_read_param.p_dest = p_value;
    write_read_param.src_bytes = SMBUS_ONE_BYTE;
    write_read_param.dest_bytes = SMBUS_THREE_BYTE; /* 2 data bytes + 1 PEC byte */

    /* Reset FSP communication common event */
    smbus_callback_args.event = (rm_comms_event_t)SMBUS_RESET_EVENT;

    /* Reset SMBus specific error */
    smbus_error->smbus_event = RM_COMMS_SMBUS_NO_ERROR;

    /* Read data from register of sensor  */
    err = RM_COMMS_SMBUS_WriteRead(&g_comms_smbus0_ctrl, write_read_param);
    APP_ERR_RET(err != FSP_SUCCESS,err, "**RM_COMMS_SMBUS_WriteRead API failed**\r\n");

    /* Wait SMBus transmission complete */
    err = smbus_wait_event();
    APP_ERR_RET(err != FSP_SUCCESS,err, "smbus_wait_event failed\r\n");

    return err;
}
/***********************************************************************************************************************
* End of function sensor_read_reg
***********************************************************************************************************************/

/***********************************************************************************************************************
 * @brief       This function waits until SMBus transmission is complete, then filters and displays any SMBus
 *              specific error.
 * @param[IN]   None
 * @retval      FSP_SUCCESS    Upon successful operation.
 * @retval      Any other error code apart from FSP_SUCCESS means the operation failed.
 **********************************************************************************************************************/
static fsp_err_t smbus_wait_event(void)
{
    /* Wait until SMBus transmission complete */
    while (RM_COMMS_EVENT_OPERATION_COMPLETE != smbus_callback_args.event)
    {
        /* Filter SMBus specific error */
        if(RM_COMMS_EVENT_ERROR == smbus_callback_args.event)
        {
            switch (smbus_error->smbus_event)
            {
                case RM_COMMS_SMBUS_MEXT_TIMEOUT:
                    APP_ERR_RET(FSP_ERR_TIMEOUT != FSP_SUCCESS,FSP_ERR_TIMEOUT, "Transmission time between each event exceeded 10 ms\r\n");
                    break;

                case RM_COMMS_SMBUS_SEXT_TIMEOUT:
                    APP_ERR_RET(FSP_ERR_TIMEOUT != FSP_SUCCESS,FSP_ERR_TIMEOUT, "Total transmission time exceeded 25 ms\r\n");
                    break;

                case RM_COMMS_SMBUS_DATA_CORRUPT:
                    APP_ERR_RET(FSP_ERR_INVALID_DATA != FSP_SUCCESS,FSP_ERR_INVALID_DATA, "PEC byte is incorrect\r\n");
                    break;

                case RM_COMMS_SMBUS_MISC_ERROR:
                    APP_ERR_RET(FSP_ERR_ASSERTION != FSP_SUCCESS,FSP_ERR_ASSERTION, "SMBus dependency modules failed\r\n");
                    break;

                default:
                    break;
            }
        }
    }

    return FSP_SUCCESS;
}
/***********************************************************************************************************************
* End of function smbus_wait_event
***********************************************************************************************************************/

/***********************************************************************************************************************
 * @brief       This function sets data to a sensor register via SMBus.
 * @param[IN]   None
 * @retval      FSP_SUCCESS    Upon successful operation
 * @retval      Any other error code apart from FSP_SUCCESS means the operation failed.
 **********************************************************************************************************************/
static fsp_err_t sensor_set_reg(void)
{
    fsp_err_t   err         = FSP_SUCCESS;
    uint8_t     read_buff[CFG_DATA_SIZE]  = {RESET_VALUE};

    /* Write data to register */
    err = sensor_write_reg(CFG_8BIT_HIGHER, CFG_8BIT_LOWER);
    APP_ERR_RET(err != FSP_SUCCESS,err, "sensor_write_reg failed\r\n");

    /* Read back data from register */
    err = sensor_read_reg(CFG_ADR,read_buff);
    APP_ERR_RET(err != FSP_SUCCESS,err, "sensor_read_reg failed\r\n");

    /* Verify data */
    if(CFG_8BIT_HIGHER == read_buff[0] && CFG_8BIT_LOWER == read_buff[1])
    {
        APP_PRINT(CTRL_TEXT_BRIGHT_GREEN "Sensor is configured successfully\r\n" CTRL_RESET);
    }
    else
    {
        APP_ERR_RET(FSP_ERR_ASSERTION != FSP_SUCCESS,FSP_ERR_ASSERTION, "Data read back mismatch data written\r\n");
    }

    return err;
}
/***********************************************************************************************************************
* End of function sensor_set_reg
***********************************************************************************************************************/

/***********************************************************************************************************************
 * @brief       This function initializes necessary hardwares and configures sensor register.
 * @param[IN]   None
 * @retval      FSP_SUCCESS    Upon successful operation.
 * @retval      Any other error code apart from FSP_SUCCESS means the operation failed.
 **********************************************************************************************************************/
static fsp_err_t sensor_init(void)
{
    fsp_err_t   err = FSP_SUCCESS;

    /* Initialize the necessary hardware to control the sensor */
    err = smbus_init();
    APP_ERR_RET(err != FSP_SUCCESS,err, "**smbus_init failed**\r\n");

    /* Configure sensor registers */
    err = sensor_set_reg();
    APP_ERR_RET(err != FSP_SUCCESS,err, "**sensor_set_reg failed**\r\n");

    return err;
}
/***********************************************************************************************************************
* End of function sensor_init
***********************************************************************************************************************/

/***********************************************************************************************************************
 * @brief       This function gets SMBus events.
 * @param[IN]   p_args      Pointer to callback arguments structure.
 * @retval      None
 **********************************************************************************************************************/
void comms_smbus_callback(rm_comms_callback_args_t * p_args)
{
    if (NULL != p_args)
    {
        /* Get FSP communication common event */
        smbus_callback_args.event = p_args->event;

        /* Get SMBus specific error */
        smbus_error = (rm_comms_smbus_error_t *)p_args->p_instance_args;
    }
}
/***********************************************************************************************************************
* End of function comms_smbus_callback
***********************************************************************************************************************/

/***********************************************************************************************************************
 * @brief       This function de-initializes SMBus and dependency modules.
 * @param[IN]   None
 * @retval      None
 **********************************************************************************************************************/
static void sensor_deinit(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Close the IIC Master module if already open */
    if (MODULE_CLOSE != g_i2c_master0_ctrl.open)
    {

#if defined (USE_IIC_MODULE)
        err = R_IIC_MASTER_Close(&g_i2c_master0_ctrl);
        if(FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("**R_IIC_MASTER_Close API failed**\r\n");
        }
#elif defined (USE_IIC_B_MODULE)
        err = R_IIC_B_MASTER_Close(&g_i2c_master0_ctrl);
        if(FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("**R_IIC_B_MASTER_Close API failed**\r\n");
        }
#endif
    }

    /* Close the GPT module if already open */
    if (MODULE_CLOSE != g_comms_smbus_rsc_ctrl0_timer_ctrl.open)
    {
        err = R_GPT_Close(&g_comms_smbus_rsc_ctrl0_timer_ctrl);
        if(FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("**R_GPT_Close API failed**\r\n");
        }
    }

    /* Close the ELC module if already open */
    if (MODULE_CLOSE != g_elc_ctrl.open)
    {
        err = R_ELC_Close(&g_elc_ctrl);
        if(FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("**R_ELC_Close API failed**\r\n");
        }
    }

    /* Close the SMBus if already open */
    if (MODULE_CLOSE != g_comms_smbus0_ctrl.open)
    {
        err = RM_COMMS_SMBUS_Close(&g_comms_smbus0_ctrl);
        if(FSP_SUCCESS != err)
        {
            APP_ERR_PRINT("**RM_COMMS_SMBUS_Close API failed**\r\n");
        }
    }
}
/***********************************************************************************************************************
* End of function sensor_deinit
***********************************************************************************************************************/

/***********************************************************************************************************************
 * @brief       This function sets the state of the Error LED.
 * @param[IN]   state   LED state want to set
 * @retval      None
 **********************************************************************************************************************/
static void led_error_state_set(led_power_t state)
{
    /* Check board has this LED */
    if (LED_ERROR_STATE < g_bsp_leds.led_count)
    {
        R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[LED_ERROR_STATE], (bsp_io_level_t)state);
    }
}
/***********************************************************************************************************************
* End of function led_error_state_set
***********************************************************************************************************************/

/***********************************************************************************************************************
 * @brief       This function handles error if error occurred, closes all opened modules, print and traps error.
 * @param[IN]   err        Error code.
 * @param[IN]   err_str    Error string.
 * @retval None
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err, uint8_t * err_str)
{
    /* Print the error */
    APP_ERR_PRINT(err_str);

    /* De-initialize SMBus and dependency modules */
    sensor_deinit();

    /* Turn ON error LED to indicate an error has occurred */
    led_error_state_set(LED_POWER_ON);

    /* Trap the error */
    APP_ERR_TRAP(err);
}
/***********************************************************************************************************************
* End of function handle_error
***********************************************************************************************************************/
