/***********************************************************************************************************************
 * File Name    : menu.h
 * Description  : Contains menu implementation macro definitions
 ***********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
************************************************************************************************************************/
#ifndef MENU_H_
#define MENU_H_

#include <common_utils.h>
#include <firmware_transfer.h>
#include <slot_info.h>
#include <stdio.h>
#include "hal_data.h"
#include "can.h"

#define FIRMWARE_INFO           (1)
#define FIRMWARE_TRANSFER       (2)

#define RESET_VALUE                     (0x00)
uint8_t menu(void);

#endif /* MENU_H_ */
