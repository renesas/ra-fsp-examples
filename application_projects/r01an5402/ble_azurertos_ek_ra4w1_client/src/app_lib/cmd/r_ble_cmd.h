/**********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#if (BSP_CFG_RTOS == 1) || (BSP_CFG_RTOS == 2)
#include "ble_core_task.h"
#else /* (BSP_CFG_RTOS == 2) */
#include "hal_data.h"
#endif /* (BSP_CFG_RTOS == 2) */

#ifndef R_BLE_CMD_H
#define R_BLE_CMD_H

#define BLE_ABS_INSTANCE    (g_ble_abs0)

/***********************************************************************************************************************
* History : DD.MM.YYYY Version Description           
*         : 15.12.2020 1.00    First Release
***********************************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief Parse comma separated hex values.
 * @param[in] p_str    Comma separated string.
 * @param[in] p_buffer uint8_t array to store values.
 * @param[in] p_length The length of values.
 **********************************************************************************************************************/
void R_BLE_CMD_ParseValues(char *p_str, uint8_t *p_buffer, uint16_t *p_length);

/*******************************************************************************************************************//**
 * @brief Print with format like printf.
 * @param[in] p_bufffer uint8_t array to print.
 * @param[in] length    The length of values.
 **********************************************************************************************************************/
void R_BLE_CMD_PrintValues(uint8_t *p_buffer, uint16_t length);

/*******************************************************************************************************************//**
 * @brief Parse bluetooth device address, the format is 00:11:22:33:44:55.
 * @param[in] p_str  uint8_t array to parse.
 * @param[in] p_addr uint8_t array to store address.
 **********************************************************************************************************************/
void R_BLE_CMD_ParseAddr(char *p_str, uint8_t *p_addr);

/*******************************************************************************************************************//**
 * @brief Print bluetooth device address, the format is 00:11:22:33:44:55.
 * @param[in] p_addr uint8_t array to print in bluetooth device address format.
 **********************************************************************************************************************/
void R_BLE_CMD_PrintAddr(uint8_t *p_addr);

#endif /* R_BLE_CMD_H */
