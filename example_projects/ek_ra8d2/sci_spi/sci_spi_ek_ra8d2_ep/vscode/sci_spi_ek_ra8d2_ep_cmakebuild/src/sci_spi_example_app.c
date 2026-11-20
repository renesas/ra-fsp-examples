/***********************************************************************************************************************
 * File Name    : sci_spi_example_app.c
 * Description  : Example application layer for the SCI SPI EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: sci_spi_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "sci_spi_example_app.h"
#include "console/uart_console.h"

/* Read register address of temperature sensor */
static uint8_t read_temperature_reg[3] = {0x01};

/* Variable to store read temperature values */
static uint8_t temperature_values[3] = {RESET_VALUE};

/* Private function */
static fsp_err_t sci_spi_operation(void);
static fsp_err_t sci_spi_event_check(void);
static void handle_error(fsp_err_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the SCI SPI example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void sci_spi_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};
    uint8_t config_read[2] = {RESET_VALUE};

    /* Value for configuration register write. Set resolution as 12 bits. */
    const uint8_t config_sensor[3] =
    {
        0x80, 0x06
    };

    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        return;     /* UART failed to open — cannot print anything */
    }

    /* Get FLEX pack version information */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the terminal */
    printf(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor,
           version.version_id_b.patch);
    printf(EP_INFO);

    /* Initialize SCI SPI channel as Master */
    err = sci_spi_init();
    handle_error(err, "sci_spi_init failed\r\n");

    /* Assert Slave select pin to start data transfer */
    err = cs_assert();
    handle_error(err, "cs_assert failed\r\n");

    /* Configure temperature sensor */
    err = sci_spi_write(&g_spi_ctrl, &config_sensor[0], sizeof(config_sensor), SPI_BIT_WIDTH_8_BITS);
    handle_error(err, "sci_spi_write for temperature sensor configuration failed\r\n");

    /* Check for transfer complete event and handle error in the case of event failure */
    err = sci_spi_event_check();
    handle_error(err, "sci_spi_event_check for temperature sensor configuration failed\r\n");

    /* De-assert Slave select pin to stop data transfer */
    err = cs_de_assert();
    handle_error(err, "cs_de_assert failed\r\n");

    /* Reset SPI Master event flag */
    g_master_event_flag = RESET_VALUE;

    /* Assert Slave select pin to start data transfer */
    err = cs_assert();
    handle_error(err, "cs_assert failed\r\n");

    /* Read configured temperature sensor */
    err = sci_spi_read(&g_spi_ctrl, &config_read[0], sizeof(config_read), SPI_BIT_WIDTH_8_BITS);
    handle_error(err, "sci_spi_read for temperature sensor configuration failed\r\n");

    /* Check for transfer complete event and handle error in the case of event failure */
    err = sci_spi_event_check();
    handle_error(err, "sci_spi_event_check for temperature sensor configuration failed\r\n");

    /* De-assert Slave select pin to stop data transfer */
    err = cs_de_assert();
    handle_error(err, "cs_de_assert failed\r\n");

    /* Check if sensor is configured as expected */
    if (config_sensor[1] != config_read[1])
    {
        /* Incorrect configuration of temperature sensor */
        err = FSP_ERR_INVALID_HW_CONDITION;
        handle_error(err, "Incorrect configuration of temperature sensor\r\n");
    }

    /* Add conversion time as 200ms for the configured 12-bit resolution,
     * before asserting CS_PIN for reading the temperature values in a loop */
    R_BSP_SoftwareDelay(CONVERSION_TIME, BSP_DELAY_UNITS_MILLISECONDS);

    while (true)
    {
        err = sci_spi_operation();
        handle_error(err, "sci_spi_operation failed\r\n");
    }
}

/*******************************************************************************************************************//**
 * @brief     This function performs SCI SPI operation.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Operated successfully.
 * @retval    Any other error codes     Failed to operate.
 **********************************************************************************************************************/
static fsp_err_t sci_spi_operation(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Variable to store temperature value */
    float temperature = RESET_VALUE;

    /* Reset SPI Master event flag */
    g_master_event_flag = RESET_VALUE;

    /* Assert Slave select pin to start data transfer */
    err = cs_assert();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Read the temperature */
    err = sci_spi_write_read(&g_spi_ctrl, read_temperature_reg, temperature_values, sizeof(temperature_values),
                             SPI_BIT_WIDTH_8_BITS);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Check for transfer complete event and handle error in the case of event failure */
    err = sci_spi_event_check();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* De-assert Slave select pin to stop data transfer */
    err = cs_de_assert();
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Manipulate the read temperature values */
    temperature = (float) ((int32_t)(((uint32_t) temperature_values[2] << 4) |\
                  ((uint32_t) temperature_values[1] >> 4) )) / 16.0f;

    /* Print temperature values */
    printf("\r\nTemperature: %f *C", temperature);

    /* Delay to display temperature values on the terminal */
    R_BSP_SoftwareDelay(PRINT_DELAY, BSP_DELAY_UNITS_SECONDS);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function checks occurrence of data transfer complete event until timeout occurs and returns error.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Transferred successfully.
 * @retval    Any other error codes     Failed to transfer.
 **********************************************************************************************************************/
static fsp_err_t sci_spi_event_check(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Wait counter for wait operation monitoring */
    int32_t g_wait_count = INT32_MAX;

    while (SPI_EVENT_TRANSFER_COMPLETE != g_master_event_flag)
    {
        g_wait_count--;
        if (RESET_VALUE >= g_wait_count)
        {
            printf("\r\nSPI module blocked in Write/Read operation\r\n");

            /* Return time out error if SPI operation fails to complete */
            return FSP_ERR_TIMEOUT;
        }

        /* All other error events are handled here */
        else if (SPI_EVENT_TRANSFER_ABORTED == g_master_event_flag)
        {
            printf("\r\nSPI module transfer aborted.\r\n");
            return FSP_ERR_TRANSFER_ABORTED;
        }
        else
        {
            /* Do nothing */
        }
    }

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function handles an error by printing error message and code, de-initializing the related modules,
 *            and entering a trap state.
 * @param[in] err           Error code.
 * @param[in] err_str       Error message.
 * @retval    None.
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err, const char * err_str)
{
    if (FSP_SUCCESS != err)
    {
        printf("\r\n[ERR]: %s", err_str);
        printf("Returned Error Code: 0x%x\r\n", (unsigned int)err);
        uart_console_flush();
        sci_spi_deinit();
        __BKPT(0);
    }
}
