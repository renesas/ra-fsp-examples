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
#define OFF                                       (0U)
#define ON                                        (1U)

#ifdef BOARD_RA6E2_EK
#define BLUE                                      (BSP_LED_LED1)
#define GREEN                                     (BSP_LED_LED2)
#define RED                                       (BSP_LED_LED3)

#define TURN_RED_ON                               R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[RED], ON); \
		                                          R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[GREEN], OFF);\
		                                          R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[BLUE], OFF);
#endif

#define KIT_NAME_MACRO                            "EK-RA6E2"
#define KIT_PARTNUM_MACRO                         "RTK7EKA6E2S00001BE"
#define KIT_LANDING_URL                           "renesas.com/ra/ek-ra6e2"
#define PRODUCT_INFO_URL                          "renesas.com/ra"
#define PRODUCT_SUPPORT_URL                       "renesas.com/ra/forum"
#define RENESAS_SUPPORT_URL                       "renesas.com/support"
