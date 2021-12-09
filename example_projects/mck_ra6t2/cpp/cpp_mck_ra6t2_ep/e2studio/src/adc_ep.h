/***********************************************************************************************************************
 * File Name    : adc_ep.h
 * Description  : Contains class, data structures and functions used in adc_ep.cpp/.h.
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

#ifndef ADC_EP_H_
#define ADC_EP_H_
#include "common_utils.h"

/* Macros for Commands to be received through RTT inputs */
#define SCAN_START       0x01
#define SCAN_STOP        0x02

/* Macro for default buff size for reading through RTT */
#define BUFF_SIZE         0x0F

/* Macros for checking the deviation in adc values */
#define TEMPERATURE_DEVIATION_LIMIT       0x04

/* Macro for indexing buffer. used to read the bytes received from RTT input */
#define BUFF_INDEX       0x00

/* Macro to provide delay in read adc data*/
#define ADC_READ_DELAY        0x01

/* Macro to set value to output voltage control*/
#define VREFADCG_VALUE    0x03

/* Macro to set value to enable VREFADC output*/
#define VREFADCG_ENABLE   0x03

#define SHIFT_BY_ONE     0x01
#define SHIFT_BY_THREE   0x03

/* Macros for menu options to be displayed */
#define MENUOPTIONS       "\r\nMENU to Select \
                           \r\nPress 1 to Start ADC Scan \
                           \r\nPress 2 to Stop ADC Scan(Only for Continuous mode) \
                           \r\nUser Input : "

#define BANNER_ADC       "\r\nThe example project demonstrates some features of C++ like class,contructors, \
                          \r\ndestructor,members functions using ADC module hal API's provided by FSP. \
                          \r\nADC module is wrapped into a class.An ADC object of class type is instantiated \
                          \r\nand is used to call respective member functions which initiates scan and \
                          \r\nstop(in case of continuous scan mode).Results and status are displayed on the \
                          \r\nJLinkRTTViewer.\r\n"

extern volatile bool b_ready_to_read;
#ifdef BOARD_RA6T2_MCK
    extern const adc_b_extended_cfg_t g_adc_cfg_extend;
#endif

class adc_ep
{
    uint16_t g_adc_data = RESET_VALUE;
    uint16_t g_prev_adc_data = RESET_VALUE;
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
#endif
#if (defined BOARD_RA2A1_EK) || (defined BOARD_RA6T2_MCK)
    fsp_err_t adc_start_calibration(void);
#endif

};

#endif /* ADC_EP_H_ */
