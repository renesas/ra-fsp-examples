/***********************************************************************************************************************
 * File Name    : dmac_hal.h
 * Description  : Contains data structures and functions used in dmac_hal.h
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2022 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef DMAC_HAL_H_
#define DMAC_HAL_H_

/* Function declaration */
fsp_err_t init_hal_dmac(transfer_ctrl_t * const p_api_ctrl_dmac, transfer_cfg_t const * const p_cfg_dmac);
fsp_err_t dmac_hal_reconfigure(transfer_ctrl_t * const p_api_ctrl_dmac, transfer_info_t * p_info);
fsp_err_t dmac_enable(transfer_ctrl_t * const p_api_ctrl_dmac);
void deinit_hal_dmac(transfer_ctrl_t * const p_api_ctrl_dmac);

#endif /* DMAC_HAL_H_ */
