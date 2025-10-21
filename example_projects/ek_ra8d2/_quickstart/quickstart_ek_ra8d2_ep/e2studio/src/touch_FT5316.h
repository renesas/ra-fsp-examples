/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : touch_FT5316.h
 * Version      : 1.0
 * Description  : Support for the GT111 Capacitive touch controller on a LCD panel.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 20.10.2025 1.00     First Release
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
#ifndef TOUCH_PANEL_THREAD_ENTRY_H_
#define TOUCH_PANEL_THREAD_ENTRY_H_

/* Touch panel */
#define FT5316_SLAVE_ADDR   (0x38)    // 7-bit address

#define TD_STATUS           (0x02)    // Frame remaining or number of events AND Number of touch points

#define ID_G_CIPHER         (0xA3)    // Chip vendor ID
#define ID_G_MODE           (0xA4)    // Interrupt status to host
#define ID_G_VENDOR_ID      (0xA8)    // CTPM Vendor ID

#define TOUCHn_XH           (0x03)    // MSB of the X coordinate of the nth touch point and the corresponding event flag
#define TOUCHn_XL           (0x04)    // LSB of the X coordinate of the nth touch point

#define TOUCHn_YH           (0x05)    // MSB of the Y coordinate of the nth touch point and the corresponding event flag
#define TOUCHn_YL           (0x06)    // LSB of the Y coordinate of the nth touch point


/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/
typedef struct touch_coords
{
    uint16_t x[5];
    uint16_t y[5];
} st_touch_coords_t;

typedef struct all_touch_data
{
    uint8_t num_touches;
    st_touch_coords_t touch_data;
} st_touch_data_t;

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
extern uint8_t g_ft5316_i2c_address;

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/
extern void touch_panel_thread_entry(void *pvParameters);
extern void irq19_tp_callback(external_irq_callback_args_t *p_args);
extern uint8_t read_ft5316_tp_status(void);
extern st_touch_data_t read_tp_details(uint8_t tp_detected);
extern void touch_panel_reset(void);
extern void touch_panel_configure(void);
extern fsp_err_t read_reg8(uint8_t reg_address, uint8_t *reg_val, i2c_master_ctrl_t *p_i2c_master_ctrl, uint8_t bytes);
extern fsp_err_t write_reg8(i2c_master_ctrl_t *p_i2c_master_ctrl, uint8_t reg_address, uint8_t value);

#endif /* TOUCH_PANEL_THREAD_ENTRY_H_ */
