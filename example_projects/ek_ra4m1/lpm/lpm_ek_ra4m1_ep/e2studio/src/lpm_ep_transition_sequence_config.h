/***********************************************************************************************************************
 * File Name    : lpm_ep.h
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

#ifndef LPM_EP_TRANSITION_SEQUENCE_CONFIG_H_
#define LPM_EP_TRANSITION_SEQUENCE_CONFIG_H_

/*******************************************************************
 * @brief Pre-defined LPM Transition Sequence
 * There are 2 kinds of sequence, one supports RA6Mx device and the other supports other RA devices
 * *****************************************************************/
#if defined (BOARD_RA6M3_EK) || defined (BOARD_RA6M3G_EK) || defined (BOARD_RA6M2_EK) || defined (BOARD_RA6M1_EK)|| defined(BOARD_RA6M4_EK)
app_lpm_states_t g_lpm_transition_sequence[] =
{
    APP_LPM_SLEEP_STATE,                 ///< Sleep mode
    APP_LPM_SW_STANDBY_STATE,            ///< SW Standby mode
    APP_LPM_SW_STANDBY_SNOOZE_STATE,     ///< SW Standby mode with Snooze enabled
    APP_LPM_DEEP_SW_STANDBY_STATE,       ///< Deep SW Standby mode
    APP_LPM_NORMAL_STATE                 ///< Normal mode
};
#elif defined (BOARD_RA4M1_EK) || defined (BOARD_RA2A1_EK)
app_lpm_states_t g_lpm_transition_sequence[] =
{
    APP_LPM_SLEEP_STATE,                 ///< Sleep mode
    APP_LPM_SW_STANDBY_STATE,            ///< SW Standby mode
    APP_LPM_SW_STANDBY_SNOOZE_STATE,     ///< SW Standby mode with Snooze enabled
    APP_LPM_NORMAL_STATE                 ///< Normal mode
};
#endif

#endif /* LPM_EP_TRANSITION_SEQUENCE_CONFIG_H_ */
