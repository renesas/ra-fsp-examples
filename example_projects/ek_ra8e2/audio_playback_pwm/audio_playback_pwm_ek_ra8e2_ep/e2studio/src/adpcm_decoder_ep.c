/***********************************************************************************************************************
 * File Name    : adpcm_decoder_ep.c
 * Description  : Contains data structures and functions used in adpcm_decoder_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#include "adpcm_decoder_ep.h"

/*******************************************************************************************************************//**
 * @brief     This function initializes the ADPCM decoder module.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Initialized successfully.
 * @retval    Any other error codes     Failed to initialize.
 **********************************************************************************************************************/
fsp_err_t init_adpcm(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open ADPCM decoder */
    err = RM_ADPCM_DECODER_Open(&g_adpcm_decoder_ctrl, &g_adpcm_decoder_cfg);

    return err;
}

/*******************************************************************************************************************//**
 * @brief     This function closes the opened ADPCM decoder module.
 * @param[in] None.
 * @retval    None.
 **********************************************************************************************************************/
void deinit_adpcm(void)
{
    /* Close the opened ADPCM decoder module */
    if (g_adpcm_decoder_ctrl.opened)
    {
        RM_ADPCM_DECODER_Close(&g_adpcm_decoder_ctrl);
    }
}

/*******************************************************************************************************************//**
 * @brief     This function decodes the ADPCM data into 16-bit signed PCM data.
 * @param[in] None.
 * @retval    FSP_SUCCESS               Decoded successfully.
 * @retval    Any other error codes     Failed to decode.
 **********************************************************************************************************************/
fsp_err_t decode_adpcm(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Decode ADPCM data into 16-bit signed PCM data */
    err = RM_ADPCM_DECODER_Decode(&g_adpcm_decoder_ctrl, g_adpcm_data, g_pcm_stream, ADPCM_BUFFER_SIZE_BYTES);

    return err;
}
