/***********************************************************************************************************************
 * File Name    : app_common.h
 * Description  : Contains data structures and functions used in app_common.h.
 **********************************************************************************************************************/

/**********************************************************************************************************************
* Copyright (c) 2022 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*********************************************************************************************************************/

#define DEBUG_FAST_CYCLE

#define SET_FLAG									(0x1)
#define CLEAR_FLAG									(0x0)

#define RAW_DATA_BUFF_SIZE							(96u) /* 2 times per hour * 24 hours * double buffer */

#define ADC0_CH19_DATA_REGISTER                     (R_ADC0->ADDR[19])
#define ADC0_CH21_DATA_REGISTER                     (R_ADC0->ADDR[21])

#define ANALOG_LEVEL_JUDGEMENT_CH21_THRESHOLD       (2287u) // = 30°C

#define DOC_DATA_INPUT_REGISTER						(R_DOC->DODIR)
#define DOC_DATA_SETTING_REGISTER					(R_DOC->DODSR)

#define LED                                         (BSP_IO_PORT_09_PIN_14)
#define ERROR_LED                                   (BSP_IO_PORT_00_PIN_15)

#define PIN_HIGH__LED_ON							(BSP_IO_LEVEL_HIGH)
#define PIN_LOW__LED_OFF							(BSP_IO_LEVEL_LOW)

#define SEND_AT_24H_PERIOD                          (0x1)
#define SEND_AT_MANUAL_TIMING                       (0x2)
#define SEND_LEVEL_JUDGEMENT_RESULT                 (0x3)

extern volatile bool g_level_jedgement_result_flag;
extern volatile bool g_manual_lpm_cancel_flag;
extern volatile bool g_uart_transmit_complete_flag;
extern volatile bool g_timer_lpm_cancel_flag;

extern uint16_t g_raw_light_data_buff[RAW_DATA_BUFF_SIZE];
extern uint16_t g_raw_temp_data_buff[RAW_DATA_BUFF_SIZE];

extern const transfer_instance_t g_dtc_data_transfer;
extern dtc_instance_ctrl_t g_dtc_data_transfer_ctrl;
extern const transfer_cfg_t g_dtc_data_transfer_cfg;

extern void app_main(void);

extern void adc_initialize(void);
extern void lpm_initialize(void);
extern void icu_initialize(void);
extern void dtc_initialize(void);
extern void doc_initialize(void);
extern void elc_initialize(void);
extern void agt_initialize(void);

extern void agt_start(void);
extern void pin_state_change(bsp_io_port_pin_t pin, bsp_io_level_t state);
extern bool all_flags_clearing_check(void);
extern void data_output(uint8_t send_mode);
extern void adc_scan_start_event_change(elc_event_t event);
