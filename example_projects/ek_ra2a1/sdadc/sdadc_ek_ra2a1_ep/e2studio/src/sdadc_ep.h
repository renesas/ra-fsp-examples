/***********************************************************************************************************************
 * File Name    : sdadc_ep.h
 * Description  : Contains declarations of data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef SDADC_EP_H_
#define SDADC_EP_H_

#define EP_INFO             "\r\nThis Example project demonstrates the basic functionalities of SDADC" \
                            "\r\nrunning on Renesas RA MCUs using RA board and external variable power" \
                            "\r\nsupply.SDADC channel 0 is connected as single ended input mode and  " \
                            "\r\nchannel 2 is connected as differential ended input mode to power supply" \
                            "\r\nData read from both the channels is displayed on JLink RTT Viewer.\r\n"


#define SDADC_READ_DELAY                    (500U)      //Delay for user convenience to see the data
#define SDADC_RESOLUTION                    (16777216U) // 24-bit SDADC i.e 2 power 24
#define GAIN_TOTAL                          (1U)        //Stage 1 gain * Stage 2 gain which is 1 here
#define BUFF_SIZE                           (30U)
#define MASK                                (0x00FFFFFF)
#define SHIFT                               (8U)
#define LEN                                 (8U)

#if (BSP_PERIPHERAL_SDADC_PRESENT == 1)
#define OFFSET_VOLTAGE                      (float)(0.2)
#define SDADC_FULL_SCALE_VOLTAGE            (float)(1.6)
#else
#define VREF_VOLT                           (0.69F)
#define VREF_FULL_SCALE                     (VREF_VOLT * 2.0)
extern const sdadc_b_scan_cfg_t g_sdadc_channel_cfg;
#endif


#endif /* SDADC_EP_H_ */
