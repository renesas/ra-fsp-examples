/***********************************************************************************************************************
 * File Name    : usr_hal.c
 * Description  : Contains data structures and functions used for the HAL init and its utilities
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright [2023] Renesas Electronics Corporation and/or its licensors. All Rights Reserved.
 *
 * The contents of this file (the "contents") are proprietary and confidential to Renesas Electronics Corporation
 * and/or its licensors ("Renesas") and subject to statutory and contractual protections.
 *
 * This file is subject to a Renesas FSP license agreement. Unless otherwise agreed in an FSP license agreement with
 * Renesas: 1) you may not use, copy, modify, distribute, display, or perform the contents; 2) you may not use any name
 * or mark of Renesas for advertising or publicity purposes or in connection with your use of the contents; 3) RENESAS
 * MAKES NO WARRANTY OR REPRESENTATIONS ABOUT THE SUITABILITY OF THE CONTENTS FOR ANY PURPOSE; THE CONTENTS ARE PROVIDED
 * "AS IS" WITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, AND NON-INFRINGEMENT; AND 4) RENESAS SHALL NOT BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, OR
 * CONSEQUENTIAL DAMAGES, INCLUDING DAMAGES RESULTING FROM LOSS OF USE, DATA, OR PROJECTS, WHETHER IN AN ACTION OF
 * CONTRACT OR TORT, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THE CONTENTS. Third-party contents
 * included in this file may be subject to different terms.
 **********************************************************************************************************************/
#include "common_utils.h"
#include "usr_hal.h"
#include "usr_app.h"
#include "user_choice.h"

#define SENSOR_QTY_PUSH  (5)

#define SENSOR_DATA_DISPLAY_LIMIT (5)

uint8_t g_incrementor = ID_HS3001;
uint8_t user_time = 0;
uint8_t show_sensor_data = ID_DISPLAY_HS3001;

/***********************************************************************************************************************
 * @brief       This function initializes the hal peripherals for the Application. The Peripherals used are ADC -  
 *              measuring the MCU temperature. Timer0 - for 30 second peridic reading of the ADC via ELC. External IRQ 
 *              for User Button handling. Timer1 for 1 second periodic toggling of Application Status and Network status. 
 * @param[IN]   void                       
 * @retval      FSP_SUCCESS                Upon successful init.
 * @retval      Any Other Error code       If hal init fails.
 **********************************************************************************************************************/
fsp_err_t usr_hal_init(void)
{
    fsp_err_t status = FSP_SUCCESS;

    status = R_GPT_Open (&gpt_ctrl, &gpt_cfg);
    if (FSP_SUCCESS != status)
    {
        IotLogErr("** R_GPT_Open failed ** %u \r\n",status);
        return status;
    }

    return status;
}

/*******************************************************************************************************************//**
 * @brief       Timer call back after a interval of time set by user
 * @param[in]   p_args
 * @retval
 * @retval
 ***********************************************************************************************************************/
void g_gpt_timer_cb(timer_callback_args_t *p_args)
{
    if ((NULL != p_args) && (TIMER_EVENT_CYCLE_END == p_args->event))
    {
        mcu_temp_msg_t ltemp_data = {RESET_VALUE};
        uint16_t val = RESET_VALUE;

        user_time++;
        if (user_time > USER_TIMER_DATA_POST_INTERVAL)
        {
            user_time=RESET_VALUE;

            if(g_incrementor>ID_OB1203)
            {
                g_incrementor=ID_HS3001;
            }
            ltemp_data.id = ( g_incrementor++);
            ltemp_data.adc_data.adc_value = val;
            tx_queue_send(&g_topic_queue, &ltemp_data, TX_NO_WAIT);
        }
        else
        {
            if(show_sensor_data>ID_DISPLAY_OB1203)
            {
                show_sensor_data=ID_DISPLAY_HS3001;
            }
            else
            {
                ;
            }
            ltemp_data.id = show_sensor_data++;
            ltemp_data.adc_data.adc_value = val;
            tx_queue_send(&g_topic_queue, &ltemp_data, TX_NO_WAIT);
        }
    }
}
/***********************************************************************************************************************
 * @brief       This function turns ON/OFF the LED based on the type(Colored LED) and its requested action.
 *              
 * @param[IN]   e_led_type    Type of LED, (GREEN, BLUE, RED).
 * @param[IN]   lled_state    Requested Action for the LED, (LED_OFF, LED_ON).
 * @retval      void        
 **********************************************************************************************************************/
void led_on_off(e_led_type_t ltype, e_led_state_t lled_state)
{
    /* Get pin to toggle */
    uint32_t pin = RESET_VALUE;
    /* Holds level to set for pins */
    bsp_io_level_t pin_level = (bsp_io_level_t)LED_OFF;

    /* LED type structure */
    bsp_leds_t leds = g_bsp_leds;

    R_BSP_PinAccessEnable ();

    if(((LED_ON == lled_state) || (LED_OFF == lled_state)) && 
      ((LED_GREEN == ltype) || (LED_BLUE == ltype) || (LED_RED == ltype)))
    {
        pin_level = (bsp_io_level_t)lled_state;
        pin = leds.p_leds[ltype];

        /* Write to this pin */
        R_BSP_PinWrite ((bsp_io_port_pin_t) pin, pin_level);
    }

    R_BSP_PinAccessDisable ();
}

/***********************************************************************************************************************
 * @brief       This function toggles(Turns ON if it was OFF or vice versa) the LED, based on the type of LED(Colored LED). 
 The periodic toggle is based on Application code handling or any other mechanism(such as Timer).
 *              
 * @param[IN]   e_led_type    Type of LED, (GREEN, BLUE, RED).
 * @retval      void        
 **********************************************************************************************************************/
void led_toggle(e_led_type_t ltype)
{
	/* Get pin to toggle */
	uint32_t pin = RESET_VALUE;
	/* Holds level to set for pins */
    static bsp_io_level_t pin_level[3] =
    { (bsp_io_level_t)LED_OFF };

    /* LED type structure */
    bsp_leds_t leds = g_bsp_leds;

    R_BSP_PinAccessEnable ();

    if (((LED_GREEN == ltype) || (LED_BLUE == ltype) || (LED_RED == ltype)))
    {
        pin = leds.p_leds[ltype];

        /* Write to this pin */
        R_BSP_PinWrite ((bsp_io_port_pin_t) pin, pin_level[ltype]);

        if (LED_OFF == (e_led_state_t) pin_level[ltype])
        {
            pin_level[ltype] = (bsp_io_level_t)LED_ON;
        }
        else
        {
            pin_level[ltype] = (bsp_io_level_t)LED_OFF;
        }
    }

    R_BSP_PinAccessDisable ();
}

