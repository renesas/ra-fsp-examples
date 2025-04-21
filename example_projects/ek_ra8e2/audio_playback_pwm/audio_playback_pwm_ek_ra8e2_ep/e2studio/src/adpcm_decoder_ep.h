/***********************************************************************************************************************
 * File Name    : adpcm_decoder_ep.h
 * Description  : Contains declarations of data structures and functions used in adpcm_decoder_ep.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef ADPCM_DECODER_EP_H_
#define ADPCM_DECODER_EP_H_

/***********************************************************************************************************************
 * User-defined APIs
 **********************************************************************************************************************/
/* Function declarations */
fsp_err_t init_adpcm(void);
fsp_err_t deinit_adpcm(void);
fsp_err_t decode_adpcm(void);

#endif /* ADPCM_DECODER_EP_H_ */
