/***********************************************************************************************************************
 * File Name    : baremetal_example_app.c
 * Description  : Example application layer for the Baremetal EP.
 *                - All printing via printf (routed to UART via console library).
 *                - hal_entry.c calls a single function: baremetal_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include "baremetal_example_app.h"
#include "console/uart_console.h"

/* Private function */
static void handle_error(fsp_err_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the Baremetal example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void baremetal_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};

    /* Initialize UART console */
    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        return;
    }

    /* Get FLEX version information */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the terminal */
    printf(BANNER_INFO, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    printf(EP_INFO);

    if (g_bsp_leds.led_count >= 3U)
    {
        printf(EP_GUIDE_3_LEDS);
    }
    else if (2U == g_bsp_leds.led_count)
    {
        printf(EP_GUIDE_2_LEDS);
    }
    else
    {
        /* Do nothing */
    }

    /* Run the baremetal DOC example project code */
    err = baremetal_process();
    handle_error(err, "baremetal_process failed.\r\n");

    printf("\r\nReached application end.\r\n");

    while (true)
    {
        __WFI();
    }
}

/*******************************************************************************************************************//**
 * @brief     This function handles an error by printing error message, code, and entering a trap state.
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
        __BKPT(0);
    }
}
