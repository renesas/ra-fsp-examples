/*
* Cright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : common_cfg.h
 * Version      : 1.00
 * Description  : Common configuration header file for  EK-RA2L2 Quick Start Example Project
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 01.05.2025 1.00     First Release
 *********************************************************************************************************************/

#ifndef QSEP_SETTING_H_
#define QSEP_SETTING_H_

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define KIT_INFO                ('1')                /* Key to press to go to kit info menu */
#define NEXT_STEPS              ('2')                /* Key to press to go to next steps menu */
#define WELCOME                 (' ')                /* Key to press to go to welcome menu */

#define LED_INTENSITY_10        (10)                /* 10 percent */
#define LED_INTENSITY_50        (50)                /* 50 percent */
#define LED_INTENSITY_90        (90)                /* 90 percent */

#define BLINK_FREQ_1HZ          (12000000)          /*  1MHZ */
#define BLINK_FREQ_5HZ          (2400000)           /*  5MHZ */
#define BLINK_FREQ_10HZ         (1200000)           /* 10MHZ */

/**********************************************************************************************************************
 Typedef definitions
 *********************************************************************************************************************/
typedef struct
{
    uint8_t led_intensity;                          /* PWM pulse width */
    uint8_t led_frequency;                          /* frequency */
    uint8_t cur_menu;                               /* current menu */
} st_board_status_t;


#endif /* QSEP_SETTING_H_ */


