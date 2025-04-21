/***********************************************************************************************************************
 * File Name    : dac_ep.h
 * Description  : Contains declarations of data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef DAC_EP_H_
#define DAC_EP_H_

#define EP_INFO             "\r\nThe project initializes and demonstrates DAC module on Renesas RA MCUs using ADC." \
                            "\r\nThe user can enter DAC value within permitted range(0-4095) and the" \
                            "\r\nconverted output is given as input to ADC channel 0." \
                            "\r\nADC output value is printed on the J-Link RTT Viewer.\r\n"

#define DAC_MIN_VAL             (1)
#define DAC_MAX_VAL             (4095)

/* Macro for ADC_VERSION */
#if (BSP_PERIPHERAL_ADC_B_PRESENT == 1U)
#define ADC_TYPE                   "ADC_B"
#else
#define ADC_TYPE                   "ADC"
#endif

 /* DAC stabilization time (1ms) */
#define STABILIZATION_DELAY  (1)

#if (defined BOARD_RA2A1_EK) || (defined BOARD_RA6T2_MCK)
/* Macro to set internal reference voltage value as 2.5V */
#define VREFADCG_VALUE          (0x06)

/* DAC Output Pin for EK-RA2A1 */
#define DAC12_OUTPUT_PIN        (BSP_IO_PORT_05_PIN_00)

/* Macro to enable VREFADC output */
#define VREFADCG_ENABLE         (0x18)
#endif

#endif /* DAC_EP_H_ */
