/***********************************************************************************************************************
 * File Name    : playback_thread_entry.c
 * Description  : This file contain playback implementation and main application implementation.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#include "playback_thread.h"
#include "common_app.h"

/* DAC Playback buffer */
static uint16_t g_dac_buf[FRAME_SIZE*NUM_DAC_FRAME] BSP_PLACE_IN_SECTION(".sdram");

/* Playback state */
static volatile bool g_playback_started = false;
static volatile bool g_audio_stop = false;

static bsp_io_level_t led_current_state = (bsp_io_level_t) BSP_IO_LEVEL_HIGH;

/* DAC frame index */
static volatile uint32_t g_dac_write_frame_idx = RESET_VALUE;
static volatile uint32_t g_dac_read_frame_idx = RESET_VALUE;

/* Boolean flag to determine switch is pressed or not */
volatile bool g_sw_press = false;

/* Private function declaration */
static void         convert_frame_to_dac(uint16_t *p_dac, const int16_t *p_pcm);
static fsp_err_t    audio_playback_init(void);
static fsp_err_t    audio_playback_start(void);
static fsp_err_t    audio_playback_stop(void);
static fsp_err_t    icu_init(void);
static uint8_t      user_input_get (void);
static fsp_err_t    main_app_control(uint8_t user_input, bsp_leds_t st_leds);

/***********************************************************************************************************************
 *  Function Name: transfer_callback
 *  Description  : This function is used to control the playback data transfer.
 *  Arguments    : None.
 *  Return Value : None.
 **********************************************************************************************************************/
void transfer_callback(transfer_callback_args_t *p_args)
{
    fsp_err_t err = FSP_SUCCESS;

    if (NULL != p_args)
    {
        /* Start Record ISR for SYSVIEW observation */
        SEGGER_SYSVIEW_RecordEnterISR();
        g_dac_read_frame_idx = (g_dac_read_frame_idx + 1) % NUM_DAC_FRAME;

        /* Calculate the position base on the number of read frame */
        uint32_t frame_read_idx = g_dac_read_frame_idx * FRAME_SIZE;

        /* Update source buffer for next transfer */
        err = R_DMAC_Reset(&g_transfer0_ctrl, &g_dac_buf[frame_read_idx], (void *)DAC12_DATA_REG, FRAME_SIZE);
        assert(FSP_SUCCESS == err);

        /* Reset AGT timer */
        err = R_AGT_Reset(&g_timer0_ctrl);
        assert(FSP_SUCCESS == err);

        /* Stop Record PDM ISR */
        SEGGER_SYSVIEW_RecordExitISR();
    }
}

/***********************************************************************************************************************
 *  Function Name: button_callback
 *  Description  : This function is used to control the on board button for denoise state control
 *  Arguments    : None.
 *  Return Value : None.
 **********************************************************************************************************************/
void external_irq_callback(external_irq_callback_args_t *p_args)
{
    if (NULL != p_args)
    {
        g_sw_press = true;

        /* Reverse LED pin state */
        led_current_state ^= BSP_IO_LEVEL_HIGH;
    }
}

/***********************************************************************************************************************
 *  Function Name: playback_thread_entry
 *  Description  : Playback thread: handles audio playback and main app functionality
 *  Arguments    : Thread specific parameters.
 *  Return Value : None.
 **********************************************************************************************************************/
void playback_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);
    fsp_err_t err           = FSP_SUCCESS;
    uint8_t user_selection  = RESET_VALUE;

    /* LED type structure */
    bsp_leds_t leds         = g_bsp_leds;

    /* Process frame buffer */
    int16_t     pcm_out_buf[FRAME_SIZE];
    uint32_t    frame_write_idx = RESET_VALUE;

    /* Initialize ICU */
    err = icu_init();
    assert(FSP_SUCCESS == err);
    if (noise_suppression_enable)
    {
        err = R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)leds.p_leds[RESET_VALUE], led_current_state);
        assert(FSP_SUCCESS == err);
    }

    /* Initialize Playback */
    err = audio_playback_init();
    assert(FSP_SUCCESS == err);

    while (!exit_sign)
    {
        /* Control Denoise state via Button */
        if (true == g_sw_press)
        {
            /* Clear user push button flag */
            g_sw_press = false;

            APP_PRINT("\r\nUser Push button Pressed\r\n");

            err = main_app_control(TOGGLE_DENOISE, leds);
            assert(FSP_SUCCESS == err);
        }

        /* Control Denoise state via Command */
        if (APP_CHECK_DATA)
        {
            /* Get the selection */
            user_selection = user_input_get();
            if (user_selection == TOGGLE_DENOISE)
            {
                /* Reverse LED pin state */
                led_current_state ^= BSP_IO_LEVEL_HIGH;
            }

            /* Process audio according to user input */
            err = main_app_control(user_selection, leds);
            assert(FSP_SUCCESS == err);
        }

        /* Receive denoised PCM frame from Audio Thread */
        xStreamBufferReceive(g_denoised_stream,
                             &pcm_out_buf[0],
                             sizeof(pcm_out_buf),
                             portMAX_DELAY);

        /* Calculate the position base on the number of write frame */
        frame_write_idx = g_dac_write_frame_idx * FRAME_SIZE;

        /* Convert PCM frame to DAC for playback operation */
        convert_frame_to_dac(&g_dac_buf[frame_write_idx], &pcm_out_buf[0]);

        /* Update the frame for write dac12 value to buffer and wrap-around */
        g_dac_write_frame_idx = (g_dac_write_frame_idx + 1) % NUM_DAC_FRAME;

        if(!g_playback_started && (g_dac_write_frame_idx >= PLAYBACK_PREBUFFER_FRAMES))
        {
            audio_playback_start();
            g_playback_started = true;
        }
    }
}

/***********************************************************************************************************************
 *  Function Name: convert_frame_to_dac
 *  Description  : This function is used to convert input frame from PCM 16-bit to DAC.
 *  Arguments    : p_dac    Pointer to DAC output data
 *                 p_pcm    Pointer to PCM frame
 *  Return Value : None
 **********************************************************************************************************************/
static void convert_frame_to_dac(uint16_t *p_dac, const int16_t *p_pcm)
{
    /* Convert percentage to Q15 format */
    static const uint32_t gain_q15     = ((PLAYBACK_VOLUME_PERCENT * PLAYBACK_Q15_ONE + 50) / 100);

    static const int32_t  min_val      = -(1 << (PCM_16BITS - 1));       /* -32768 */
    static const int32_t  max_val      = (1  << (PCM_16BITS - 1)) - 1;   /*  32767 */
    static const uint32_t max_unsigned = (1U << PCM_16BITS) - 1U;        /*  65535 */

    for (size_t i = 0; i < FRAME_SIZE; i++)
    {
        int32_t pcm_with_gain = (int32_t)(((int64_t)p_pcm[i] * gain_q15) >> 15);

        if (pcm_with_gain > max_val) pcm_with_gain = max_val;
        if (pcm_with_gain < min_val) pcm_with_gain = min_val;

        uint32_t pcm_unsigned = (uint32_t)(pcm_with_gain + (1 << (PCM_16BITS - 1)));

        /* Scale 12-bit DAC range [0, 4095] with rounding */
        uint32_t dac_val = (uint32_t)(pcm_unsigned * DAC12_MAX + (max_unsigned >> 1)) / max_unsigned;

        p_dac[i] = (uint16_t)dac_val;
    }
}

/***********************************************************************************************************************
 *  Function Name: audio_playback_init
 *  Description  : This function is used to initialize the modules use to playback audio.
 *  Arguments    : None.
 *  Return Value : FSP_SUCCESS    Upon successful operation.
 *                 Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
static fsp_err_t audio_playback_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Set transfer source address to p_src */
    g_transfer0_cfg.p_info->p_src = (void *) &g_dac_buf[0];

    /* Set transfer destination address to the CRC data input register */
    g_transfer0_cfg.p_info->p_dest = (void *)DAC12_DATA_REG;

    /* Initialize the AGT */
    err = R_AGT_Open(&g_timer0_ctrl, &g_timer0_cfg);
    assert(FSP_SUCCESS == err);

    /* Initialize the DMAC */
    err = R_DMAC_Open(&g_transfer0_ctrl, &g_transfer0_cfg);
    assert(FSP_SUCCESS == err);

    /* Enable DMAC transfers. */
    err = R_DMAC_Enable(&g_transfer0_ctrl);
    assert(FSP_SUCCESS == err);

    /* Initialize the DAC */
    err = R_DAC_B_Open(&g_dac_b0_ctrl, &g_dac_b0_cfg);
    assert(FSP_SUCCESS == err);

    return err;
}

/***********************************************************************************************************************
 *  Function Name: audio_playback_start
 *  Description  : This function is used to start the playback operation.
 *  Arguments    : None.
 *  Return Value : FSP_SUCCESS    Upon successful operation.
 *                 Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
static fsp_err_t audio_playback_start(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Start DAC_B module */
    err = R_DAC_B_Start(&g_dac_b0_ctrl);
    assert(FSP_SUCCESS == err);

    /* Start AGT to trigger DMAC with sample rate same as the record audio frequency */
    err = R_AGT_Start(&g_timer0_ctrl);
    assert(FSP_SUCCESS == err);

    return err;
}

/***********************************************************************************************************************
 *  Function Name: audio_playback_stop
 *  Description  : This function is used to stop the playback operation.
 *  Arguments    : None.
 *  Return Value : FSP_SUCCESS    Upon successful operation.
 *                 Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
static fsp_err_t audio_playback_stop(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Set g_audio_stop flag to stop play the audio */
    g_audio_stop = true;

    /* Stop DAC_B module */
    err = R_DAC_B_Stop(&g_dac_b0_ctrl);
    APP_ERR_RET(FSP_SUCCESS != err, err, "**R_DAC_B_Stop API failed**\r\n");

    /* Stop AGT module */
    err = R_AGT_Stop(&g_timer0_ctrl);
    APP_ERR_RET(FSP_SUCCESS != err, err, "**R_AGT_Stop API failed**\r\n");

    return err;
}

/***********************************************************************************************************************
 *  Function Name: button_init
 *  Description  : This function initializes and enable ICU module.
 *  Arguments    : None.
 *  Return Value : FSP_SUCCESS    Upon successful open of ICU module
 *                 Any Other Error code apart from FSP_SUCCESS  Unsuccessful open
 **********************************************************************************************************************/
static fsp_err_t icu_init(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Configure the external interrupt. */
    err = R_ICU_ExternalIrqOpen(&g_external_irq0_ctrl, &g_external_irq0_cfg);
    assert(FSP_SUCCESS == err);

    /* Enable the external interrupt. */
    err = R_ICU_ExternalIrqEnable(&g_external_irq0_ctrl);
    assert(FSP_SUCCESS == err);

    return err;
}

/***********************************************************************************************************************
 *  Function Name: main_app_control
 *  Description  : This function is used to process the main application according to user input.
 *  Arguments    : user_input     Main app control selection from user input.
 *  Return Value : FSP_SUCCESS    Upon successful operation.
 *                 Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
static fsp_err_t main_app_control(uint8_t user_input, bsp_leds_t st_leds)
{
    fsp_err_t err = FSP_SUCCESS;
    switch (user_input)
    {
        /* Toggle application Denoise state */
        case TOGGLE_DENOISE:
        {
            noise_suppression_enable = !noise_suppression_enable;

            /* Toggle user LED */
            err = R_IOPORT_PinWrite(&g_ioport_ctrl, (bsp_io_port_pin_t)st_leds.p_leds[RESET_VALUE], led_current_state);

            if (noise_suppression_enable)
            {
                APP_PRINT("\r\nDenoise ENABLED\r\n");
            }
            else
            {
                APP_PRINT("\r\nDenoise DISABLED (RAW)\r\n");
            }
        }
        break;
        /* Request to calculate the application Latency for DSP process */
        case CALCULATE_LATENCY:
        {
            int16_t latency = calculate_DSP_latency();
            if (latency == -1)
            {
                APP_PRINT("\r\nFail to calculate the latency\r\n");
                break;
            }

            APP_PRINT("\r\nTotal DSP latency: %d us/frame\n", latency);

        }
        break;
        /* Request to capture the application NPU time line */
        case CALCULATE_NPU_CYCLE:
        {
            int32_t ret = capture_NPU_cycles();
            if (ret == -1)
            {
                APP_PRINT("\r\nFail to calculate the cycle\r\n");
                break;
            }

            APP_PRINT("\r\nCompleted capture\n");

        }
        break;
        /* Exit the application */
        case EXIT_SIGN:
        {
            /* Force stop playing audio*/
            err = audio_playback_stop();
            APP_ERR_RET(FSP_SUCCESS != err, err, "audio_playback_stop failed\r\n");

            APP_PRINT("\r\nApplication exited!!!\r\n");
            /* Break the playback thread and capture audio thread */
            exit_sign = true;
        }
        break;
        default:
        {
            APP_PRINT("\r\nInvalid Input entered\r\n");
        }
        break;
    }
    return err;
}

/***********************************************************************************************************************
 *  Function Name: user_input_get
 *  Description  : This function is used to get the user input from terminal.
 *  Arguments    : None.
 *  Return Value : user input in integer.
 **********************************************************************************************************************/
static uint8_t user_input_get (void)
{
    uint8_t input_value = RESET_VALUE;
    uint8_t user_input[TERM_BUFFER_SIZE + 1] = {RESET_VALUE};

    /* Clean user input buffer */
    memset(user_input, NULL_CHAR, sizeof(user_input));

    /* Read user input data from terminal */
    TERM_READ(user_input, sizeof(user_input));

    /* Convert to integer value */
    input_value = (uint8_t)atoi((char *) &user_input[0]);

    return input_value;
}
