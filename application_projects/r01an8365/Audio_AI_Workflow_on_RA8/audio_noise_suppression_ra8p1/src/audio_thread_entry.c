/***********************************************************************************************************************
 * File Name    : audio_thread_entry.c
 * Description  : This file contain Audio capture and processing implementation.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "stdint.h"
#include "audio_thread.h"
#include "common_app.h"
#include "common_utils.h"
#include "rnnoise.h"
#include "pmu_ethosu.h"
#include "SEGGER_SYSVIEW_FreeRTOS.h"

/***********************************************************************************************************************
*   PCM input buffer: contains 2 frames, uses toggle mechanism:
    - one buffer for storage
    - one buffer for data transfer
 **********************************************************************************************************************/
static int32_t  g_pcm_input_buffer[FRAME_SIZE * NUM_FRAME]  BSP_PLACE_IN_SECTION(".sdram");
static int16_t  g_pcm_input[FRAME_SIZE];
static volatile uint32_t g_pcm_frame_idx        = 0;
static volatile bool g_sound_detect             = false;

/* Private state declaration */
volatile bool noise_suppression_enable   = true;
volatile bool exit_sign   = false;

/* Latency measurement state */
volatile bool     g_measure_latency = false;
volatile bool     g_latency_ready   = false;
volatile int16_t  g_latency_us_pre  = 0;
volatile int16_t  g_latency_us_post = 0;
volatile int16_t  g_latency_us_rnnoise = 0;

/* NPU capture state */
volatile bool     g_npu_cyc_ready   = false;
volatile bool     g_capture_npu_cyc = false;

/* Private function declaration */
static void     DWT_init(void);
static void     pcm_to_float(const int16_t *p_pcm, float *p_float);
static int16_t  float_to_pcm16(float s);

/* Public function declaration */
uint8_t         user_input_get (void);
int16_t         calculate_DSP_latency(void);
int32_t         capture_NPU_cycles(void);

/***********************************************************************************************************************
 *  Function Name: pdm_callback
 *  Description  : This function is used to handle PDM event.
 *  Arguments    : p_args      Pointer to PDM callback argument.
 *  Return Value : None.
 **********************************************************************************************************************/
void pdm_callback(pdm_callback_args_t *p_args)
{
    BaseType_t  xHigherPriorityTaskWoken    = pdFALSE;
    uint32_t    toggle                      = g_pcm_frame_idx * FRAME_SIZE;

    switch (p_args->event)
    {
        case PDM_EVENT_DATA:
        {
            /* Start Record ISR for SYSVIEW observation */
            SEGGER_SYSVIEW_RecordEnterISR();

            for (uint32_t i = 0; i < FRAME_SIZE; i++)
            {
                g_pcm_input[i] = (int16_t) (g_pcm_input_buffer[toggle + i] & 0xFFFF);
            }

            /* Send current frame for Audio process */
            xStreamBufferSendFromISR(g_raw_stream,
                                     g_pcm_input,
                                     FRAME_BYTES,
                                     &xHigherPriorityTaskWoken);

            /* Update frame index */
            g_pcm_frame_idx ^= 1U;

            /* Force context switch */
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

            /* Stop Record PDM ISR */
            SEGGER_SYSVIEW_RecordExitISR();

            break;
        }
        case PDM_EVENT_SOUND_DETECTION:
        {
            g_sound_detect = true;
            break;
        }
        case PDM_EVENT_ERROR:

            break;

        default:
            break;
    }
}

/***********************************************************************************************************************
 *  Function Name: audio_thread_entry
 *  Description  : Audio thread: handles audio capture and frame processing for denoising
 *  Arguments    : Thread specific parameters.
 *  Return Value : None.
 **********************************************************************************************************************/
void audio_thread_entry(void *pvParameters)
{
    FSP_PARAMETER_NOT_USED (pvParameters);
    fsp_err_t err = FSP_SUCCESS;

    /* Application Banner */
    APP_PRINT(INFO_BANNER);
    APP_PRINT(APP_BANNER);

    /* Define processing frame */
    int16_t received_frame[FRAME_SIZE];
    float   float_input_frame[FRAME_SIZE];
    float   denoised_frame[FRAME_SIZE];
    int16_t pcm16_denoised_frame[FRAME_SIZE];

    /* Initialize DWT once at startup */
    DWT_init();

    /* Initialize PDM module */
    err = R_PDM_Open(&g_pdm0_ctrl, &g_pdm0_cfg);

    /* Initialize NPU */
    err = RM_ETHOSU_Open(&g_rm_ethosu0_ctrl, &g_rm_ethosu0_cfg);
    if (err != FSP_SUCCESS)
    {
         APP_PRINT("Failed to start NPU\r\n");
    }
    /* Set PMU event counter */
    ETHOSU_PMU_Enable(&g_ethosu0);
    ETHOSU_PMU_CNTR_Enable(&g_ethosu0, ETHOSU_PMU_CCNT_Msk);
    enum ethosu_pmu_event_type events[] = {
      ETHOSU_PMU_NPU_IDLE,
      ETHOSU_PMU_NPU_ACTIVE,
      ETHOSU_PMU_AXI0_ENABLED_CYCLES,
      ETHOSU_PMU_AXI1_ENABLED_CYCLES
    };
    for (uint32_t i = 0; i < sizeof(events) / sizeof(events[0]); i += 1) {
      ETHOSU_PMU_Set_EVTYPER(&g_ethosu0, i, events[i]);
      ETHOSU_PMU_CNTR_Enable(&g_ethosu0, 1u << i);
    }
    ETHOSU_PMU_CYCCNT_Reset(&g_ethosu0);
    ETHOSU_PMU_EVCNTR_ALL_Reset(&g_ethosu0);

    /* Wait for filter settling and startup time */
    R_BSP_SoftwareDelay(PDM2_FILTER_SETTLING_TIME_US + PDM_MIC_STARTUP_TIME_US, BSP_DELAY_UNITS_MICROSECONDS);

    /* Configure the sound detection range */
    pdm_sound_detection_setting_t sound_detection_setting =
    {
        .sound_detection_lower_limit = PDM_SDE_LOWER_LIMIT,
        .sound_detection_upper_limit = PDM_SDE_UPPER_LIMIT
    };

    /* Enable sound detection */
    err = R_PDM_SoundDetectionEnable(&g_pdm0_ctrl, sound_detection_setting);
    APP_PRINT("\r\nWaiting for sound detection to begin...\r\n");

    /* Make sure detect the sound before recording the audio */
    while (!g_sound_detect)
    {
        vTaskDelay(pdMS_TO_TICKS(1));
    }
    /* Reset g_sound_detect flag */
    g_sound_detect = false;

    /* Disable sound detection */
    err = R_PDM_SoundDetectionDisable(&g_pdm0_ctrl);

    /* Start PDM */
    err = R_PDM_Start(&g_pdm0_ctrl, g_pcm_input_buffer, sizeof(g_pcm_input_buffer), FRAME_SIZE);
    assert(FSP_SUCCESS == err);

    /*Define RNNOISE instance  */
    DenoiseState *rnn_state = rnnoise_create(NULL);
    configASSERT(rnn_state != NULL);

    /* Print MENU selection */
    APP_PRINT(APP_MENU);

    while (!exit_sign)
    {
        /* Receive PDM captured data */
        xStreamBufferReceive(g_raw_stream,
                             received_frame,
                             sizeof(received_frame),
                             portMAX_DELAY);

        /* Start process frame with Model execute */
        /* Process frame with RNNoise */
        if(noise_suppression_enable)
        {
            /* Convert PCM data to float */
            pcm_to_float(received_frame, float_input_frame);

            /* Execute RNNoise Model to process the frame */
            rnnoise_process_frame(rnn_state, denoised_frame, float_input_frame);

            /* Revert float to int16 pcm data */
            for (int i = 0; i < FRAME_SIZE; i++)
            {
                pcm16_denoised_frame[i] = float_to_pcm16(denoised_frame[i]);
            }
        }
        /* Process frame directly without RNNoise */
        else
        {
            for (int i = 0; i < FRAME_SIZE; i++)
            {
                pcm16_denoised_frame[i] = received_frame[i];
            }
        }

        /* Send denoised frame to Playback Thread */
        xStreamBufferSend(g_denoised_stream,
                          pcm16_denoised_frame,
                          FRAME_SIZE * sizeof(int16_t),
                          portMAX_DELAY);
    }
}

/***********************************************************************************************************************
 *  Function Name: pcm_to_float
 *  Description  : Helper to convert int16 PCM to float
 *  Arguments    : p_pcm    PCM input pointer
*                  p_float  Float output pointer
 *  Return Value : None
 **********************************************************************************************************************/
static void pcm_to_float(const int16_t *p_pcm, float *p_float)
{
    for (uint32_t i = 0; i < FRAME_SIZE; i++)
    {
        p_float[i] = (float)p_pcm[i];
    }
}

/***********************************************************************************************************************
 *  Function Name: float_to_pcm16
 *  Description  : Helper to invert float to int16 PCM
 *  Arguments    : data     Float input data
 *  Return Value : data     PCM16 output data
 **********************************************************************************************************************/
static int16_t float_to_pcm16(float data)
{
    if (data >  32767.0f)
    {
        data =  32767.0f;
    }
    if (data < -32768.0f)
    {
        data = -32768.0f;
    }
    return (int16_t)data;
}

/***********************************************************************************************************************
 *  Function Name: DWT_init
 *  Description  : Initialize DWT for Latency measurement
 *  Arguments    : None.
 *  Return Value : None.
 **********************************************************************************************************************/
static void DWT_init(void)
{
    /* Enable trace */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    /* Reset cycle counter */
    DWT->CYCCNT = 0;
    /* Enable CYCCNT */
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}


/***********************************************************************************************************************
 *  Function Name: calculate_DSP_latency
 *  Description  : This function is helper to calculate and print the pre-DSP and post-DSP latency
 *  Arguments    : None.
 *  Return Value : Total latency of pre-DSP and post-DSP
 **********************************************************************************************************************/
int16_t calculate_DSP_latency(void)
{
    if (!noise_suppression_enable)
    {
        APP_PRINT("\r\n[Latency Measure] Enable denoise mode first.\r\n");
        return -1;
    }

    /* Active flag */
    g_latency_ready = false;
    g_measure_latency = true;

    APP_PRINT("\r\n[Latency Measure] Measuring over 100 frames...\r\n");

    /* Wait for audio thread to finish accumulating */
    while (!g_latency_ready)
    {
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    int16_t total = g_latency_us_pre + g_latency_us_post;

    APP_PRINT("\r\npre-DSP:   %d \tus/frame"
              "\r\nrnnoise:   %d \t\tus/frame"
              "\r\npost-DSP:  %d \tus/frame",
              (int)g_latency_us_pre,
              (int)g_latency_us_rnnoise,
              (int)g_latency_us_post);

    return total;
}

/***********************************************************************************************************************
 *  Function Name: capture_NPU_cycles
 *  Description  : Capture the NPU time line (cycles count)
 *  Arguments    : None.
 *  Return Value : Return capture state value
 **********************************************************************************************************************/
int32_t capture_NPU_cycles(void)
{
    if (!noise_suppression_enable)
    {
        APP_PRINT("\r\n[NPU time line Capture] Enable denoise mode first.\r\n");
        return -1;
    }

    /* Active flag */
    g_npu_cyc_ready = false;
    g_capture_npu_cyc = true;

    APP_PRINT("\r\n[NPU time line Capture] Capturing for 1 frame...\r\n");

    /* Wait for audio thread to finish accumulating */
    while (!g_npu_cyc_ready)
    {
        vTaskDelay(pdMS_TO_TICKS(10));
    }

    return 0;
}
