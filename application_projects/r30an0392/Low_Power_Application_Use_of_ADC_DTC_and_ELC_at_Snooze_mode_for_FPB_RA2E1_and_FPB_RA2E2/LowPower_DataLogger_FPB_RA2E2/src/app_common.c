/***********************************************************************************************************************
 * File Name    : app_common.c
 * Description  : Contains data structures and functions used in app_common.c.
 **********************************************************************************************************************/

/**********************************************************************************************************************
* Copyright (c) 2022 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/


#include "hal_data.h"
#include "math.h"
#include "app_common.h"

static uint16_t g_doc_analog_level_judgement_threshold;

dtc_instance_ctrl_t g_dtc_data_transfer_ctrl;

transfer_info_t g_dtc_data_transfer_info[4] = {
    { /* ADC Channel 19 data register -> ch19 raw data buff */
        .transfer_settings_word_b = {
            .dest_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED,
            .repeat_area = TRANSFER_REPEAT_AREA_DESTINATION,
            .irq = TRANSFER_IRQ_END,
            .chain_mode = TRANSFER_CHAIN_MODE_EACH,
            .src_addr_mode = TRANSFER_ADDR_MODE_FIXED,
            .size = TRANSFER_SIZE_2_BYTE,
            .mode = TRANSFER_MODE_REPEAT,
        },
        .p_dest = (void*) &g_raw_light_data_buff,
        .p_src = (void const*) &ADC0_CH19_DATA_REGISTER,
        .num_blocks = 0,
        .length = RAW_DATA_BUFF_SIZE,
    },

    { /* ADC Channel 21 data register -> ch21 raw data buff */
        .transfer_settings_word_b = {
            .dest_addr_mode = TRANSFER_ADDR_MODE_INCREMENTED,
            .repeat_area = TRANSFER_REPEAT_AREA_DESTINATION,
            .irq = TRANSFER_IRQ_END,
            .chain_mode = TRANSFER_CHAIN_MODE_EACH,
            .src_addr_mode = TRANSFER_ADDR_MODE_FIXED,
            .size = TRANSFER_SIZE_2_BYTE,
            .mode = TRANSFER_MODE_REPEAT,
        },
        .p_dest = (void*) &g_raw_temp_data_buff,
        .p_src = (void const*) &ADC0_CH21_DATA_REGISTER,
        .num_blocks = 0,
        .length = RAW_DATA_BUFF_SIZE,
    },

    { /* level judgment threshold -> DOC setting register */
        .transfer_settings_word_b = {
            .dest_addr_mode = TRANSFER_ADDR_MODE_FIXED,
            .repeat_area = TRANSFER_REPEAT_AREA_DESTINATION,
            .irq = TRANSFER_IRQ_END,
            .chain_mode = TRANSFER_CHAIN_MODE_EACH,
            .src_addr_mode = TRANSFER_ADDR_MODE_FIXED,
            .size = TRANSFER_SIZE_2_BYTE,
            .mode = TRANSFER_MODE_REPEAT,
        },
        .p_dest = (void*) &DOC_DATA_SETTING_REGISTER,
        .p_src = (void const*) &g_doc_analog_level_judgement_threshold,
        .num_blocks = 0,
        .length = 1,
    },

    { /* ADC Channel 21 data register -> DOC input register */
        .transfer_settings_word_b = {
          .dest_addr_mode = TRANSFER_ADDR_MODE_FIXED,
          .repeat_area = TRANSFER_REPEAT_AREA_DESTINATION,
          .irq = TRANSFER_IRQ_END,
          .chain_mode = TRANSFER_CHAIN_MODE_EACH,
          .src_addr_mode = TRANSFER_ADDR_MODE_FIXED,
          .size = TRANSFER_SIZE_2_BYTE,
          .mode = TRANSFER_MODE_REPEAT,
        },
        .p_dest = (void*) &DOC_DATA_INPUT_REGISTER,
        .p_src = (void const*) &ADC0_CH21_DATA_REGISTER,
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
void adc_initialize(void);
void lpm_initialize(void);
void icu_initialize(void);
void dtc_initialize(void);
void doc_initialize(void);
void elc_initialize(void);
void agt_initialize(void);

void agt_start(void);
void pin_state_change(bsp_io_port_pin_t pin, bsp_io_level_t state);
bool all_flags_clearing_check(void);
void adc_scan_start_event_change(elc_event_t event);

/*******************************************************************************************************************//**
 * @brief Callback of manual LPM canceling interrupt
 * @param (*p_args) Callback function parameter data
 * @retval None
 **********************************************************************************************************************/
void irq_manual_lpm_cancel_callback(external_irq_callback_args_t *p_args)
{
	FSP_PARAMETER_NOT_USED(p_args);
	/* Set a flag */
	g_manual_lpm_cancel_flag = SET_FLAG;
}

/*******************************************************************************************************************//**
 * @brief Callback of timer LPM canceling interrupt
 * @param (*p_args) Callback function parameter data
 * @retval None
 **********************************************************************************************************************/
void irq_timer_lpm_cancel_callback(external_irq_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);
    /* Set a flag */
    g_timer_lpm_cancel_flag = SET_FLAG;
}

/*******************************************************************************************************************//**
 * @brief Callback of DOC interrupt
 * @param (*p_args) Callback function parameter data
 * @retval None
 **********************************************************************************************************************/
void doc_callback(doc_callback_args_t *p_args)
{
    FSP_PARAMETER_NOT_USED(p_args);
    /* Set a flag */
    g_level_jedgement_result_flag = SET_FLAG;
}

/*******************************************************************************************************************//**
 * @brief Callback of SCI interrupt
 * @param (*p_args) Callback function parameter data
 * @retval None
 **********************************************************************************************************************/
void sci_uart_callback(uart_callback_args_t *p_args)
{
    if(p_args->event == UART_EVENT_TX_COMPLETE)
	{
		/* Set a flag */
		g_uart_transmit_complete_flag = SET_FLAG;
	}
}

/*******************************************************************************************************************//**
 * @brief Callback of ADC interrupt
 * @param (*p_args) Callback function parameter data
 * @retval None
 **********************************************************************************************************************/
void adc_callback(adc_callback_args_t *p_args)
{
	FSP_PARAMETER_NOT_USED(p_args);
	/* Do nothing */
}

/*******************************************************************************************************************//**
 * @brief Open ADC module and setup scan configuration
 * @param None
 * @retval None
 **********************************************************************************************************************/
void adc_initialize(void)
{
	fsp_err_t err = FSP_SUCCESS;

	/* Open the ADC module */
	err = R_ADC_Open(&g_adc0_ctrl, &g_adc0_cfg);
	if(err != FSP_SUCCESS)
	{
	    pin_state_change(ERROR_LED, PIN_HIGH__LED_ON);
		__BKPT(0);
	}

	/* Configure the ADC scan parameters */
	err = R_ADC_ScanCfg(&g_adc0_ctrl, &g_adc0_channel_cfg);
	if(err != FSP_SUCCESS)
	{
	    pin_state_change(ERROR_LED, PIN_HIGH__LED_ON);
		__BKPT(0);
	}

    //---Set up the ADCSR---
    //Start ADC by ELC
    R_ADC0->ADCSR_b.EXTRG = 0x0;

    //Enable Synchronous as the ADC trigger
    R_ADC0->ADCSR_b.TRGE = 0x1;

    //Set ADC to low power mode
    R_ADC0->ADCSR_b.ADHSC = 0x1;

    // Enable Window_A operation to use ADC12 during Snooze mode.
    // See Note 10 on Table "Operating conditions of each low power mode" of the RA2E2 manual R01UH0919
    R_ADC0->ADCMPCR_b.CMPAE = 0x1;
    R_ADC0->ADCMPCR_b.CMPBE = 0x1;

}

/*******************************************************************************************************************//**
 * @brief Open Low Power Mode module
 * @param None
 * @retval None
 **********************************************************************************************************************/
void lpm_initialize(void)
{
	fsp_err_t err = FSP_SUCCESS;

	/* Open the LPM module */
	err = R_LPM_Open(&g_lpm_ctrl, &g_lpm_cfg);
	if(err != FSP_SUCCESS)
	{
	    pin_state_change(ERROR_LED, PIN_HIGH__LED_ON);
		__BKPT(0);
	}
}

/*******************************************************************************************************************//**
 * @brief Open and enable ICU module
 * @param None
 * @retval None
 **********************************************************************************************************************/
void icu_initialize(void)
{
	fsp_err_t err = FSP_SUCCESS;

	/* Open the External IRQ module */
	err = R_ICU_ExternalIrqOpen(&g_external_irq_manual_lpm_cancel_ctrl, &g_external_irq_manual_lpm_cancel_cfg);
	if(err != FSP_SUCCESS)
	{
	    pin_state_change(ERROR_LED, PIN_HIGH__LED_ON);
		__BKPT(0);
	}

	/* Enable the External IRQ */
	err = R_ICU_ExternalIrqEnable(&g_external_irq_manual_lpm_cancel_ctrl);
	if(err != FSP_SUCCESS)
	{
	    pin_state_change(ERROR_LED, PIN_HIGH__LED_ON);
		__BKPT(0);
	}

    /* Open the External IRQ module */
    err = R_ICU_ExternalIrqOpen(&g_external_irq_timer_lpm_cancel_ctrl, &g_external_irq_timer_lpm_cancel_cfg);
    if(err != FSP_SUCCESS)
    {
        pin_state_change(ERROR_LED, PIN_HIGH__LED_ON);
        __BKPT(0);
    }

    /* Enable the External IRQ */
    err = R_ICU_ExternalIrqEnable(&g_external_irq_timer_lpm_cancel_ctrl);
    if(err != FSP_SUCCESS)
    {
        pin_state_change(ERROR_LED, PIN_HIGH__LED_ON);
        __BKPT(0);
    }
}

/*******************************************************************************************************************//**
 * @brief Open and enable DTC module
 * @param None
 * @retval None
 **********************************************************************************************************************/
void dtc_initialize(void)
{
	fsp_err_t err = FSP_SUCCESS;

	/* Open the DTC module */
	err = R_DTC_Open(&g_dtc_data_transfer_ctrl, &g_dtc_data_transfer_cfg);
	if(err != FSP_SUCCESS)
	{
	    pin_state_change(ERROR_LED, PIN_HIGH__LED_ON);
		__BKPT(0);
	}

	/* Enable DTC transfer */
	err = R_DTC_Enable(&g_dtc_data_transfer_ctrl);
	if(err != FSP_SUCCESS)
	{
	    pin_state_change(ERROR_LED, PIN_HIGH__LED_ON);
		__BKPT(0);
	}
}

/*******************************************************************************************************************//**
 * @brief Open DOC module and initialize level judgment threshold
 * @param None
 * @retval None
 **********************************************************************************************************************/
void doc_initialize(void)
{
	fsp_err_t err = FSP_SUCCESS;

	/* Open the DOC module */
	err = R_DOC_Open(&g_doc_ctrl, &g_doc_cfg);
	if(err != FSP_SUCCESS)
	{
	    pin_state_change(ERROR_LED, PIN_HIGH__LED_ON);
		__BKPT(0);
	}

	/* Set a threshold for DOC Analog level judgement */
	g_doc_analog_level_judgement_threshold = ANALOG_LEVEL_JUDGEMENT_CH21_THRESHOLD;
}

/*******************************************************************************************************************//**
 * @brief Open and enable ELC module
 * @param None
 * @retval None
 **********************************************************************************************************************/
void elc_initialize(void)
{
	fsp_err_t err = FSP_SUCCESS;

	/* Open the ELC module */
	err = R_ELC_Open(&g_elc_ctrl, &g_elc_cfg);
	if(err != FSP_SUCCESS)
	{
	    pin_state_change(ERROR_LED, PIN_HIGH__LED_ON);
		__BKPT(0);
	}

	/* Enable the ELC */
	err = R_ELC_Enable(&g_elc_ctrl);
	if(err != FSP_SUCCESS)
	{
	    pin_state_change(ERROR_LED, PIN_HIGH__LED_ON);
		__BKPT(0);
	}
}

/*******************************************************************************************************************//**
 * @brief Open AGT0/1 module, and reset period if DEBUG_FAST_CYCLE macro enabled
 * @param None
 * @retval None
 **********************************************************************************************************************/
void agt_initialize(void)
{
	fsp_err_t err = FSP_SUCCESS;

	/* Open the AGT0 module */
	err = R_AGT_Open(&g_agt_timer0_ctrl, &g_agt_timer0_cfg);
    if(err != FSP_SUCCESS)
    {
        pin_state_change(ERROR_LED, PIN_HIGH__LED_ON);
        __BKPT(0);
    }

	/* Open the AGT1 module */
	err = R_AGT_Open(&g_agt_timer1_ctrl, &g_agt_timer1_cfg);
	if(err != FSP_SUCCESS)
	{
	    pin_state_change(ERROR_LED, PIN_HIGH__LED_ON);
		__BKPT(0);
	}

#ifdef DEBUG_FAST_CYCLE
    /* Set a 30 second value for AGTW1 and 24 minute for AGTW0 in fast cycle debugging */
    uint32_t agt_count_value = (uint32_t)(g_agt_timer0_cfg.period_counts / 60);
    err |= R_AGT_PeriodSet(&g_agt_timer0_ctrl, agt_count_value);
    agt_count_value = (uint32_t)(g_agt_timer1_cfg.period_counts / 60);
    err |= R_AGT_PeriodSet(&g_agt_timer1_ctrl, agt_count_value);
    if(err != FSP_SUCCESS)
    {
        pin_state_change(ERROR_LED, PIN_HIGH__LED_ON);
        __BKPT(0);
    }
#endif
}

/*******************************************************************************************************************//**
 * @brief Start AGT0/1 module
 * @param None
 * @retval None
 **********************************************************************************************************************/
void agt_start(void)
{
	fsp_err_t err = FSP_SUCCESS;

	/* Start the AGT1 counting */
	err = R_AGT_Start(&g_agt_timer1_ctrl);
	if(err != FSP_SUCCESS)
	{
	    pin_state_change(ERROR_LED, PIN_HIGH__LED_ON);
		__BKPT(0);
	}

	/* Start the AGT0 counting */
	err = R_AGT_Start(&g_agt_timer0_ctrl);
    if(err != FSP_SUCCESS)
    {
        pin_state_change(ERROR_LED, PIN_HIGH__LED_ON);
        __BKPT(0);
    }
}

/*******************************************************************************************************************//**
 * @brief Change pin state
 * @param (pin) pin information
 * @param (state) state to be changed
 * @retval None
 **********************************************************************************************************************/
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

/*******************************************************************************************************************//**
 * @brief Check status of all flags
 * @param None
 * @retval CLEAR_FLAG All flags cleared
 * @retval SET_FLAG   Not all flags cleared
 **********************************************************************************************************************/
bool all_flags_clearing_check(void){

    /* Check status of all flags */
    if((g_manual_lpm_cancel_flag == CLEAR_FLAG) && (g_level_jedgement_result_flag == CLEAR_FLAG) && (g_timer_lpm_cancel_flag == CLEAR_FLAG))
    {
        return CLEAR_FLAG;
    }
    else
    {
        return SET_FLAG;
    }
}

/*******************************************************************************************************************//**
 * @brief Change ADC scan start event in ELC module
 * @param (event) event value to be changed
 * @retval None
 **********************************************************************************************************************/
void adc_scan_start_event_change(elc_event_t event)
{
	fsp_err_t err = FSP_SUCCESS;

	/* Set a ELC link */
	err = R_ELC_LinkSet(&g_elc_ctrl, ELC_PERIPHERAL_ADC0, event);
	if(err != FSP_SUCCESS){
	    pin_state_change(ERROR_LED, PIN_HIGH__LED_ON);
		__BKPT(0);
	}

	/* Enable the ELC */
	err = R_ELC_Enable(&g_elc_ctrl);
	if(err != FSP_SUCCESS){
	    pin_state_change(ERROR_LED, PIN_HIGH__LED_ON);
		__BKPT(0);
	}
}
