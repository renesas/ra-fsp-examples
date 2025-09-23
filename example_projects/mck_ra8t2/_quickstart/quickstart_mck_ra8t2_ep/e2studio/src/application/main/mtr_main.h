/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name   : mtr_main.h
* Description : Definitions for the application layer
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version
*         : 19.01.2024 1.00
***********************************************************************************************************************/

/* Guard against multiple inclusion */
#ifndef MTR_MAIN_H
#define MTR_MAIN_H

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include <stdint.h>

/***********************************************************************************************************************
* Macro definitions
***********************************************************************************************************************/
#define SW_ON                    (0)                            /* Active level of SW */
#define SW_OFF                   (1)                            /* Inactive level of SW */
#define SW1_ON                   (1)                            /* Active level of SW */
#define SW1_OFF                  (0)                            /* Inactive level of SW */
#define SW2_ON                   (0)                            /* Active level of SW */
#define SW2_OFF                  (1)                            /* Inactive level of SW */
#define CHATTERING_CNT           (10)                           /* Counts to remove chattering */
#define MTR_CW                   (0)                            /* Rotation clockwise */
#define MTR_CCW                  (1)                            /* Rotation counterclockwise */
#define WAIT_STOP_COUNT          (600)
#define MTR_LED_ON               (BSP_IO_LEVEL_LOW)             /* Active level of LED */
#define MTR_LED_OFF              (BSP_IO_LEVEL_HIGH)            /* Inactive level of LED */
#define ICS_UI                   (0)                            /* ICS (Analyzer) */
#define BOARD_UI                 (1)                            /* Board */
#define LOOP_SPEED               (0)                            /* Speed control */
#define LOOP_POSITION            (1)                            /* Position control */
#define MTR_MAX_SPEED_RPM        (2400)                         /* Maximum rotational speed [rpm] */
#define STOP_RPM                 (400)                          /* Judgement speed to stop at VR speed reference */
#define MTR_AD12BIT_DATA         (4095.0f)                      /* A/D 12Bit data */
#define VR1_SCALING              ((MTR_MAX_SPEED_RPM + 100) / (MTR_AD12BIT_DATA * 0.5f))
                                                                /* Scaling factor for speed reference (A/D) */
                                                                /* (max speed + margin) / (A/D(12 bits)/2) */
#define VR1_SCALING_POS          ((180 + 10) / (MTR_AD12BIT_DATA/2 + 1))
                                                                /* Scaling factor for position reference (A/D) */
                                                                /* (180degree + margin) / (A/D(12 bits)/2 + 1) */
#define VR1_180                  (180.0f)
#define VR1_POSITION_DEAD_BAND   (2)                            /* position dead-band [rad] */
#define ADJUST_OFFSET            (0x7FF)                        /* Adjusting offset for reference */
#define MTR_FLG_CLR              (0)                            /* For flag clear */
#define MTR_FLG_SET              (1)                            /* For flag set */

/* Defines the UI used as default UI (BOARD_UI/ICS_UI)*/
#define CONFIG_DEFAULT_UI        (BOARD_UI)
// #define CONFIG_DEFAULT_UI        (ICS_UI)

/* Defines the control mode used as default (LOOP_POSITION/LOOP_SPEED)*/
// #define CONFIG_LOOP_MODE         (LOOP_POSITION)
#define CONFIG_LOOP_MODE         (LOOP_SPEED)

#define MTR_ADCH_VR1             (15)                            /* A/D channel of vr1 */

#define MTR_PORT_SW1             (BSP_IO_PORT_10_PIN_00)        /* Input port of SW1 */
#define MTR_PORT_SW2             (BSP_IO_PORT_10_PIN_07)        /* Input port of SW2 */

#define MTR_PORT_LED1            (BSP_IO_PORT_06_PIN_14)        /* Output port of LED1 */
#define MTR_PORT_LED2            (BSP_IO_PORT_10_PIN_15)        /* Output port of LED2 */
#define MTR_PORT_LED3            (BSP_IO_PORT_10_PIN_04)        /* Output port of LED3 */

/***********************************************************************************************************************
* global functions
***********************************************************************************************************************/
/***********************************************************************************************************************
* Function Name : mtr_init
* Description   : Initialization for Motor Control
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void mtr_init( void );

/***********************************************************************************************************************
* Function Name : mtr_main
* Description   : Initialization and main routine
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void mtr_main(void);

#endif /* MTR_MAIN_H */
