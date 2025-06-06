/*
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : board_info.h
 * Version      : 1.00
 * Description  : Header file for board information of EK-RA2L2 Quick Start Example Project
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 01.05.2025 1.00     First Release
 *********************************************************************************************************************/
#ifndef R_BOARD_INFO_H_
#define R_BOARD_INFO_H_

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "hal_data.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/
#define USB_EP_PACKET_SIZE              (128U)
#define PWR_V                           (3.3f)             /** Board power in voltage */
#define ADC_PRECISION                   (12)
#define T_J                             (140.0f)           /** Permissible junction temperature in Celsius */

#define OFF                             (0U)               /** LED OFF */
#define ON                              (1U)               /** LED ON */

#define BLUE                            (BSP_LED_LED1)
#define GREEN                           (BSP_LED_LED2)
#define RED                             (BSP_LED_LED3)

#define TURN_RED_ON                     do {\
    R_BSP_PinAccessEnable();\
    R_BSP_PinWrite(g_bsp_leds.p_leds[RED], ON);\
    R_BSP_PinAccessDisable();\
    } while (0)
#define TURN_RED_OFF                    do {\
    R_BSP_PinAccessEnable();\
    R_BSP_PinWrite(g_bsp_leds.p_leds[RED], OFF);\
    R_BSP_PinAccessDisable();\
    } while (0)
#define TURN_GREEN_ON                   do {\
    R_BSP_PinAccessEnable();\
    R_BSP_PinWrite(g_bsp_leds.p_leds[GREEN], ON);\
    R_BSP_PinAccessDisable();\
    } while (0)
#define TURN_GREEN_OFF                  do {\
    R_BSP_PinAccessEnable();\
    R_BSP_PinWrite(g_bsp_leds.p_leds[GREEN], OFF);\
    R_BSP_PinAccessDisable();\
    } while (0)
#define TURN_BLUE_ON                    do {\
    R_BSP_PinAccessEnable();\
    R_BSP_PinWrite(g_bsp_leds.p_leds[BLUE], ON);\
    R_BSP_PinAccessDisable();\
    } while (0)
#define TURN_BLUE_OFF                   do {\
    R_BSP_PinAccessEnable();\
    R_BSP_PinWrite(g_bsp_leds.p_leds[BLUE], OFF);\
    R_BSP_PinAccessDisable();\
    } while (0)

#define SYSTEM_OK                       do {\
    R_BSP_PinAccessEnable();\
    TURN_GREEN_ON;\
    TURN_RED_OFF;\
    R_BSP_PinAccessDisable();\
    } while (0)
#define SYSTEM_ERROR                    do {\
    R_BSP_PinAccessEnable();\
    TURN_GREEN_OFF;\
    TURN_RED_ON;\
    R_BSP_PinAccessDisable();\
    APP_ERR_TRAP(1);\
    } while (0)

#define KIT_NAME                        "EK-RA2L2"
#define KIT_PARTNUM                     "RTK7EKA2L2S00001BJ"
#define DEVICE_NUMBER                   "R7FA2L2094CFM"
#define KIT_LANDING_URL                 "renesas.com/ek-ra2l2"
#define KIT_RA_URL                      "renesas.com/ra/kits"
#define PRODUCT_INFO_URL                "renesas.com/ra"
#define PRODUCT_SUPPORT_URL             "renesas.com/ra/forum"
#define RENESAS_SUPPORT_URL             "renesas.com/support"

/******************************************************************************
Imported global variables
******************************************************************************/
extern bsp_leds_t g_bsp_leds;

#endif /* R_BOARD_INFO_H_ */
