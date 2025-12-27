/***********************************************************************************************************************
 * File Name    : audio_playback_dac.h
 * Description  : Contains data structures and functions used in audio_playback_dac.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "pdm_ep.h"

#if (ENABLE_PLAYBACK == 1U)

#ifndef AUDIO_PLAYBACK_DAC_H_
#define AUDIO_PLAYBACK_DAC_H_

#define MENU        "\r\nAudio control:"\
                    "\r\nPress 1 to start playing audio"\
                    "\r\nPress 2 to stop playing audio"\
                    "\r\nPress 3 to record new audio"\
                    "\r\nUser Input: \r\n"

/* Enumeration for audio control selections */
typedef enum e_audio_control
{
    PLAY_AUDIO = 1,
    STOP_AUDIO,
    RECORD_NEW_AUDIO
} audio_control_t;

/* Macros for DAC limits and audio volume control */
#define DAC12_MAX                   (4095)
#define DAC12_DATA_REG              (&(R_DAC_B0->DADR) )
#define Q15_ONE                     (32768)     /* 1.0 in Q15 */

/* Set the volume percent in the range of 10 to 1000 to avoid significant impact on audio quality */
#define PLAYBACK_VOLUME_PERCENT     (500)

/* Public functions declarations */
fsp_err_t audio_playback_init(void);
fsp_err_t playback_operation(void);

#endif /* AUDIO_PLAYBACK_DAC_H_ */
#endif /* ENABLE_PLAYBACK */
