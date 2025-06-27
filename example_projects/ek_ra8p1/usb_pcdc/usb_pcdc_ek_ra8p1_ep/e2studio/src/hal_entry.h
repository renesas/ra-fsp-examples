/***********************************************************************************************************************
 * File Name    : hal_entry.h
 * Description  : Contains macros used in hal_entry.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
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

#define KIT_INFO_PRIMARY_TEXT                "\r\n1. KIT INFORMATION \r\n\r\na) Kit name: \t\t\t\t"\
                                              ""KIT_NAME_MACRO""\
                                             "\r\nb) Kit ordering part number:\t\t"\
                                              ""KIT_PARTNUM_MACRO""\

/* Macro for ADC version */
#if (BSP_PERIPHERAL_ADC_B_PRESENT)
#define  ADC_TYPE               "ADC_B"
#else
#define  ADC_TYPE               "ADC"
#endif /* BSP_PERIPHERAL_ADC_B_PRESENT */

#define LINE_CODING_LENGTH          (0x07U)
#define ADCTEMP_AS_C(a)             ((((float)(a)) * (0.298147f)) - 287.388f)
#define ADCTEMP_AS_F(a)             ((((float)(a)) * (0.536652f)) - 485.298f)
#define READ_BUF_SIZE               (8U)

/** @} */
#endif /* HAL_ENTRY_H_ */
