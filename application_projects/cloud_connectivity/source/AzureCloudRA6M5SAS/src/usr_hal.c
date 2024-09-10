/***********************************************************************************************************************
 * File Name    : usr_hal.c
 * Description  : Contains data structures and functions used for the HAL init and its utilities
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/ 
#include "common_utils.h"
#include "usr_hal.h"
#include "usr_app.h"


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

    status = R_ADC_Open (&adc_ctrl, &adc_cfg);
    if (FSP_SUCCESS != status)
    {
        IotLogErr("** R_ADC_Open failed ** %u \r\n",status);
        return status;
    }

    status = R_ADC_ScanCfg (&adc_ctrl, &adc_channel_cfg);
    if (FSP_SUCCESS != status)
    {
        IotLogErr("** R_ADC_ScanCfg failed ** %u \r\n",status);
        return status;
    }

    status = R_ADC_ScanStart (&adc_ctrl);
    if (FSP_SUCCESS != status)
    {
        IotLogErr("** R_ADC_ScanStart failed ** %u \r\n",status);
        return status;
    }

    status = R_ICU_ExternalIrqOpen (&pushButtonS1_ctrl, &pushButtonS1_cfg);
    if (FSP_SUCCESS != status)
    {
        IotLogErr("** R_ICU_ExternalIrqOpen (S1) failed ** %u \r\n",status);
        return status;
    }

    status = R_ICU_ExternalIrqEnable (&pushButtonS1_ctrl);
    if (FSP_SUCCESS != status)
    {
        IotLogErr("** R_ICU_ExternalIrqEnable (S1) failed ** %u \r\n",status);
        return status;
    }

    status = R_ICU_ExternalIrqOpen (&pushButtonS2_ctrl, &pushButtonS2_cfg);
    if (FSP_SUCCESS != status)
    {
        IotLog("** R_ICU_ExternalIrqOpen (S2) failed ** %u \r\n",status);
        return status;
    }

    status = R_ICU_ExternalIrqEnable (&pushButtonS2_ctrl);
    if (FSP_SUCCESS != status)
    {
        IotLog("** R_ICU_ExternalIrqEnable (S2) failed ** %u \r\n",status);
        return status;
    }

    status = R_GPT_Start (&gpt_ctrl);
    if (FSP_SUCCESS != status)
    {
        IotLog("** R_GPT_Start failed ** %u \r\n",status);
        return status;
    }
    return status;
}

/***********************************************************************************************************************
 * @brief       This function is the callback function for the External IRQ for User Push button handling.
 *              Based on the channel number, the Push button switch pressed message is sent to the application.
 * @param[IN]   p_args    callback arguments.
 * @retval      void        
 **********************************************************************************************************************/
void pb_callback(external_irq_callback_args_t *p_args)
{
	usr_pb_msg_t lpb_data = {RESET_VALUE};

    if (NULL != p_args)
    {
        if (PBS1_CHANNEL == p_args->channel)
        {
            lpb_data.pb_data.pb_num = PBS1;
        }
        else if (PBS2_CHANNEL == p_args->channel)
        {
            lpb_data.pb_data.pb_num = PBS2;
        }
        lpb_data.id = ID_PB;
        lpb_data.pb_data.pb_state = PB_PRESSED;

        tx_queue_send(&g_topic_queue, &lpb_data, TX_NO_WAIT);
    }
}


void g_gpt_timer_cb(timer_callback_args_t *p_args)
{
    if ((NULL != p_args) && (TIMER_EVENT_CYCLE_END == p_args->event))
    {
        fsp_err_t err = FSP_SUCCESS;     // Error status
        mcu_temp_msg_t ltemp_data = {RESET_VALUE};
        uint16_t val = RESET_VALUE;

        /* Read the result */
        err = R_ADC_Read(&adc_ctrl, ADC_CHANNEL_TEMPERATURE, &val);

        /* handle error */
        if (FSP_SUCCESS != err)
        {
            /* ADC Failure message */
            APP_ERR_PRINT("** R_ADC_Read API failed ** \r\n");
        }
        else
        {
            ltemp_data.id = ID_TEMPERATURE;
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
    bsp_io_level_t pin_level = LED_OFF;

    /* LED type structure */
    bsp_leds_t leds = g_bsp_leds;

    R_BSP_PinAccessEnable ();

	if(((LED_ON == lled_state) || (LED_OFF == lled_state)) && 
	   ((LED_GREEN == ltype) || (LED_BLUE == ltype) || (LED_RED == ltype)))
	{
        pin_level = lled_state;
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
	static bsp_io_level_t pin_level[3] = {LED_OFF};

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
            pin_level[ltype] = LED_ON;
        }
        else
        {
            pin_level[ltype] = LED_OFF;
        }
    }

    R_BSP_PinAccessDisable ();
}

