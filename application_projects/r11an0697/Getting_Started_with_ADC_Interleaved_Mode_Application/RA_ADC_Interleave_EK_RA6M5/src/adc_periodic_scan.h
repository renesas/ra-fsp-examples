/***********************************************************************************************************************
 * File Name    : adc_periodic_scan.h
 * Description  : Contains data structures and functions used in adc_periodic_scan.h
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2022 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef ADC_PERIODIC_SCAN_H_
#define ADC_PERIODIC_SCAN_H_

typedef enum e_module_name
{
    ELC_MODULE,
    ELC_DMAC0,
    ELC_DMAC01,
    ELC_DMAC_MODULE_ALL,
    ELC_DMAC_ADC0_MODULE,
    ELC_DMAC_ADC_MODULE_ALL,
    ALL
}module_name_t;

#endif /* ADC_PERIODIC_SCAN_H_ */
