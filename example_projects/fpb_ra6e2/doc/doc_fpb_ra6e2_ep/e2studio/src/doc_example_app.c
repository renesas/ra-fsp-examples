/***********************************************************************************************************************
 * File Name    : doc_example_app.c
 * Description  : Example application layer for the DOC EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: doc_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "doc_ep.h"
#include "doc_example_app.h"
#include "console/uart_console.h"

/* Private function */
static void handle_error(fsp_err_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the DOC example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void doc_example_app_run(void)
{
    fsp_err_t err = FSP_SUCCESS;
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

    /* Initialize DOC module */
    err = doc_init();
    handle_error(err, "doc_init failed\r\n");

    /* Write the predefined data to DOC register, depends on the selected event operations will take place */
    switch (g_doc_cfg.event)
    {
        case DOC_EVENT_COMPARISON_MISMATCH:
        {
            printf("\r\nSelected event is Comparison Mismatch");

            /* Write mismatch data to the DOC data input register */
            err = doc_write(DOC_DATA_COMPARISON_MISMATCH);
            handle_error(err, "doc_write for Comparison Mismatch event failed\r\n");
            break;
        }

        case DOC_EVENT_COMPARISON_MATCH:
        {
            printf("\r\nSelected event is Comparison Match");

            /* Write match data to the DOC data input register */
            err = doc_write(DOC_DATA_COMPARISON_MATCH);
            handle_error(err, "doc_write for Comparison Match event failed\r\n");
            break;
        }

        case DOC_EVENT_ADDITION:
        {
            printf("\r\nSelected event is Addition Overflow");

            /* Write first data to the DOC data input register */
            err = doc_write(DOC_DATA_ADD);
            handle_error(err, "doc_write first data for Addition Overflow event failed\r\n");

            /* Write second data to the DOC data input register to occur overflow event */
            err = doc_write(DOC_DATA_OVERFLOW);
            handle_error(err, "doc_write second data for Addition Overflow event failed\r\n");
            break;
        }

        case DOC_EVENT_SUBTRACTION:
        {
            printf("\r\nSelected event is Subtraction Underflow");

            /* Write first data to the DOC data input register */
            err = doc_write(DOC_DATA_SUB);
            handle_error(err, "doc_write first data for Subtraction Underflow event failed\r\n");

            /* Write second data to the DOC data input register to occur underflow event */
            err = doc_write(DOC_DATA_UNDERFLOW);
            handle_error(err, "doc_write second data for Subtraction Underflow event failed\r\n");
            break;
        }

        default:
        {
            break;
        }
    }

    /* Print the result once the callback has set the event flag for a successful operation */
    if (true == g_doc_event_flag)
    {
        /* Reset flag */
        g_doc_event_flag = false;
        printf("\r\nDOC operation is successful for the selected event");

        /* Turn ON LED to indicate callback triggered, along with output on the terminal */
        R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[0], LED_ON);
    }
    else
    {
        printf("\r\nDOC operation failed for the selected event");
    }

    printf("\r\nReset the MCU to run the application again");

    while (true)
    {
        ;
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
        doc_deinit();
        __BKPT(0);
    }
}
