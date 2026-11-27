/***********************************************************************************************************************
 * File Name    : smbus_example_app.c
 * Description  : Example application layer for the SMBus EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: smbus_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include "smbus_ep.h"
#include "smbus_example_app.h"
#include "console/uart_console.h"

/* Private functions */
static void led_error_state_set(void);
static void handle_error(fsp_err_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the SMBus example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void smbus_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};
    uint8_t temp_val[TEMP_DATA_SIZE] = {RESET_VALUE};
    float temperature = FLOAT_RESET_VALUE;

    /* Turn OFF all LEDs on the board at startup */
    for (int i = 0; i < g_bsp_leds.led_count; i++)
    {
        R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[i], (bsp_io_level_t) LED_OFF);
    }

    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        /* Turn ON the error LED to indicate uart_console_init failed */
        led_error_state_set();

        /* UART failed to open — cannot print anything */
        return;
    }

    /* Get FLEX pack version information */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the terminal */
    printf(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    printf(EP_INFO);

    /* Initialize the MAX31875 temperature sensor */
    err = sensor_init();
    handle_error(err, "sensor_init failed\r\n");

    printf(CTRL_TEXT_BRIGHT_GREEN "Sensor is configured successfully\r\n" CTRL_RESET);

    while (true)
    {
        /* Read raw data from sensor */
        err = sensor_read_reg(DATA_ADR, temp_val);
        handle_error(err, "sensor_read_reg failed\r\n");

        /* Convert the temperature reading to degree before printing */
        temperature = (float) ((int16_t) (((uint16_t) temp_val[TEMPERATURE_INDEX] << SHIFT_FOUR_BITS)
                      | ((uint8_t) temp_val[TEMPERATURE_INDEX + 1] >> SHIFT_FOUR_BITS))) / FLOAT_DIVISOR;

        printf("Temperature: %.04f%sC\r\n", temperature, DEGREE_SYMBOL);

        /* Delay between temperature readings */
        R_BSP_SoftwareDelay(PRINT_DELAY, BSP_DELAY_UNITS_SECONDS);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function turns ON the error LED.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void led_error_state_set(void)
{
    /* Turn ON the LED */
    R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[0U], (bsp_io_level_t) LED_ON);
}

/*******************************************************************************************************************//**
 * @brief     This function handles an error by printing error message and code, de-initializing the related modules,
 *            turning ON the LED, and entering a trap state.
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
        led_error_state_set();
        __BKPT(0);
    }
}
