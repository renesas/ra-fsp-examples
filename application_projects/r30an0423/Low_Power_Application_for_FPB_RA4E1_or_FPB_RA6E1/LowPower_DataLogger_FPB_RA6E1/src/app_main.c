/***********************************************************************************************************************
 * File Name    : app_main.c
 * Description  : Contains data structures and functions used in app_main.c.
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2022 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
************************************************************************************************************************/

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

    while (1)
    {
        /* Check all flags clear */
        if (all_flags_clearing_check() == CLEAR_FLAG)
        {

            /* Change the ADC scan start event to Snooze request */
            adc_scan_start_event_change(ELC_EVENT_LPM_SNOOZE_REQUEST);

            /* DEBUG LED LOW */
            pin_state_change(LED, PIN_LOW__LED_OFF);

            /* Enter the Low Power Mode */
            err = R_LPM_LowPowerModeEnter(&g_lpm_ctrl);
            if (err != FSP_SUCCESS)
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
        if (g_manual_lpm_cancel_flag == SET_FLAG)
        {
            /* Clear the flag */
            g_manual_lpm_cancel_flag = CLEAR_FLAG;

            /* Data Output */
            data_output(SEND_AT_MANUAL_TIMING);

            /* Set Software break point if needed */
            //__BKPT(0);
        }

        /* Level Judgement Result Output fuction */
        if (g_level_jedgement_result_flag == SET_FLAG)
        {
            /* Clear the flag */
            g_level_jedgement_result_flag = CLEAR_FLAG;

            /* Data Output */
            data_output(SEND_LEVEL_JUDGEMENT_RESULT);
        }

        /* Data Output function */
        if (g_24h_flag == SET_FLAG)
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
