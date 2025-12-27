/***********************************************************************************************************************
 * File Name    : pdm_ep.h
 * Description  : Contains data structures and functions used in pdm_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef PDM_EP_2_H_
#define PDM_EP_2_H_

#include <stdint.h>
#include <stddef.h>

#include "common_utils.h"

#define EP_VERSION      ("1.0")
#define MODULE_NAME         "r_pdm"
#define BANNER_INFO         "\r\n********************************************************************************"\
                            "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                               *"\
                            "\r\n*   Example Project Version %s                                                *"\
                            "\r\n*   Flex Software Pack Version  %d.%d.%d                                          *"\
                            "\r\n********************************************************************************"\
							"\r\nRefer to the accompanying .md file for Example Project details and" \
							"\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO             "\r\nThe project demonstrates the basic functionality of the PDMIF (Pulse Density\r\n"\
                            "Modulation Interface) on Renesas RA MCUs using the Renesas FSP. PDM MEMS\r\n"\
                            "microphones are responsible for capturing environmental sound and converting\r\n"\
                            "it into a 1-bit digital audio stream. First, the user specifies the recording\r\n"\
                            "duration in seconds, and the MCU enters LPM software standby mode. When a sound\r\n"\
                            "is detected, the MCU wakes up and starts the PDMIF to receive and convert the\r\n"\
                            "1-bit data into PCM samples, which are subsequently stored in external SDRAM.\r\n"\
                            "By default, the conversion outputs 20-bit PCM samples, but the user can change\r\n"\
                            "this setting in the configuration.xml as described in the guidelines provided\r\n"\
                            "in the markdown file. For playback, the stored PCM samples are converted into\r\n"\
                            "an analog signal, amplified by an external audio amplifier (such as the PMOD\r\n"\
                            "AMP2), and played through a standard speaker or headphones. The user enters\r\n"\
                            "'1' to start or resume audio playback, '2' to stop or pause the playback, or\r\n"\
                            "'3' to record new audio.\r\n"\
                            "The EP information and error messages are displayed in the terminal application\r\n\r\n"

#define TIME_RECORD         "\r\nThe valid recording time: 1 to %d seconds"\
		                    "\r\nPlease enter the desired recording time in seconds: "

/* Perform VOX wakeup MCU from LPM SW standby operation */
#define ENABLE_VOX_WAKEUP           (1U)

/* Perform playback the recorded audio operation */
#define ENABLE_PLAYBACK             (1U)

/* PDM microphones startup time, RA8P1_EK microphones startup time is 35ms */
#define PDM_MIC_STARTUP_TIME_US     (35000)
#define PDM_SDE_UPPER_LIMIT         (1000)
#define PDM_SDE_LOWER_LIMIT         (0xFFF80000)

/* Output sample rate after PDM decimation (Hz) */
#define PDM_FS_HZ                   (32000U)

/* Number of interleaved channels: 1 = mono, 2 = stereo
 * This EP only supports a mono microphone */
#define PDM_CHANNELS                (1U)

/* Target duration (seconds) of audio captured in the buffer */
#define PDM_DURATION_MAX_IN_SEC     (20U)

/* PCM output data bits per sample */
#define PCM_16BITS                  (16U)
#define PCM_20BITS                  (20U)

/* PDM alignment (samples per interrupt/frame per channel) */
#define PDM_FRAME_SAMPLES           ((1<<PDM_INTERRUPT_THRESHOLD_16) * PDM_CHANNELS )

/* Align n up to the next multiple of a (a must be > 0) */
#define PDM_ALIGN_UP(n,a)           (((uint32_t)((n) + (a) - 1U) / (uint32_t)(a)) * (uint32_t)(a) )

/*
 * Total number of interleaved samples to hold PDM_DURATION_MAX_IN_SEC seconds.
 * samples_raw = duration * Fs_out * channels
 * samples     = align_up(samples_raw, frame_samples)
 */
#define PDM_BUFFER_NUM_SAMPLES    PDM_ALIGN_UP((uint32_t)((uint64_t)PDM_DURATION_MAX_IN_SEC * PDM_FS_HZ * PDM_CHANNELS), PDM_FRAME_SAMPLES)

/* PDM buffer size */
typedef struct
{
    uint32_t samples;  /* Interleaved sample count, aligned to frame size */
    uint32_t bytes;    /* Buffer size in bytes */
} pdm_buf_size_t;

/* Public functions declarations */
void pdm_ep_entry(void);
void handle_error(fsp_err_t err, char * err_str);
uint8_t user_input_get(void);

#endif /* PDM_EP_H_ */
