/***********************************************************************************************************************
 * File Name    : adc_ep.h
 * Description  : Contains data structures and functions used in adc_ep.c/.h.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#ifndef ADC_EP_H_
#define ADC_EP_H_

/*******************************************************************************************************************//**
 * @ingroup adc_ep
 * @{
 **********************************************************************************************************************/


/* Macros for Commands to be received through RTT inputs */
#define SCAN_START                      (0x01)
#define SCAN_STOP                       (0x02)

/* Macro for default buff size for reading through RTT */
#define BUFF_SIZE                       (0x0F)

/* Macros for checking the deviation in adc values */
#define TEMPERATURE_DEVIATION_LIMIT     (0x04)

/* Macro for indexing buffer. used to read the bytes received from RTT input */
#define BUFF_INDEX                      (0x00)

/* Macro to provide delay in read adc data*/
#define ADC_READ_DELAY                  (0x01)

/* Macro to set value to output voltage control*/
#define VREFADCG_VALUE                  (0x03)

/* Macro to set value to enable VREFADC output*/
#define VREFADCG_ENABLE                 (0x03)

#define SHIFT_BY_ONE                    (0x01)
#define SHIFT_BY_THREE                  (0x03)

/* Macros for menu options to be displayed */
#define MENUOPTION1       "\r\nMENU to Select\r\n"
#define MENUOPTION2       "Press 1 to Start ADC Scan\r\n"

#if (BSP_PERIPHERAL_ADC_D_PRESENT == 1U)
#define MENUOPTION3       "Press 2 to Stop ADC Scan (Only for Sequential mode)\r\n"
#else
#define MENUOPTION3       "Press 2 to Stop ADC Scan(Only for Continuous mode)\r\n"
#endif

#define MENUOPTION4       "User Input :"

#if (BSP_PERIPHERAL_ADC_D_PRESENT == 1U)
#define BANNER_7          "\r\nThe project initializes the ADC in One-shot or Sequential mode \
                           \r\nbased on user selection in RA configurator. Once initialized, user can start \
                           \r\nthe ADC scan and also stop the scan (in the case of Sequential mode)\
                           \r\nusing J-Link RTT Viewer by sending commands. Results are displayed on J-Link RTT Viewer.\r\n"
#else
#define BANNER_7          "\r\nThe project initializes the ADC in single scan or continuous scan mode \
                           \r\nbased on user selection in RA configurator. Once initialized, user can start \
                           \r\nthe ADC scan and also stop the scan (in the case of continuous scan mode)\
                           \r\nusing J-Link RTT Viewer by sending commands. Results are displayed on J-Link RTT Viewer.\r\n"
#endif
/* Reading the commands from RTT input and process it*/
fsp_err_t read_process_input_from_RTT(void);

/* Read the adc data available */
fsp_err_t adc_read_data(void);
#if (BSP_PERIPHERAL_ADC_D_PRESENT == 1U)
/* Extern configure to check scan mode*/
    extern const adc_d_extended_cfg_t g_adc_cfg_extend;
#endif

/* Macro for ADC_VERSION */
#if (BSP_PERIPHERAL_ADC_D_PRESENT == 1U)
#define ADC_TYPE                   "ADC_D"
#else
#define ADC_TYPE                   "ADC"
#endif

/* close the open adc module  */
void deinit_adc_module(void);

/** @} */
#endif /* ADC_EP_H_ */
