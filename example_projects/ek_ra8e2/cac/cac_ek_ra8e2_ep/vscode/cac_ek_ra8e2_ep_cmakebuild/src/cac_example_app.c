/***********************************************************************************************************************
 * File Name    : cac_example_app.c
 * Description  : Example application layer for the CAC EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: cac_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include "cac_example_app.h"
#include "console/uart_console.h"

/* Private function */
static void handle_error(fsp_err_t err, const char *err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the CAC example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void cac_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};

    /* Initialize UART console before any printing */
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

    /* Initialize CGC */
    err = cgc_init();
    handle_error(err, "cgc_init failed\r\n");

    /* Initialize CAC, then start and verify configured clocks */
    err = cac_init();
    handle_error(err, "cac_init failed\r\n");

    printf(EP_GUIDE);

    while (true)
    {
        /* Process input only when the user has provided one */
        if (uart_console_has_char())
        {
            getchar();
            printf("\r\nStart CAC measurement operation\r\n");

            err = cac_measurement_process();
            handle_error(err, "cac_measurement_process failed\r\n");

            printf("\r\nCAC measurement completed and selected target clock is accurate\r\n");
            printf("\r\nCAC counter value: %d\r\n", (unsigned int)g_cac_counter_val);
            printf("\r\nCAC measurement stopped\r\n");

            printf(EP_GUIDE);
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
        cac_deinit();
        cgc_deinit();
        __BKPT(0);
    }
}
