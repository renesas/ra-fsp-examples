/***********************************************************************************************************************
 * File Name    : adc_ep.h
 * Description  : Contains class, data structures and functions used in adc_ep.cpp/.h.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef ADC_EP_H_
#define ADC_EP_H_
#include "common_utils.h"

/* Macros for Commands to be received through RTT inputs */
#define SCAN_START       (0x01)
#define SCAN_STOP        (0x02)

/* Macro for default buff size for reading through RTT */
#define BUFF_SIZE         (0x0F)

/* Macros for checking the deviation in adc values */
#define TEMPERATURE_DEVIATION_LIMIT       (0x04)

/* Macro for indexing buffer. used to read the bytes received from RTT input */
#define BUFF_INDEX       (0x00)

/* Macro to provide delay in read adc data*/
#define ADC_READ_DELAY        (0x01)

/* Macro to set value to output voltage control*/
#define VREFADCG_VALUE    (0x03)

/* Macro to set value to enable VREFADC output*/
#define VREFADCG_ENABLE   (0x03)

#define SHIFT_BY_ONE     (0x01)
#define SHIFT_BY_THREE   (0x03)

/* Macros for menu options to be displayed */
#define MENUOPTIONS       "\r\nMENU to Select \
                           \r\nPress 1 to Start ADC Scan \
                           \r\nPress 2 to Stop ADC Scan(Only for Sequential mode) \
                           \r\nUser Input : "

#define BANNER_ADC       "\r\nThe example project demonstrates some features of C++ like class,contructors, \
                          \r\ndestructor,members functions using ADC module hal API's provided by FSP. \
                          \r\nADC module is wrapped into a class.An ADC object of class type is instantiated \
                          \r\nand is used to call respective member functions which initiates scan and \
                          \r\nstop(in case of Sequential mode).Results and status are displayed on the \
                          \r\nJLinkRTTViewer.\r\n"

extern volatile bool g_ready_to_read;
extern const adc_d_extended_cfg_t g_adc_cfg_extend;

class adc_ep
{
    uint16_t g_adc_data = RESET_VALUE;
public:
    adc_ep();
    virtual ~adc_ep();
    fsp_err_t read_process_input_from_RTT(void);
    fsp_err_t adc_read_data(void);
    fsp_err_t init_adc_module(void);
    void deinit_adc_module(void);
private:
    fsp_err_t adc_scan_start(void);
    fsp_err_t adc_scan_stop(void);
#ifdef BOARD_RA2A1_EK
    fsp_err_t adc_deviation_in_output(void);
    fsp_err_t adc_start_calibration(void);
#endif
};

#endif /* ADC_EP_H_ */
