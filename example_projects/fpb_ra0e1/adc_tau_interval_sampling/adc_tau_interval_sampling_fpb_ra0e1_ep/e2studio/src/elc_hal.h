/***********************************************************************************************************************
 * File Name    : elc_hal.h
 * Description  : Contains data structures and functions used in elc_hal.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef ELC_HAL_H_
#define ELC_HAL_H_

/*function declaration */
fsp_err_t init_hal_elc(elc_ctrl_t * const p_ctrl_instance_elc,  elc_cfg_t const * const p_cfg_instance_elc);
fsp_err_t elc_enable(elc_ctrl_t * const p_ctrl_instance_elc);
void deinit_hal_elc(elc_ctrl_t * const p_ctrl_instance_elc);

#endif /* ELC_HAL_H_ */
