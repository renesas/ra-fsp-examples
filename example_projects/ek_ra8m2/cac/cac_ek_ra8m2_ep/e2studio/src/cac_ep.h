/***********************************************************************************************************************
 * File Name    : cac_ep.h
 * Description  : Contains macros and function declarations.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef CAC_EP_H_
#define CAC_EP_H_

/* Macro definitions */
/* Event flag to identify CAC events */
#define CAC_EVENT_VALUE         (0XFF)
/* To determine number of measurement clocks (CGC) available in CAC configurator */
#define TARGET_CGC_CLK_CNT      (0x06)

#define EP_INFO                 "\r\nThis example project demonstrates basic functionalities of CAC driver."\
                                "\r\nOn any key press from J-Link RTT Viewer, CAC module starts measuring the"\
								"\r\naccuracy of selected target clock against the selected reference clock."\
								"\r\nResult of the measurement is displayed on J-Link RTT Viewer.\r\n"

/* Private enumeration */
/* Enumeration of the possible target clocks. As CGC and CAC clock sources have different enumeration values */
typedef enum e_target_clk_src
{
    TARGET_CLOCK_SOURCE_HOCO     = 0x00,  /* To Select Target clock as HOCO (High speed on chip oscillator) */
    TARGET_CLOCK_SOURCE_MOCO     = 0x01,  /* To Select Target clock as MOCO (Middle speed on chip oscillator) */
    TARGET_CLOCK_SOURCE_LOCO     = 0x02,  /* To Select Target clock as LOCO (Middle speed on chip oscillator) */
    TARGET_CLOCK_SOURCE_MAIN_OSC = 0x03,  /* To Select Target clock as Main clock oscillator */
    TARGET_CLOCK_SOURCE_SUBCLOCK = 0x04,  /* To Select Target clock as Sub-clock */
    TARGET_CLOCK_SOURCE_PCLKB    = 0x03   /* To select Target clock as PCLKB and its one of clock source is Main OSC */
} target_clock_t;

/* Function declarations */
void deinit_cac(void);
void deinit_cgc(void);
void clean_up(void);

#endif /* CAC_EP_H_ */
