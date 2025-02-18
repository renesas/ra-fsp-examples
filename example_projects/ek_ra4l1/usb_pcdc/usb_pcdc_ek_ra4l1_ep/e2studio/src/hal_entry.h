/***********************************************************************************************************************
 * File Name    : hal_entry.h
 * Description  : Entry function.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef HAL_ENTRY_H_
#define HAL_ENTRY_H_

#include "board_cfg.h"

/*******************************************************************************************************************//**
 * @ingroup hal_entry
 * @{
 **********************************************************************************************************************/


#define KIT_INFO_PRIMARY_TEXT                "\r\n 1. KIT INFORMATION \r\n\r\n a) Kit name: \t\t\t\t"\
                                              ""KIT_NAME_MACRO""\
                                             "\r\n b) Kit ordering part number:\t\t"\
                                              ""KIT_PARTNUM_MACRO""\

#define LINE_CODING_LENGTH          (0x07U)
#define ADCTEMP_AS_C(a)             ((((float)(a)) * (-0.244140f)) + 358.263f)
#define ADCTEMP_AS_F(a)             ((((float)(a)) * (-0.439452f)) + 676.873f)
#define READ_BUF_SIZE               (8U)

/** @} */
#endif /* HAL_ENTRY_H_ */
