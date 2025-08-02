/***********************************************************************************************************************
 * File Name    : adc_ep.h
 * Description  : Contains data structures and functions used in adc_ep.c/.h.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef ADC_EP_H_
#define ADC_EP_H_

/*******************************************************************************************************************//**
 * @ingroup adc_ep
 * @{
 **********************************************************************************************************************/

/* Macros for Commands to be received through RTT Viewer inputs */
#define SCAN_START                      (0x01)
#define SCAN_STOP                       (0x02)

/* Macro for default buff size for reading through RTT Viewer */
#define BUFF_SIZE                       (0x0F)

/* Macro to provide delay in read ADC data */
#define ADC_READ_DELAY                  (0x01)

#if (defined BOARD_RA2A1_EK)
/* Macro for checking the deviation in ADC values */
#define TEMPERATURE_DEVIATION_LIMIT     (0x04)

/* Macro to set value to output voltage control */
#define VREFADCG_VALUE                  (0x03)

/* Macro to set value to enable VREFADC output */
#define VREFADCG_ENABLE                 (0x03)

#define SHIFT_BY_ONE                    (0x01)
#define SHIFT_BY_THREE                  (0x03)

#endif /* Defined board BOARD_RA2A1_EK */

/* Macros for menu options to be displayed */
#define MENUOPTION1       "\r\nMENU to Select\r\n"
#define MENUOPTION2       "Press 1 to Start ADC Scan\r\n"

#if (BSP_PERIPHERAL_ADC_D_PRESENT)
#define MENUOPTION3       "Press 2 to Stop ADC Scan (Only for Sequential mode)\r\n"
#else
#define MENUOPTION3       "Press 2 to Stop ADC Scan (Only for Continuous mode)\r\n"
#endif

#define MENUOPTION4       "User Input :"

#if (BSP_PERIPHERAL_ADC_D_PRESENT)
#define BANNER_7    "\r\nThe project initializes the ADC in One-shot or Sequential mode\
                    \r\nbased on the user selection in RA Configurator. Once initialized, the user\
                    \r\ncan start the ADC scan and also stop the scan (in the case of Sequential mode)\
                    \r\nusing J-Link RTT Viewer by sending commands.\
                    \r\nResults are displayed on J-Link RTT Viewer.\r\n"
#else
#define BANNER_7    "\r\nThe project initializes the ADC in Single Scan or Continuous Scan mode \
                    \r\nbased on the user selection in RA Configurator. Once initialized, the user\
                    \r\ncan start the ADC scan and also stop the scan (in the case of Continuous Scan mode)\
                    \r\nusing J-Link RTT Viewer by sending commands.\
                    \r\nResults are displayed on J-Link RTT Viewer.\r\n"
#endif

/* Reading the commands from RTT Viewer input and process it */
fsp_err_t read_process_input_from_RTT(void);

/* Read the ADC data available */
fsp_err_t adc_read_data(void);
#if (BSP_PERIPHERAL_ADC_D_PRESENT)
/* Extern configure to check scan mode */
extern const adc_d_extended_cfg_t g_adc_cfg_extend;
#elif (BSP_PERIPHERAL_ADC_B_PRESENT)
extern const adc_b_extended_cfg_t g_adc_b_cfg_extend;
#endif

/* Macro for ADC_VERSION */
#if (BSP_PERIPHERAL_ADC_D_PRESENT)
#define ADC_TYPE                   "ADC_D"
#elif (BSP_PERIPHERAL_ADC_B_PRESENT)
#define ADC_TYPE                   "ADC_B"
#else
#define ADC_TYPE                   "ADC"
#endif

/* Macro to define the ADC channel to be used for each board */
#if defined(BOARD_RA4W1_EK)
#define ADC_CHANNEL  (ADC_CHANNEL_4)
#elif defined(BOARD_RA2E2_EK)
#define ADC_CHANNEL  (ADC_CHANNEL_9)
#elif defined(BOARD_RA6T1_RSSK) || defined(BOARD_RA8T1_MCK) || defined(BOARD_RA4L1_EK) || defined(BOARD_RA4C1_EK)
#define ADC_CHANNEL  (ADC_CHANNEL_2)
#elif defined(BOARD_RA6T2_MCK)
#define ADC_CHANNEL  (ADC_CHANNEL_TEMPERATURE)
#elif defined(BOARD_RA2A1_EK) || defined(BOARD_RA8P1_EK)
#define ADC_CHANNEL  (ADC_CHANNEL_1)
#else
#define ADC_CHANNEL  (ADC_CHANNEL_0)
#endif

/* Close the open ADC module  */
void deinit_adc_module(void);

/** @} */
#endif /* ADC_EP_H_ */
