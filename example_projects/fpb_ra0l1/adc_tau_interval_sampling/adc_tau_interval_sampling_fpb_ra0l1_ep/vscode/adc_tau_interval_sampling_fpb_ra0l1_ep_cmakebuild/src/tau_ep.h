/***********************************************************************************************************************
 * File Name    : tau_ep.h
 * Description  : Contains functions used in tau_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef TAU_EP_H_
#define TAU_EP_H_

#include "hal_data.h"

/* Function declarations */
fsp_err_t tau_init(void);
fsp_err_t tau_start(void);
void tau_deinit(void);

#endif /* TAU_EP_H_ */
