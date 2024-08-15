/***********************************************************************************************************************
 * Description  : Contains declarations of data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef PERIODIC_SIGNAL_GENERATOR_EP_H_
#define PERIODIC_SIGNAL_GENERATOR_EP_H_

#ifndef M_PI
    #define M_PI (3.14159265358979323846)
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
    #define DADR0                       (0x4005E000)
    #define DAC_MAX_CONVERSION_PERIOD   (30)
#endif

#ifdef BOARD_RA2A1_EK
    #define DADR0                       (0x4005E000)
    #define DAC_MAX_CONVERSION_PERIOD   (1)
#endif

#ifdef BOARD_RA4M2_EK
    #define DADR0                       (0x40171000)
    #define DAC_MAX_CONVERSION_PERIOD   (4)
#endif

#ifdef BOARD_RA6M4_EK
    #define DADR0                       (0x40171000)
    #define DAC_MAX_CONVERSION_PERIOD   (4)
#endif

#ifdef BOARD_RA6E2_EK
    #define DADR0                       (0x40171000)
    #define DAC_MAX_CONVERSION_PERIOD   (4)
#endif

#ifdef BOARD_RA8D1_EK
    #define DADR0                       (0x40333000)
    #define DAC_MAX_CONVERSION_PERIOD   (3)
#endif

#ifdef BOARD_RA8M1_EK
    #define DADR0                       (0x40333000)
    #define DAC_MAX_CONVERSION_PERIOD   (3)
#endif

#endif /*PERIODIC_SIGNAL_GENERATOR_EP_H_ */
