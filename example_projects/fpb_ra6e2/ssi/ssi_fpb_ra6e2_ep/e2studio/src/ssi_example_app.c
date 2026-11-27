/***********************************************************************************************************************
 * File Name    : ssi_example_app.c
 * Description  : Example application layer for the SSI EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: ssi_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include <string.h>
#include "ssi_ep.h"
#include "ssi_example_app.h"
#include "console/uart_console.h"

/* Private function */
static void handle_error(fsp_err_t err, const char *err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the SSI example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void ssi_example_app_run(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};
    volatile uint32_t time_out = MAX_TIME;  /* time_out value which is used to break the infinite loop */

    /* Initialize UART console before any printing */
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

    /* Open SSI module */
    err = ssi_init();
    handle_error(err, "ssi_init failed\r\n");

    /* Open GPT in periodic mode as internal clock for SSI bit clock, then start it */
    err = gpt_init();
    handle_error(err, "gpt_init failed\r\n");

    /* Prepare samples in the currently active buffer */
    ssi_example_calculate_samples(g_buffer_index);

    /* Prepare samples in the next buffer */
    ssi_example_calculate_samples(!g_buffer_index);

    while (true)
    {
        /* Send data in main loop the first time, and if it was not ready in the interrupt */
        if (g_send_data_in_main_loop)
        {
            /* Clear flag */
            g_send_data_in_main_loop = false;

            /* Reload transmit buffer and handle errors */
            err = ssi_example_write();

            /* Getting here most likely means a transmit overflow occurred before the transmit buffer
             * could be reloaded. The application must wait until the SSI is idle, then restart
             * transmission. In this example, the idle callback transmits data or resets the flag
             * g_send_data_in_main_loop. */
            handle_error(err, "ssi_example_write failed\r\n");
        }

        /* Wait for all transfer was complete of WriteRead operation using I2S_EVENT_RX_FULL event */
        while (I2S_EVENT_RX_FULL != g_i2s_event)
        {
            __NOP();
        }

        /* Set the timeout */
        time_out = MAX_TIME;

        /* Wait for completion of WriteRead operation using I2S_EVENT_IDLE event and time_out. Using these both
         * ensures that the DTC transfer will be over by the time transmit underflow occurs during R_SSI_WriteRead
         * processing. This is important so the receive buffer can be flushed in the transmit underflow error
         * processing. Without this, the last frame (two samples) could be lost during R_SSI_WriteRead */
        while ((I2S_EVENT_IDLE != g_i2s_event) && --time_out)
        {
            __NOP();
        }

        /* Check if a timeout occurred */
        if (RESET_VALUE == time_out)
        {
            handle_error(FSP_ERR_TIMEOUT, "WriteRead operation not complete due to timeout\r\n");
        }

        /* Compare the transmission of sample audio data from source buffer
         * to destination buffer with WriteRead API */
        int cmp_result = memcmp(g_src_buff[!g_buffer_index], g_dest_buff,
                                SSI_STREAMING_EXAMPLE_SAMPLES_PER_CHUNK);
        if (RESET_VALUE == cmp_result)
        {
            printf("\r\nTransmitted and received sample audio data comparison successful\r\n");
        }
        else
        {
            printf("\r\nTransmitted and received sample audio data comparison failed\r\n");
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function handles an error by printing a message and error code, de-initializing the
 *            related modules, and entering a trap state.
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
        /* De-initialize opened modules */
        deinit_ssi();
        deinit_gpt();
        /* Trap here */
        __BKPT(0);
    }
}
