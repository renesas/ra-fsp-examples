/***********************************************************************************************************************
 * File Name    : poeg_ep.h
 * Description  : Contains macros and functions declaration
 ***********************************************************************************************************************/
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
 * Copyright (C) 2022 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#ifndef POEG_EP_H_
#define POEG_EP_H_
#include "common_utils.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
/* Macros for commands to be received through RTT input */
#define POEG_TRIGGER_ON_GTETRG_INPUT_PIN_LEVEL (1U)
#define POEG_OUTPUT_DISABLE_USING_SOFTWARE_API (2U)
#define POEG_OUTPUT_DISABLE_ON_GTIOC_OUTPUT_LEVEL (3U)
#define POEG_OUTPUT_DISABLE_ON_ACMPHS_EVENTS (4U)
#define POEG_RESET (5U)

#define POEG_CHANNEL_0 (1U)
#define POEG_CHANNEL_1 (2U)


/* Macro definition */
#define DAC_MAX_VAL (4095U)     /* Max count value for 12 bit DAC */
#define DAC_REF_VAL (1024U)     /* Reference value for 12 bit DAC */
#define LOW_VAL (2000U)         /* Lower value of fluctuating range*/
#define HIGH_VAL (2100U)        /* Higher value of fluctuating range*/
#define CHANNEL_ZERO (0U)       /* Channel 0 of ACMPHS */
#if defined(BOARD_RA4T1_MCK) || defined(BOARD_RA6T3_MCK)
#define LED_PIN (BSP_IO_PORT_01_PIN_13)
#else
#define LED_PIN (BSP_IO_PORT_04_PIN_00)
#endif
#define GTETRG_PIN "P104"
#define SET_FLAG    (1)
#define CLEAR_FLAG  (0)

#define NULL_CHAR   ('\0')

#define EP_INFO                "\r\nThe example project demonstrates the modes of POEG module"\
                               "\r\n1.POEG Trigger Pin mode-GPT Output disable on GTETRG pin level"\
                               "\r\n2.POEG Software API mode-GPT Output disable by software API"\
                               "\r\n3.POEG Trigger Output Level mode-GPT Output disable on GTIOC level"\
                               "\r\n4.POEG Trigger ACMPHS mode-GPT Output disable on comparator events"\
                               "\r\n5.Enable Port output using POEG_Reset API."\
                               "\r\nUsing RTT input, user can choose the POEG method.\r\n"\

#define MENU_INFO              "\r\nPOEG Application Menu\r\n"\
                               "1 :  Disable GPT output pin using POEG trigger pin\r\n"\
                               "2 :  Disable GPT output pin using POEG software API\r\n"\
                               "3 :  Disable GPT output pin using POEG trigger GPT output level\r\n"\
                               "4 :  Disable GPT output pin using POEG trigger ACMPHS Mode\r\n"\
                               "5 :  Reset the POEG Channel\r\n"\
                               "** Reset of application is needed prior to continuous selection of the\r\n"\
                               "menu option 4 to restart the DAC0 and DAC1 and ACMPHS **\r\n"\
                               "** Reset of application is needed prior to continuous selection of the\r\n"\
                               "menu option 1 **\r\n"\
                               "User Input :  "

#define RESET_MENU_INFO         "\r\nPOEG RESET Menu\r\n"\
                               "1 :  RESET POEG Channel 0 \r\n"\
                               "2 :  RESET POEG Channel 1 \r\n"\
                               "User Input :  "

void deinit_gpt_timer(timer_ctrl_t * const p_timer_ctl);
fsp_err_t init_poeg0_module(void);
fsp_err_t init_poeg1_module(void);
fsp_err_t init_gpt_timer_pwm1(void);
fsp_err_t init_gpt_timer_pwm2(void);
fsp_err_t init_gpt_timer_pwm3(void);
fsp_err_t deinit_poeg0_module(void);
fsp_err_t deinit_poeg1_module(void);
fsp_err_t enable_acmphs_mode_modules(void);
void deinit_dac_and_acmphs(void);
void deinit_acmphs(void);
void deinit_dac(dac_instance_ctrl_t * p_ctrl);


extern comparator_info_t g_stabilize_time;
fsp_err_t poeg_ouput_disable_and_reset_options(void);
fsp_err_t reset_poeg_module(void);

#endif /* POEG_EP_H_ */
