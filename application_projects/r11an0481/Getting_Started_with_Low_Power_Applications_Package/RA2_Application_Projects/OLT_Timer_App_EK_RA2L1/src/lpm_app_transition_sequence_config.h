/***********************************************************************************************************************
 * File Name    : lpm_app_transition_sequence_config.h
 * Description  : Contains declarations of data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
/**********************************************************************************************************************
* Copyright (c) 2019 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/

#ifndef LPM_APP_TRANSITION_SEQUENCE_CONFIG_H_
#define LPM_APP_TRANSITION_SEQUENCE_CONFIG_H_

/*******************************************************************
 * @brief Pre-defined LPM Transition Sequence
 * There are 2 kinds of sequence, one supports RA6Mx device and the other supports other RA devices
 * *****************************************************************/
#if defined (BOARD_RA6M3_EK) || defined (BOARD_RA6M3G_EK) || defined (BOARD_RA6M2_EK) || defined (BOARD_RA6M1_EK)
app_lpm_states_t g_lpm_transition_sequence[] = {
	APP_LPM_SW_STANDBY_STATE,            ///< SW Standby mode
	APP_LPM_DEEP_SW_STANDBY_STATE,       ///< Deep SW Standby mode
	APP_LPM_NORMAL_STATE                 ///< Normal mode
};
#elif defined (BOARD_RA4M1_EK) || defined (BOARD_RA2A1_EK) || defined (BOARD_RA2L1_EK)
app_lpm_states_t g_lpm_transition_sequence[] = {
	APP_LPM_SW_STANDBY_STATE,            ///< SW Standby mode
	APP_LPM_NORMAL_STATE                 ///< Normal mode
};
#endif

#endif /* LPM_APP_TRANSITION_SEQUENCE_CONFIG_H_ */
