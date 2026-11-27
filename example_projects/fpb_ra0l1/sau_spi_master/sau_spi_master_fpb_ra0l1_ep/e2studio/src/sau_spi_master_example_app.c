/***********************************************************************************************************************
 * File Name    : sau_spi_master_example_app.c
 * Description  : Example application layer for the SAU SPI MASTER EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: sau_spi_master_example_app_run().
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
#include "sau_spi_master_ep.h"
#include "sau_spi_master_example_app.h"
#include "console/uart_console.h"

/* Private function */
static void handle_error(fsp_err_t err, const char * err_str);

/***********************************************************************************************************************
 * Function Name: sau_spi_master_example_app_run
 * Description  : Single entry-point for the SAU SPI MASTER example application.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
void sau_spi_master_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};
    uint8_t read_temperature_reg [DATA_FRAME_SIZE] = {DATA_READ_ADDR};
    uint8_t temperature_values[DATA_FRAME_SIZE] = {RESET_VALUE};
    char temp_buff[BUFF_SIZE] = {RESET_VALUE};
    float temperature = RESET_VALUE;

    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        return;     /* UART failed to open — cannot print anything */
    }

    /* Get FLEX pack version information */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the terminal */
    printf(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    printf(EP_INFO);

    /* Initialize SAU SPI Master module */
    err = sau_spi_master_init();
    handle_error(err, "sau_spi_master_init failed\r\n");

    /* Configure temperature sensor */
    err = sensor_config();
    handle_error(err, "sensor_config failed\r\n");

    printf("\r\nSensor is configured successfully\r\n");

    while (true)
    {
        /* Reset SPI master event flag */
        g_master_event_flag = (spi_event_t)RESET_VALUE;

        /* Clear temperature buffer */
        memset(temp_buff, RESET_VALUE, sizeof(temp_buff));

        /* Add conversion time for 12-bit resolution */
        R_BSP_SoftwareDelay(CONVERSION_TIME, CONVERSION_TIME_UNIT);

        /* Read the temperature */
        err = sau_spi_write_read(read_temperature_reg, temperature_values, DATA_FRAME_SIZE);
        handle_error(err, "sau_spi_write_read for temperature read failed\r\n");

        /* Manipulating the temperature values to print for users */
        temperature = (float) ((int32_t) (((uint32_t) temperature_values[INTEGER_PART] << SHIFT_FOUR_BITS) |
                      ((uint32_t) temperature_values[FRACTIONAL_PART] >> SHIFT_FOUR_BITS))) / FLOAT_DIVISOR;

        /* Ignore dummy data received from slave device */
        if (RESET_VALUE != (uint32_t)temperature)
        {
            printf("\r\nTemperature: %.04f%sC", temperature, DEGREE_SYMBOL);

            /* Delay to display temperature values on the terminal */
            R_BSP_SoftwareDelay(PRINT_DELAY, BSP_DELAY_UNITS_SECONDS);
        }
    }
}
/***********************************************************************************************************************
* End of function sau_spi_master_example_app_run
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: handle_error
 * Description  : This function handles an error by printing error message and code, de-initializing the related
 *                modules, and entering a trap state.
 * Arguments    : err           Error code.
 *                err_str       Error message.
 * Return Value : None.
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err, const char * err_str)
{
    if (FSP_SUCCESS != err)
    {
        printf("\r\n[ERR]: %s", err_str);
        printf("Returned Error Code: 0x%x\r\n", (unsigned int)err);
        uart_console_flush();
        sau_spi_master_deinit();
        __BKPT(0);
    }
}
/***********************************************************************************************************************
* End of function handle_error
***********************************************************************************************************************/
