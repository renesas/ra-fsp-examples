/***********************************************************************************************************************
 * File Name    : audio_playback_pwm_example_app.h
 * Description  : Public interface for the Audio Playback PWM example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef AUDIO_PLAYBACK_PWM_EXAMPLE_APP_H_
#define AUDIO_PLAYBACK_PWM_EXAMPLE_APP_H_

#define EP_VERSION      "1.0"

#define MODULE_NAME     "Audio Playback PWM"

#define BANNER_INFO     "\r\n******************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module    *"\
                        "\r\n*   Example Project Version %s                                  *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
                        "\r\n******************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" module\r\n"

#define EP_INFO         "\r\nThis example project demonstrates Audio Playback with PWM module's"\
                        "\r\nfunctionality. It uses ADPCM module to decode the PCM data. The Decoded"\
                        "\r\nPCM data is in signed 16-bit format. The application converts this"\
                        "\r\nsigned PCM data into unsigned PCM data and provides the converted unsigned"\
                        "\r\ndata to the audio playback with PWM module. The converted unsigned data is"\
                        "\r\nprovided as input and output is generated as PWM signal."\
                        "\r\nError and info messages will be printed on the terminal.\r\n\r\n"

#define EP_MENU         "\r\nMenu:"\
                        "\r\nPress 1 to start playing audio"\
                        "\r\nPress 2 to stop playing audio"\
                        "\r\nUser input: "

/* Single entry-point called in hal_entry.c */
void audio_playback_pwm_example_app_run(void);

#endif /* AUDIO_PLAYBACK_PWM_EXAMPLE_APP_H_ */
