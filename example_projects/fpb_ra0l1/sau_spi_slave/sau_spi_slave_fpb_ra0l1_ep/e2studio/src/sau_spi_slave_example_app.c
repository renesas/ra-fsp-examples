/***********************************************************************************************************************
 * File Name    : sau_spi_slave_example_app.c
 * Description  : Example application layer for the SAU SPI Slave EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: sau_spi_slave_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "sau_spi_slave_ep.h"
#include "sau_spi_slave_example_app.h"
#include "temp_sensor.h"
#include "console/uart_console.h"

/* Private function */
static void handle_error(fsp_err_t err, const char * err_str);

/***********************************************************************************************************************
 * Function Name: sau_spi_slave_example_app_run.
 * Description  : Single entry-point for the SAU SPI Slave example application.
 * Arguments    : None.
 * Return Value : None.
 **********************************************************************************************************************/
void sau_spi_slave_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};

    /* Initialize the UART to print logs to the serial terminal */
    if (FSP_SUCCESS != uart_console_init())
    {
        return;     /* UART failed to open — cannot print anything */
    }

    /* Get FLEX pack version information */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the terminal */
    printf(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    printf(EP_INFO);

    /* Initialize, configure, and start the ADC module */
    err = adc_init();
    handle_error(err, "adc_init failed\r\n");

    /* Initialize the SAU SPI Slave module */
    err = sau_spi_slave_init();
    handle_error(err, "sau_spi_slave_init failed\r\n");

    /* Initialize and enable the ICU module to detect the active transfer signal from the Master board */
    err = active_transfer_init();
    handle_error(err, "active_transfer_init failed\r\n");

    while (true)
    {
        if (g_active_transfer)
        {
            /* Reset active transfer flag */
            g_active_transfer = false;

            sau_spi_slave_event_t event = SAU_SPI_SLAVE_EVENT_NONE;
            char temp_buff[30] = {RESET_VALUE};

            /* Receive data and transmit the corresponding data to the Master board */
            err = sau_spi_slave_communicate(&event, temp_buff, sizeof(temp_buff));
            handle_error(err, "sau_spi_slave_communicate failed\r\n");

            switch (event)
            {
                case SAU_SPI_SLAVE_EVENT_CFG_RECEIVED:
                {
                    printf("\r\nReceived data as expected.");
                    break;
                }

                case SAU_SPI_SLAVE_EVENT_READ_CFG_RECEIVED:
                {
                    printf("\r\nStart transmitting the MCU temperature to Master.\r\n");
                    break;
                }

                case SAU_SPI_SLAVE_EVENT_TEMP_SENT:
                {
                    printf("\r\nMCU Temperature Reading from ADC: %sC", temp_buff);
                    break;
                }

                default:
                {
                    /* Do nothing */
                    break;
                }
            }
        }
        else
        {
            /* Delay to reduce CPU usage while waiting for the active transfer flag */
            R_BSP_SoftwareDelay(10, BSP_DELAY_UNITS_MILLISECONDS);
        }
    }
}
/***********************************************************************************************************************
* End of function sau_spi_slave_example_app_run
***********************************************************************************************************************/

/***********************************************************************************************************************
 * Function Name: handle_error.
 * Description  : This function handles an error by printing error message and code, de-initializing the related
 *                modules, and entering a trap state.
 * Arguments    : err                       Error code.
 *                err_str                   Error message.
 * Return Value : None.
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err, const char * err_str)
{
    if (FSP_SUCCESS != err)
    {
        printf("\r\n[ERR] %s", err_str);
        printf("Returned Error Code: 0x%x\r\n", (unsigned int)err);
        uart_console_flush();
        sau_spi_slave_deinit();
        adc_deinit();
        active_transfer_deinit();
        __BKPT(0);
    }
}
/***********************************************************************************************************************
* End of function handle_error
***********************************************************************************************************************/
