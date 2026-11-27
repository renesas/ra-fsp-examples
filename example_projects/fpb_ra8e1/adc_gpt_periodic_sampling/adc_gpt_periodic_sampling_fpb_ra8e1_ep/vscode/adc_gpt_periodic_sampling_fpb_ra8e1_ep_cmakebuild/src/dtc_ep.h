/***********************************************************************************************************************
 * File Name    : dtc_ep.h
 * Description  : Contains functions used in dtc_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef DTC_EP_H_
#define DTC_EP_H_

#include "hal_data.h"

/* Function declarations */
fsp_err_t dtc_init(void);
fsp_err_t dtc_enable(transfer_ctrl_t * const p_api_ctrl_dtc);
fsp_err_t dtc_reconfigure(transfer_ctrl_t * const p_api_ctrl_dtc, transfer_info_t * p_info);
void dtc_deinit(void);

#endif /* DTC_EP_H_ */
