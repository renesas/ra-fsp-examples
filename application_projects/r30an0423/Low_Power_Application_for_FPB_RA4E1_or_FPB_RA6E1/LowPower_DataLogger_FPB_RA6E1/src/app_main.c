/***********************************************************************************************************************
 * File Name    : app_main.c
 * Description  : Contains data structures and functions used in app_main.c.
 **********************************************************************************************************************/

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
 * Copyright (C) 2022 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#include "hal_data.h"
#include "app_common.h"
#include "app_data_output.h"

volatile bool g_24h_flag;
volatile bool g_level_jedgement_result_flag;
volatile bool g_manual_lpm_cancel_flag;
volatile bool g_uart_transmit_complete_flag;

uint16_t g_raw_light_data_buff[RAW_DATA_BUFF_SIZE];
uint16_t g_raw_temp_data_buff[RAW_DATA_BUFF_SIZE];

// Prototype declaration
void app_main(void);

/*******************************************************************************************************************//**
 * @brief Main routine
 * @param None
 * @retval None
 **********************************************************************************************************************/
void app_main(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Initialize module */
    adc_initialize();
    lpm_initialize();
    icu_initialize();
    dtc_initialize();
    doc_initialize();
    elc_initialize();
    agt_initialize();
    rtc_initialize();

    /* Initialize PIN state */
    pin_state_change(LED, PIN_LOW__LED_OFF);
    pin_state_change(ERROR_LED, PIN_LOW__LED_OFF);

    /* Initialize flags */
    g_24h_flag = CLEAR_FLAG;
    g_level_jedgement_result_flag = CLEAR_FLAG;
    g_manual_lpm_cancel_flag = CLEAR_FLAG;
    g_uart_transmit_complete_flag = CLEAR_FLAG;

    /* Start AGT counting */
    agt_start();

    /* Set the RTC Alarm */
    rtc_alarm_set();

    while(1){

        /* Check all flags clear */
        if(all_flags_clearing_check() == CLEAR_FLAG){

            /* Change the ADC scan start event to Snooze request */
            adc_scan_start_event_change(ELC_EVENT_LPM_SNOOZE_REQUEST);

            /* DEBUG LED LOW */
            pin_state_change(LED, PIN_LOW__LED_OFF);

            /* Enter the Low Power Mode */
            err = R_LPM_LowPowerModeEnter(&g_lpm_ctrl);
            if(err != FSP_SUCCESS)
            {
                pin_state_change(ERROR_LED, PIN_HIGH__LED_ON);
                __BKPT(0);
            }

            /* DEBUG LED HIGH */
            pin_state_change(LED, PIN_HIGH__LED_ON);

            /* Change the ADC scan start event to AGT1_underflow */
            adc_scan_start_event_change(ELC_EVENT_AGT1_INT);
        }

        /* Manual LPM Canceling function for DEBUG */
        if(g_manual_lpm_cancel_flag == SET_FLAG)
        {
            /* Clear the flag */
            g_manual_lpm_cancel_flag = CLEAR_FLAG;

            /* Data Output */
            data_output(SEND_AT_MANUAL_TIMING);

            /* Set Software break point if needed */
            //__BKPT(0);
        }

        /* Level Judgement Result Output fuction */
        if(g_level_jedgement_result_flag == SET_FLAG)
        {
            /* Clear the flag */
            g_level_jedgement_result_flag = CLEAR_FLAG;

            /* Data Output */
            data_output(SEND_LEVEL_JUDGEMENT_RESULT);
        }

        /* Data Output function */
        if(g_24h_flag == SET_FLAG)
        {
            /* Clear the flag */
            g_24h_flag = CLEAR_FLAG;

            /* Data Output */
            data_output(SEND_AT_24H_PERIOD);

            /* Set the RTC Alarm */
            rtc_alarm_set();
        }

    }

}
