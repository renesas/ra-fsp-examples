/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : touch_gt911.h
 * Version      : 1.0
 * Description  : Support for the GT111 Capacitive touch controller on a LCD panel.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 20.05.2025 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/

#include <stdint.h>
#include <stdbool.h>

#include "r_external_irq_api.h"
#include "r_i2c_master_api.h"

 /**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#ifndef TOUCH_GT911_H_
#define TOUCH_GT911_H_

/* Touch panel selectable slave address */
#define GT_911_I2C_ADDRESS_0x5D         (0x5D)
#define GT_911_I2C_ADDRESS_0x14         (0x14)

/* Select slave (see schematic) */
#define GT_911_I2C_ADDRESS               (GT_911_I2C_ADDRESS_0x5D)

#define GT911_REG_PRODUCT_ID       (0x8140)
#define GT911_REG_READ_COORD_ADDR  (0x814E)
#define GT911_REG_POINT1_X_ADDR    (0x814F)
#define GT911_REG_COMMAND          (0x8040)
#define GT911_REG_CONFIG_VERSION   (0x8047)
#define GT911_REG_CONFIG_CHECKSUM  (0x80FF)
#define GT911_REG_CONFIG_FRESH     (0x8100)
#define GT911_REG_FW_VER_HIGH      (0x8145)

/*Reg GT911_REG_READ_COORD_ADDR bit fields */
#define BUFFER_READY           (1<< 7)
#define NUM_TOUCH_POINTS_MASK  (0x0F)

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/

typedef enum
{
    TOUCH_EVENT_NONE,
    TOUCH_EVENT_DOWN,
    TOUCH_EVENT_HOLD,
    TOUCH_EVENT_MOVE,
    TOUCH_EVENT_UP
} touch_event_t;

/**********************************************************************************************************************
 External global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/
fsp_err_t enable_ts(i2c_master_ctrl_t * p_api_i2c_ctrl, external_irq_ctrl_t * p_api_irq_ctrl);
fsp_err_t init_ts(i2c_master_ctrl_t * p_api_ctrl);
bool_t display_ts_detected(void);

#endif /* TOUCH_GT911_H_ */
