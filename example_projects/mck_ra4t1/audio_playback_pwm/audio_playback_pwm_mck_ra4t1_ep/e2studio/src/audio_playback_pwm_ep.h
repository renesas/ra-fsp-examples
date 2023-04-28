/***********************************************************************************************************************
 * File Name    : audio_playback_pwm_ep.h
 * Description  : Contains declarations of data structures and functions used in audio_playback_pwm_ep.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#ifndef AUDIO_PLAYBACK_PWM_EP_H_
#define AUDIO_PLAYBACK_PWM_EP_H_


/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

#define EP_INFO     "\r\nThis example project demonstrates Audio Playback with PWM module's "\
                    "\r\nfunctionality. It uses ADPCM module to decode the PCM data. The Decoded"\
                    "\r\nPCM data is in signed 16 bit format. Application has to convert this "\
                    "\r\nsigned PCM data into unsigned PCM data and provide the converted unsigned " \
                    "\r\ndata to the audio playback with PWM module. The converted unsigned data is" \
                    "\r\nprovided as input and output is generated as PWM signal." \
                    "\r\nError and info messages will be printed on JlinkRTTViewer\r\n\n"

#define MENU        "\r\nMenu:" \
                    "\r\nPress 1 to start playing audio" \
                    "\r\nPress 2 to stop playing audio" \
                    "\r\nUser Input: \r\n"

#define ONE         (1U)
#define TWO         (2U)
#define EIGHT       (8U)

/*
 *  3500U buffer size is used to play the audio.
 */
#define ADPCM_BUFFER_SIZE_BYTES     (3000U)

enum
{
  PLAY_AUDIO = ONE,
  STOP_AUDIO
};

/***********************************************************************************************************************
 * User-defined APIs
 **********************************************************************************************************************/
/*
 * function declarations
 */
fsp_err_t init_audio_playback_pwm(void);
fsp_err_t deinit_audio_playback_pwm(void);
fsp_err_t process_user_input(uint8_t input_buffer);
fsp_err_t play_audio(void);
fsp_err_t stop_audio(void);
void  sign_to_unsign(void);

#endif /* AUDIO_PLAYBACK_PWM_EP_H_ */
