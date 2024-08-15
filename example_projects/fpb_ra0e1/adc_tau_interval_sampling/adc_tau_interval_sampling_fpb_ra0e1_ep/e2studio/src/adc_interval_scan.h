/***********************************************************************************************************************
 * File Name    : adc_interval_scan.h
 * Description  : Contains data structures and functions used in adc_interval_scan.h
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef ADC_INTERVAL_SCAN_H_
#define ADC_INTERVAL_SCAN_H_

typedef enum e_module_name
{
    ELC_MODULE,
    ELC_DTC1,
    ELC_DTC12,
    ELC_DTC_123,
    ELC_DTC_MODULE_ALL,
    ELC_DTC_ADC0_MODULE,
    ELC_DTC_ADC_MODULE_ALL,
    ELC_DTC_ADC_TAU_MODULE,
    ALL
}module_name_t;


#define EP_INFO  "\r\nThe Example Project demonstrates the basic functionality of ADC TAU Interval Sampling on Renesas \r\n"\
                  "RA MCUs based on FSP. On successful initialization of ADC, TAU, ELC and DTC module,\r\n"\
                  "TAU triggers an ADC group Scan at intervals. When each group scan completes, DTC triggers\r\n"\
                  "data transfer and copies data to user buffer. The sample ADC data can be viewed through,\r\n"\
                  "waveform rendering of memory using memory viewer in e2studio.\r\n\n"\

#endif /* ADC_INTERVAL_SCAN_H_ */
