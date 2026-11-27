/***********************************************************************************************************************
 * File Name    : interval_sampling_example_app.c
 * Description  : Example application layer for the ADC TAU Interval Sampling EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: interval_sampling_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include "dtc_ep.h"
#include "tau_ep.h"
#include "elc_ep.h"
#include "interval_sampling_example_app.h"
#include "console/uart_console.h"

/* Private functions */
static void general_signal_acquisition_init(void);
static void handle_error(fsp_err_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the ADC TAU Interval Sampling example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void interval_sampling_example_app_run(void)
{
    fsp_pack_version_t version = {RESET_VALUE};
    fsp_err_t err = FSP_SUCCESS;

    uint32_t status = uart_console_init();
    if (FSP_SUCCESS != status)
    {
        return;     /* UART failed to open — cannot print anything */
    }

    /* Get FLEX pack version information */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the terminal */
    printf(BANNER_INFO, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    printf(EP_INFO);

    /* Initialize all modules used for signal acquisition */
    general_signal_acquisition_init();

    /* Start ADC module */
    printf("\r\nADC periodic scan started...\r\n");
    err = adc_scan_start();
    handle_error(err, "adc_scan_start failed\r\n");

    /* Start TAU timer */
    err = tau_start();
    handle_error(err, "tau_start failed\r\n");

    printf(EP_GUIDE);

    while (true)
    {
        if (true == g_adc0_group_flag)
        {
            /* Buffer select for ping-pong buffer */
            static uint8_t buffer_select = 0U;
            buffer_select = (buffer_select == 0U) ? 1U : 0U;

            /* Update destination address and length for transfer */
            g_transfer_adc_group[0U].p_dest
            = (void *) &g_buffer_adc[ADC_UNIT_0][ADC_GROUP_SCAN][0U][buffer_select][0U];
            g_transfer_adc_group[1U].p_dest
            = (void *) &g_buffer_adc[ADC_UNIT_0][ADC_GROUP_SCAN][1U][buffer_select][0U];
            g_transfer_adc_group[0U].length = NUM_SAMPLES_PER_CHANNEL;
            g_transfer_adc_group[1U].length = NUM_SAMPLES_PER_CHANNEL;

            /* Enable transfer */
            err = dtc_enable();
            handle_error(err, "dtc_enable failed\r\n");

            /* Reset the variable */
            g_adc0_group_flag = false;
        }
        /* Check if ADC scan complete event is not received */
        else if (true == g_err_flag_adc0)
        {
            handle_error(FSP_ERR_ABORTED, "ADC callback received an unexpected event\r\n");
        }
        else
        {
            /* Do nothing */
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function initializes and enables the ADC, TAU, DTC, and ELC modules.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void general_signal_acquisition_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize ELC module */
    err = elc_init();
    handle_error(err, "elc_init failed\r\n");

    /* Initialize DTC module */
    err = dtc_init();
    handle_error(err, "dtc_init failed\r\n");

    /* Reconfigure in chain mode */
    err = dtc_reconfigure();
    handle_error(err, "dtc_reconfigure failed\r\n");

    /* Initialize ADC module */
    err = adc_init();
    handle_error(err, "adc_init failed\r\n");

    /* Configure ADC scan channels */
    err = adc_channel_config();
    handle_error(err, "adc_channel_config failed\r\n");

    /* Initialize TAU timer */
    err = tau_init();
    handle_error(err, "tau_init failed\r\n");

    /* Enable the operation of the Event Link Controller */
    err = elc_enable();
    handle_error(err, "elc_enable failed\r\n");

    /* Enable DTC module */
    err = dtc_enable();
    handle_error(err, "dtc_enable failed\r\n");
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
        adc_deinit();
        dtc_deinit();
        elc_deinit();
        tau_deinit();
        __BKPT(0);
    }
}
