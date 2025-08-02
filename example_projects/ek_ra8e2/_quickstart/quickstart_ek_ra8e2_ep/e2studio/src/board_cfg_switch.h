/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : board_cfg_switch.h
 * Version      : 1.0
 * Description  : Handlier for I/F to sw4, greenpak and associated i2c channel used to communicate with devices.
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 20.05.2025 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "hal_data.h"
#include "r_typedefs.h"

 /**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#ifndef BOARD_CFG_SWITCH_H_
#define BOARD_CFG_SWITCH_H_

#define SWITCH_EXPECTED_VALUE              (0xF0)

/* switch control via port expander */
#define SW4_NEG_LOGIC (0) /* Uses a pull UP on the board */
#define SW4_POS_LOGIC (1) /* Uses a pull DOWN on the board */

#define SW4_1 (1)
#define SW4_2 (2)
#define SW4_3 (3)
#define SW4_4 (4)
#define SW4_5 (5)
#define SW4_6 (6)
#define SW4_7 (7)
#define SW4_8 (8)

#define SW4_1_LOGIC (SW4_POS_LOGIC)
#define SW4_2_LOGIC (SW4_POS_LOGIC)
#define SW4_3_LOGIC (SW4_NEG_LOGIC)
#define SW4_4_LOGIC (SW4_NEG_LOGIC)
#define SW4_5_LOGIC (SW4_NEG_LOGIC)
#define SW4_6_LOGIC (SW4_NEG_LOGIC)
#define SW4_7_LOGIC (SW4_NEG_LOGIC)
#define SW4_8_LOGIC (SW4_NEG_LOGIC)

#define SW4_PMOD1_MD_SEL0     (SW4_1)
#define SW4_PMOD1_MD_SEL1     (SW4_2)
#define SW4_OSPI_OE           (SW4_3)
#define SW4_ARDUINO_OE        (SW4_4)
#define SW4_I3C_SEL           (SW4_5)
#define SW4_MIPI_SEL          (SW4_6)
#define SW4_USBFS_ROLE_SW     (SW4_7)
#define SW4_USBHS_ROLE_SW     (SW4_8)

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 External global variables
 *********************************************************************************************************************/
extern char_t g_switch_warning_str[256];

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/
extern fsp_err_t board_cfg_switch_init (void);
extern fsp_err_t board_cfg_switch_func_read (int sw_pin, bool_t *state);
extern fsp_err_t board_cfg_switch_funct_on (int sw_pin);
extern fsp_err_t board_cfg_switch_funct_off (int sw_pin);
extern void switch_set_status_msg(void);

#endif /* BOARD_CFG_SWITCH_H_ */
