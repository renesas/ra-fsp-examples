/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
* this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name   : mtr_main.c
* Description : The main function and the processes of motor control application layer
***********************************************************************************************************************/
/**********************************************************************************************************************
* History : DD.MM.YYYY Version
*         : 30.04.2023 1.00
***********************************************************************************************************************/

/***********************************************************************************************************************
* Includes <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include <stdint.h>
#include "mtr_main.h"
#include "hal_data.h"

/***********************************************************************************************************************
* Global variables
***********************************************************************************************************************/
float       g_f4_speed_ref = 0.0F;
uint8_t     g_u1_motor_status;            /* Motor status */
uint8_t     com_u1_mode_system;           /* System mode */
uint8_t     g_u1_mode_system;             /* System mode */
uint16_t    g_u2_max_speed_rpm;
uint8_t     g_u1_stop_req;
uint16_t    g_u2_chk_error;
uint16_t    g_u2_vr1_ad;
uint8_t     g_u1_reset_req;             /* Reset request flag */
uint8_t     g_u1_sw_cnt;                /* Counter to remove chattering */

/***********************************************************************************************************************
* Private functions
***********************************************************************************************************************/
static void     motor_fsp_init (void);
static void     mtr_board_led_control (uint8_t u1_motor_status);
static uint8_t  mtr_remove_sw_chattering (uint8_t u1_sw, uint8_t u1_on_off);
static void     board_ui (void);           /* Board user interface */
static void     software_init (void);      /* Software initialize */
static uint16_t get_vr1 (void);
static uint8_t  get_sw1 (void);
static uint8_t  get_sw2 (void);
static void     led1_on (void);
static void     led2_on (void);
static void     led3_on (void);
static void     led1_off (void);
static void     led2_off (void);
static void     led3_off (void);

/***********************************************************************************************************************
* Function Name : mtr_init
* Description   : Initialization for Motor Control
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void mtr_init(void)
{
    motor_fsp_init();
    software_init();                              /* Initialize private global variables */

    /* Execute reset event */
    g_motor_sensorless0.p_api->reset(g_motor_sensorless0.p_ctrl);
} /* End of function mtr_init() */

/***********************************************************************************************************************
* Function Name : mtr_main
* Description   : Main routine for Motor Control
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
void mtr_main(void)
{
    board_ui();                           /* User interface control routine */
} /* End of function mtr_main() */

/***********************************************************************************************************************
* Function Name : board_ui
* Description   : User interface using board UI
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
static void board_ui(void)
{
    uint8_t u1_temp_sw_signal;

    /*======================*/
    /*     Mode control     */
    /*======================*/

    /* Get status of motor control system */
    g_motor_sensorless0.p_api->statusGet(g_motor_sensorless0.p_ctrl, &g_u1_motor_status);
    switch (g_u1_motor_status)
    {
        case MOTOR_SENSORLESS_CTRL_STOP:
            u1_temp_sw_signal = get_sw1();

            /* Check SW1 */
            if ((MTR_FLG_SET == mtr_remove_sw_chattering(u1_temp_sw_signal, SW1_ON)) && (MTR_FLG_SET != g_u1_stop_req))
            {
                g_motor_sensorless0.p_api->run(g_motor_sensorless0.p_ctrl);
            }
            break;

        case MOTOR_SENSORLESS_CTRL_RUN:
            u1_temp_sw_signal = get_sw1();

            /* Check SW1 */
            if ((MTR_FLG_SET == mtr_remove_sw_chattering(u1_temp_sw_signal, SW1_OFF)) || (MTR_FLG_CLR != g_u1_stop_req))
            {
                g_motor_sensorless0.p_api->stop(g_motor_sensorless0.p_ctrl);
            }
            break;

        case MOTOR_SENSORLESS_CTRL_ERROR:

            /* check SW2 & reset request flag */
            u1_temp_sw_signal = get_sw2();
            if ((SW_OFF == g_u1_reset_req) && (MTR_FLG_SET == mtr_remove_sw_chattering(u1_temp_sw_signal, SW2_ON)))
            {
                g_u1_reset_req = SW_ON;
            }
            else if ((SW_ON == g_u1_reset_req) && (MTR_FLG_SET == mtr_remove_sw_chattering(u1_temp_sw_signal, SW2_OFF)))
            {
                g_u1_reset_req = SW_OFF;
                g_motor_sensorless0.p_api->reset(g_motor_sensorless0.p_ctrl);
            }
            else
            {
                /* Do nothing */
            }
            break;

        default: /* Do nothing */
            break;
    }

    /*=============================*/
    /*      Set speed reference    */
    /*=============================*/
    g_f4_speed_ref = ((float)g_u2_vr1_ad - ADJUST_OFFSET) * VR1_SCALING; /* Read speed reference from VR1 */
    g_motor_sensorless0.p_api->speedSet(g_motor_sensorless0.p_ctrl, g_f4_speed_ref);

    if ((g_f4_speed_ref > (-STOP_RPM)) && (g_f4_speed_ref < STOP_RPM)) /* Check SW1 */
    {
        g_u1_stop_req = MTR_FLG_SET;
    }
    else
    {
        g_u1_stop_req = MTR_FLG_CLR;
    }

    /***** LED control *****/
    mtr_board_led_control(g_u1_motor_status);
} /* End of function board_ui */

/***********************************************************************************************************************
* Function Name : software_init
* Description   : Initialize private global variables
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
static void software_init(void)
{
    g_u1_motor_status            = MOTOR_SENSORLESS_CTRL_STOP;
    g_u2_max_speed_rpm           = MTR_MAX_SPEED_RPM;
    g_u1_mode_system             = MOTOR_SENSORLESS_CTRL_EVENT_STOP;
    g_u1_reset_req               = SW_OFF;
} /* End of function software_init */

/***********************************************************************************************************************
* Function Name : g_poe_overcurrent
* Description   : POEG2 Interrupt callback function
* Arguments     : p_args - Callback argument
* Return Value  : None
***********************************************************************************************************************/
void g_poe_overcurrent(poeg_callback_args_t *p_args)
{
    if (NULL != p_args)
    {
        R_POEG_Reset(g_poeg0.p_ctrl);
        g_motor_sensorless0.p_api->errorSet(g_motor_sensorless0.p_ctrl, MOTOR_ERROR_OVER_CURRENT_HW);
        g_u2_chk_error |= MOTOR_ERROR_OVER_CURRENT_HW;
    }
} /* End of function g_poe_overcurrent */

/***********************************************************************************************************************
* Function Name : motor_fsp_init
* Description   : Initialize Motor FSP modules
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
static void motor_fsp_init(void)
{
    g_motor_sensorless0.p_api->open(g_motor_sensorless0.p_ctrl, g_motor_sensorless0.p_cfg);
    R_POEG_Open(g_poeg0.p_ctrl, g_poeg0.p_cfg);
    g_elc.p_api->open(g_elc.p_ctrl, g_elc.p_cfg);
    g_elc.p_api->enable(g_elc.p_ctrl);
} /* End of function motor_fsp_init */

/***********************************************************************************************************************
* Function Name : mtr_callback_event
* Description   : Initialize Motor FSP modules
* Arguments     : p_args - Callback argument
* Return Value  : None
***********************************************************************************************************************/
void mtr_callback_event(motor_callback_args_t * p_args)
{
    switch (p_args->event)
    {
        case MOTOR_CALLBACK_EVENT_SPEED_FORWARD:
            break;

        case MOTOR_CALLBACK_EVENT_SPEED_BACKWARD:
        {
            g_u2_vr1_ad = get_vr1();
            break;
        }

        case MOTOR_CALLBACK_EVENT_CURRENT_FORWARD:
        {
            if (MOTOR_SENSORLESS_CTRL_ERROR != g_u1_motor_status)
            {
                g_motor_sensorless0.p_api->errorCheck(g_motor_sensorless0.p_ctrl, &g_u2_chk_error);
            }
            break;
        }

        case MOTOR_CALLBACK_EVENT_CURRENT_BACKWARD:
            break;

        default:
            break;
    }
} /* End of function mtr_callback_event */

/***********************************************************************************************************************
* Function Name : mtr_board_led_control
* Description   : Set LED pattern depend on motor status
* Arguments     : u1_motor_status - Motor control status
* Return Value  : None
***********************************************************************************************************************/
static void mtr_board_led_control(uint8_t u1_motor_status)
{
    /***** LED control *****/
    if (MOTOR_SENSORLESS_CTRL_STOP == u1_motor_status)         /* Check motor status */
    {
        led1_off();                                     /* LED1 off */
        led2_off();                                     /* LED2 off */
        led3_off();                                     /* LED3 off */
    }
    else if (MOTOR_SENSORLESS_CTRL_RUN == u1_motor_status)      /* Check motor status */
    {
        led1_on();                                      /* LED1 on */
        led2_off();                                     /* LED2 off */
    }
    else if (MOTOR_SENSORLESS_CTRL_ERROR == u1_motor_status)       /* Check motor status */
    {
        led1_off();                                     /* LED1 off */
        led2_on();                                      /* LED2 on */
        led3_off();                                     /* LED3 off */
    }
    else
    {
        led1_on();                                      /* LED1 on */
        led2_on();                                      /* LED2 on */
        led3_on();                                      /* LED3 on */
    }
} /* End of function mtr_board_led_control */

/***********************************************************************************************************************
* Function Name : mtr_remove_sw_chattering
* Description   : Get switch status with removing chattering
* Arguments     : u1_sw - Board interface switch signal
*                 u1_on_off - Detected status (ON/OFF)
* Return Value  : u1_remove_chattering_flg - Detection result
***********************************************************************************************************************/
static uint8_t mtr_remove_sw_chattering(uint8_t u1_sw, uint8_t u1_on_off)
{
    uint8_t u1_remove_chattering_flg;

    u1_remove_chattering_flg = 0;
    if (u1_on_off == u1_sw)
    {
        g_u1_sw_cnt++;
        if (CHATTERING_CNT < g_u1_sw_cnt)
        {
            u1_remove_chattering_flg = MTR_FLG_SET;
            g_u1_sw_cnt = 0;
        }
    }
    else
    {
        g_u1_sw_cnt = 0;
    }

    return (u1_remove_chattering_flg);
} /* End of function mtr_remove_sw_chattering */

/***********************************************************************************************************************
* Function Name : get_vr1
* Description   : Get A/D converted value of VR1
* Arguments     : None
* Return Value  : A/D converted value of VR1
***********************************************************************************************************************/
static uint16_t get_vr1(void)
{
    uint16_t ad_data;

    g_adc0.p_api->read(g_adc0.p_ctrl, MTR_ADCH_VR1, &ad_data);

    return (ad_data);
} /* End of function get_vr1 */

/***********************************************************************************************************************
* Function Name : get_sw1
* Description   : Get state of SW1
* Arguments     : None
* Return Value  : State of SW1
***********************************************************************************************************************/
static uint8_t get_sw1(void)
{
    uint8_t tmp_port;

    R_IOPORT_PinRead(&g_ioport_ctrl, MTR_PORT_SW1, &tmp_port);

    return (tmp_port);
} /* End of function get_sw1 */

/***********************************************************************************************************************
* Function Name : get_sw2
* Description   : Get state of SW2
* Arguments     : None
* Return Value  : State of SW2
***********************************************************************************************************************/
static uint8_t get_sw2(void)
{
    uint8_t tmp_port;

    R_IOPORT_PinRead(&g_ioport_ctrl, MTR_PORT_SW2, &tmp_port);

    return (tmp_port);
} /* End of function get_sw2 */

/***********************************************************************************************************************
* Function Name : led1_on
* Description   : Turn on LED1
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
static void led1_on(void)
{
    R_IOPORT_PinWrite(&g_ioport_ctrl, MTR_PORT_LED1, MTR_LED_ON);
} /* End of function led1_on */

/***********************************************************************************************************************
* Function Name : led2_on
* Description   : Turn on LED2
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
static void led2_on(void)
{
    R_IOPORT_PinWrite(&g_ioport_ctrl, MTR_PORT_LED2, MTR_LED_ON);
} /* End of function led2_on */

/***********************************************************************************************************************
* Function Name : led3_on
* Description   : Turn on LED3
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
static void led3_on(void)
{
    // LED3 is not assigned on MCB-RA6T3/RA4T1.
    // Do nothing.
} /* End of function led3_on */

/***********************************************************************************************************************
* Function Name : led1_off
* Description   : Turn off LED1
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
static void led1_off(void)
{
    R_IOPORT_PinWrite(&g_ioport_ctrl, MTR_PORT_LED1, MTR_LED_OFF);
} /* End of function led1_off */

/***********************************************************************************************************************
* Function Name : led2_off
* Description   : Turn off LED2
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
static void led2_off(void)
{
    R_IOPORT_PinWrite(&g_ioport_ctrl, MTR_PORT_LED2, MTR_LED_OFF);
} /* End of function led2_off */

/***********************************************************************************************************************
* Function Name : led3_off
* Description   : Turn off LED3
* Arguments     : None
* Return Value  : None
***********************************************************************************************************************/
static void led3_off(void)
{
    // LED3 is not assigned on MCB-RA6T3/RA4T1.
    // Do nothing.
} /* End of function led3_off */
