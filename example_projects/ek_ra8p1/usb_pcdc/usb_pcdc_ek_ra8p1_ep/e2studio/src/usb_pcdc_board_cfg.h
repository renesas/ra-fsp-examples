/***********************************************************************************************************************
 * File Name    : usb_pcdc_board_cfg.h
 * Description  : Contains macros and variables used for the board specific configuration.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef USB_PCDC_BOARD_CFG_H_
#define USB_PCDC_BOARD_CFG_H_

#include "hal_data.h"

/* External variable */
extern bsp_leds_t g_bsp_leds;

/* Macro for the USB end point packet size */
#define USB_EP_PACKET_SIZE      (512U)

/* Macros for the LED state */
#define LED_OFF                 (0U)
#define LED_ON                  (1U)

#if defined (BOARD_RA8P1_EK)
/* Macros for the on-board LEDs */
#define LED_BLUE                (BSP_LED_LED1)
#define LED_GREEN               (BSP_LED_LED2)
#define LED_RED                 (BSP_LED_LED3)

/* Macro to turn on the red LED and turn off the other LEDs to indicate a fatal error */
#define TURN_RED_ON             R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[LED_RED], LED_ON);\
                                R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[LED_GREEN], LED_OFF);\
                                R_IOPORT_PinWrite(&g_ioport_ctrl, g_bsp_leds.p_leds[LED_BLUE], LED_OFF);
#endif /* Specific board */

/* Macros for converting the raw ADC temperature value to Celsius and Fahrenheit */
/* The conversion coefficients used in these formulas are board-specific.
 * Refer to the Temperature Sensor (TSN) section of the target device Hardware
 * Manual and adjust the coefficients as required */
#define ADCTEMP_AS_C(a)         ((((float)(a)) * (0.298394f)) - 275.751f)
#define ADCTEMP_AS_F(a)         ((((float)(a)) * (0.537109f)) - 464.352f)

/* Macros for the kit information */
#define KIT_NAME_MACRO          "EK-RA8P1"
#define KIT_PARTNUM_MACRO       "RTK7EKA8P1S00001BE"
#define KIT_LANDING_URL         "renesas.com/ek-ra8p1"
#define PRODUCT_INFO_URL        "renesas.com/ra"
#define PRODUCT_SUPPORT_URL     "renesas.com/ra/forum"
#define RENESAS_SUPPORT_URL     "renesas.com/support"

#endif /* USB_PCDC_BOARD_CFG_H_ */
