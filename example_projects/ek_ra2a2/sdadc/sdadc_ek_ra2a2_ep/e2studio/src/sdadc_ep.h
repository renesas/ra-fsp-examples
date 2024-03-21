/***********************************************************************************************************************
 * File Name    : sdadc_ep.h
 * Description  : Contains declarations of data structures and functions used in hal_entry.c.
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
