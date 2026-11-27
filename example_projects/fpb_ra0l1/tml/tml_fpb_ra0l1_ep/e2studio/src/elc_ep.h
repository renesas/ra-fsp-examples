/***********************************************************************************************************************
 * File Name    : elc_ep.h
 * Description  : Contains functions used in elc_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef ELC_EP_H_
#define ELC_EP_H_

#include "hal_data.h"

/* Function declarations */
fsp_err_t elc_init(void);
fsp_err_t elc_enable(void);
fsp_err_t elc_event_gen(void);
void elc_deinit(void);

#endif /* ELC_EP_H_ */
