/***********************************************************************************************************************
 * File Name    : common_app.h
 * Description  : Contains application macros, functionalities, structures commonly used.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef COMMON_COMMON_APP_H_
#define COMMON_COMMON_APP_H_

#include <stdint.h>
#include <stdbool.h>
#include <common_utils.h>
#include "SEGGER_SYSVIEW.h"
#include "SEGGER_SYSVIEW_FreeRTOS.h"

#define INFO_BANNER "\r\n*************************************************************************************"\
                    "\r\n*                          AUDIO NOISE SUPPRESSION ON RA8P1                         *"\
                    "\r\n*************************************************************************************"\

#define APP_BANNER  "\r\nThis project demonstrates a real-time audio noise suppression solution implemented on" \
                    "\r\nthe Renesas RA8 MCU"\
                    "\r\nThe application runs on the RA8P1 evaluation board under FreeRTOS and demonstrates a " \
                    "\r\nreal-time audio processing pipeline. Incoming audio data is captured, processed using" \
                    "\r\nsignal processing and AI-based noise suppression, and then output through a connected" \
                    "\r\nspeaker.\r\n"

#define APP_MENU    "\r\nApplication running with Denoise state as default"\
                    "\r\nInput 1 to switch Denoise state"   \
                    "\r\nInput 2 to calculate DSP latency"  \
                    "\r\nInput 3 to calculate NPU cycle"    \
                    "\r\nInput 4 to Exit the application"   \
                    "\r\nUser Input: \r\n"

/* Audio macros */
#define PDM_FS_HZ                   (48000)
#define FRAME_SIZE_SHIFT            (2)
#define FRAME_SIZE                  (120<<FRAME_SIZE_SHIFT)
#define NUM_FRAME                   (2)
#define PDM_MIC_STARTUP_TIME_US     (35000)
#define PDM_SDE_UPPER_LIMIT         (1000)
#define PDM_SDE_LOWER_LIMIT         (0xFFF80000)
#define PCM_16BITS                  (16)
#define PCM_20BITS                  (20)

/* Playback macros */
#define NUM_DAC_FRAME               (16U)
#define PLAYBACK_PREBUFFER_FRAMES   (2U)

/* Macros for DAC limits and audio volume control */
#define DAC12_MAX                   (4095)
#define DAC12_DATA_REG              (&(R_DAC_B0->DADR) )
#define PLAYBACK_Q15_ONE            (32768)     /* 1.0 in Q15 */

/* Set the volume percent in the range of 10 to 1000 to avoid significant impact on audio quality */
#define PLAYBACK_VOLUME_PERCENT     (100)

/* Application common macros */
#define FRAME_BYTES                 (FRAME_SIZE * sizeof(int16_t))

/* Measure macros */
#define LATENCY_MEASURE_FRAMES  (100U)    /* 1 second @ 100 fps */
#define CYCLE_CAPTURE_FRAMES    (1U)

/* Application common state */
extern volatile bool noise_suppression_enable;
extern volatile bool exit_sign;

/* Application measure state */
extern volatile bool     g_measure_latency;     /* measure flag      */
extern volatile bool     g_latency_ready;       /* result ready flag */
extern volatile int16_t  g_latency_us_pre;      /* avg pre-DSP us    */
extern volatile int16_t  g_latency_us_rnnoise;  /* avg rnnoise us    */
extern volatile int16_t  g_latency_us_post;     /* avg post-DSP us   */

extern volatile bool     g_npu_cyc_ready;       /* capture ready flag  */
extern volatile bool     g_capture_npu_cyc;     /* capture flag        */

/* Board's user LED */
extern bsp_leds_t       g_bsp_leds;

/* Enumeration for audio control selections */
typedef enum e_menu_control
{
    TOGGLE_DENOISE = 1,
    CALCULATE_LATENCY,
    CALCULATE_NPU_CYCLE,
    EXIT_SIGN
} menu_control_t;

/* Public function prototypes */
extern int16_t calculate_DSP_latency();
extern int32_t capture_NPU_cycles();

#endif /* COMMON_COMMON_APP_H_ */
