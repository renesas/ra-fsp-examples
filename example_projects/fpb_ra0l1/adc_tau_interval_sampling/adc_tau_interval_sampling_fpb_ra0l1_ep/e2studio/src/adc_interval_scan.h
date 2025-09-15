/***********************************************************************************************************************
 * File Name    : adc_interval_scan.h
 * Description  : Contains data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef ADC_INTERVAL_SCAN_H_
#define ADC_INTERVAL_SCAN_H_

typedef enum e_module_name
{
    ELC_MODULE,
    ELC_DTC_MODULE,
    ELC_DTC_ADC_MODULE,
    ALL
}module_name_t;

#define EP_INFO   "\r\nThe Example Project demonstrates the basic functionality of ADC TAU Interval Sampling\r\n"\
                  "on Renesas RA MCUs based on FSP. On successful initialization of ADC, TAU, ELC and DTC\r\n"\
                  "module, TAU triggers an ADC group scan at intervals. When each group scan completes, DTC\r\n"\
                  "triggers data transfer and copies data to user buffer. The sample ADC data can be viewed\r\n"\
                  "through waveform rendering of memory using memory viewer in e2studio.\r\n\n"\

#endif /* ADC_INTERVAL_SCAN_H_ */
