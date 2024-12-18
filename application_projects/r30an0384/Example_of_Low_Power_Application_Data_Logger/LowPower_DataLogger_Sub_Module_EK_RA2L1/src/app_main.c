/*
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#include "hal_data.h"
#include "app_common.h"
#include "app_data_output.h"
#include "app_data_processing.h"

volatile bool g_1h_flag;
volatile bool g_wakeup_flag;
volatile bool g_manual_lpm_cancel_flag;
volatile bool g_uart_recv_flag;
volatile bool g_uart_transmit_complete_flag;
volatile bool g_datafull_flag;

uint16_t g_raw_light_data_buff[RAW_DATA_BUFF_SIZE];
uint16_t g_raw_temp_data_buff[RAW_DATA_BUFF_SIZE];

processed_data_t g_processed_data_buff[PROCESSED_DATA_BUFF_SIZE];
uint8_t g_processed_data_buff_addr;

// Prototype declaration
void app_main(void);

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
	ioport_eventout_initialize();
	agt_initialize();
	rtc_initialize();

	/* Initialize PIN state */
	pin_state_change(LED1_BLUE, PIN_LOW__LED_OFF);
	pin_state_change(LED2_GREEN, PIN_LOW__LED_OFF);
	pin_state_change(LED3_RED, PIN_LOW__LED_OFF);
	pin_state_change(DATAFULL_PIN, PIN_LOW__LED_OFF);
	pin_state_change(ANALOG_LEVEL_CH00_PIN, PIN_LOW__LED_OFF);
	pin_state_change(ANALOG_LEVEL_CH01_PIN, PIN_LOW__LED_OFF);

	/* Start AGT counting */
	agt_start();

	/* Set the RTC Alarm */
	rtc_alarm_set();

	while (1)
	{
		/* Change the ADC scan start event to Snooze request */
		adc_scan_start_event_change(ELC_EVENT_LPM_SNOOZE_REQUEST);

		/* DEBUG LED LOW */
		pin_state_change(LED2_GREEN, PIN_LOW__LED_OFF);

		/* Enter the Low Power Mode */
		err = R_LPM_LowPowerModeEnter(&g_lpm_ctrl);
		if (err != FSP_SUCCESS)
		{
			__BKPT(0);
		}

		/* DEBUG LED HIGH */
		pin_state_change(LED2_GREEN, PIN_HIGH__LED_ON);

		/* Change the ADC scan start event to AGT1_underflow */
		adc_scan_start_event_change(ELC_EVENT_AGT1_INT);

		/* Manual LPM canceling function for DEBUG */
		if (g_manual_lpm_cancel_flag == SET_FLAG)
		{
			/* Clear the flag */
			g_manual_lpm_cancel_flag = CLEAR_FLAG;

			/* Set Software break point if needed */
			//__BKPT(0);

			/* Toggling the LED1 */
			for (uint8_t i = 0; i < 10; i++)
			{
				/* DEBUG LED1 HIGH */
				pin_state_change(LED1_BLUE, PIN_HIGH__LED_ON);

				/* Wait for 100 ms */
				R_BSP_SoftwareDelay(100, BSP_DELAY_UNITS_MILLISECONDS);

				/* DEBUG LED1 HIGH */
				pin_state_change(LED1_BLUE, PIN_LOW__LED_OFF);

				/* Wait for 100 ms */
				R_BSP_SoftwareDelay(100, BSP_DELAY_UNITS_MILLISECONDS);
			}
		}

		/* Data Output function */
		if (g_wakeup_flag == SET_FLAG)
		{
			/* Clear the flag */
			g_wakeup_flag = CLEAR_FLAG;

			/* Data Output */
			data_output();

			__NOP();
		}

		/* Data Processing function */
		if (g_1h_flag == SET_FLAG)
		{
			/* Clear the flag */
			g_1h_flag = CLEAR_FLAG;

			/* Data Processing */
			data_processing();
		}
	}
}
