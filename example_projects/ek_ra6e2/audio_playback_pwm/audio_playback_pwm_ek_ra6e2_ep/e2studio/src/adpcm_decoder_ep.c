/***********************************************************************************************************************
 * File Name    : adpcm_decoder_ep.c
 * Description  : Contains variables and functions used in adpcm_decoder_ep.c.
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

#include "common_utils.h"
#include "adpcm_decoder_ep.h"
#include "audio_playback_pwm_ep.h"

extern const uint8_t g_adpcm_data[ADPCM_BUFFER_SIZE_BYTES];
extern int16_t g_pcm_stream[ADPCM_BUFFER_SIZE_BYTES * TWO];

/*******************************************************************************************************************//**
  * @brief       This function initializes by opening the ADPCM decoder modules
  * @param[IN]   None.
  * @retval      FSP_SUCCESS                 Upon successful initialization of ADPCM module.
  * @retval      Any Other Error code        Unsuccessful open
  **********************************************************************************************************************/
fsp_err_t init_adpcm(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Open ADPCM decoder */
    err = RM_ADPCM_DECODER_Open(&g_adpcm_decoder_ctrl, &g_adpcm_decoder_cfg);
    return err;
}
/*******************************************************************************************************************//**
  * @brief       This function de-initializes by closing the adpcm decoder modules
  * @param[IN]   None.
  * @retval      FSP_SUCCESS                 Upon successful de-initialization of adpcm module.
  * @retval      Any Other Error code        Unsuccessful close
  **********************************************************************************************************************/
fsp_err_t deinit_adpcm(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Close ADPCM decoder */
    err = RM_ADPCM_DECODER_Close(&g_adpcm_decoder_ctrl);
    if(FSP_SUCCESS != err)
    {
        /* ADPCM decoder close failure message */
        APP_ERR_PRINT("** RM_ADPCM_DECODER_Close API failed ** \r\n");
    }
   return err;
}
/*******************************************************************************************************************//**
  * @brief       This function decodes the ADPCM data into 16-bit signed PCM data
  * @param[IN]   None
  * @retval      FSP_SUCCESS                 Upon successful decoding of ADPCM into 16-bit signed PCM data.
  * @retval      Any Other Error code        Unsuccessful  to decode the audio data.
 **********************************************************************************************************************/
fsp_err_t decode_adpcm(void)
{
    fsp_err_t err = FSP_SUCCESS;
    /* Decode ADPCM data into 16-bit signed PCM data */
    err = RM_ADPCM_DECODER_Decode(&g_adpcm_decoder_ctrl, g_adpcm_data, g_pcm_stream, ADPCM_BUFFER_SIZE_BYTES);
    return err;
}
