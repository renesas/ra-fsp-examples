
#include "hal_data.h"

//#define DEBUG_FAST_CYCLE
//#define DISABLE_SENSOR_DEPENDENCE

#define SET_FLAG									(0x1)
#define CLEAR_FLAG									(0x0)

#define RAW_DATA_BUFF_SIZE							(120u)
#define PROCESSED_DATA_BUFF_SIZE					(50u)

#define RTC_ALARM_VALUE_1HOUR						(3600u)
#define RTC_ALARM_VALUE_1MIN						(60u)

#define ADC0_CH00_DATA_REGISTER						(R_ADC0->ADDR[0])
#define ADC0_CH01_DATA_REGISTER						(R_ADC0->ADDR[1])

#ifndef DISABLE_SENSOR_DEPENDENCE
#define ANALOG_LEVEL_JUDGEMENT_CH00_THRESHOLD		(1585u)
#define ANALOG_LEVEL_JUDGEMENT_CH01_THRESHOLD		(2287u)
#else
#define ANALOG_LEVEL_JUDGEMENT_CH00_THRESHOLD		(2500u)
#define ANALOG_LEVEL_JUDGEMENT_CH01_THRESHOLD		(2500u)
#endif

#define DOC_DATA_INPUT_REGISTER						(R_DOC->DODIR)
#define DOC_DATA_SETTING_REGISTER					(R_DOC->DODSR)

#define LED1_BLUE									(BSP_IO_PORT_05_PIN_03)
#define LED2_GREEN									(BSP_IO_PORT_05_PIN_04)
#define LED3_RED									(BSP_IO_PORT_05_PIN_05)

#define DATAFULL_PIN								(BSP_IO_PORT_01_PIN_06)
#define ANALOG_LEVEL_CH00_PIN						(BSP_IO_PORT_01_PIN_05)
#define ANALOG_LEVEL_CH01_PIN						(BSP_IO_PORT_02_PIN_08)
#define WAKE_UP_PIN									(BSP_IO_PORT_04_PIN_02)

#define PIN_HIGH__LED_ON							(BSP_IO_LEVEL_HIGH)
#define PIN_LOW__LED_OFF							(BSP_IO_LEVEL_LOW)

#define CONVERT_UNIT_LIGHT_LIST_SIZE				(9u)
#define ADC_READ_VALUE_MAX_HEX						(0xFFF)
#define ADC_READ_VALUE_MAX_DEC						(4095.0)
#define GROVE_TEMP_SENSOR_RESISTANCE0				(10000.0)
#define B_CONSTANT									(4250.0)

#define TEMP_KELVIN_VALUE_OF_25_DEGREE_CELSIUS		(298.15)
#define TEMP_KELVIN_VALUE_OF_0_DEGREE_CELSIUS		(273.15)
#define TEMP_READ_DATA_VALUE_OF_0_DEGREE_CELSIUS	(874u)
#define TEMP_READ_DATA_VALUE_OF_99_DEGREE_CELSIUS	(3874u)

typedef struct _processed_data_t
{
	uint32_t time;
	float light_ave;
	float light_min;
	float light_max;
	float temp_ave;
	float temp_min;
	float temp_max;
}processed_data_t;

extern volatile bool g_1h_flag;
extern volatile bool g_wakeup_flag;
extern volatile bool g_manual_lpm_cancel_flag;
extern volatile bool g_uart_recv_flag;
extern volatile bool g_uart_transmit_complete_flag;
extern volatile bool g_datafull_flag;

extern uint16_t g_raw_light_data_buff[RAW_DATA_BUFF_SIZE];
extern uint16_t g_raw_temp_data_buff[RAW_DATA_BUFF_SIZE];

extern processed_data_t g_processed_data_buff[PROCESSED_DATA_BUFF_SIZE];
extern uint8_t g_processed_data_buff_addr;

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
extern void ioport_eventout_initialize(void);
extern void agt_initialize(void);
extern void rtc_initialize(void);

extern void agt_start(void);
extern void pin_state_change(bsp_io_port_pin_t pin, bsp_io_level_t state);
extern void data_output(void);
extern void data_processing(void);
extern float convert_unit_light(uint16_t input);
extern float convert_unit_temp(uint16_t input);
extern void processed_data_buff_clear(processed_data_t* buff, uint16_t buff_size);
extern uint32_t rtc_current_time_get(void);
extern void rtc_alarm_set(void);
extern void adc_scan_start_event_change(elc_event_t event);
