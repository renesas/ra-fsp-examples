/***********************************************************************************************************************
 * File Name    : flash_access_ns.h
 * Description  : interface for flash_access_ns.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef FLASH_ACCESS_NS_H_
#define FLASH_ACCESS_NS_H_

#include <app_definitions.h>
#include "hal_data.h"

bool flash_write_ns(uint32_t Flash_address, uint8_t *writeBuffer);

#endif /* FLASH_ACCESS_NS_H_ */

