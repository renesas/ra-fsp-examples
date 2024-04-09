/***********************************************************************************************************************
 * File Name    : sau_spi_ep.c
 * Description  : Contains data structures and functions used in hal_entry.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2024 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#include "sau_spi_ep.h"
#include "common_utils.h"
/*******************************************************************************************************************//**
 * @addtogroup r_sau_spi_ep
 * @{
 **********************************************************************************************************************/

/* Global Variables */
static volatile spi_event_t g_master_event_flag = SPI_EVENT_TRANSFER_ABORTED;
static fsp_err_t sensor_init ();
static void sau_spi_handle_error (fsp_err_t err, const char * err_str);
static fsp_err_t wait_for_sau_spi_event (const spi_event_t event);

/*******************************************************************************************************************//**
 *  @brief       This function is used to start the sau spi example operation.
 *  @param[in]   None
 *  @retval      None
 **********************************************************************************************************************/
void sau_spi_entry(void)
{
    fsp_err_t err                   = FSP_SUCCESS;
    fsp_pack_version_t version      = { RESET_VALUE };

    uint8_t read_temperature_reg [DATA_FRAME_SIZE] = {DATA_READ_ADDR};
    uint8_t temperature_values[DATA_FRAME_SIZE]    = {RESET_VALUE};
    char    temp_buff[BUFF_SIZE]                   = {RESET_VALUE};
    float   temperature                            = RESET_VALUE;

    /* Get FSP version */
    R_FSP_VersionGet (&version);

    /* Example Project information printed on the Console */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    APP_PRINT(EP_INFO);

    /* Initialize PMOD MAX31723PMB1 temperature sensor */
    err = sensor_init();
    sau_spi_handle_error(err, "Temperature sensor initialization failed");

    while (true)
    {
        /* Resetting SPI Master event flag */
        g_master_event_flag = (spi_event_t) RESET_VALUE;
        /* Clear temperature buffer */
        memset (temp_buff, RESET_VALUE, sizeof(temp_buff));

        /* Adding conversion time as 200ms for the configured 12-bit resolution,
           before asserting CS_PIN for reading the temperature values in a loop */
        R_BSP_SoftwareDelay (CONVERSION_TIME, CONVERSION_TIME_UNIT);

        /* Assert Slave select pin to start data transfer */
        CS_ASSERT (CS_PIN);

        /* Read the temperature */
        err = R_SAU_SPI_WriteRead (&g_sau_spi_ctrl, read_temperature_reg, temperature_values, sizeof(temperature_values), SPI_BIT_WIDTH_8_BITS);
        sau_spi_handle_error(err, "r\n **SPI WriteRead API failed**");

        /* Waiting for transfer complete event and handle error in the case of event failure */
        err = wait_for_sau_spi_event(SPI_EVENT_TRANSFER_COMPLETE);
        sau_spi_handle_error (err, "\r\n **SAU SPI WriteRead timeout expired.**");

        /* De-assert Slave select pin to stop data transfer */
        CS_DE_ASSERT (CS_PIN);

        /* Manipulating the temperature values to print for users */
        temperature = (float) ((int32_t) (((uint32_t) temperature_values[TEMPERATURE_INDEX] << SHIFT_FOUR_BITS) | ((uint32_t) temperature_values[TEMPERATURE_INDEX - 1] >> SHIFT_FOUR_BITS))) / FLOAT_DIVISOR;

        /* Convert float to string to perform temperature values */
        snprintf (temp_buff, sizeof(temp_buff), "%.04f%s", temperature,DEGREE_SYMBOL);
        APP_PRINT ("\r\nTemperature: %sC", temp_buff);

        /* Delay to display temperature values on RTT viewer */
        R_BSP_SoftwareDelay (PRINT_DELAY, BSP_DELAY_UNITS_SECONDS);
    }
}

/*******************************************************************************************************************//**
 *  @brief       This function configures sensor register so that sensor behaves as expected
 *  @param[in]   None
 *  @retval      FSP_SUCCESS    Upon successful operation
 *  @retval      Any Other Error code apart from FSP_SUCCES
 **********************************************************************************************************************/

static fsp_err_t sensor_init ()
{
    fsp_err_t   err                                = FSP_SUCCESS;
    uint8_t read_config_addr                       = CFG_READ_ADDR;
    uint8_t read_config_data[DATA_FRAME_SIZE]      = {RESET_VALUE};

    /* Value for configuration register write. Set resolution as 12-bits*/
    const uint8_t config_sensor[DATA_FRAME_SIZE] = { CFG_WRITE_ADDR, SENSOR_RES_CFG };

    /* Initialize sau_spi channel as master */
    err = R_SAU_SPI_Open (&g_sau_spi_ctrl, &g_sau_spi_cfg);
    APP_ERR_RETURN (err, "\r\n **SAU_SPI open failed**");

    /* Assert Slave select pin to start data transfer */
    CS_ASSERT (CS_PIN);

    /* Resetting SPI Master event flag */
    g_master_event_flag = (spi_event_t) RESET_VALUE;

    /* Configure temperature sensor */
    err = R_SAU_SPI_Write (&g_sau_spi_ctrl, config_sensor, sizeof(config_sensor), SPI_BIT_WIDTH_8_BITS);
    APP_ERR_RETURN (err, "\r\n **SPI Write API failed**");

    /* Waiting for transfer complete event and handle error in the case of event failure */
    err = wait_for_sau_spi_event(SPI_EVENT_TRANSFER_COMPLETE);
    APP_ERR_RETURN (err, "\r\n **SAU SPI Write timeout expired.**");

    /* De-assert Slave select pin to stop data transfer */
    CS_DE_ASSERT (CS_PIN);

    /* Resetting SPI Master event flag */
    g_master_event_flag = (spi_event_t) RESET_VALUE;

    /* Assert Slave select pin to start data transfer */
    CS_ASSERT (CS_PIN);

    /* Read configured temperature sensor */
    err = R_SAU_SPI_WriteRead (&g_sau_spi_ctrl, &read_config_addr, read_config_data, sizeof(read_config_data), SPI_BIT_WIDTH_8_BITS);
    APP_ERR_RETURN(err, "\r\n **SPI WriteRead API failed**");

    /* Waiting for transfer complete event and handle error in the case of event failure */
    err = wait_for_sau_spi_event(SPI_EVENT_TRANSFER_COMPLETE);
    APP_ERR_RETURN (err, "\r\n **SAU SPI WriteRead timeout expired.**");

    /* De-assert Slave select pin to stop data transfer */
    CS_DE_ASSERT (CS_PIN);

    /* Check if sensor is configured as expected */
    if (config_sensor[1] != read_config_data[1])
    {
        /* Incorrect configuration of temperature sensor */
        err = FSP_ERR_INVALID_HW_CONDITION;
        APP_ERR_RETURN(err, "\r\n **Incorrect configuration of temperature sensor**");
    }
    else
    {
        APP_PRINT ("\r\nSensor is configured successful\r\n");
    }
    return err;
}

/*******************************************************************************************************************//**
 *  @brief This function wait data transfer complete event until timeout occurs and handles errors.
 *  @param[in]   event          Expected events need to wait
 *  @retval      FSP_SUCCESS    Upon successful operation
 *  @retval      Any other error code apart from FSP_SUCCESS
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

/*******************************************************************************************************************//**
 * @brief This function handles errors, closes all opened modules, and prints errors.
 * @param[IN]   err             Error status
 * @param[IN]   err_str         Error message
 * @retval      None
 **********************************************************************************************************************/
static void sau_spi_handle_error(fsp_err_t err, const char *err_str)
{
    if (FSP_SUCCESS != err)
    {
        /* Print the error */
        APP_ERR_PRINT(err_str);

        /* Close opened SAU SPI module*/
        if (RESET_VALUE != g_sau_spi_ctrl.open)
        {
            if (FSP_SUCCESS != R_SAU_SPI_Close (&g_sau_spi_ctrl))
            {
                APP_ERR_PRINT("\r\n**SAU SPI Close API failed**\r\n");
            }
        }
        /* Error trap */
        APP_ERR_TRAP(err);
    }
}

/*******************************************************************************************************************//**
 * @brief Master SAU_SPI callback function.
 * @param[in]  p_args   Callback
 * @retval     None
 **********************************************************************************************************************/
void sau_spi_callback(spi_callback_args_t *p_args)
{
    if (NULL != p_args)
    {
        g_master_event_flag = p_args->event;
    }
}
