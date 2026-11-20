/***********************************************************************************************************************
 * File Name    : dmac_transfers.h
 * Description  : Contains data structures and functions used in dmac_transfers.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef DMAC_TRANSFERS_H_
#define DMAC_TRANSFERS_H_

#include "hal_data.h"

/* This function performs transfer LED blink operation */
fsp_err_t transfer_led_blink_operation(void);

/* This function performs transfer GPT timer operation */
fsp_err_t transfer_gpt_timer_operation(void);

/* This function performs transfer write IOPORT operation */
fsp_err_t transfer_write_ioport_operation(void);

#endif /* DMAC_TRANSFERS_H_ */
