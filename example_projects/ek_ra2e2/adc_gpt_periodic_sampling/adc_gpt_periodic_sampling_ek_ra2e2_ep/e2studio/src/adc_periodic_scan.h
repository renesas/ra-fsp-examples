/***********************************************************************************************************************
 * File Name    : adc_periodic_scan.h
 * Description  : Contains data structures and functions used in adc_periodic_scan.h
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#ifndef ADC_PERIODIC_SCAN_H_
#define ADC_PERIODIC_SCAN_H_

typedef enum e_module_name
{
    ELC_MODULE,
    ELC_DTC1,
    ELC_DTC12,
    ELC_DTC_123,
    ELC_DTC_MODULE_ALL,
    ELC_DTC_ADC0_MODULE,
    ELC_DTC_ADC_MODULE_ALL,
    ELC_DTC_ADC_GPT0_MODULE,
    ELC_DTC_ADC_GPT1_MODULE,
    ALL
}module_name_t;


#define EP_INFO  "\r\nThe Example Project demonstrates the basic functionality of ADC GPT Periodic Sampling on Renesas \r\n"\
                  "RA MCUs based on FSP. On successful initialization of ADC, GPT, ELC and DTC module,\r\n"\
                  "GPT triggers an ADC group Scan at periodic intervals. When each group scan completes, DTC triggers\r\n"\
                  "data transfer and copies data to user buffer. The sample ADC data can be viewed through,\r\n"\
                  "waveform rendering of memory using memory viewer in e2studio.\r\n\n"\

#endif /* ADC_PERIODIC_SCAN_H_ */
