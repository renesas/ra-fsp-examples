/***********************************************************************************************************************
 * File Name    : icu_example_app.c
 * Description  : Example application layer for the ICU EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: icu_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include "icu_example_app.h"
#include "icu_ep.h"
#include "console/uart_console.h"

/* Private function */
static void handle_error(fsp_err_t err, const char *err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the ICU example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void icu_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;
    bsp_io_level_t led_current_state = (bsp_io_level_t) RESET_VALUE;
    fsp_pack_version_t version = {RESET_VALUE};

    /* LED type structure */
    bsp_leds_t leds = g_bsp_leds;

    /* Get FLEX pack version information */
    R_FSP_VersionGet(&version);

    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        return;     /* UART failed to open — cannot print anything */
    }

    /* Example project information printed on the terminal */
    printf(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    printf(EP_INFO);

    /* Initialize External IRQ driver */
    err = icu_init();
    handle_error(err, "icu_init failed\r\n");

    /* Enable External IRQ driver */
    err = icu_enable();
    handle_error(err, "icu_enable failed\r\n");

    /* Main loop */
    while (true)
    {
        /* Toggle on-board LED when user push button is pressed */
        if (true == g_sw_press)
        {
            /* Clear user push button flag */
            g_sw_press = false;

            /* Notify that user push button is pressed */
            printf("\r\nUser push button pressed\r\n");

            /* Read on-board LED pin */
            err = R_IOPORT_PinRead(&g_ioport_ctrl, (bsp_io_port_pin_t)leds.p_leds[0], &led_current_state);
            handle_error(err, "R_IOPORT_PinRead API failed\r\n");

            /* Reverse LED pin state */
            led_current_state ^= BSP_IO_LEVEL_HIGH;

            /* Toggle on-board LED */
            err = R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)leds.p_leds[0], led_current_state);
            handle_error(err, "R_IOPORT_PinWrite API failed\r\n");

#if defined (BOARD_RA6T1_RSSK) || defined (BOARD_RA4W1_EK) || defined (BOARD_RA4T1_MCK) || defined (BOARD_RA6T2_MCK)\
    || defined (BOARD_RA6T3_MCK) || defined (BOARD_RA8T1_MCK) || defined (BOARD_RA8T2_MCK)
            if (BSP_IO_LEVEL_LOW == led_current_state)
#else
            if (BSP_IO_LEVEL_HIGH == led_current_state)
#endif /* Specific board */
            {
                /* Print LED pin state */
                printf("LED state: High (ON)\r\n");
            }
            else
            {
                /* Print LED pin state */
                printf("LED state: Low (OFF)\r\n");
            }
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
static void handle_error(fsp_err_t err, const char *err_str)
{
    if (FSP_SUCCESS != err)
    {
        printf("\r\n[ERR] %s", err_str);
        printf("Returned Error Code: 0x%x\r\n", (unsigned int)err);
        uart_console_flush();
        icu_deinit();
        __BKPT(0);
    }
}
