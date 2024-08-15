/***********************************************************************************************************************
 * File Name    : board_cfg.h
 * Description  : Board specific/configuration data.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "hal_data.h"

extern bsp_leds_t g_bsp_leds;

#define USB_EP_PACKET_SIZE                        (512U)
#define OFF                                       (1U)
#define ON                                        (0U)

#ifdef BOARD_RA8T1_MCK
#define LED_1                                      (BSP_LED_LED1)
#define LED_2                                      (BSP_LED_LED2)
#define LED_3                                      (BSP_LED_LED3)
#define LED_4                                      (BSP_LED_LED4)

#define TURN_RED_ON                               R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[LED_1], ON); \
		                                          R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[LED_2], OFF);\
		                                          R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[LED_3], OFF);\
		                                          R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[LED_4], OFF);
#endif

#define KIT_NAME_MACRO                            "MCK-RA8T1"
#define KIT_PARTNUM_MACRO                         "RTK0EMA5K0C00000BJ"
#define KIT_LANDING_URL                           "renesas.com/ra/mck-ra8t1"
#define PRODUCT_INFO_URL                          "renesas.com/ra"
#define PRODUCT_SUPPORT_URL                       "renesas.com/ra/forum"
#define RENESAS_SUPPORT_URL                       "renesas.com/support"
