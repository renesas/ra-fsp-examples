/***********************************************************************************************************************
 * File Name    : audio_playback_pwm_ep.h
 * Description  : Contains declarations of data structures and functions used in audio_playback_pwm_ep.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef AUDIO_PLAYBACK_PWM_EP_H_
#define AUDIO_PLAYBACK_PWM_EP_H_

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define EP_INFO     "\r\nThis example project demonstrates Audio Playback with PWM module's"\
                    "\r\nfunctionality. It uses ADPCM module to decode the PCM data. The Decoded"\
                    "\r\nPCM data is in signed 16-bit format. Application has to convert this"\
                    "\r\nsigned PCM data into unsigned PCM data and provide the converted unsigned"\
                    "\r\ndata to the audio playback with PWM module. The converted unsigned data is"\
                    "\r\nprovided as input and output is generated as PWM signal."\
                    "\r\nError and info messages will be printed on the J-link RTT Viewer\r\n\n"

#define MENU        "\r\nMenu:"\
                    "\r\nPress 1 to start playing audio"\
                    "\r\nPress 2 to stop playing audio"\
                    "\r\nUser Input: \r\n"

#define ONE         (1U)
#define TWO         (2U)
#define EIGHT       (8U)

#if defined(BOARD_RA2A2_EK) || defined(BOARD_RA6T2_MCK) || defined(BOARD_RA4E2_EK) || defined(BOARD_RA6E2_EK) ||\
    defined(BOARD_RA4T1_MCK) || defined(BOARD_RA6T3_MCK) || defined(BOARD_RA4L1_EK)
/* Define boards use 32-bit AGT */
#define AGT_32BIT   (1)
#endif
/* 3500U buffer size is used to storage the audio data */
#define ADPCM_BUFFER_DATA_SIZE      (3500U)

#if  defined(BOARD_RA4E2_EK) || defined(BOARD_RA6E2_EK) || defined(BOARD_RA4T1_MCK) || defined(BOARD_RA6T3_MCK)
/* 3000U buffer size is used to play the audio */
#define ADPCM_BUFFER_SIZE_BYTES     (3000U)
#else
/* 3500U buffer size is used to play the audio */
#define ADPCM_BUFFER_SIZE_BYTES     (3500U)
#endif

enum
{
  PLAY_AUDIO = ONE,
  STOP_AUDIO
};

/***********************************************************************************************************************
 * User-defined APIs
 **********************************************************************************************************************/
/* Function declarations */
fsp_err_t init_audio_playback_pwm(void);
fsp_err_t deinit_audio_playback_pwm(void);
fsp_err_t process_user_input(uint8_t input_buffer);
fsp_err_t play_audio(void);
fsp_err_t stop_audio(void);
void  sign_to_unsign(void);

#endif /* AUDIO_PLAYBACK_PWM_EP_H_ */
