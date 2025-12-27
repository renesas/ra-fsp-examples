/***********************************************************************************************************************
* File Name    : base64.h
* Description  : This file contains base64 utility functions prototypes.
***********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef BASE64_H_
#define BASE64_H_

#include <stdio.h>

unsigned int base64_int(unsigned int ch);
unsigned int base64_decode(const unsigned char* in, unsigned int in_len, unsigned char* out);

#endif /* BASE64_H_ */
