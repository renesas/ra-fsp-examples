/***********************************************************************************************************************
 * File Name    : sau_i2c_master_example_app.c
 * Description  : Example application layer for the SAU I2C Master EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: sau_i2c_master_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include "sau_i2c_master_ep.h"
#include "sau_i2c_master_example_app.h"
#include "console/uart_console.h"

/* Private function */
static void handle_error(fsp_err_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the SAU I2C Master example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void sau_i2c_master_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};
    uint8_t xyz_axis[SENSOR_DATA_SIZE] = {RESET_VALUE};
    float x_axis = SENSOR_RESET_VALUE;
    float y_axis = SENSOR_RESET_VALUE;
    float z_axis = SENSOR_RESET_VALUE;

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

    /* Initialize the SAU I2C module and configure the Pmod ACL sensor */
    err = sensor_init();
    handle_error(err, "sensor_init failed\r\n");

    while (true)
    {
        /* Wait until the sensor has new data */
        err = sensor_wait_data_ready();
        handle_error(err, "sensor_wait_data_ready failed\r\n");

        /* Read raw axis data from the sensor */
        err = sensor_read_data(xyz_axis);
        handle_error(err, "sensor_read_data failed\r\n");

        /* Convert raw sensor data to gravitational unit */
        err = sensor_convert_to_g_unit(xyz_axis, &x_axis, &y_axis, &z_axis);
        handle_error(err, "sensor_convert_to_g_unit failed\r\n");

        printf("X-axis = %.2f g, Y-axis = %.2f g, Z-axis = %.2f g\r\n", x_axis, y_axis, z_axis);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function handles an error by printing error message and code, de-initializing the related modules,
 *            and entering a trap state.
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
        sensor_deinit();
        __BKPT(0);
    }
}
