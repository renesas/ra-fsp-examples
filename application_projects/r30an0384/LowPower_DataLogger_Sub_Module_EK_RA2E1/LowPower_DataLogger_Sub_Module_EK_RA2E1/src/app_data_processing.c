
#include "hal_data.h"
#include "app_common.h"
#include "app_data_processing.h"

static uint8_t past_transfer_addr = 0x0;

// Prototype declaration
void data_processing(void);
void calculate_ave_min_max(uint16_t* raw_light_buff, uint16_t* raw_temp_buff, uint8_t p_transfer_addr, uint8_t c_transfer_addr, unit_unconvert_processed_data_t* output);
uint8_t	current_dtc_transfer_addr_get(void);
void store_time_data_to_buff(processed_data_t* buff, uint8_t buff_addr);
void store_processed_data_to_buff(processed_data_t* store_data, processed_data_t*  buff, uint8_t buff_addr);
void store_unconvert_processed_data_to_buff(unit_unconvert_processed_data_t* store_data, processed_data_t*  buff, uint8_t buff_addr);
void convert_unit(unit_unconvert_processed_data_t* input, processed_data_t* output);

void data_processing(void)
{
	/* Local variables */
	uint8_t current_transfer_addr;
	unit_unconvert_processed_data_t	unconvert_processed_data;
#ifndef DISABLE_SENSOR_DEPENDENCE
	processed_data_t converted_processed_data;
#endif

	/* Store the time data */
	store_time_data_to_buff(&g_processed_data_buff[0], g_processed_data_buff_addr);

	/* Get current DTC transfer address */
	current_transfer_addr = current_dtc_transfer_addr_get();

	/* Calculate average/minimum/maximum data */
	calculate_ave_min_max(&g_raw_light_data_buff[0], &g_raw_temp_data_buff[0], past_transfer_addr, current_transfer_addr, &unconvert_processed_data);

#ifndef DISABLE_SENSOR_DEPENDENCE
	/* Convert unit */
	convert_unit(&unconvert_processed_data, &converted_processed_data);

	/* Store the sensor data */
	store_processed_data_to_buff(&converted_processed_data, &g_processed_data_buff[0], g_processed_data_buff_addr);

#else
	/* Store the sensor data */
	store_unconvert_processed_data_to_buff(&unconvert_processed_data, &g_processed_data_buff[0], g_processed_data_buff_addr);
#endif

	/* Update the processed data buff address */
	g_processed_data_buff_addr++;

	/* Data full detection */
	if(g_processed_data_buff_addr == PROCESSED_DATA_BUFF_SIZE)
	{
		/* Set a flag */
		g_datafull_flag = SET_FLAG;

		/* Clear the processed data buff address */
		g_processed_data_buff_addr = 0;

		/* Set High level */
		pin_state_change(DATAFULL_PIN, PIN_HIGH__LED_ON);
	}
	/* Update the past DTC transfer address */
	past_transfer_addr = current_transfer_addr;

	/* Set next rtc alarm */
	rtc_alarm_set();
}

void calculate_ave_min_max(uint16_t* raw_light_buff, uint16_t* raw_temp_buff, uint8_t p_transfer_addr, uint8_t c_transfer_addr, unit_unconvert_processed_data_t* output)
{
	/* Local variables */
	uint32_t light_sum = 0;
	uint16_t light_min = 0xFFFF;
	uint16_t light_max = 0;
	uint32_t temp_sum = 0;
	uint16_t temp_min = 0xFFFF;
	uint16_t temp_max = 0;

	uint8_t c_buff_read_point = p_transfer_addr;
	uint8_t data_count = 0;

	while(1)
	{
		//  sum
		light_sum += raw_light_buff[c_buff_read_point];
		temp_sum += raw_temp_buff[c_buff_read_point];

		// min
		if(raw_light_buff[c_buff_read_point] < light_min)
		{
			light_min = raw_light_buff[c_buff_read_point];
		}
		if(raw_temp_buff[c_buff_read_point] < temp_min)
		{
			temp_min = raw_temp_buff[c_buff_read_point];
		}

		// max
		if(raw_light_buff[c_buff_read_point] > light_max)
		{
			light_max = raw_light_buff[c_buff_read_point];
		}
		if(raw_temp_buff[c_buff_read_point] > temp_max)
		{
			temp_max = raw_temp_buff[c_buff_read_point];
		}

		data_count++;

		c_buff_read_point++;
		if(c_buff_read_point == RAW_DATA_BUFF_SIZE)
		{
			c_buff_read_point = 0;
		}
		if(c_buff_read_point == c_transfer_addr)
		{
			break;
		}
	}

	output->light_ave = (uint16_t)(light_sum / data_count);
	output->light_min = light_min;
	output->light_max = light_max;

	output->temp_ave = (uint16_t)(temp_sum / data_count);
	output->temp_min = temp_min;
	output->temp_max = temp_max;
}

uint8_t	current_dtc_transfer_addr_get(void)
{
	fsp_err_t err = FSP_SUCCESS;

	/* Local variables */
	transfer_properties_t DTC_current_transfer_info;

	err = R_DTC_InfoGet(&g_dtc_data_transfer_ctrl, &DTC_current_transfer_info);
	if(err != FSP_SUCCESS)
	{
		__BKPT(0);
	}

	return (uint8_t)(RAW_DATA_BUFF_SIZE - DTC_current_transfer_info.transfer_length_remaining);
}

void store_time_data_to_buff(processed_data_t* buff, uint8_t buff_addr)
{
	/* Local variables */
	uint32_t a_RTC_current_time;

	/* Get current time */
	a_RTC_current_time = rtc_current_time_get();

	buff[buff_addr].time = a_RTC_current_time;
}

void store_processed_data_to_buff(processed_data_t* store_data, processed_data_t* buff, uint8_t buff_addr)
{
	buff[buff_addr].light_ave = store_data->light_ave;
	buff[buff_addr].light_min = store_data->light_min;
	buff[buff_addr].light_max = store_data->light_max;
	buff[buff_addr].temp_ave = store_data->temp_ave;
	buff[buff_addr].temp_min = store_data->temp_min;
	buff[buff_addr].temp_max = store_data->temp_max;
}

void store_unconvert_processed_data_to_buff(unit_unconvert_processed_data_t* store_data, processed_data_t* buff, uint8_t buff_addr)
{
	buff[buff_addr].light_ave = (float)(store_data->light_ave);
	buff[buff_addr].light_min = (float)(store_data->light_min);
	buff[buff_addr].light_max = (float)(store_data->light_max);
	buff[buff_addr].temp_ave = (float)(store_data->temp_ave);
	buff[buff_addr].temp_min = (float)(store_data->temp_min);
	buff[buff_addr].temp_max = (float)(store_data->temp_max);
}


void convert_unit(unit_unconvert_processed_data_t* input, processed_data_t* output)
{
	output->light_ave = convert_unit_light(input->light_ave);
	output->light_min = convert_unit_light(input->light_min);
	output->light_max = convert_unit_light(input->light_max);
	output->temp_ave = convert_unit_temp(input->temp_ave);
	output->temp_min = convert_unit_temp(input->temp_min);
	output->temp_max = convert_unit_temp(input->temp_max);
}

