/**********************************************************************************************************************
 * File Name    : board_cfg.h
 * Description  : Board specific/configuration data.
 *********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2021 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "bsp_api.h"
#include "hal_data.h"
#include "common_data.h"

#ifndef BOARD_CFG_
#define BOARD_CFG_

#define USB_EP_PACKET_SIZE                        (512U)
#define OFF                                       (0U)
#define ON                                        (1U)

#ifdef BOARD_RA4M2_EK

#define BLUE                                      (BSP_LED_LED1)
#define GREEN                                     (BSP_LED_LED2)
#define RED                                       (BSP_LED_LED3)

#define TURN_RED_ON                               R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[RED], ON);
#define TURN_RED_OFF                              R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[RED], OFF);
#define TURN_GREEN_ON                             R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[GREEN], ON);
#define TURN_GREEN_OFF                            R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[GREEN], OFF);
#define TURN_BLUE_ON                              R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[BLUE], ON);
#define TURN_BLUE_OFF                             R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[BLUE], OFF);

#define SYSTEM_OK                                 TURN_GREEN_ON; TURN_RED_OFF;
#define SYSTEM_ERROR                              TURN_RED_ON; TURN_GREEN_OFF; APP_ERR_TRAP(1);

#endif /* BOARD_RA4M2_EK */

#define KIT_NAME_MACRO                            "EK-RA4M2"
#define KIT_LANDING_URL                           "renesas.com/ra/ek-ra4m2"
#define PRODUCT_INFO_URL                          "renesas.com/ra"
#define PRODUCT_SUPPORT_URL                       "renesas.com/ra/forum"
#define RENESAS_SUPPORT_URL                       "renesas.com/support"

extern bsp_leds_t g_bsp_leds;

#endif /* BOARD_CFG_ */

