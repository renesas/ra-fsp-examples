/***********************************************************************************************************************
 * File Name    : periodic_sampling_example_app.c
 * Description  : Example application layer for the ADC GPT Periodic Sampling EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: periodic_sampling_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include "dtc_ep.h"
#include "adc_ep.h"
#include "gpt_ep.h"
#include "elc_ep.h"
#include "periodic_sampling_example_app.h"
#include "console/uart_console.h"

/* Private functions */
static void general_signal_acquisition_init(void);
static void handle_error(fsp_err_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the ADC GPT Periodic Sampling example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void periodic_sampling_example_app_run(void)
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

    /* Initialize the all modules used for signal acquisition */
    general_signal_acquisition_init();

    /* Start ADC module */
    printf("\r\nADC periodic scan started...\r\n");
    err = adc_scan_start();
    handle_error(err, "adc_scan_start failed\r\n");

    /* Start GPT timer */
    err = gpt_start();
    handle_error(err, "gpt_start failed\r\n");

    printf(EP_GUIDE);

    while (true)
    {
        if (true == g_adc0_group_a_flag)
        {
            /* Buffer select for ping-pong buffer */
            static uint8_t buffer_select = 0U;
            buffer_select = (buffer_select == 0U) ? 1U : 0U;

            /* Update destination address and length for transfer */
            g_transfer_adc_group_a[0U].p_dest =
            (void*) &g_buffer_adc[ADC_UNIT_0][ADC_GROUP_SCAN_A][0U][buffer_select][0U];
            g_transfer_adc_group_a[1U].p_dest =
            (void*) &g_buffer_adc[ADC_UNIT_0][ADC_GROUP_SCAN_A][1U][buffer_select][0U];
            g_transfer_adc_group_a[0U].length = NUM_SAMPLES_PER_CHANNEL;
            g_transfer_adc_group_a[1U].length = NUM_SAMPLES_PER_CHANNEL;

            /* Enable transfer */
            err = dtc_enable(&g_transfer_adc0_group_a_ctrl);
            handle_error(err, "dtc_enable for unit 0 group A failed\r\n");

            /* Reset the variable */
            g_adc0_group_a_flag = false;
        }

        else if (true == g_adc0_group_b_flag)
        {
            /* Buffer select for ping-pong buffer */
            static uint8_t buffer_select = 0U;
            buffer_select = (buffer_select == 0U) ? 1U : 0U;

            /* Update destination address and length for transfer */
            g_transfer_adc_group_b[0U].p_dest =
            (void*) &g_buffer_adc[ADC_UNIT_0][ADC_GROUP_SCAN_B][0U][buffer_select][0U];
            g_transfer_adc_group_b[1U].p_dest =
            (void*) &g_buffer_adc[ADC_UNIT_0][ADC_GROUP_SCAN_B][1U][buffer_select][0U];
            g_transfer_adc_group_b[0U].length = NUM_SAMPLES_PER_CHANNEL;
            g_transfer_adc_group_b[1U].length = NUM_SAMPLES_PER_CHANNEL;

            /* Enable transfer */
            err = dtc_enable(&g_transfer_adc0_group_b_ctrl);
            handle_error(err, "dtc_enable for unit 0 group B failed\r\n");

            /* Reset the variable */
            g_adc0_group_b_flag = false;
        }
#if BSP_FEATURE_ADC_UNIT_1_CHANNELS_MASK || BSP_FEATURE_ADC_B_UNIT_1_CHANNELS_MASK
        else if (true == g_adc1_group_a_flag)
        {
            /* Buffer select for ping-pong buffer */
            static uint8_t buffer_select = 0U;
            buffer_select = (buffer_select == 0U) ? 1U : 0U;

            /* Update destination address and length for transfer */
            g_transfer_adc_group_a[2U].p_dest =
            (void*) &g_buffer_adc[ADC_UNIT_1][ADC_GROUP_SCAN_A][0U][buffer_select][0U];
            g_transfer_adc_group_a[3U].p_dest =
            (void*) &g_buffer_adc[ADC_UNIT_1][ADC_GROUP_SCAN_A][1U][buffer_select][0U];
            g_transfer_adc_group_a[2U].length   = NUM_SAMPLES_PER_CHANNEL;
            g_transfer_adc_group_a[3U].length = NUM_SAMPLES_PER_CHANNEL;

            /* Enable transfer */
            err = dtc_enable(&g_transfer_adc1_group_a_ctrl);
            handle_error(err, "dtc_enable for unit 1 group A failed\r\n");

            /* Reset the variable */
            g_adc1_group_a_flag = false;
        }
        else if (true == g_adc1_group_b_flag)
        {
            /* Buffer select for ping-pong buffer */
            static uint8_t buffer_select = 0U;
            buffer_select = (buffer_select == 0U) ? 1U : 0U;

            /* Update destination address and length for transfer */
            g_transfer_adc_group_b[2U].p_dest =
            (void*) &g_buffer_adc[ADC_UNIT_1][ADC_GROUP_SCAN_B][0U][buffer_select][0U];
            g_transfer_adc_group_b[3U].p_dest =
            (void*) &g_buffer_adc[ADC_UNIT_1][ADC_GROUP_SCAN_B][1U][buffer_select][0U];
            g_transfer_adc_group_b[2U].length = NUM_SAMPLES_PER_CHANNEL;
            g_transfer_adc_group_b[3U].length = NUM_SAMPLES_PER_CHANNEL;

            /* Enable transfer */
            err = dtc_enable(&g_transfer_adc1_group_b_ctrl);
            handle_error(err, "dtc_enable for unit 1 group B failed\r\n");

            /* Reset the flag */
            g_adc1_group_b_flag = false;
        }
#endif /* BSP_FEATURE_ADC_UNIT_1_CHANNELS_MASK || BSP_FEATURE_ADC_B_UNIT_1_CHANNELS_MASK */

#if (BSP_PERIPHERAL_ADC_B_PRESENT)
        else if (true == g_err_flag_adc_b)
        {
            handle_error(FSP_ERR_ABORTED, "ADC scan complete event is not received\r\n");
        }
#elif (BSP_PERIPHERAL_ADC_PRESENT)
#if (BSP_FEATURE_ADC_UNIT_1_CHANNELS_MASK)
        else if ((true == g_err_flag_adc0) || (true == g_err_flag_adc1))
        {
            handle_error(FSP_ERR_ABORTED, "ADC scan complete event is not received\r\n");
        }
#else
        else if (true == g_err_flag_adc0)
        {
            handle_error(FSP_ERR_ABORTED, "ADC scan complete event is not received\r\n");
        }
#endif /* BSP_FEATURE_ADC_UNIT_1_CHANNELS_MASK */
#endif /* BSP_PERIPHERAL_ADC_B_PRESENT || BSP_PERIPHERAL_ADC_PRESENT */
        else
        {
            /* Do nothing */
        }
    }
}

/*******************************************************************************************************************//**
 * @brief     This function initializes and enables the ADC, GPT, DTC, and ELC modules.
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
    err = dtc_reconfigure(&g_transfer_adc0_group_a_ctrl, &g_transfer_adc_group_a[0U]);
    handle_error(err, "dtc_reconfigure for unit 0, group A failed\r\n");

    err = dtc_reconfigure(&g_transfer_adc0_group_b_ctrl, &g_transfer_adc_group_b[0U]);
    handle_error(err, "dtc_reconfigure for unit 0, group B failed\r\n");

#if BSP_FEATURE_ADC_UNIT_1_CHANNELS_MASK || BSP_FEATURE_ADC_B_UNIT_1_CHANNELS_MASK
    err = dtc_reconfigure(&g_transfer_adc1_group_a_ctrl, &g_transfer_adc_group_a[2U]);
    handle_error(err, "dtc_reconfigure for unit 1, group A failed\r\n");

    err = dtc_reconfigure(&g_transfer_adc1_group_b_ctrl, &g_transfer_adc_group_b[2U]);
    handle_error(err, "dtc_reconfigure for unit 1, group B failed\r\n");
#endif /* BSP_FEATURE_ADC_UNIT_1_CHANNELS_MASK || BSP_FEATURE_ADC_B_UNIT_1_CHANNELS_MASK */

    /* Initialize ADC module */
    err = adc_init();
    handle_error(err, "adc_init failed\r\n");

    /* Configure ADC scan channels */
    err = adc_channel_config();
    handle_error(err, "adc_channel_config failed\r\n");

    /* Initialize GPT timer */
    err = gpt_init();
    handle_error(err, "gpt_init failed\r\n");

    /* Enable ELC module */
    err = elc_enable();
    handle_error(err, "elc_enable failed\r\n");

    /* Enable DTC module */
    err = dtc_enable(&g_transfer_adc0_group_a_ctrl);
    handle_error(err, "dtc_enable for unit 0, group A failed\r\n");

    err = dtc_enable(&g_transfer_adc0_group_b_ctrl);
    handle_error(err, "dtc_enable for unit 0, group B failed\r\n");

#if BSP_FEATURE_ADC_UNIT_1_CHANNELS_MASK || BSP_FEATURE_ADC_B_UNIT_1_CHANNELS_MASK
    err = dtc_enable(&g_transfer_adc1_group_a_ctrl);
    handle_error(err, "dtc_enable for unit 1, group A failed\r\n");

    err = dtc_enable(&g_transfer_adc1_group_b_ctrl);
    handle_error(err, "dtc_enable for unit 1, group B failed\r\n");
#endif /* BSP_FEATURE_ADC_UNIT_1_CHANNELS_MASK || BSP_FEATURE_ADC_B_UNIT_1_CHANNELS_MASK */
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
        gpt_deinit();
        __BKPT(0);
    }
}
