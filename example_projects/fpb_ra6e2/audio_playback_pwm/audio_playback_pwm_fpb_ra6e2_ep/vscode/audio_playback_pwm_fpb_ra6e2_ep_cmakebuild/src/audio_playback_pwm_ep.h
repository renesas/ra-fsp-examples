/***********************************************************************************************************************
 * File Name    : audio_playback_pwm_ep.h
 * Description  : Contains macros, variables, and functions used in audio_playback_pwm_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef AUDIO_PLAYBACK_PWM_EP_H_
#define AUDIO_PLAYBACK_PWM_EP_H_

#include <stdbool.h>
#include "hal_data.h"

/* Macros for the menu options received through the terminal */
#define PLAY_AUDIO                      ('1')
#define STOP_AUDIO                      ('2')

/* Macro for the reset value */
#define RESET_VALUE                     (0x00)

/* Macro for 32-bit AGT */
#if defined(BOARD_RA2A2_EK) || defined(BOARD_RA6T2_MCK) || defined(BOARD_RA4E2_EK) || defined(BOARD_RA6E2_EK) ||\
    defined(BOARD_RA4T1_MCK) || defined(BOARD_RA6T3_MCK) || defined(BOARD_RA4L1_EK) || defined(BOARD_RA4C1_EK) ||\
    defined(BOARD_RA6E2_FPB)
#define AGT_32BIT                       (1U)
#endif /* Specific board */

/* Macro for ADPCM audio data size in bytes */
#define ADPCM_BUFFER_DATA_SIZE          (3500U)

/* Macro for ADPCM playback data size in bytes */
#if defined(BOARD_RA4E2_EK) || defined(BOARD_RA6E2_EK) || defined(BOARD_RA4T1_MCK) || defined(BOARD_RA6T3_MCK) ||\
    defined(BOARD_RA6E2_FPB)
#define ADPCM_BUFFER_SIZE_BYTES         (3000U)
#else
#define ADPCM_BUFFER_SIZE_BYTES         (3500U)
#endif /* Specific board */

/* Extern variables - Defined in audio_playback_pwm_ep.c */
extern volatile bool g_playback_started;
extern volatile bool g_audio_stop;
extern const uint8_t g_adpcm_data[ADPCM_BUFFER_DATA_SIZE];
extern int16_t g_pcm_stream[ADPCM_BUFFER_SIZE_BYTES * 2U];

/* Function declarations */
fsp_err_t init_audio_playback_pwm(void);
void deinit_audio_playback_pwm(void);
fsp_err_t audio_playback_start(void);
fsp_err_t play_audio(void);
fsp_err_t stop_audio(void);
void sign_to_unsign(void);

#endif /* AUDIO_PLAYBACK_PWM_EP_H_ */
