/***********************************************************************************************************************
 * File Name    : periodic_signal_generator_ep.h
 * Description  : Contains declarations of data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef PERIODIC_SIGNAL_GENERATOR_EP_H_
#define PERIODIC_SIGNAL_GENERATOR_EP_H_

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

#define EP_INFO       "\r\nThis example project demonstrates the configuration of the DAC, AGT, and DTC"\
		              "\r\nto generate a continuous sinusoidal signal. The user will define the samples"\
                      "\r\nper period and frequency in Hz of the desired output wave via J-Link RTT Viewer."\
                      "\r\nThe samples per period are used to compute and store one period of the sinusoidal"\
                      "\r\nin a lookup table (LUT), and the frequency is used to set the AGT period to"\
                      "\r\nthe appropriate sampling rate. The AGT generates an interrupt that triggers"\
                      "\r\nthe DTC to transfer the next value of the LUT to the DADR0 register. The output"\
					  "\r\nsinusoidal wave will oscillate between the minimum and maximum DAC values at the"\
                      "\r\nuser-specified samples per period and frequency.\r\n\r\n"

#define SPP_MIN_VAL                 (2)
#define SPP_MAX_VAL                 (256)

#define DAC_MID_VAL                 (2047.5)

#if defined (BOARD_RA2L1_EK)
#define DADR0                       (0x4005E000)
#define DAC_MAX_CONVERSION_PERIOD   (30)
#elif defined (BOARD_RA2A1_EK)
#define DADR0                       (0x4005E000)
#define DAC_MAX_CONVERSION_PERIOD   (1)
#elif defined (BOARD_RA4E2_EK) || defined (BOARD_RA4M2_EK) || defined (BOARD_RA4T1_MCK) || defined (BOARD_RA6M4_EK)\
      || defined (BOARD_RA6E2_EK) || defined (BOARD_RA6T3_MCK) || defined (BOARD_RA4L1_EK)
#define DADR0                       (0x40171000)
#define DAC_MAX_CONVERSION_PERIOD   (4)
#elif defined (BOARD_RA8D1_EK) || defined (BOARD_RA8M1_EK) || defined (BOARD_RA8E1_FPB) || defined (BOARD_RA8E2_EK)
#define DADR0                       (0x40333000)
#define DAC_MAX_CONVERSION_PERIOD   (3)
#elif defined (BOARD_RA8T1_MCK)
#define DADR1                       (0x40333002)
#define DAC_MAX_CONVERSION_PERIOD   (3)
#elif defined (BOARD_RA8P1_EK) || defined (BOARD_RA8T2_MCK)
#define DADR0                       (0x40233000)
#define DAC_MAX_CONVERSION_PERIOD   (3.5)
#endif

#endif /* PERIODIC_SIGNAL_GENERATOR_EP_H_ */
