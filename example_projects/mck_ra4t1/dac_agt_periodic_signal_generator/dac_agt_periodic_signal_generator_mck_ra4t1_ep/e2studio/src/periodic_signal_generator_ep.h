/***********************************************************************************************************************
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

#ifndef PERIODIC_SIGNAL_GENERATOR_EP_H_
#define PERIODIC_SIGNAL_GENERATOR_EP_H_

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif


#define EP_INFO       "\r\nThis example project demonstrates generation of a sinusoidal signal "\
		              "\r\nusing the DAC, AGT, and DTC modules. The output sinusoid's properties "\
                      "\r\nare set by inputting the desired samples per period and frequency in "\
                      "\r\nJLink RTT Viewer. The application first calculates and stores one period "\
                      "\r\nof the sinusoid to a look up table, and then iterates through the table  "\
                      "\r\nto generate the continuous wave from the DAC. The output wave will "\
                      "\r\noscillate between the minimum and maximum DAC values at the user-specified"\
                      "\r\nsamples per period and frequency.\n\n "


#define SPP_MIN_VAL   (2)
#define SPP_MAX_VAL   (256)

#define DAC_MID_VAL (2047.5)

#ifdef BOARD_RA2L1_EK
    #define DADR0                       0x4005E000
    #define DAC_MAX_CONVERSION_PERIOD   30
#endif

#ifdef BOARD_RA2A1_EK
    #define DADR0                       0x4005E000
    #define DAC_MAX_CONVERSION_PERIOD   1
#endif

#ifdef BOARD_RA4M2_EK
    #define DADR0                       0x40171000
    #define DAC_MAX_CONVERSION_PERIOD   4
#endif

#ifdef BOARD_RA6M4_EK
    #define DADR0                       0x40171000
    #define DAC_MAX_CONVERSION_PERIOD   4
#endif

#ifdef BOARD_RA4E2_EK
    #define DADR0                       0x40171000
    #define DAC_MAX_CONVERSION_PERIOD   4
#endif

#ifdef BOARD_RA4T1_MCK
    #define DADR0                       0x40171000
    #define DAC_MAX_CONVERSION_PERIOD   4
#endif

#endif /*PERIODIC_SIGNAL_GENERATOR_EP_H_ */
