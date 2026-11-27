/***********************************************************************************************************************
 * File Name    : sci_i2c_example_app.c
 * Description  : Example application layer for the SCI I2C EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: sci_i2c_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include "sci_i2c_example_app.h"
#include "console/uart_console.h"

/* Private function */
static void handle_error(fsp_err_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the SCI I2C example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void sci_i2c_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};

    /* Initialize UART console */
    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        return;
    }

    /* Get FLEX pack version information */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the terminal */
    printf(BANNER_INFO, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    printf(EP_INFO);

    /* Initialize SCI I2C Master and IIC Slave modules */
    err = init_i2c();
    handle_error(err, "init_i2c failed\r\n");

    while (true)
    {
        /* Master writes data to the Slave */
        set_led(LED_OFF);
        err = sci_i2c_master_write();
        handle_error(err, "sci_i2c_master_write failed\r\n");
        toggle_led();
        printf("\r\nSCI I2C Master write operation is successful");

        /* Delay 1 second before perform the next operation */
        R_BSP_SoftwareDelay(DELAY_OPERATION, BSP_DELAY_UNITS_SECONDS);

        /* Master reads back the data transmitted by the Slave */
        set_led(LED_OFF);
        err = sci_i2c_master_read();
        handle_error(err, "sci_i2c_master_read failed\r\n");
        toggle_led();
        printf("\r\nSCI I2C Master read operation is successful\r\n");

        /* Delay 1 second before perform the next operation */
        R_BSP_SoftwareDelay(DELAY_OPERATION, BSP_DELAY_UNITS_SECONDS);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function handles an error by printing error message and code, de-initializing the related modules,
 *            turning ON the on-board LEDs, and entering a trap state.
 * @param[in] err                       Error code.
 * @param[in] err_str                   Error message.
 * @retval    None.
 **********************************************************************************************************************/
static void handle_error(fsp_err_t err, const char * err_str)
{
    if (FSP_SUCCESS != err)
    {
        printf("\r\n[ERR] %s", err_str);
        printf("Returned Error Code: 0x%x\r\n", (unsigned int)err);
        uart_console_flush();
        deinit_sci_i2c();
        deinit_iic();
        set_led(LED_ON);
        __BKPT(0);
    }
}
