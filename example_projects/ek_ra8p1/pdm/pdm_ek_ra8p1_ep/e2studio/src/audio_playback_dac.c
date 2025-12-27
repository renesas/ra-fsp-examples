/***********************************************************************************************************************
 * File Name    : audio_playback_dac.c
 * Description  : Contains data structures and functions used to play back the recorded audio by DAC.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "audio_playback_dac.h"

#if (ENABLE_PLAYBACK == 1U)

/* Private global variables */
static volatile bool g_playback_started = false;
static volatile bool g_audio_stop = false;
static volatile bool g_exit_flag = false;
static uint32_t g_sample_offset = RESET_VALUE;

/* External variables */
extern uint16_t g_dac_buf[PDM_BUFFER_NUM_SAMPLES];
extern int16_t g_pdm0_buffer[PDM_BUFFER_NUM_SAMPLES];
extern uint8_t g_pcm_bits;
extern pdm_buf_size_t g_buf_size;

/* Private functions declarations */
static fsp_err_t audio_playback_start(void);
static fsp_err_t audio_playback_stop(void);
static fsp_err_t process_audio_control(uint8_t user_input);
static uint16_t pcm_to_dac12_with_gain(int32_t pcm_data, uint32_t gain_q15);
static void convert_block_pcm_to_dac12(size_t samples, uint16_t volume_percent);

/***********************************************************************************************************************
 *  Function Name: playback_operation
 *  Description  : This function is used to playback the recorded audio by DAC.
 *  Arguments    : None.
 *  Return Value : FSP_SUCCESS    Upon successful operation.
 *                 Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
fsp_err_t playback_operation (void)
{
    fsp_err_t err = FSP_SUCCESS;
    uint8_t user_selection = RESET_VALUE;

    /* Convert to DAC data */
    convert_block_pcm_to_dac12(g_buf_size.samples, PLAYBACK_VOLUME_PERCENT);

    /* Play back menu options */
    APP_PRINT(MENU);

    /* Clear g_exit_flag */
    g_exit_flag = false;

    while (!g_exit_flag)
    {
        if (APP_CHECK_DATA)
        {
            /* Get the selection */
            user_selection = user_input_get();

            /* Process audio according to user input */
            err = process_audio_control(user_selection);
            APP_ERR_RET(FSP_SUCCESS != err, err, "process_user_input function failed\r\n");
        }

        /* Play continuous audio until the user selects audio stop */
        if (g_playback_started && (!g_audio_stop))
        {
            /* Clear the g_playback_started flag */
            g_playback_started = false;

            /* Transfer DAC data from SDRAM to DAC register */
            err = R_DMAC_Reset(&g_transfer0_ctrl, &g_dac_buf[g_sample_offset], (void *) DAC12_DATA_REG, 1);
            APP_ERR_RET(FSP_SUCCESS != err, err, "** R_DMAC_Reset API failed **\r\n");

            /* Replay the audio */
            if (g_sample_offset >= g_buf_size.samples)
            {
                APP_PRINT("\r\nAudio replaying...\r\n");
                g_sample_offset = RESET_VALUE;
            }
        }
    }

    return err;
}
/***********************************************************************************************************************
* End of function playback_operation.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: audio_playback_init
 *  Description  : This function is used to initialize the modules use to playback audio.
 *  Arguments    : None.
 *  Return Value : FSP_SUCCESS    Upon successful operation.
 *                 Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
fsp_err_t audio_playback_init(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize the AGT */
    err = R_AGT_Open(&g_timer0_ctrl, &g_timer0_cfg);
    APP_ERR_RET(FSP_SUCCESS != err, err, "** R_AGT_Open API failed **\r\n");

    /* Initialize the DMAC */
    err = R_DMAC_Open(&g_transfer0_ctrl, &g_transfer0_cfg);
    APP_ERR_RET(FSP_SUCCESS != err, err, "** R_DMAC_Open API failed **\r\n");

    /* Initialize the DAC */
    err = R_DAC_B_Open(&g_dac_b0_ctrl, &g_dac_b0_cfg);
    APP_ERR_RET(FSP_SUCCESS != err, err, "** R_DAC_B_Open API failed **\r\n");

    return err;
}
/***********************************************************************************************************************
* End of function audio_playback_init.
***********************************************************************************************************************/

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

    /* Set g_playback_started flag and clear g_audio_stop flag to start play the audio */
    g_playback_started = true;
    g_audio_stop = false;

    /* Start DAC_B module */
    err = R_DAC_B_Start(&g_dac_b0_ctrl);
    APP_ERR_RET(FSP_SUCCESS != err, err, "**R_DAC_B_Start API failed**\r\n");

    /* Start AGT to trigger DMAC with sample rate same as the record audio frequency */
    err = R_AGT_Start(&g_timer0_ctrl);
    APP_ERR_RET(FSP_SUCCESS != err, err, "**R_AGT_Start API failed**\r\n");

    return err;
}
/***********************************************************************************************************************
* End of function audio_playback_start.
***********************************************************************************************************************/

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
* End of function audio_playback_stop.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: process_audio_control
 *  Description  : This function is used to process the audio control according to user input.
 *  Arguments    : user_input     Audio control selection from user input.
 *  Return Value : FSP_SUCCESS    Upon successful operation.
 *                 Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
static fsp_err_t process_audio_control(uint8_t user_input)
{
    fsp_err_t err = FSP_SUCCESS;
    switch (user_input)
    {
        case PLAY_AUDIO:
        {
            /* Start playing audio */
            err = audio_playback_start();
            APP_ERR_RET(FSP_SUCCESS != err, err, "audio_playback_start failed\r\n");

            APP_PRINT("\r\nAudio replay started (loop mode)\r\n");
            APP_PRINT("Playing recorded audio...\r\n");
        }
        break;
        case STOP_AUDIO:
        {
            /* Stop playing audio */
            err = audio_playback_stop();
            APP_ERR_RET(FSP_SUCCESS != err, err, "audio_playback_stop failed\r\n");

            APP_PRINT("\r\nAudio replay stopped\r\n");
        }
        break;
        case RECORD_NEW_AUDIO:
        {
            /* Force stop playing audio before recording the new audio */
            err = audio_playback_stop();
            APP_ERR_RET(FSP_SUCCESS != err, err, "audio_playback_stop failed\r\n");

            /* Break the playback operation loop */
            g_exit_flag = true;
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
* End of function process_audio_control.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: pcm_to_dac12_with_gain
 *  Description  : This function is used to convert PCM data to DAC 12-bit data.
 *  Arguments    : pcm_data       PCM value.
 *                 gain_q15       Gain value in Q15 format converted from volume percent.
 *  Return Value : dac_val        DAC value converted from PCM value.
 **********************************************************************************************************************/
static uint16_t pcm_to_dac12_with_gain(int32_t pcm_data, uint32_t gain_q15)
{
    int64_t pcm_with_gain = ((int64_t)pcm_data * (int64_t)gain_q15) >> 15;

    /* Signed PCM range */
    int32_t min_val = -(1 << (g_pcm_bits - 1));         /* -2^(N-1) */
    int32_t max_val =  (1 << (g_pcm_bits - 1)) - 1;     /* 2^(N-1)-1 */

    /* Clamp pcm_with_gain within signed PCM range */
    if (pcm_with_gain < min_val) pcm_with_gain = min_val;
    if (pcm_with_gain > max_val) pcm_with_gain = max_val;

    uint32_t pcm_unsigned = (uint32_t)(pcm_with_gain + (1 << (g_pcm_bits - 1)));

    /* Scale to 12-bit DAC range with rounding */
    uint32_t max_unsigned = (1 << g_pcm_bits) - 1;
    uint32_t dac_val = (uint32_t)(pcm_unsigned * DAC12_MAX + (max_unsigned >> 1)) / max_unsigned;

    return (uint16_t) dac_val;
}
/***********************************************************************************************************************
* End of function pcm_to_dac12_with_gain.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: convert_block_pcm_to_dac12
 *  Description  : This function is used to convert the entire recorded data from PCM to DAC values.
 *  Arguments    : samples          Number of desired samples to be converted.
 *                 volume_percent   Target playback volume percentage.
 *  Return Value : None.
 **********************************************************************************************************************/
static void convert_block_pcm_to_dac12(size_t samples, uint16_t volume_percent)
{
    /* Convert percentage to Q15 format with rounding */
    uint32_t gain_q15 = ((volume_percent * Q15_ONE + 50) / 100);

    for (size_t i = 0; i < samples; ++i)
    {
        /* Convert PCM data to DAC 12-bit data */
        g_dac_buf[i] = pcm_to_dac12_with_gain(g_pdm0_buffer[i], gain_q15);
    }
}
/***********************************************************************************************************************
* End of function convert_block_pcm_to_dac12.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: transfer_callback
 *  Description  : This function is used to control the playback data transfer.
 *  Arguments    : None.
 *  Return Value : None.
 **********************************************************************************************************************/
void transfer_callback(transfer_callback_args_t *p_args)
{
    if (NULL != p_args)
    {
        /* Continue playback operation */
        g_playback_started = true;

        /* Transfer next sample */
        g_sample_offset++;
    }
}
/***********************************************************************************************************************
* End of function transfer_callback.
***********************************************************************************************************************/

#else
/* Do nothing when disabling the playback operation */
void transfer_callback(transfer_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);
}

#endif /* ENABLE_PLAYBACK */
