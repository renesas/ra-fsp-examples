/***********************************************************************************************************************
 * File Name    : dac8_ep.h
 * Description  : Contains declarations of data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef DAC8_EP_H_
#define DAC8_EP_H_

#define EP_INFO             "\r\nThe project initializes DAC8 module in Normal mode." \
                            "\r\nIn Normal mode, user can enter DAC value within permitted range(0-255)." \
                            "\r\nWhen DAC output value is given as input to ADC channel 0," \
                            "\r\nADC output value is printed on the RTT JlinkViewer.\r\n"

#define DAC_MIN_VAL   (1)
#define DAC_MAX_VAL   (255)

/* Macro to set internal reference voltage value as 2.5V*/
#define VREFADCG_VALUE    0x06

/* Macro to enable VREFADC output*/
#define VREFADCG_ENABLE   0x18

/* DAC8 output pin for board RA2A1 */
#define DAC8_OUTPUT_PIN (BSP_IO_PORT_00_PIN_13)

#endif /* DAC8_EP_H_ */
