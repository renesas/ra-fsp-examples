/***********************************************************************************************************************
 * File Name    : sdadc_ep.h
 * Description  : Contains declarations of data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef SDADC_EP_H_
#define SDADC_EP_H_

#define EP_INFO             "\r\nThis Example project demonstrates the basic functionalities of SDADC" \
                            "\r\nrunning on Renesas RA MCUs using RA board and external variable power" \
                            "\r\nsupply.SDADC channel 0 is connected as single ended input mode and  " \
                            "\r\nchannel 1 is connected as differential ended input mode to power supply" \
                            "\r\nData read from both the channels is displayed on JLink RTT Viewer.\r\n"


#define SDADC_READ_DELAY      (500U)      //Delay for user convenience to see the data
#define SDADC_RESOLUTION      (16777216U) // 24-bit SDADC i.e 2 power 24
#define OFFSET_VOLTAGE        (float)(0.2)
#define GAIN_TOTAL            (1U)        //Stage 1 gain * Stage 2 gain which is 1 here
#define BUFF_SIZE             (30U)
#define MASK                  (0x00FFFFFF)
#define SHIFT                 (8U)
#define LEN                   (8U)
#define VREF_VOLT_0_8_V  (float)0.8f    // Vref is 0.8 V
#define VREF_VOLT_1_0_V  (float)1.0f    // Vref is 1.0 V
#define VREF_VOLT_1_2_V  (float)1.2f    // Vref is 1.2 V
#define VREF_VOLT_1_4_V  (float)1.4f    // Vref is 1.4 V
#define VREF_VOLT_1_6_V  (float)1.6f    // Vref is 1.6 V
#define VREF_VOLT_1_8_V  (float)1.8f    // Vref is 1.8 V
#define VREF_VOLT_2_0_V  (float)2.0f    // Vref is 2.0 V
#define VREF_VOLT_2_2_V  (float)2.2f    // Vref is 2.2 V



#endif /* SDADC_EP_H_ */
