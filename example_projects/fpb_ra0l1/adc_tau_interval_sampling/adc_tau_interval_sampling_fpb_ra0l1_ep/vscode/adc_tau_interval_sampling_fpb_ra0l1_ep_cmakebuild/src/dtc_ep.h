/***********************************************************************************************************************
 * File Name    : dtc_ep.h
 * Description  : Contains functions used in dtc_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef DTC_EP_H_
#define DTC_EP_H_

#include "adc_ep.h"

/* Function declarations */
fsp_err_t dtc_init(void);
fsp_err_t dtc_enable(void);
fsp_err_t dtc_reconfigure(void);
void dtc_deinit(void);

#endif /* DTC_EP_H_ */
