/***********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#if !defined MEMORY_CHECKS_H
#define MEMORY_CHECKS_H

extern bool memoryChecksIsBufferInRam(const void *pBuf, uint32_t length);
extern bool memoryChecksIsBufferInRom(const void *pBuf, uint32_t length);
extern bool memoryChecksIsBufferInMemory(const void *pBuf, uint32_t length);


#endif

