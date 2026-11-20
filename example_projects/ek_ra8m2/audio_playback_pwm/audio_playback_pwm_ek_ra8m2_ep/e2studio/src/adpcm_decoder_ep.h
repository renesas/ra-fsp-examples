/***********************************************************************************************************************
 * File Name    : adpcm_decoder_ep.h
 * Description  : Contains functions used in adpcm_decoder_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef ADPCM_DECODER_EP_H_
#define ADPCM_DECODER_EP_H_

#include "audio_playback_pwm_ep.h"

/* Function declarations */
fsp_err_t init_adpcm(void);
void deinit_adpcm(void);
fsp_err_t decode_adpcm(void);

#endif /* ADPCM_DECODER_EP_H_ */
