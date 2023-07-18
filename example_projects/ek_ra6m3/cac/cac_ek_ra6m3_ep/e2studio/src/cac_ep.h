/***********************************************************************************************************************
 * File Name    : cac_ep.h
 * Description  : Contains Macros and function declarations.
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
#ifndef CAC_EP_H_
#define CAC_EP_H_

/* Macro definition */
/* Event flag to identify CAC events */
#define CAC_EVENT_VALUE       (0xFF)
/* To determine number of measurement clocks(CGC) available in CAC configurator */
#define TARGET_CGC_CLK_CNT    (0x06)

#define EP_INFO       "\r\nThis example project demonstrates basic functionalities of CAC driver."\
        "\r\nOn any key press from RTT viewer, CAC module starts measuring the \r\naccuracy of selected target clock"\
        " against the selected reference clock."\
        "\r\nResult of the measurement is displayed on RTTViewer.\r\n"

/* Private enum*/
/* Enumeration of the possible target clocks. As CGC and CAC clock sources have different enum values.*/
typedef enum e_target_clk_src
{
    TARGET_CLOCK_SOURCE_HOCO     = 0x00,     /* To Select Target clock as HOCO (High speed on chip oscillator)*/
    TARGET_CLOCK_SOURCE_MOCO     = 0x01,     /* To Select Target clock as MOCO (Middle speed on chip oscillator)*/
    TARGET_CLOCK_SOURCE_lOCO     = 0x02,     /* To Select Target clock as LOCO (Middle speed on chip oscillator)*/
    TARGET_CLOCK_SOURCE_MAIN_OSC = 0x03,     /* To Select Target clock as Main clock oscillator*/
    TARGET_CLOCK_SOURCE_SUBCLOCK = 0x04,     /* To Select Target clock as Sub-clock*/
    TARGET_CLOCK_SOURCE_PCLKB    = 0x03      /* To select Target clock as PCLKB and its one of clock source is Main OSC.*/
} target_clock_t;

/* Function declaration */
void deinit_cac(void);
void deinit_cgc(void);
void clean_up(void);

#endif /* CAC_EP_H_ */
