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


#define EP_INFO       "\r\nThis example project demonstrates configuration of the DAC, AGT, and DTC "\
		              "\r\nto generate a continuous sinusoid signal. The user will define the samples "\
                      "\r\nper period and frequency in Hz of the desired output wave in J-Link RTT Viewer. "\
                      "\r\nThe samples per period is used to compute and store 1 period of the sinusoid "\
                      "\r\nin a look up table (LUT), and the frequency is used to set the AGT period to "\
                      "\r\nthe appropriate sampling rate. The AGT generates an interrupt which triggers "\
                      "\r\nthe DTC to transfer the next value of the LUT to the DADR0 register. The output "\
					  "\r\nsinusoid wave will oscillate between the minimum and maximum DAC values at the "\
                      "\r\nuser-specified samples per period and frequency.\n\n "


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

#ifdef BOARD_RA4E2_EK
    #define DADR0                       (0x40171000)
    #define DAC_MAX_CONVERSION_PERIOD   (4)
#endif

#ifdef BOARD_RA4M2_EK
    #define DADR0                       (0x40171000)
    #define DAC_MAX_CONVERSION_PERIOD   (4)
#endif

#ifdef BOARD_RA4T1_MCK
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

#ifdef BOARD_RA6T3_MCK
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

#ifdef BOARD_RA8T1_MCK
    #define DADR1                       (0x40333002)
    #define DAC_MAX_CONVERSION_PERIOD   (3)
#endif

#ifdef BOARD_RA8E1_FPB
    #define DADR0                       (0x40333000)
    #define DAC_MAX_CONVERSION_PERIOD   (3)
#endif

#endif /*PERIODIC_SIGNAL_GENERATOR_EP_H_ */
