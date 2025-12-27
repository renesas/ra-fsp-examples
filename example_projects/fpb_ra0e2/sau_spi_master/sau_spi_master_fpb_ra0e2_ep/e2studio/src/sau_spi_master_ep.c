/***********************************************************************************************************************
 * File Name    : sau_spi_master_ep.c
 * Description  : Contains data structures and functions.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "sau_spi_master_ep.h"

/* Global variable */
static volatile spi_event_t g_master_event_flag = SPI_EVENT_TRANSFER_ABORTED;

/* User defined functions */
static fsp_err_t sensor_init(void);
static void sau_spi_handle_error(fsp_err_t err, const char * err_str);
static fsp_err_t wait_for_sau_spi_event(const spi_event_t event);

/***********************************************************************************************************************
 *  Function Name: sau_spi_master_entry
 *  Description  : This function is used to start the SAU SPI Master example operation.
 *  Arguments    : None
 *  Return Value : None
 **********************************************************************************************************************/
void sau_spi_master_entry(void)
{
    fsp_err_t err                   = FSP_SUCCESS;
    fsp_pack_version_t version      = { RESET_VALUE };

    uint8_t read_temperature_reg [DATA_FRAME_SIZE] = {DATA_READ_ADDR};
    uint8_t temperature_values[DATA_FRAME_SIZE]    = {RESET_VALUE};
    char    temp_buff[BUFF_SIZE]                   = {RESET_VALUE};
    float   temperature                            = RESET_VALUE;

    /* Get FSP version */
    R_FSP_VersionGet(&version);

    /* Initialize UART module first to print log to serial terminal */
    err = TERM_INIT();
    if (FSP_SUCCESS != err)
    {
        /* Error trap */
        ERROR_TRAP;
    }

    /* Example project information printed on the Console */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor,
              version.version_id_b.patch);
    APP_PRINT(EP_INFO);

    /* Initialize PMOD MAX31723PMB1 temperature sensor */
    err = sensor_init();
    sau_spi_handle_error(err, "Temperature sensor initialization failed");

    while (true)
    {
        /* Resetting SPI Master event flag */
        g_master_event_flag = (spi_event_t) RESET_VALUE;
        /* Clear temperature buffer */
        memset(temp_buff, RESET_VALUE, sizeof(temp_buff));

        /* Adding conversion time as 200ms for the configured 12-bit resolution,
           before asserting CS_PIN for reading the temperature values in a loop */
        R_BSP_SoftwareDelay(CONVERSION_TIME, CONVERSION_TIME_UNIT);

        /* Assert Slave select pin to start data transfer */
        SS_ASSERT(SS_PIN);

        /* Read the temperature */
        err = R_SAU_SPI_WriteRead(&g_sau_spi_ctrl, read_temperature_reg, temperature_values, sizeof(temperature_values),
                                  SPI_BIT_WIDTH_8_BITS);
        sau_spi_handle_error(err, "r\n**R_SAU_SPI_WriteRead API failed**");

        /* Waiting for transfer complete event and handle error in the case of event failure */
        err = wait_for_sau_spi_event(SPI_EVENT_TRANSFER_COMPLETE);
        sau_spi_handle_error(err, "\r\n**SAU SPI WriteRead timeout expired.**");

        /* De-assert Slave select pin to stop data transfer */
        SS_DE_ASSERT(SS_PIN);

        /* Manipulating the temperature values to print for users */
        temperature = (float) ((int32_t) (((uint32_t) temperature_values[INTEGER_PART] << SHIFT_FOUR_BITS) |
                      ((uint32_t) temperature_values[FRACTIONAL_PART] >> SHIFT_FOUR_BITS))) / FLOAT_DIVISOR;

        /* Ignore dummy data receiving from slave device */
        if (RESET_VALUE != (uint32_t) temperature)
        {
            /* Convert float to string to perform temperature values */
            snprintf(temp_buff, sizeof(temp_buff), "%.04f%s", temperature, DEGREE_SYMBOL);
            APP_PRINT("\r\nTemperature: %sC", temp_buff);

            /* Delay to display temperature values on terminal */
            R_BSP_SoftwareDelay(PRINT_DELAY, BSP_DELAY_UNITS_SECONDS);
        }
    }
}
/***********************************************************************************************************************
* End of function sau_spi_entry
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: sensor_init
 *  Description  : This function configures sensor register so that sensor behaves as expected.
 *  Arguments    : None
 *  Return Value : FSP_SUCCESS     Upon successful operation.
 *                 Any other error code apart from FSP_SUCCESS Unsuccessful operation.
 **********************************************************************************************************************/
static fsp_err_t sensor_init(void)
{
    fsp_err_t   err                                     = FSP_SUCCESS;
    uint8_t read_config_addr                            = CFG_READ_ADDR;
    uint8_t read_config_data[DATA_FRAME_SIZE]           = {RESET_VALUE};

    /* Value for configuration register write. Set resolution as 12-bits */
    const uint8_t config_sensor[DATA_FRAME_SIZE] = { CFG_WRITE_ADDR, SENSOR_RES_CFG };

    /* Initialize sau_spi channel as master */
    err = R_SAU_SPI_Open(&g_sau_spi_ctrl, &g_sau_spi_cfg);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**SAU_SPI open failed**");

    /* Assert Slave select pin to trigger the slave device initiates the first transfer */
    SS_ASSERT(SS_PIN);

    /* Reset Slave select pin for next transfer */
    SS_DE_ASSERT(SS_PIN);

    /* Make sure the CS pin is de-asserted */
    R_BSP_SoftwareDelay(10, BSP_DELAY_UNITS_MILLISECONDS);

    /* Assert Slave select pin to start data transfer */
    SS_ASSERT(SS_PIN);

    /* Resetting SPI Master event flag */
    g_master_event_flag = (spi_event_t) RESET_VALUE;

    /* Configure temperature sensor */
    err = R_SAU_SPI_Write(&g_sau_spi_ctrl, config_sensor, DATA_FRAME_SIZE, SPI_BIT_WIDTH_8_BITS);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**R_SAU_SPI_Write API failed**");

    /* Waiting for transfer complete event and handle error in the case of event failure */
    err = wait_for_sau_spi_event(SPI_EVENT_TRANSFER_COMPLETE);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**SAU SPI Write timeout expired.**");

    /* De-assert Slave select pin to stop data transfer */
    SS_DE_ASSERT(SS_PIN);

    /* Make sure the CS pin is de-asserted */
    R_BSP_SoftwareDelay(10, BSP_DELAY_UNITS_MILLISECONDS);

    /* Assert Slave select pin to start data transfer */
    SS_ASSERT(SS_PIN);

    /* Resetting SPI Master event flag */
    g_master_event_flag = (spi_event_t) RESET_VALUE;

    /* Read configured temperature sensor */
    err = R_SAU_SPI_WriteRead(&g_sau_spi_ctrl, &read_config_addr, read_config_data, DATA_FRAME_SIZE,
                              SPI_BIT_WIDTH_8_BITS);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**R_SAU_SPI_WriteRead API failed**");

    /* Waiting for transfer complete event and handle error in the case of event failure */
    err = wait_for_sau_spi_event(SPI_EVENT_TRANSFER_COMPLETE);
    APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**SAU SPI WriteRead timeout expired.**");

    /* De-assert Slave select pin to stop data transfer */
    SS_DE_ASSERT(SS_PIN);

    if (config_sensor[1] != read_config_data[1])
    {
        /* Incorrect configuration of temperature sensor */
        err = FSP_ERR_INVALID_HW_CONDITION;
        APP_ERR_RET(FSP_SUCCESS != err, err, "\r\n**Incorrect configuration of temperature sensor**");
    }

    APP_PRINT("\r\nSensor is configured successful\r\n");

    return err;
}
/***********************************************************************************************************************
* End of function sensor_init
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: wait_for_sau_spi_event
 *  Description  : This function waits for the data transfer complete event until a timeout occurs and handles errors.
 *  Arguments    : event           Expected events need to wait.
 *  Return Value : FSP_SUCCESS     Upon successful operation.
 *                 Any other error code apart from FSP_SUCCESS Unsuccessful operation.
 **********************************************************************************************************************/
static fsp_err_t wait_for_sau_spi_event(const spi_event_t event)
{
    fsp_err_t   err     = FSP_SUCCESS;
    uint8_t     timeout = UINT8_MAX;
    while(event != g_master_event_flag && --timeout )
    {
        R_BSP_SoftwareDelay(SPI_TIMEOUT_DIV, SPI_TIMEOUT_UNIT);
    }
    if (RESET_VALUE == timeout)
    {
       /* Return time out error if SPI operation fails to complete */
       err = FSP_ERR_TIMEOUT;
    }
    return err;
}
/***********************************************************************************************************************
* End of function wait_for_sau_spi_event
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: sau_spi_handle_error
 *  Description  : This function handles errors, closes all opened modules, and prints errors.
 *  Arguments    : err             Error status.
 *                 err_str         Error message.
 *  Return Value : None
 **********************************************************************************************************************/
static void sau_spi_handle_error(fsp_err_t err, const char *err_str)
{
    if (FSP_SUCCESS != err)
    {
        /* Print the error */
        APP_ERR_PRINT(err_str);

        /* Close opened SAU SPI module */
        if (RESET_VALUE != g_sau_spi_ctrl.open)
        {
            if (FSP_SUCCESS != R_SAU_SPI_Close(&g_sau_spi_ctrl))
            {
                APP_ERR_PRINT("\r\n**R_SAU_SPI_Close API failed**\r\n");
            }
        }
        /* Error trap */
        APP_ERR_TRAP(err);
    }
}
/***********************************************************************************************************************
* End of function sau_spi_handle_error
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: sau_spi_callback
 *  Description  : Master SAU_SPI callback function.
 *  Arguments    : p_args          Pointer to the callback arguments.
 *  Return Value : None
 **********************************************************************************************************************/
void sau_spi_callback(spi_callback_args_t *p_args)
{
    if (NULL != p_args)
    {
        g_master_event_flag = p_args->event;
    }
}
/***********************************************************************************************************************
* End of function sau_spi_callback
***********************************************************************************************************************/
