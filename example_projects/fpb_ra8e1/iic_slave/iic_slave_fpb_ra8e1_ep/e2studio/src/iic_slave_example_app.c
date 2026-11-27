/***********************************************************************************************************************
 * File Name    : iic_slave_example_app.c
 * Description  : Example application layer for the IIC Slave EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: iic_slave_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include "iic_slave_example_app.h"
#include "console/uart_console.h"

/* Private function */
static void handle_error(fsp_err_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the IIC Slave example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void iic_slave_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};
    slave_transfer_mode_t slave_operation = SLAVE_READ;

    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        return;     /* UART failed to open — cannot print anything */
    }

    /* Get FLEX pack version information */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the terminal */
    printf(BANNER_INFO, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    printf(EP_INFO);

    set_led(LED_OFF);

    /* Initialize IIC Master and Slave drivers */
    err = init_i2c_driver();
    handle_error(err, "init_i2c_driver failed\r\n");

    while (true)
    {
        /* Perform IIC Slave read and write operations */
        switch (slave_operation)
        {
            case SLAVE_READ:
            {
                set_led(LED_OFF);

                err = i2c_slave_read();
                handle_error(err, "i2c_slave_read failed\r\n");
                blink_led();
                printf("** Slave read operation is successful **\r\n");

                /* Switch to Slave write operation */
                slave_operation = SLAVE_WRITE;

                break;
            }

            case SLAVE_WRITE:
            {
                set_led(LED_OFF);

                err = i2c_slave_write();
                handle_error(err, "i2c_slave_write failed\r\n");
                blink_led();
                printf("** Slave write operation is successful **\r\n");

                /* Switch to Slave read operation */
                slave_operation = SLAVE_READ;

                break;
            }

            default:
            {
                /* Do nothing */
                break;
            }
        }

        /* Delay 1 second to observe LED toggling between read and write operations */
        R_BSP_SoftwareDelay(TOGGLE_DELAY, BSP_DELAY_UNITS_MILLISECONDS);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function handles an error by printing a message and error code, turning ON on-board LEDs,
 *            de-initializing the related modules, and entering a trap state.
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
        set_led(LED_ON);
        deinit_i2c_driver();
        __BKPT(0);
    }
}
