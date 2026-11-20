/***********************************************************************************************************************
 * File Name    : iic_master_example_app.c
 * Description  : Example application layer for the IIC Master EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: iic_master_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <stdint.h>
#include "iic_master_example_app.h"
#include "console/uart_console.h"

/* Private function */
static void handle_error(fsp_err_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the IIC Master example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void iic_master_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};
    uint8_t xyz_axis[SENSOR_DATA_SIZE] = {RESET_VALUE};
    float x_axis = DEFAULT_AXIS_VALUE;
    float y_axis = DEFAULT_AXIS_VALUE;
    float z_axis = DEFAULT_AXIS_VALUE;

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

    /* Open the IIC Master module and configure the Pmod ACL sensor */
    err = init_sensor();
    handle_error(err, "init_sensor failed\r\n");

    while (true)
    {
        /* Read axis data from sensor */
        err = read_sensor_data(xyz_axis);
        handle_error(err, "read_sensor_data failed\r\n");

        /* Reconstruct 16-bit axis values from byte pairs */
        x_axis = (float)((int16_t)((xyz_axis[0] | (xyz_axis[1] << BIT_SHIFT_8))));
        y_axis = (float)((int16_t)((xyz_axis[2] | (xyz_axis[3] << BIT_SHIFT_8))));
        z_axis = (float)((int16_t)((xyz_axis[4] | (xyz_axis[5] << BIT_SHIFT_8))));

        printf("X-axis = %.02f, Y-axis = %.02f, Z-axis = %.02f\r\n", x_axis, y_axis, z_axis);

        R_BSP_SoftwareDelay(SENSOR_READ_DELAY, BSP_DELAY_UNITS_SECONDS);
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
        deinit_sensor();
        __BKPT(0);
    }
}
