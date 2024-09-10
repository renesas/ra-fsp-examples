/***********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
/*
 *  Flash description structure for the R7FA6M3AH3 chip variant.
 */


#include <flashR7FA6M3AH3.h>

FlashDescriptionRecord flashR7FA6M3AH3Records[3] = {
    { code_memory, 0x00000000, 0x0000FFFF, 0x2000, 0x080},
    { code_memory, 0x00010000, 0x001FFFFF, 0x8000, 0x080},
    { data_memory, 0x40100000, 0x4010FFFF, 0x0040, 0x004}
};


FlashDescription flashR7FA6M3AH3 = {
    3,
	flashR7FA6M3AH3Records
};

