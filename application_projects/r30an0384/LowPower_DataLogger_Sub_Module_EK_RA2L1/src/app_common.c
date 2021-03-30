
#include "hal_data.h"
#include "math.h"
#include "app_common.h"

static uint16_t g_doc_analog_level_judgement_threshold;

static const uint16_t convert_list_read_data[] = { 1, 4, 14, 52, 189, 662, 1699, 2366, 2854};
static const float convert_list_luminance[] = { 1.0108f, 3.1201f, 9.8051f, 27.43f, 69.545f, 232.67f, 645.11f, 973.52f, 1000};

dtc_instance_ctrl_t g_dtc_data_transfer_ctrl;
transfer_info_t g_dtc_data_transfer_info[4] = {
{
		.dest_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED,
		.repeat_area = TRANSFER_REPEAT_AREA_DESTINATION,
		.irq = TRANSFER_IRQ_END,
		.chain_mode = TRANSFER_CHAIN_MODE_EACH,
		.src_addr_mode = TRANSFER_ADDR_MODE_FIXED,
		.size = TRANSFER_SIZE_2_BYTE,
		.mode = TRANSFER_MODE_REPEAT,
		.p_dest = (void*) &g_raw_light_data_buff,
		.p_src = (void const*) &ADC0_CH00_DATA_REGISTER,
		.num_blocks = 0,
		.length = RAW_DATA_BUFF_SIZE,
},
{
		.dest_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED,
		.repeat_area = TRANSFER_REPEAT_AREA_DESTINATION,
		.irq = TRANSFER_IRQ_END,
		.chain_mode = TRANSFER_CHAIN_MODE_EACH,
		.src_addr_mode = TRANSFER_ADDR_MODE_FIXED,
		.size = TRANSFER_SIZE_2_BYTE,
		.mode = TRANSFER_MODE_REPEAT,
		.p_dest = (void*) &g_raw_temp_data_buff,
		.p_src = (void const*) &ADC0_CH01_DATA_REGISTER,
		.num_blocks = 0,
		.length = RAW_DATA_BUFF_SIZE,
},
{
		.dest_addr_mode = TRANSFER_ADDR_MODE_FIXED,
		.repeat_area = TRANSFER_REPEAT_AREA_DESTINATION,
		.irq = TRANSFER_IRQ_END,
		.chain_mode = TRANSFER_CHAIN_MODE_EACH,
		.src_addr_mode = TRANSFER_ADDR_MODE_FIXED,
		.size = TRANSFER_SIZE_2_BYTE,
		.mode = TRANSFER_MODE_REPEAT,
		.p_dest = (void*) &DOC_DATA_SETTING_REGISTER,
		.p_src = (void const*) &g_doc_analog_level_judgement_threshold,
		.num_blocks = 0,
		.length = 1,
},
{
		.dest_addr_mode = TRANSFER_ADDR_MODE_FIXED,
		.repeat_area = TRANSFER_REPEAT_AREA_DESTINATION,
		.irq = TRANSFER_IRQ_END,
		.chain_mode = TRANSFER_CHAIN_MODE_DISABLED,
		.src_addr_mode = TRANSFER_ADDR_MODE_FIXED,
		.size = TRANSFER_SIZE_2_BYTE,
		.mode = TRANSFER_MODE_REPEAT,
		.p_dest = (void*) &DOC_DATA_INPUT_REGISTER,
		.p_src = (void const*) &ADC0_CH00_DATA_REGISTER,
		.num_blocks = 0,
		.length = 1,
}
};

const dtc_extended_cfg_t g_dtc_data_transfer_cfg_extend = {
		.activation_source = VECTOR_NUMBER_ADC0_SCAN_END,
};
const transfer_cfg_t g_dtc_data_transfer_cfg = {
		.p_info = &g_dtc_data_transfer_info[0],
		.p_extend =&g_dtc_data_transfer_cfg_extend,
};
const transfer_instance_t g_dtc_data_transfer = {
		.p_ctrl = &g_dtc_data_transfer_ctrl,
		.p_cfg = &g_dtc_data_transfer_cfg,
		.p_api = &g_transfer_on_dtc
};

// Prototype declaration
void irq_manual_lpm_cancel_interrupt(external_irq_callback_args_t *p_args);
void irq_wakeup_interrupt(external_irq_callback_args_t *p_args);
void rtc_1h_interrupt(rtc_callback_args_t *p_args);
void sci_uart_interrupt(uart_callback_args_t *p_args);

void adc_initialize(void);
void lpm_initialize(void);
void icu_initialize(void);
void dtc_initialize(void);
void doc_initialize(void);
void elc_initialize(void);
void ioport_eventout_initialize(void);
void agt_initialize(void);
void rtc_initialize(void);

void agt_start(void);
void pin_state_change(bsp_io_port_pin_t pin, bsp_io_level_t state);
float convert_unit_light(uint16_t input);
float convert_unit_temp(uint16_t input);
void processed_data_buff_clear(processed_data_t* buff, uint16_t buff_size);
uint32_t rtc_current_time_get(void);
void rtc_alarm_set(void);
void adc_scan_start_event_change(elc_event_t event);


void irq_manual_lpm_cancel_interrupt(external_irq_callback_args_t *p_args)
{
	FSP_PARAMETER_NOT_USED(p_args);
	/* Set a flag */
	g_manual_lpm_cancel_flag = SET_FLAG;
}

void irq_wakeup_interrupt(external_irq_callback_args_t *p_args)
{
	FSP_PARAMETER_NOT_USED(p_args);
	/* Set a flag */
	g_wakeup_flag = SET_FLAG;
}

void rtc_1h_interrupt(rtc_callback_args_t *p_args)
{
	if(p_args->event == RTC_EVENT_ALARM_IRQ)
	{
		/* Set a flag */
		g_1h_flag = SET_FLAG;
	}
}

void sci_uart_interrupt(uart_callback_args_t *p_args)
{
	if(p_args->event == UART_EVENT_RX_COMPLETE)
	{
		/* Set a flag */
		g_uart_recv_flag = SET_FLAG;
	}else if(p_args->event == UART_EVENT_TX_COMPLETE)
	{
		/* Set a flag */
		g_uart_transmit_complete_flag = SET_FLAG;
	}
}

void adc_interrupt(adc_callback_args_t *p_args)
{
	FSP_PARAMETER_NOT_USED(p_args);
	/* Do nothing */
}

void doc_interrupt(doc_callback_args_t *p_args)
{
	FSP_PARAMETER_NOT_USED(p_args);
	/* Do nothing */
}


void adc_initialize()
{
	fsp_err_t err = FSP_SUCCESS;

	/* Open the ADC module */
	err = R_ADC_Open(&g_adc0_ctrl, &g_adc0_cfg);
	if(err != FSP_SUCCESS)
	{
		__BKPT(0);
	}

	/* Configure the ADC scan parameters */
	err = R_ADC_ScanCfg(&g_adc0_ctrl, &g_adc0_channel_cfg);
	if(err != FSP_SUCCESS)
	{
		__BKPT(0);
	}

	// Set up for Compare match
    //---Set up the ADCSR---
    //Start ADC by ELC
    R_ADC0->ADCSR_b.EXTRG = 0x0;

    //Enable Synchronous as the ADC trigger
    R_ADC0->ADCSR_b.TRGE = 0x1;

    //Set ADC to low power mode
    R_ADC0->ADCSR_b.ADHSC = 0x1;

    //--- Set the Window_A ------
    //Select a channel for Window_A Comparison
    R_ADC0->ADCMPANSR_b[0].CMPCHA1 = 0x1;

    //Set the Window_A Lower boundary
    R_ADC0->ADCMPDR0_b.ADCMPDR0 = ANALOG_LEVEL_JUDGEMENT_CH01_THRESHOLD;

    //Set the Window_A comparison condition
    R_ADC0->ADCMPLR_b[0].CMPLCHA1 = 0x1;

    //Enable Window_A operation
    R_ADC0->ADCMPCR_b.CMPAE = 0x1;

    //Enable Window_A interrupt for meeting the condition, ADC140_CMPAI
    R_ADC0->ADCMPCR_b.CMPAIE = 0x1;

    //--- Set the Window_B------
    //Select the AN001 for Window_B
    R_ADC0->ADCMPBNSR_b.CMPCHB = 0x3F;

    //Set the Window_B comparison condition
    R_ADC0->ADCMPBNSR_b.CMPLB = 0x1;

    //Set the Window_B Lower boundary
    R_ADC0->ADWINLLB_b.ADWINLLB = 0x0;

    //Set the Window_B Upper boundary
    R_ADC0->ADWINULB_b.ADWINULB = 0x0;

    //Enable Window_B operation
    R_ADC0->ADCMPCR_b.CMPBE = 0x1;

    //Enable Window_A/B composite condition
    R_ADC0->ADCMPCR_b.CMPAB = 0x0;
}

void lpm_initialize()
{
	fsp_err_t err = FSP_SUCCESS;

	/* Open the LPM module */
	err = R_LPM_Open(&g_lpm_ctrl, &g_lpm_cfg);
	if(err != FSP_SUCCESS)
	{
		__BKPT(0);
	}
}

void icu_initialize()
{
	fsp_err_t err = FSP_SUCCESS;

	/* Open the External IRQ3 module */
	err = R_ICU_ExternalIrqOpen(&g_external_irq4_wakeup_ctrl, &g_external_irq4_wakeup_cfg);
	if(err != FSP_SUCCESS)
	{
		__BKPT(0);
	}

	/* Enable the External IRQ3 */
	err = R_ICU_ExternalIrqEnable(&g_external_irq4_wakeup_ctrl);
	if(err != FSP_SUCCESS)
	{
		__BKPT(0);
	}

	/* Open the External IRQ4 module */
	err = R_ICU_ExternalIrqOpen(&g_external_irq3_manual_lpm_cancel_ctrl, &g_external_irq3_manual_lpm_cancel_cfg);
	if(err != FSP_SUCCESS)
	{
		__BKPT(0);
	}

	/* Enable the External IRQ4 */
	err = R_ICU_ExternalIrqEnable(&g_external_irq3_manual_lpm_cancel_ctrl);
	if(err != FSP_SUCCESS)
	{
		__BKPT(0);
	}
}

void dtc_initialize()
{
	fsp_err_t err = FSP_SUCCESS;

	/* Open the DTC module */
	err = R_DTC_Open(&g_dtc_data_transfer_ctrl, &g_dtc_data_transfer_cfg);
	if(err != FSP_SUCCESS)
	{
		__BKPT(0);
	}

	/* Enable DTC transfer */
	err = R_DTC_Enable(&g_dtc_data_transfer_ctrl);
	if(err != FSP_SUCCESS)
	{
		__BKPT(0);
	}
}

void doc_initialize()
{
	fsp_err_t err = FSP_SUCCESS;

	/* Open the DOC module */
	err = R_DOC_Open(&g_doc_ctrl, &g_doc_cfg);
	if(err != FSP_SUCCESS)
	{
		__BKPT(0);
	}

	/* Set a threshold for DOC Analog level judgement */
	g_doc_analog_level_judgement_threshold = ANALOG_LEVEL_JUDGEMENT_CH00_THRESHOLD;
}

void elc_initialize()
{
	fsp_err_t err = FSP_SUCCESS;

	/* Open the ELC module */
	err = R_ELC_Open(&g_elc_ctrl, &g_elc_cfg);
	if(err != FSP_SUCCESS)
	{
		__BKPT(0);
	}

	/* Enable the ELC */
	err = R_ELC_Enable(&g_elc_ctrl);
	if(err != FSP_SUCCESS)
	{
		__BKPT(0);
	}
}

void ioport_eventout_initialize()
{
	fsp_err_t err = FSP_SUCCESS;

	/* Set a pin event output for outputting the DOC analog level judgement result */
	err = R_IOPORT_PinEventOutputWrite(&g_ioport_ctrl, ANALOG_LEVEL_CH00_PIN, BSP_IO_LEVEL_HIGH);
	if(err != FSP_SUCCESS)
	{
		__BKPT(0);
	}

	/* Set a pin event output for outputting the ADC analog level judgement result */
	err = R_IOPORT_PinEventOutputWrite(&g_ioport_ctrl, ANALOG_LEVEL_CH01_PIN, BSP_IO_LEVEL_HIGH);
	if(err != FSP_SUCCESS)
	{
		__BKPT(0);
	}
}

void agt_initialize()
{
	fsp_err_t err = FSP_SUCCESS;

	/* Open the AGT module */
	err = R_AGT_Open(&g_agt_timer1_ctrl, &g_agt_timer1_cfg);
	if(err != FSP_SUCCESS)
	{
		__BKPT(0);
	}

#ifdef DEBUG_FAST_CYCLE
	/* Set a 1sec count value for fast cycle debugging */
	uint32_t agt_1sec_count_value = (uint32_t)(g_agt_timer1_cfg.period_counts / 60);
	err = R_AGT_PeriodSet(&g_agt_timer1_ctrl, agt_1sec_count_value);
	if(err != FSP_SUCCESS)
	{
		__BKPT(0);
	}
#endif

}

void rtc_initialize()
{
	g_rtc_ctrl.p_cfg = &g_rtc_cfg;
	g_rtc_ctrl.p_callback = g_rtc_cfg.p_callback;
	g_rtc_ctrl.p_context = g_rtc_cfg.p_context;
	g_rtc_ctrl.p_callback_memory = NULL;

	g_rtc_ctrl.carry_isr_triggered = false;

	/* Configure irq */
	if (g_rtc_cfg.alarm_irq >= 0)
	{
		R_BSP_IrqCfg(g_rtc_cfg.alarm_irq, g_rtc_cfg.alarm_ipl, &g_rtc_ctrl);
	}

	if (g_rtc_cfg.carry_irq >= 0)
	{
		R_BSP_IrqCfg(g_rtc_cfg.carry_irq, g_rtc_cfg.carry_ipl, &g_rtc_ctrl);
	}

	// Setting of Clocks, Operation mode, and Count mode.
	// Operation Mode Select
	R_RTC->RCR4_b.ROPSEL = 0;

    // Count Source Select
    R_RTC->RCR4_b.RCKSEL = 0;

	// Supply 6 clocks of the count source
	R_BSP_SoftwareDelay(100, BSP_DELAY_UNITS_MILLISECONDS);

	/* Set or Clear START bit in RCR2 register depending on the value */
	R_RTC->RCR2_b.START = 0;

	/* Wait for Start bit update */
	FSP_HARDWARE_REGISTER_WAIT(R_RTC->RCR2_b.START, 0);

	/* Set the frequency register */
	if(R_RTC->RCR4_b.RCKSEL != 0){
		R_RTC->RFRL_b.RFC = 0x00FF;
	}

	/* Set the CNTMD bit in the RCR2 register */
	R_RTC->RCR2_b.CNTMD = 1;

	/* Wait for CNTMD bit update */
	FSP_HARDWARE_REGISTER_WAIT(R_RTC->RCR2_b.CNTMD, 1);

	/* Set the RESET bit in the RCR2 register */
	R_RTC->RCR2_b.RESET = 1;

	/* Wait for Reset bit update */
	FSP_HARDWARE_REGISTER_WAIT(R_RTC->RCR2_b.RESET, 0);

	// Setting of clock value
	/* Set or Clear START bit in RCR2 register depending on the value */
	R_RTC->RCR2_b.START = 0;

	/* Wait for Start bit update */
	FSP_HARDWARE_REGISTER_WAIT(R_RTC->RCR2_b.START, 0);

	/* Set the RESET bit in the RCR2 register */
	R_RTC->RCR2_b.RESET = 1;

	/* Wait for Reset bit update */
	FSP_HARDWARE_REGISTER_WAIT(R_RTC->RCR2_b.RESET, 0);

	R_RTC->BCNT0 = 0;
	R_RTC->BCNT1 = 0;
	R_RTC->BCNT2 = 0;
	R_RTC->BCNT3 = 0;

	if (R_RTC->RCR4_b.RCKSEL == 0)
	{
		/*
		 * Please set a time error adjustment function if needed.
		 * on-board sub-clock oscillator of EK-RA2L1 is provided 32,768 Hz.
		 * So it is not necessary to perform the adjustment.
		 * (See section 22.3.8 "Time Error Adjustment Function" of the RA2L1 manual R01UH0853EJxxxx)
		 */
	}

	/* Set START bit in RCR2 register depending on the value */
	R_RTC->RCR2_b.START = 1;

	/* Wait for Start bit update */
	FSP_HARDWARE_REGISTER_WAIT(R_RTC->RCR2_b.START, 1);
}

void agt_start(void)
{
	fsp_err_t err = FSP_SUCCESS;

	/* Start the AGT counting */
	err = R_AGT_Start(&g_agt_timer1_ctrl);
	if(err != FSP_SUCCESS)
	{
		__BKPT(0);
	}

}

void pin_state_change(bsp_io_port_pin_t pin, bsp_io_level_t state)
{
	/* Enable pin access */
	R_BSP_PinAccessEnable();

	/* Change the state */
	if(state == PIN_HIGH__LED_ON)
	{
		R_BSP_PinWrite(pin, BSP_IO_LEVEL_HIGH);
	}
	else
	{
		R_BSP_PinWrite(pin, BSP_IO_LEVEL_LOW);
	}

	/* Disable pin access */
	R_BSP_PinAccessDisable();
}

// This code is created with the reference to the Example code on https://wiki.seeedstudio.com/Grove-Luminance_Sensor/
float convert_unit_light(uint16_t input)
{
	uint8_t convert_list_addr;

	if(input <= convert_list_read_data[0])
	{
		return convert_list_luminance[0];
	}
	if(input >= convert_list_read_data[CONVERT_UNIT_LIGHT_LIST_SIZE - 1])
	{
		return convert_list_luminance[CONVERT_UNIT_LIGHT_LIST_SIZE - 1];
	}

	convert_list_addr = 1;
	while(1){
		if(input == convert_list_read_data[convert_list_addr])
		{
			return convert_list_luminance[convert_list_addr];
		}
		if(input < convert_list_read_data[convert_list_addr])
		{
			break;
		}
		convert_list_addr++;
	}

	return (float)(input - convert_list_read_data[convert_list_addr - 1])
			/ (float)(convert_list_read_data[convert_list_addr] - convert_list_read_data[convert_list_addr - 1])
			* (convert_list_luminance[convert_list_addr] - convert_list_luminance[convert_list_addr - 1])
			+ convert_list_luminance[convert_list_addr - 1];
}

// This code is created with the reference to the Example code on https://wiki.seeedstudio.com/Grove-Temperature_Sensor_V1.2/
float convert_unit_temp(uint16_t input)
{
	float resistance;

	if(input <= TEMP_READ_DATA_VALUE_OF_0_DEGREE_CELSIUS)
	{
		input = TEMP_READ_DATA_VALUE_OF_0_DEGREE_CELSIUS;
	}
	if(input >= TEMP_READ_DATA_VALUE_OF_99_DEGREE_CELSIUS)
	{
		input = TEMP_READ_DATA_VALUE_OF_99_DEGREE_CELSIUS;
	}
	resistance = (float)((ADC_READ_VALUE_MAX_DEC/input - 1.0) * GROVE_TEMP_SENSOR_RESISTANCE0);

	return (float)(1.0/(log(resistance/GROVE_TEMP_SENSOR_RESISTANCE0)/B_CONSTANT + 1/TEMP_KELVIN_VALUE_OF_25_DEGREE_CELSIUS) - TEMP_KELVIN_VALUE_OF_0_DEGREE_CELSIUS);
}

void processed_data_buff_clear(processed_data_t* buff, uint16_t buff_size)
{
	memset(buff, 0, sizeof(processed_data_t) * buff_size);
}

uint32_t rtc_current_time_get(void)
{
	uint32_t r_RTC_current_time;

	/* Wait for 1/128 sec */
	R_BSP_SoftwareDelay(8, BSP_DELAY_UNITS_MILLISECONDS);

	uint32_t carry_irq_status = NVIC_GetEnableIRQ(g_rtc_ctrl.p_cfg->carry_irq);
	if ((uint32_t) 0U == carry_irq_status)
	{
		/* Set the CIE bit in the RCR1 register */
		R_RTC->RCR1_b.CIE = 0x1;

		/* Wait for CIE bit update */
        FSP_HARDWARE_REGISTER_WAIT(R_RTC->RCR1_b.CIE, 1);

		R_BSP_IrqEnable(g_rtc_ctrl.p_cfg->carry_irq);
	}

	do
	{
		g_rtc_ctrl.carry_isr_triggered = false; /** This flag will be set to 'true' in the carry ISR */

		r_RTC_current_time = (uint32_t)((R_RTC->BCNT3 << 24) | (R_RTC->BCNT2 << 16) | (R_RTC->BCNT1 << 8) | R_RTC->BCNT0);
	} while (g_rtc_ctrl.carry_isr_triggered);

	/** Restore the state of carry IRQ. */
	if ((uint32_t) 0U == carry_irq_status)
	{
		/* Set the CIE bit in the RCR1 register */
		R_RTC->RCR1_b.CIE = 0x0;

        /* Wait for CIE bit update */
        FSP_HARDWARE_REGISTER_WAIT(R_RTC->RCR1_b.CIE, 0);

		/* Disable this interrupt in the NVIC */
		R_BSP_IrqDisable(g_rtc_ctrl.p_cfg->carry_irq);
	}

	return r_RTC_current_time;
}

void rtc_alarm_set(void)
{
	uint32_t rs_RTC_current_time;
	uint32_t RTC_set_time;

	rs_RTC_current_time = rtc_current_time_get();

	// Disable IRQ */
	if (g_rtc_cfg.alarm_irq >= 0)
	{
		/* Disable the ICU alarm interrupt request */
		R_BSP_IrqDisable(g_rtc_cfg.alarm_irq);
	}

#ifdef DEBUG_FAST_CYCLE
	RTC_set_time = rs_RTC_current_time + RTC_ALARM_VALUE_1MIN;
#else
	RTC_set_time = rs_RTC_current_time + RTC_ALARM_VALUE_1HOUR;
#endif

	/* Set the Alarm value */
	R_RTC->BCNT0AR = (uint8_t)((RTC_set_time >> 0) & 0xFF);
	R_RTC->BCNT1AR = (uint8_t)((RTC_set_time >> 8) & 0xFF);
	R_RTC->BCNT2AR = (uint8_t)((RTC_set_time >> 16) & 0xFF);
	R_RTC->BCNT3AR = (uint8_t)((RTC_set_time >> 24) & 0xFF);

	/* Enable the Alarm value setting */
	R_RTC->BCNT0AER_b.ENB = 0xFF;
	R_RTC->BCNT1AER_b.ENB = 0xFF;
	R_RTC->BCNT2AER_b.ENB = 0xFF;
	R_RTC->BCNT3AER_b.ENB = 0xFF;

	/* Enable the Alarm */
	R_RTC->RCR1_b.AIE = 1;

	/* Wait for AIE bit update */
	FSP_HARDWARE_REGISTER_WAIT(R_RTC->RCR1_b.AIE, 1);

	/* Wait for two times of 1/64 sec */
	R_BSP_SoftwareDelay(32, BSP_DELAY_UNITS_MILLISECONDS);

	/* Enable IRQ */
	R_BSP_IrqEnable(g_rtc_cfg.alarm_irq);
}

void adc_scan_start_event_change(elc_event_t event)
{
	fsp_err_t err = FSP_SUCCESS;

	err = R_ELC_LinkSet(&g_elc_ctrl, ELC_PERIPHERAL_ADC0, event);
	if(err != FSP_SUCCESS){
		__BKPT(0);
	}

	err = R_ELC_Enable(&g_elc_ctrl);
	if(err != FSP_SUCCESS){
		__BKPT(0);
	}
}
