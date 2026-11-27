/***********************************************************************************************************************
 * File Name    : lvd_example_app.c
 * Description  : Example application layer for the LVD EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: lvd_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include "lvd_example_app.h"
#include "console/uart_console.h"

/* Private function */
static void handle_error(fsp_err_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the LVD example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void lvd_example_app_run(void)
{
    fsp_pack_version_t version = {RESET_VALUE};
    fsp_err_t err = FSP_SUCCESS;
    bsp_io_level_t led_current_state = (bsp_io_level_t) RESET_VALUE;

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
    printf(EP_GUIDE);

    /* If this board has no LEDs then trap here */
    if (RESET_VALUE == g_bsp_leds.led_count)
    {
        handle_error(FSP_ERR_UNSUPPORTED, "No on-board LEDs found\r\n");
    }

    /* Check if reset is caused by LVD0 monitor */
    /* LVD0 is enabled in OFS1 register for this example project with specific detection voltage for each MCU.
     * The user can disable it by going to configuration.xml -> BSP -> Properties -> OFS1 register settings.
     * Here the user will get both the options to enable voltage detection 0 circuit and the voltage level. */
    if (true == lvd0_reset())
    {
        /* Notify the user that reset is caused by LVD0 monitor */
        printf("\r\n************************ LVD0 Reset detected ************************\r\n");

        /* Turn ON LED1 to indicate LVD0 reset detection */
        err = R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t) g_bsp_leds.p_leds[0], LED_ON);
        handle_error(err, "Unable to turn LED ON\r\n");
    }

    /* Open LVD1 instance to detect voltage raise above threshold value by generating a maskable interrupt */
    err = lvd_init();
    handle_error(err, "lvd_init failed\r\n");

    printf("\r\nLVD1 is enabled to generate a maskable interrupt when the input voltage raises above "\
           "the threshold value\r\n");

    while (true)
    {
        /* Check if voltage has increased above the threshold value. If yes, notify the user */
        if (true == g_lvd1_status)
        {
            printf("\r\nVoltage raised above threshold value.");

            /* Read LED2 pin */
            err = R_IOPORT_PinRead(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[1], &led_current_state);
            handle_error(err, "Unable to read LED state\r\n");

            /* Reverse LED pin state */
            led_current_state ^= BSP_IO_LEVEL_HIGH;

            /* Toggle LED2 pin */
            err = R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)g_bsp_leds.p_leds[1], led_current_state);
            handle_error(err, "Unable to toggle LED\r\n");

            /* Clear LVD1 status */
            err = lvd_status_clear();
            handle_error(err, "lvd_status_clear failed\r\n");

            printf("\r\nLVD1 status flag is cleared.\r\n");
            g_lvd1_status = false;
        }
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
        printf("\r\n[ERR]: %s", err_str);
        printf("Returned Error Code: 0x%x\r\n", (unsigned int)err);
        uart_console_flush();
        lvd_deinit();
        __BKPT(0);
    }
}
