/***********************************************************************************************************************
 * File Name    : terminal.c
 * Description  : Contains data structures and functions used in terminal.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include "terminal.h"
#include "console/uart_console.h"

/* Global variable */
volatile bool g_write_flag = false;

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the Terminal thread.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void terminal_thread_entry_run(void)
{
    fsp_pack_version_t version = {RESET_VALUE};

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

    while (true)
    {
        /* Check for user input */
        if (uart_console_has_char())
        {
            getchar();
            printf(EP_NOTE);

            /* Delay for the user to open Notepad */
            vTaskDelay(DELAY_3_SEC / portTICK_PERIOD_MS);

            /* Set write flag */
            g_write_flag = true;

            printf(EP_GUIDE);
        }

        vTaskDelay(TASK_DELAY);
    }
}
