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


#ifdef BOARD_RA4M1_EK
#define RED                                       (BSP_LED_LED1)

#define TURN_RED_ON                               R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[RED], ON); 

#endif

#define KIT_NAME_MACRO                            "EK-RA4M1"
#define KIT_PARTNUM_MACRO                         "RTK7EKA4M1S00001BU"
#define KIT_LANDING_URL                           "renesas.com/ra/ek-ra4m1"
#define PRODUCT_INFO_URL                          "renesas.com/ra"
#define PRODUCT_SUPPORT_URL                       "renesas.com/ra/forum"
#define RENESAS_SUPPORT_URL                       "renesas.com/support"
