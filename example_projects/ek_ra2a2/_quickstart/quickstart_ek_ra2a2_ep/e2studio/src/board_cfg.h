/***********************************************************************************************************************
 * File Name    : board_cfg.h
 * Description  : Board specific/configuration data.
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2019 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef R_BOARD_CFG_H_
#define R_BOARD_CFG_H_


#include "hal_data.h"

#ifndef EK_2A2_BOARD_CFG_H_

#define OFF             (0U)
#define ON              (1U)

#ifdef BOARD_RA2A2_EK
#define BLUE            (BSP_LED_LED1)
#define GREEN           (BSP_LED_LED2)
#define RED             (BSP_LED_LED3)

#define TURN_RED_ON    R_BSP_PinAccessEnable(); R_BSP_PinWrite(g_bsp_leds.p_leds[RED], ON); R_BSP_PinAccessDisable();
#define TURN_RED_OFF   R_BSP_PinAccessEnable(); R_BSP_PinWrite(g_bsp_leds.p_leds[RED], OFF); R_BSP_PinAccessDisable();
#define TURN_GREEN_ON  R_BSP_PinAccessEnable(); R_BSP_PinWrite(g_bsp_leds.p_leds[GREEN], ON); R_BSP_PinAccessDisable();
#define TURN_GREEN_OFF R_BSP_PinAccessEnable(); R_BSP_PinWrite(g_bsp_leds.p_leds[GREEN], OFF); R_BSP_PinAccessDisable();

#define TURN_BLUE_ON   R_BSP_PinAccessEnable(); R_BSP_PinWrite(g_bsp_leds.p_leds[BLUE], ON); R_BSP_PinAccessDisable();
#define TURN_BLUE_OFF  R_BSP_PinAccessEnable(); R_BSP_PinWrite(g_bsp_leds.p_leds[BLUE], OFF); R_BSP_PinAccessDisable();

#define SYSTEM_OK       R_BSP_PinAccessEnable(); TURN_GREEN_ON; TURN_RED_OFF; R_BSP_PinAccessDisable();
#define SYSTEM_ERROR    R_BSP_PinAccessEnable(); TURN_GREEN_OFF; TURN_RED_ON; R_BSP_PinAccessDisable(); APP_ERR_TRAP(1);

#endif

extern bsp_leds_t g_bsp_leds;

#endif /* EK_2A2_BOARD_CFG_H_ */

#endif /* R_BOARD_CFG_H_ */
