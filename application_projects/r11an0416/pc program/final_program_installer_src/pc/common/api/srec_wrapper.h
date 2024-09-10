/***********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef _SREC_WRAPPER_H
#define _SREC_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "flashDescription.h"

typedef struct
{
	uint32_t  u4Size;          // the length of the malloc'ed buffer
	uint8_t*  pu1Data;         // pointer to a malloc�ed buffer containing the raw image data.
	uint32_t  u4Address;       // flash address on target for the image. Corresponds to the first byte in pBuffer.
} SrecRegion;

typedef struct
{
	uint8_t u1NumRegions;
	SrecRegion *psRegions;
}SrecImage;

extern bool srecFileRead(const char *pFilename, SrecImage* pImage);

#ifdef __cplusplus
}
#endif

#endif //_SREC_WRAPPER_H