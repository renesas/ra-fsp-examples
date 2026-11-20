/***********************************************************************************************************************
 * File Name    : audio_playback_pwm_example_app.c
 * Description  : Example application layer for the Audio Playback PWM EP.
 *                - All printing via printf (routed to UART via user stdio library).
 *                - hal_entry.c calls a single function: audio_playback_pwm_example_app_run().
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include <stdio.h>
#include "adpcm_decoder_ep.h"
#include "audio_playback_pwm_example_app.h"
#include "console/uart_console.h"

/* Private functions */
static void audio_playback_operation(void);
static void handle_error(fsp_err_t err, const char * err_str);

/*******************************************************************************************************************//**
 * @brief     Single entry-point for the Audio Playback PWM example application.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void audio_playback_pwm_example_app_run(void)
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

    /* Initialize ADPCM module */
    err = init_adpcm();
    handle_error(err, "init_adpcm failed\r\n");

    /* Initialize Audio Playback PWM module */
    err = init_audio_playback_pwm();
    handle_error(err, "init_audio_playback_pwm failed\r\n");

    printf("Initialization completed successfully\r\n");

    /* Decode the ADPCM data into 16-bit signed PCM data */
    err = decode_adpcm();
    handle_error(err, "decode_adpcm failed\r\n");

    /* Convert signed PCM data into unsigned PCM data */
    sign_to_unsign();

    /* Menu options */
    printf(EP_MENU);

    while (true)
    {
        /* Perform Audio Playback PWM operation */
        audio_playback_operation();
    }
}

/*******************************************************************************************************************//**
 * @brief     This function performs Audio Playback PWM operation.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
static void audio_playback_operation(void)
{
    fsp_err_t err = FSP_SUCCESS;
    int32_t user_input = RESET_VALUE;

    if (uart_console_has_char())
    {
        /* Get the user input */
        user_input = getchar();

        switch (user_input)
        {
            case PLAY_AUDIO:
            {
                /* Start playing the selected audio stream */
                err = audio_playback_start();
                handle_error(err, "audio_playback_start failed\r\n");

                /* Start playing audio */
                err = play_audio();
                handle_error(err, "play_audio failed\r\n");

                printf("\r\nStarted playing audio\r\n");
                break;
            }

            case STOP_AUDIO:
            {
                /* Stop playing audio */
                err = stop_audio();
                handle_error(err, "stop_audio failed\r\n");

                printf("\r\nStopped playing audio\r\n");
                break;
            }

            default:
            {
                printf("\r\nInvalid input. Please enter valid input\r\n");
                break;
            }
        }
    }

    /* Play audio continuously until the user selects stop playing audio */
    if (g_playback_started && (!g_audio_stop))
    {
        /* Clear the Playback started flag */
        g_playback_started = false;

        /* Start playing audio */
        err = play_audio();
        handle_error(err, "play_audio failed\r\n");

        printf("\r\nPlaying audio continuously\r\n");
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
        printf("\r\n[ERR] %s", err_str);
        printf("Returned Error Code: 0x%x\r\n", (unsigned int)err);
        uart_console_flush();
        deinit_adpcm();
        deinit_audio_playback_pwm();
        __BKPT(0);
    }
}
