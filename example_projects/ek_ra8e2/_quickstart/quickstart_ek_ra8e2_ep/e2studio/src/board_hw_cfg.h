/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : board_hw_cfg.h
 * Version      : 1.0
 * Description  : Board hardware helper functions and definitions
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 01.01.2025 1.00     First Release
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Includes   <System Includes> , "Project Includes"
 *********************************************************************************************************************/
#include "bsp_api.h"
#include "common_init.h"

/**********************************************************************************************************************
 Macro definitions
 *********************************************************************************************************************/

#ifndef BOARD_HW_CFG_H_
#define BOARD_HW_CFG_H_

#ifndef BOARD_INIT_H
#include "board_leds.h"
#endif

/* LEDs defined in the FSP Board Support file board_leds.c */
extern const bsp_leds_t g_bsp_leds;

#define OFF_CFG_                         (0)
#define ON_CFG_                          (1)

/* BLUE LED Configuration */
#define BLUE_CFG_      (BSP_LED_LED1)
/* GREEN LED Configuration */
#define GREEN_CFG_     (BSP_LED_LED2)
/* RED LED Configuration */
#define RED_CFG_       (BSP_LED_LED3)

/* TURN RED LED ON Macro */
#define TURN_RED_ON_CFG_    {R_BSP_PinAccessEnable();\
                            R_BSP_PinWrite(g_bsp_leds.p_leds[RED_CFG_], ON_CFG_);\
                            R_BSP_PinAccessDisable();}
/* TURN RED LED OFF Macro */
#define TURN_RED_OFF_CFG_   {R_BSP_PinAccessEnable();\
                            R_BSP_PinWrite(g_bsp_leds.p_leds[RED_CFG_], OFF_CFG_);\
                            R_BSP_PinAccessDisable();}
/* TURN GREEN LED ON Macro */
#define TURN_GREEN_ON_CFG_  {R_BSP_PinAccessEnable();\
                            R_BSP_PinWrite(g_bsp_leds.p_leds[GREEN_CFG_], ON_CFG_);\
                            R_BSP_PinAccessDisable();}
/* TURN GREEN LED OFF Macro */
#define TURN_GREEN_OFF_CFG_ {R_BSP_PinAccessEnable();\
                            R_BSP_PinWrite(g_bsp_leds.p_leds[GREEN_CFG_], OFF_CFG_); \
                            R_BSP_PinAccessDisable();}
/* TURN BLUE LED ON Macro */
#define TURN_BLUE_ON_CFG_   {R_BSP_PinAccessEnable();\
                            R_BSP_PinWrite(g_bsp_leds.p_leds[BLUE_CFG_], ON_CFG_);\
                            R_BSP_PinAccessDisable();}

/* TURN BLUE LED OFF Macro */
#define TURN_BLUE_OFF_CFG_  {R_BSP_PinAccessEnable(); \
                            R_BSP_PinWrite(g_bsp_leds.p_leds[BLUE_CFG_], OFF_CFG_);\
                            R_BSP_PinAccessDisable();}

/* TURN GREEN LED ON to represent system OK Macro */
#define SYSTEM_OK_CFG_      {TURN_GREEN_ON_CFG_; \
                            TURN_RED_OFF_CFG_;}

/* TURN RED LED ON to represent system ERROR Macro
 * Note GCC compile time __asm used */
#define SYSTEM_ERROR_CFG_   {TURN_GREEN_OFF_CFG_; \
                            TURN_RED_ON_CFG_; \
                            __asm("BKPT #0\n");}

/**********************************************************************************************************************
 Global Typedef definitions
 *********************************************************************************************************************/

/**********************************************************************************************************************
 External global variables
 *********************************************************************************************************************/

/**********************************************************************************************************************
 Exported global functions
 *********************************************************************************************************************/
extern void switch_ioport_cfg(ioport_cfg_t const * p_cfg);

#endif /* BOARD_HW_CFG_H_ */
