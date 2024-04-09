/***********************************************************************************************************************
 * File Name    : dac_ep.h
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

#ifndef DAC_EP_H_
#define DAC_EP_H_

#define EP_INFO             "\r\nThe project initializes and demonstrates DAC module on Renesas RA MCUs using ADC." \
                            "\r\nUser can enter DAC value within permitted range(0-4095) and the" \
                            "\r\nconverted output is given as input to ADC channel 0." \
                            "\r\nADC output value is printed on the RTT JlinkViewer.\r\n"

#define DAC_MIN_VAL   (1)
#define DAC_MAX_VAL   (4095)

/*DAC Output Pin for EK-RA2A1 */
#define DAC12_OUTPUT_PIN (BSP_IO_PORT_05_PIN_00)

/* DAC stabilization time (1ms)*/
#define STABILIZATION_DELAY    (1)

/* Macro to set internal reference voltage value as 2.5V*/
#define VREFADCG_VALUE    (0x06)

/* Macro to enable VREFADC output*/
#define VREFADCG_ENABLE   (0x18)

#endif /* DAC_EP_H_ */
