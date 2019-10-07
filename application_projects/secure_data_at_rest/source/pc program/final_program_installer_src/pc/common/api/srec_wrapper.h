/***********************************************************************************************************************
* Copyright [2015-2017] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
*
* This file is part of Renesas SynergyTM Software Package (SSP)
*
* The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
* and/or its licensors ("Renesas") and subject to statutory and contractual protections.
*
* This file is subject to a Renesas SSP license agreement. Unless otherwise agreed in an SSP license agreement with
* Renesas: 1) you may not use, copy, modify, distribute, display, or perform the contents; 2) you may not use any name
* or mark of Renesas for advertising or publicity purposes or in connection with your use of the contents; 3) RENESAS
* MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED
* "AS IS" WITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
* PARTICULAR PURPOSE, AND NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR
* CONSEQUENTIAL DAMAGES, INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF
* CONTRACT OR TORT, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents
* included in this file may be subject to different terms.
**********************************************************************************************************************/
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
	uint8_t*  pu1Data;         // pointer to a malloc’ed buffer containing the raw image data.
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