/***********************************************************************************************************************
 * File Name    : ssi_ep.c
 * Description  : Contains data structures and functions used in ssi_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "ssi_ep.h"

/* Private functions */
static void ssi_example_calculate_samples(uint32_t buffer_index);
static void ssi_example_write();
static void deinit_gpt(void);
static void deinit_ssi(void);

/*******************************************************************************************************************//**
 * @addtogroup SSI_EP
 * @{
 **********************************************************************************************************************/

/* Global variables */
volatile i2s_event_t g_i2s_event       = I2S_EVENT_TX_EMPTY;  /* An actual event updates in callback */
volatile bool g_send_data_in_main_loop = true;
volatile bool g_data_ready             = false;
volatile uint8_t g_buffer_index        = 0;

/* Destination buffer to receive the sample audio data */
uint8_t g_dest_buff[BUFF_SIZE] = {RESET_VALUE};

/* Source buffer to transmit the sample audio data */
int16_t g_src_buff[2][SSI_STREAMING_EXAMPLE_SAMPLES_PER_CHUNK];

/***********************************************************************************************************************
 * The RA Configuration tool generates main() and uses it to generate threads if an RTOS is used. This function is
 * called by main() when no RTOS is used.
 **********************************************************************************************************************/
void ssi_entry(void)
{
    /* Error status */
    fsp_err_t err = FSP_SUCCESS;
    fsp_pack_version_t version = {RESET_VALUE};
    volatile uint32_t time_out = MAX_TIME;          /* time_out value which is used to break the infinite loop */

    /* Version get API for FLEX pack information */
    R_FSP_VersionGet(&version);

    /* Example project information printed on the Console */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor,\
              version.version_id_b.patch);
    APP_PRINT("\r\nThis example project demonstrates audio streaming using the SSI module"
              "\r\nwith a double-buffer mechanism. A sine wave is generated and continuously"
              "\r\nstreamed via a loopback connection between the SSITX and SSIRX pins."
              "\r\nThe sine wave data is stored in two buffers, alternating between them"
              "\r\nto ensure continuous and uninterrupted streaming. The transmitted and"
              "\r\nreceived audio data are compared to verify the successful completion of"
              "\r\nthe SSI transfer. The results will be displayed on the RTT Viewer."
              "\r\nThe sample data can be observed using waveform rendering in the memory"
              "\r\nviewer of e2studio.\r\n");

    /* Open SSI module */
    err = R_SSI_Open(&g_i2s_ctrl, &g_i2s_cfg);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_SSI_Open API failed\r\n");
        /* Trap here */
        APP_ERR_TRAP(err);
    }

    /* Open GPT in periodic mode as internal clock for SSI bit clock */
    err = R_GPT_Open(&g_timer_ctrl, &g_timer_cfg);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_GPT_Open API failed, closing SSI\r\n");
        deinit_ssi();
        /* Trap here */
        APP_ERR_TRAP(err);
    }

    /* Start GPT in periodic mode */
    err = R_GPT_Start(&g_timer_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_GPT_Start API failed, closing SSI and GPT\r\n");
        deinit_ssi();
        deinit_gpt();
        /* Trap here */
        APP_ERR_TRAP(err);
    }

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
                ssi_example_write();
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
                APP_PRINT("\r\nWriteRead operation not complete due to timeout");
                deinit_ssi();
                deinit_gpt();
                /* Trap here */
                APP_ERR_TRAP(FSP_ERR_TIMEOUT);
            }

            /* Compare the transmission of sample audio data from source buffer
             * to destination buffer with WriteRead API */
            int cmp_result = memcmp(g_src_buff[!g_buffer_index], g_dest_buff, SSI_STREAMING_EXAMPLE_SAMPLES_PER_CHUNK);
            if (RESET_VALUE == cmp_result)
            {
                APP_PRINT("\r\nTransmitted and received sample audio data comparison successful");
            }
            else
            {
                APP_PRINT("\r\nTransmitted and received sample audio data comparison failed");
            }
        }
}

/***********************************************************************************************************************
 *  @brief      This function generates stereo audio samples in the form of a sine wave and stores them into 2 buffers
 *              for later transmission.
 *  @param[IN]  buffer_index
 *  @retval     None
 **********************************************************************************************************************/
static void ssi_example_calculate_samples(uint32_t buffer_index)
{
    uint32_t source_freq_hz                 = RESET_VALUE;

    /* Get the source clock frequency (in Hz) */
#if (BSP_PERIPHERAL_GPT_GTCLK_PRESENT && !GPT_CFG_GPTCLK_BYPASS)
    /* Calculate the GPTCK Divider */
     const uint8_t divisor = (uint8_t) R_FSP_ClockDividerGet(R_SYSTEM->GPTCKDIVCR_b.GPTCKDIV);

    /* Calculate the GPTCK Frequency */
    source_freq_hz = R_BSP_SourceClockHzGet((fsp_priv_source_clock_t) R_SYSTEM->GPTCKCR_b.GPTCKSEL)\
                     / divisor;
#else
    /* Get the PCLKD Frequency */
    uint32_t source_freq_hz = R_FSP_SystemClockHzGet (FSP_PRIV_CLOCK_PCLKD);
#endif /* BSP_PERIPHERAL_GPT_GTCLK_PRESENT && !GPT_CFG_GPTCLK_BYPASS */
    uint32_t period = source_freq_hz / g_timer_cfg.period_counts;

    /* Audio sample frequency (bit_clock (Hz) = sampling_frequency (Hz) * channels * system_word_bits
     * (the bit clock for transmitting 2 channels of 16-bit data),
     * audio_clock (Hz) = desired_bit_clock (Hz) * bit_clock_divider) */
    uint32_t audio_sample = (period / (2 * 2 * 16));

    static uint32_t t = 0U;
    /* Create a stereo sine wave. Using formula sample = sin(2 * pi * tone_frequency * t / sampling_frequency) */
    uint32_t freq = SSI_STREAMING_EXAMPLE_TONE_FREQUENCY_HZ;
    for (uint32_t i = 0; i < SSI_STREAMING_EXAMPLE_SAMPLES_PER_CHUNK / 2; i += 1)
    {
        float input = (float) ((2.0f * M_PI * freq * t) / (audio_sample));
        t++;
        /* Store sample twice, once for left channel and once for right channel */
        int16_t sample = (int16_t) ((INT16_MAX * sinf (input)));
        g_src_buff[buffer_index][2 * i] = sample;
        g_src_buff[buffer_index][2 * i + 1] = sample;
    }
    /* Data is ready to be sent in the interrupt */
    g_data_ready = true;
}

/***********************************************************************************************************************
 *  @brief      This function is responsible for transferring data between two buffers
 *              (source buffer and destination buffer).
 *  @param[IN]  None
 *  @retval     None
 **********************************************************************************************************************/
static void ssi_example_write()
{
    /* Set g_dest_buff to zero */
    memset(g_dest_buff, 0, sizeof(g_dest_buff));

    /* Transfer data. This call is non-blocking */
    fsp_err_t err = R_SSI_WriteRead(&g_i2s_ctrl,\
                                    (uint8_t *)g_src_buff[g_buffer_index],\
                                    (uint8_t *)g_dest_buff,\
                                    SSI_STREAMING_EXAMPLE_SAMPLES_PER_CHUNK * sizeof(int16_t));
    if (FSP_SUCCESS == err)
    {
        /* Switch the buffer after data is sent */
        g_buffer_index = !g_buffer_index;
        /* Allow loop to calculate next buffer only if transmission was successful. Clear flag. */
        g_data_ready = false;
    }
    else
    {
        /* Handle error */
        APP_ERR_PRINT("\r\nR_SSI_WriteRead API failed, closing SSI and GPT\r\n");
        deinit_ssi();
        deinit_gpt();
        /* Trap here */
        APP_ERR_TRAP(err);
        /* Getting here most likely means a transmit overflow occurred before the transmit buffer could be reloaded
         * The application must wait until the SSI is idle, then restart transmission
         * In this example, the idle callback transmits data or resets the flag g_send_data_in_main_loop */
    }
  }

/***********************************************************************************************************************
 *  @brief      This function gets SSI events
 *  @param[IN]  p_args
 *  @retval     None
 **********************************************************************************************************************/
void i2s_callback(i2s_callback_args_t *p_args)
{
    if ( NULL != p_args)
    {
        /* Capture callback event for validating the i2s transfer event */
        g_i2s_event = p_args->event;
    }
    /* Reload the transmit buffer if we hit the transmit water mark or restart transmission if the SSI is idle
     * because it was stopped after a transmit buffer overflow */
        if ((I2S_EVENT_TX_EMPTY == p_args->event) || (I2S_EVENT_IDLE == p_args->event))
    {
            if (g_data_ready)
            {
                /* Reload the transmit buffer and handle errors */
                ssi_example_write();
            }
            else
            {
                /* Data was not ready yet, send it in the main loop */
                g_send_data_in_main_loop = true;
            }
     }

}

/***********************************************************************************************************************
 *  @brief      This function is used to close SSI module.
 *  @param[IN]  None
 *  @retval     None
 **********************************************************************************************************************/
static void deinit_ssi(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Close SSI module */
    err = R_SSI_Close(&g_i2s_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_SSI_Close API failed\r\n");
    }
}

/***********************************************************************************************************************
 *  @brief      This function is used to close GPT module.
 *  @param[IN]  None
 *  @retval     None
 **********************************************************************************************************************/
static void deinit_gpt(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Close GPT module */
    err = R_GPT_Close(&g_timer_ctrl);
    /* Handle error */
    if (FSP_SUCCESS != err)
    {
        APP_ERR_PRINT("\r\nR_GPT_Close API failed\r\n");
    }
}
