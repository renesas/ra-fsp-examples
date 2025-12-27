/***********************************************************************************************************************
 * File Name    : pdm_ep.c
 * Description  : Contains data structures and functions use to record audio.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "pdm_ep.h"
#include "vox_wake_up.h"
#include "audio_playback_dac.h"

/* Private function declaration */
static pdm_buf_size_t pdm_calc_buffer_from_seconds(uint8_t seconds);
static fsp_err_t pdm_error_check(void);

/* Public global variables */
int32_t g_pdm0_buffer[PDM_BUFFER_NUM_SAMPLES] BSP_PLACE_IN_SECTION (BSP_UNINIT_SECTION_PREFIX ".sdram") = {0};
pdm_buf_size_t g_buf_size = {RESET_VALUE, RESET_VALUE};
uint8_t g_pcm_bits = RESET_VALUE;

#if (ENABLE_PLAYBACK == 1U)
uint16_t g_dac_buf[PDM_BUFFER_NUM_SAMPLES] BSP_PLACE_IN_SECTION (BSP_UNINIT_SECTION_PREFIX ".sdram")= {0};
#endif /* ENABLE_PLAYBACK */

/* Private global variables */
static volatile bool g_stop_receive_data = false;
static volatile bool g_sound_detect = false;
static volatile pdm_error_t g_pdm_error = PDM_ERROR_NONE;

/***********************************************************************************************************************
 *  Function Name: pdm_ep_entry
 *  Description  : This function is used to start PDM example operation.
 *  Arguments    : None.
 *  Return Value : None.
 **********************************************************************************************************************/
void pdm_ep_entry(void)
{
    fsp_pack_version_t  version = {RESET_VALUE};
    fsp_err_t err = FSP_SUCCESS;
    uint8_t record_time = RESET_VALUE;

    /* Initialize the terminal */
    err = TERM_INIT();
    if (FSP_SUCCESS != err)
    {
        ERROR_TRAP;
    }

    /* Version get API for FSP pack information */
    R_FSP_VersionGet(&version);

    /* Example Project information printed on the terminal */
    APP_PRINT(BANNER_INFO, EP_VERSION, version.version_id_b.major, version.version_id_b.minor, version.version_id_b.patch);
    APP_PRINT(EP_INFO);

#if (ENABLE_PLAYBACK == 1U)

    /* Initialize the required modules to play back recorded audio */
    err = audio_playback_init();
    handle_error(err, "**audio_playback_init failed**\r\n");
#endif /* ENABLE_PLAYBACK */

    /* Initialize PDM module */
    err = R_PDM_Open(&g_pdm0_ctrl, &g_pdm0_cfg);
    handle_error(err, "**R_PDM_Open API failed**\r\n");

    /* Wait for filter settling and startup time */
    R_BSP_SoftwareDelay(PDM2_FILTER_SETTLING_TIME_US + PDM_MIC_STARTUP_TIME_US, BSP_DELAY_UNITS_MICROSECONDS);

    /* Configure the sound detection range */
    pdm_sound_detection_setting_t sound_detection_setting =
    {
        .sound_detection_lower_limit = PDM_SDE_LOWER_LIMIT,
        .sound_detection_upper_limit = PDM_SDE_UPPER_LIMIT
    };

    /* Check configured PCM bits output data */
    g_pcm_bits = g_pdm0_cfg.pcm_width <= PDM_PCM_WIDTH_20_BITS_3_18 ? PCM_20BITS : PCM_16BITS;

    while (true)
    {
        APP_PRINT(TIME_RECORD, PDM_DURATION_MAX_IN_SEC);

        while (!APP_CHECK_DATA) {;}

        /* Get recorded time from user input */
        record_time = user_input_get();

        if (PDM_DURATION_MAX_IN_SEC < record_time || RESET_VALUE >= record_time)
        {
            APP_PRINT("\r\nInvalid input \r\n");
            continue;
        }

        /* Enable sound detection */
        err = R_PDM_SoundDetectionEnable(&g_pdm0_ctrl, sound_detection_setting);
        handle_error(err, "**R_PDM_SoundDetectionEnable API failed**\r\n");

#if (ENABLE_VOX_WAKEUP == 1U)
        APP_PRINT("\r\n\r\nMCU entered LPM SW standby mode.");
#endif /* ENABLE_VOX_WAKEUP */

        APP_PRINT("\r\n\r\nWaiting for sound detection to begin recording...\r\n");

#if (ENABLE_VOX_WAKEUP == 1U)

        /* Perform VOX wake up operation from LPM SW standby mode by sound detection */
        err = vox_wake_up_operation();
        handle_error(err, "vox_wake_up_operation failed\r\n");
#endif /* ENABLE_VOX_WAKEUP */

        /* Make sure detect the sound before recording the audio */
        while (!g_sound_detect) {;}

        /* Reset g_sound_detect flag */
        g_sound_detect = false;

        /* Disable sound detection */
        err = R_PDM_SoundDetectionDisable(&g_pdm0_ctrl);
        handle_error(err, "**R_PDM_SoundDetectionDisable API failed**\r\n");

        /* Calculate size of record audio */
        g_buf_size = pdm_calc_buffer_from_seconds(record_time);

        APP_PRINT("\r\nStart recording in %d seconds ...\r\n", record_time);

        /* Start recording data */
        err = R_PDM_Start(&g_pdm0_ctrl, g_pdm0_buffer, g_buf_size.bytes, (g_buf_size.samples)/2);
        handle_error(err, "**R_PDM_Start API failed**\r\n");

        /* Wait until the desired data is fully recorded */
        while (!g_stop_receive_data);

        /* Clear g_stop_receive_data flag */
        g_stop_receive_data = false;

        /* Handle all occurred error events */
        err = pdm_error_check();
        handle_error(err, "PDM errors are detected\r\n");

        APP_PRINT("\r\nRecord successfully\r\n");

        /* Stop recording */
        err = R_PDM_Stop(&g_pdm0_ctrl);
        handle_error(err, "**R_PDM_Stop API failed**\r\n");

#if (ENABLE_PLAYBACK == 1U)

        /* Perform playback operation */
        playback_operation();
        handle_error(err, "playback_operation failed\r\n");
#endif /* ENABLE_PLAYBACK */
    }
}
/***********************************************************************************************************************
* End of function pdm_ep_entry.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: user_input_get
 *  Description  : This function is used to get the user input from terminal.
 *  Arguments    : None.
 *  Return Value : user input in integer.
 **********************************************************************************************************************/
uint8_t user_input_get (void)
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
/***********************************************************************************************************************
* End of function user_input_get.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: pdm_calc_buffer_from_seconds
 *  Description  : This function is used to calculate the recorded buffer size.
 *  Arguments    : None.
 *  Return Value : Recorded buffer size (in samples and bytes).
 **********************************************************************************************************************/
static pdm_buf_size_t pdm_calc_buffer_from_seconds(uint8_t seconds)
{
    uint32_t raw = seconds * PDM_FS_HZ * PDM_CHANNELS;

    /* Protect against overflow prior to cast */
    raw = raw > UINT32_MAX ? UINT32_MAX : raw;

    /* Round to nearest sample and align up to the frame boundary */
    uint32_t samples     = PDM_ALIGN_UP(raw, PDM_FRAME_SAMPLES);

    /* Calculate and round up the used bytes */
    uint32_t bytes       = samples * (g_pcm_bits + (8-1) / 8U);

    pdm_buf_size_t out   = {samples, bytes};
    return out;
}
/***********************************************************************************************************************
* End of function pdm_calc_buffer_from_seconds.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: pdm_callback
 *  Description  : This function is used to handle PDM event.
 *  Arguments    : p_args      Pointer to PDM callback argument.
 *  Return Value : None.
 **********************************************************************************************************************/
void pdm_callback(pdm_callback_args_t *p_args)
{
    switch (p_args->event)
    {

        case PDM_EVENT_DATA:
        {
            g_stop_receive_data = true;
            break;
        }

        case PDM_EVENT_SOUND_DETECTION:
        {
            g_sound_detect = true;
            break;
        }

        case PDM_EVENT_ERROR:
        {
            g_pdm_error = p_args->error;
            break;
        }

        default:
            break;
     }
}
/***********************************************************************************************************************
* End of function pdm_callback.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: pdm_error_check
 *  Description  : This function is used to check the PDM errors.
 *  Arguments    : None.
 *  Return Value : FSP_SUCCESS    Upon successful operation.
 *                 Any other error code apart from FSP_SUCCESS.
 **********************************************************************************************************************/
static fsp_err_t pdm_error_check(void)
{
    fsp_err_t err = FSP_SUCCESS;

    if (PDM_ERROR_NONE != g_pdm_error)
    {
        if (g_pdm_error & PDM_ERROR_BUFFER_OVERWRITE)
        {
            APP_ERR_PRINT("Buffer overwrite. Decrease PDM clock output\r\n");
        }
        if (g_pdm_error & PDM_ERROR_SHORT_CIRCUIT)
        {
            APP_ERR_PRINT("Short circuit detected on PDM_DATAn pin. Please check microphone connection\r\n");
        }
        if (g_pdm_error & PDM_ERROR_OVERVOLTAGE_LOWER)
        {
            APP_ERR_PRINT("Low voltage. Verify microphone output and threshold settings\r\n");
        }
        if (g_pdm_error & PDM_ERROR_OVERVOLTAGE_UPPER)
        {
            APP_ERR_PRINT("High voltage. Verify microphone output and threshold settings\r\n");
        }

        return FSP_ERR_ASSERTION;
    }

    return err;
}
/***********************************************************************************************************************
* End of function pdm_error_check.
***********************************************************************************************************************/

/***********************************************************************************************************************
 *  Function Name: handle_error
 *  Description  : This function is used to close all opened modules, print and trap error.
 *  Arguments    : err            error code.
 *                 err_str        error string.
 *  Return Value : None.
 **********************************************************************************************************************/
void handle_error(fsp_err_t err, char * err_str)
{
    if (FSP_SUCCESS != err)
    {
        /* Print the error */
        APP_PRINT(err_str);

        /* Close opened PDM module*/
        if (MODULE_CLOSE != g_pdm0_ctrl.open)
        {
            if (FSP_SUCCESS != R_PDM_Close(&g_pdm0_ctrl))
            {
                APP_ERR_PRINT("** R_PDM_Close API failed **\r\n");
            }
        }

        /* Close opened DMAC module*/
        if (MODULE_CLOSE != g_transfer0_ctrl.open)
        {
            if (FSP_SUCCESS != R_DMAC_Close(&g_transfer0_ctrl))
            {
                APP_ERR_PRINT("** R_DMAC_Close API failed **\r\n");
            }
        }

        /* Close opened AGT module*/
        if (MODULE_CLOSE != g_timer0_ctrl.open)
        {
            if(FSP_SUCCESS != R_AGT_Close(&g_timer0_ctrl))
            {
                APP_ERR_PRINT("** R_AGT_Close API failed **\r\n");
            }
        }

        /* Close opened DAC_B module*/
        if (MODULE_CLOSE != g_dac_b0_ctrl.channel_opened)
        {
            if (FSP_SUCCESS != R_DAC_B_Close(&g_dac_b0_ctrl))
            {
                APP_ERR_PRINT("** R_DAC_B_Close API failed **\r\n");
            }
        }

        /* Close opened LPM SW standby module*/
        if (MODULE_CLOSE != g_sw_standby_ctrl.lpm_open)
        {
            if (FSP_SUCCESS != R_LPM_Close(&g_sw_standby_ctrl))
            {
                APP_ERR_PRINT("** R_LPM_Close API failed **\r\n");
            }
        }

        /* Trap the error */
        APP_ERR_TRAP(err);
    }
}
/***********************************************************************************************************************
* End of function handle_error.
***********************************************************************************************************************/
