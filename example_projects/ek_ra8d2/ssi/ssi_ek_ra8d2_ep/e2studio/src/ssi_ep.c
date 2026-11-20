/***********************************************************************************************************************
 * File Name    : ssi_ep.c
 * Description  : Contains data structures and functions used in ssi_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "ssi_ep.h"

/* Global variables */
volatile i2s_event_t g_i2s_event = I2S_EVENT_TX_EMPTY;      /* An actual event updates in callback */
volatile bool g_send_data_in_main_loop = true;
volatile uint8_t g_buffer_index = RESET_VALUE;

/* Destination buffer to receive the sample audio data */
uint8_t g_dest_buff[BUFF_SIZE] = {RESET_VALUE};

/* Source buffer to transmit the sample audio data */
int16_t g_src_buff[2][SSI_STREAMING_EXAMPLE_SAMPLES_PER_CHUNK];

/*******************************************************************************************************************//**
 * @brief     This function opens the SSI module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Opened successfully.
 * @retval    Any other error codes     Failed to open.
 **********************************************************************************************************************/
fsp_err_t ssi_init(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Open SSI module */
    err = R_SSI_Open(&g_i2s_ctrl, &g_i2s_cfg);
    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function opens and starts the GPT module used as the SSI bit clock.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Opened and started successfully.
 * @retval    Any other error codes     Failed to open or start.
 **********************************************************************************************************************/
fsp_err_t gpt_init(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Open GPT in Periodic mode as internal clock for SSI bit clock */
    err = R_GPT_Open(&g_timer_ctrl, &g_timer_cfg);
    if (FSP_SUCCESS != err)
    {
        return err;
    }

    /* Start GPT in Periodic mode */
    err = R_GPT_Start(&g_timer_ctrl);
    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function generates stereo audio samples in the form of a sine wave and stores them in
 *            the specified buffer slot for later transmission.
 * @param[in] buffer_index              Buffer slot to fill (0 or 1).
 * @retval    None.
 **********************************************************************************************************************/
void ssi_example_calculate_samples(uint32_t buffer_index)
{
    timer_info_t timer_info = {RESET_VALUE, RESET_VALUE, RESET_VALUE};

    /* Get the source clock frequency (in Hz) */
    R_GPT_InfoGet(&g_timer_ctrl, &timer_info);

    uint32_t period = timer_info.clock_frequency / g_timer_cfg.period_counts;

    /* Audio sample frequency (bit_clock (Hz) = sampling_frequency (Hz) * channels * system_word_bits
     * (the bit clock for transmitting 2 channels of 16-bit data),
     * audio_clock (Hz) = desired_bit_clock (Hz) * bit_clock_divider) */
    uint32_t audio_sample = (period / (2U * 2U * 16U));

    static uint32_t t = 0U;

    /* Create a stereo sine wave. Using formula sample = sin(2 * pi * tone_frequency * t / sampling_frequency) */
    uint32_t freq = SSI_STREAMING_EXAMPLE_TONE_FREQUENCY_HZ;
    for (uint32_t i = 0; i < (SSI_STREAMING_EXAMPLE_SAMPLES_PER_CHUNK / 2U); i++)
    {
        float input = (float)((2.0f * M_PI * freq * t) / audio_sample);
        t++;

        /* Store sample twice, once for left channel and once for right channel */
        int16_t sample = (int16_t)(INT16_MAX * sinf(input));
        g_src_buff[buffer_index][2U * i]      = sample;
        g_src_buff[buffer_index][2U * i + 1U] = sample;
    }
}

/*******************************************************************************************************************//**
 * @brief     This function is responsible for transferring data between source and destination buffers
 *            via an SSI WriteRead operation. On success it advances the buffer index and clears
 *            the data-ready flag.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Transfer started successfully.
 * @retval    Any other error codes     Failed to start transfer.
 **********************************************************************************************************************/
fsp_err_t ssi_example_write(void)
{
    /* Set g_dest_buff to zero */
    memset(g_dest_buff, 0, sizeof(g_dest_buff));

    /* Error status */
    fsp_err_t err = FSP_SUCCESS;

    /* Transfer data. This call is non-blocking */
    err = R_SSI_WriteRead(&g_i2s_ctrl,
                          (uint8_t *)g_src_buff[g_buffer_index],
                          (uint8_t *)g_dest_buff,
                          SSI_STREAMING_EXAMPLE_SAMPLES_PER_CHUNK * sizeof(int16_t));
    if (FSP_SUCCESS == err)
    {
        /* Switch the buffer after data is sent */
        g_buffer_index = (uint8_t)(!g_buffer_index);
    }
    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function is the SSI callback function that captures I2S events and sets a flag
 *            to reload the transmit buffer in the main loop.
 * @param[in] p_args                    Callback function parameter data.
 * @retval    None.
 **********************************************************************************************************************/
void i2s_callback(i2s_callback_args_t *p_args)
{
    if (NULL != p_args)
    {
        /* Capture callback event for validating the i2s transfer event */
        g_i2s_event = p_args->event;
    }

    /* Reload the transmit buffer if we hit the transmit watermark, or restart transmission if the SSI is idle
     * because it was stopped after a transmit buffer overflow */
    if ((I2S_EVENT_TX_EMPTY == p_args->event) || (I2S_EVENT_IDLE == p_args->event))
    {
        g_send_data_in_main_loop = true;
    }
}

/*******************************************************************************************************************//**
 * @brief     This function closes the opened SSI module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void deinit_ssi(void)
{
    /* Close the opened SSI module */
    if (g_i2s_ctrl.open)
    {
        R_SSI_Close(&g_i2s_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function closes the opened GPT module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void deinit_gpt(void)
{
    /* Close the opened GPT module */
    if (g_timer_ctrl.open)
    {
        R_GPT_Close(&g_timer_ctrl);
    }
}
