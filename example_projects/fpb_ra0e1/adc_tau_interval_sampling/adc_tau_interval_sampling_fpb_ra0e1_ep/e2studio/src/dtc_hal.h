/***********************************************************************************************************************
 * File Name    : dtc_hal.h
 * Description  : Contains data structures and functions used in dtc_hal.h
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef DTC_HAL_H_
#define DTC_HAL_H_

/* function declaration */
fsp_err_t init_hal_dtc(transfer_ctrl_t * const p_api_ctrl_dtc, transfer_cfg_t const * const p_cfg_dtc);
fsp_err_t dtc_hal_reconfigure(transfer_ctrl_t * const p_api_ctrl_dtc, transfer_info_t * p_info);
fsp_err_t dtc_enable(transfer_ctrl_t * const p_api_ctrl_dtc);
void deinit_hal_dtc(transfer_ctrl_t * const p_api_ctrl_dtc);

#endif /* DTC_HAL_H_ */
