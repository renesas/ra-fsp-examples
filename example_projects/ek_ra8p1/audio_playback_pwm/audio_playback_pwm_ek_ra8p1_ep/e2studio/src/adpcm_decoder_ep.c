/***********************************************************************************************************************
 * File Name    : adpcm_decoder_ep.c
 * Description  : Contains variables and functions used in adpcm_decoder_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"
#include "adpcm_decoder_ep.h"
#include "audio_playback_pwm_ep.h"

extern const uint8_t g_adpcm_data[ADPCM_BUFFER_SIZE_BYTES];
extern int16_t g_pcm_stream[ADPCM_BUFFER_SIZE_BYTES * TWO];

/*******************************************************************************************************************//**
  * @brief       This function initializes by opening the ADPCM decoder modules.
  * @param[IN]   None.
  * @retval      FSP_SUCCESS                 Upon successful initialization of ADPCM module.
  * @retval      Any other error code        Unsuccessful open.
 **********************************************************************************************************************/
fsp_err_t init_adpcm(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Open ADPCM decoder */
    err = RM_ADPCM_DECODER_Open(&g_adpcm_decoder_ctrl, &g_adpcm_decoder_cfg);
    return err;
}

/*******************************************************************************************************************//**
  * @brief       This function de-initializes by closing the ADPCM decoder modules.
  * @param[IN]   None.
  * @retval      FSP_SUCCESS                 Upon successful de-initialization of ADPCM module.
  * @retval      Any other error code        Unsuccessful close.
 **********************************************************************************************************************/
fsp_err_t deinit_adpcm(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Close ADPCM decoder */
    err = RM_ADPCM_DECODER_Close(&g_adpcm_decoder_ctrl);
    if(FSP_SUCCESS != err)
    {
        /* ADPCM decoder close failure message */
        APP_ERR_PRINT("** RM_ADPCM_DECODER_Close API failed **\r\n");
    }
   return err;
}

/*******************************************************************************************************************//**
  * @brief       This function decodes the ADPCM data into 16-bit signed PCM data.
  * @param[IN]   None
  * @retval      FSP_SUCCESS                 Upon successful decoding of ADPCM into 16-bit signed PCM data.
  * @retval      Any other error code        Unsuccessful to decode the audio data.
 **********************************************************************************************************************/
fsp_err_t decode_adpcm(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Decode ADPCM data into 16-bit signed PCM data */
    err = RM_ADPCM_DECODER_Decode(&g_adpcm_decoder_ctrl, g_adpcm_data, g_pcm_stream, ADPCM_BUFFER_SIZE_BYTES);
    return err;
}
