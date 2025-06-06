/***********************************************************************************************************************
 * File Name    : ssi_ep.h
 * Description  : Contains macros, data buffers used in ssi_ep.h
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef SSI_EP_H_
#define SSI_EP_H_

#include <stdint.h>
#include "common_utils.h"

#define BUFF_SIZE (0x800U)    // Sample audio buffer size - 2048
#define SSI_STREAMING_EXAMPLE_TONE_FREQUENCY_HZ (800) // Tone frequency
#define SSI_STREAMING_EXAMPLE_SAMPLES_PER_CHUNK (1024) // Number of samples per chunk
#define M_PI (3.14159265358979323846) // Pi

void ssi_entry(void);

#endif /* SSI_EP_H_ */
