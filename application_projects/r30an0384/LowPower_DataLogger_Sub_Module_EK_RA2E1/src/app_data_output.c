
#include "hal_data.h"
#include "stdio.h"
#include "app_common.h"
#include "app_data_output.h"

static uint8_t uart_recv_buff;
static volatile bool wakeup_pin_state;
static uint16_t packet_size;
static uint8_t uart_send_buff[SEND_BUFF_SIZE] = {0};

void data_output(void);
void data_send(uint8_t send_mode);
uint16_t  packet_create(uint8_t send_mode, processed_data_t* buff, uint8_t buff_addr, bool datafull_flag, uint16_t* analog_value_ch00, uint16_t* analog_value_ch01, uint8_t* send_buff);
bool wakeup_pin_state_check(void);

void data_output(void)
{
	fsp_err_t err = FSP_SUCCESS;

	/* Open the SCI module */
	err = R_SCI_UART_Open(&g_sci_uart0_ctrl, &g_sci_uart0_cfg);
	if(err != FSP_SUCCESS)
	{
		__BKPT(0);
	}

	/* Set SCI receive buff */
	err = R_SCI_UART_Read(&g_sci_uart0_ctrl, &uart_recv_buff, COMMAND_SIZE);
	if(err != FSP_SUCCESS)
	{
		__BKPT(0);
	}

	/* Send Wake up complete message */
	data_send(SEND_WAKEUP_COMPLETE);

	/* Check the state of wake up pin */
	wakeup_pin_state = wakeup_pin_state_check();

	while(wakeup_pin_state == PIN_HIGH__LED_ON)
	{
		if(g_uart_recv_flag == SET_FLAG){
			/* Clear the flag */
			g_uart_recv_flag = CLEAR_FLAG;

			switch(uart_recv_buff)
			{
			case COMMAND_ONLY_AVE:
				/* Send average only data */
				data_send(SEND_ONLY_AVE);

				processed_data_buff_clear(&g_processed_data_buff[0], PROCESSED_DATA_BUFF_SIZE);
				g_processed_data_buff_addr = 0;

				/* Clear the data full flag */
				g_datafull_flag = CLEAR_FLAG;
				pin_state_change(DATAFULL_PIN, PIN_LOW__LED_OFF);

				break;

			case COMMAND_ALL:
				/* Send all data */
				data_send(SEND_ALL);

				/* Clear the buffer and buffer address */
				processed_data_buff_clear(&g_processed_data_buff[0], PROCESSED_DATA_BUFF_SIZE);
				g_processed_data_buff_addr = 0;

				/* Clear the data full flag */
				g_datafull_flag = CLEAR_FLAG;
				pin_state_change(DATAFULL_PIN, PIN_LOW__LED_OFF);

				break;

			case COMMAND_RECV_ANA_LEVEL:
				/* Send current adc read value */
				data_send(SEND_CURRENT_ANA);

				/* Clear the analog level 1/2 pin */
				pin_state_change(ANALOG_LEVEL_CH00_PIN, PIN_LOW__LED_OFF);
				pin_state_change(ANALOG_LEVEL_CH01_PIN, PIN_LOW__LED_OFF);

				break;

			default:
				/* Send command error message */
				data_send(SEND_COMMAND_ERR);

				break;
			}

			/* Clear the uart receive buff */
			uart_recv_buff = 0x0;

			/* Set SCI receive buff for next transfer */
			err = R_SCI_UART_Read(&g_sci_uart0_ctrl, &uart_recv_buff, COMMAND_SIZE);
			if(err != FSP_SUCCESS)
			{
				__BKPT(0);
			}
		}

		/* Wait for 10ms */
		R_BSP_SoftwareDelay(10, BSP_DELAY_UNITS_MILLISECONDS);

		/* Check the state of wake up pin */
		wakeup_pin_state = wakeup_pin_state_check();
	}

	/* Close the SCI module */
	err = R_SCI_UART_Close(&g_sci_uart0_ctrl);
	if(err != FSP_SUCCESS)
	{
		__BKPT(0);
	}

}

void data_send(uint8_t send_mode)
{
	fsp_err_t err = FSP_SUCCESS;

	/* Local variables */
	uint16_t current_analog_value_ch00;
	uint16_t current_analog_value_ch01;

	/* Get Current ADC read value when SEND_CURRENT_ANA mode */
	if(send_mode == SEND_CURRENT_ANA)
	{
		current_analog_value_ch00 = ADC0_CH00_DATA_REGISTER;
		current_analog_value_ch01 = ADC0_CH01_DATA_REGISTER;
	}

	/* Create a packet */
	packet_size = packet_create(send_mode, &g_processed_data_buff[0], g_processed_data_buff_addr, g_datafull_flag,
			&current_analog_value_ch00, &current_analog_value_ch01, &uart_send_buff[0]);

	/* Send the packet */
	err = R_SCI_UART_Write(&g_sci_uart0_ctrl, &uart_send_buff[0], packet_size);
	if(err != FSP_SUCCESS) __BKPT(0);

	/* Wait for transmit complete */
	while(g_uart_transmit_complete_flag != SET_FLAG);
	g_uart_transmit_complete_flag = CLEAR_FLAG;
}

uint16_t  packet_create(uint8_t send_mode, processed_data_t* data_buff, uint8_t data_buff_addr, bool datafull_flag, uint16_t* analog_value_ch00, uint16_t* analog_value_ch01, uint8_t* send_buff)
{
	/* Local variables */
	uint8_t processed_buff_stored_data_size;
	uint16_t send_buff_addr;
	uint16_t packet_data_count;
	char ASCII_data[42];
	uint16_t ASCII_data_count;
	uint8_t p_buff_read_point;
	uint32_t s_RTC_current_time;

	if(g_datafull_flag == SET_FLAG)
	{
		processed_buff_stored_data_size = PROCESSED_DATA_BUFF_SIZE;
	}
	else
	{
		processed_buff_stored_data_size = data_buff_addr - 0x0;
	}

	send_buff_addr = 0;
	packet_data_count = 0;

	// Start code
	send_buff[send_buff_addr] = START_CODE;
	send_buff_addr = (uint16_t)(send_buff_addr + PACKET_FORMAT_SIZE_START_CODE);

	// Attribute code
	switch(send_mode)
	{
	case SEND_ONLY_AVE:
		send_buff[send_buff_addr] = ATTRIBUTE_ONLY_AVE;
		break;
	case SEND_ALL:
		send_buff[send_buff_addr] = ATTRIBUTE_ALL;
		break;
	case SEND_CURRENT_ANA:
		send_buff[send_buff_addr] = ATTRIBUTE_CURRENT_ANA;
		break;
	case SEND_WAKEUP_COMPLETE:
		send_buff[send_buff_addr] = ATTRIBUTE_WAKEUP_COMPLETE;
		break;
	case SEND_COMMAND_ERR:
		send_buff[send_buff_addr] = ATTRIBUTE_COMMAND_ERR;
		break;
	default:
		break;
	}
	send_buff_addr = (uint16_t)(send_buff_addr + PACKET_FORMAT_SIZE_ATTRIBUTE_CODE);

	// Length
	send_buff_addr = (uint16_t)(send_buff_addr + PACKET_FORMAT_SIZE_DATA_LENGTH_CODE);

	// Data
	switch(send_mode)
	{
	case SEND_ONLY_AVE:
	case SEND_ALL:
		// Number of data
		ASCII_data_count = (uint16_t)sprintf(ASCII_data, "%02d",
				processed_buff_stored_data_size);
		memcpy(send_buff + send_buff_addr, ASCII_data, ASCII_data_count);
		send_buff_addr = (uint16_t)(send_buff_addr + ASCII_data_count);
		packet_data_count = (uint16_t)(packet_data_count + ASCII_data_count);

		// DELIMITER_CODE_S + data
		if(datafull_flag == SET_FLAG)
		{
			p_buff_read_point = data_buff_addr;
		}
		else
		{
			p_buff_read_point = 0;
		}

		while(processed_buff_stored_data_size > 0)
		{
			if(SEND_ONLY_AVE == send_mode)
			{
#ifndef DISABLE_SENSOR_DEPENDENCE
				ASCII_data_count = (uint16_t)sprintf(ASCII_data, "/%010d,%04d,%04d",
						(int)data_buff[p_buff_read_point].time,
						(int)(data_buff[p_buff_read_point].light_ave+0.5),
						(int)(data_buff[p_buff_read_point].temp_ave*100+0.5));
#else
				ASCII_data_count = (uint16_t)sprintf(ASCII_data, "/%010d,%04d,%04d",
										(int)data_buff[p_buff_read_point].time,
										(int)data_buff[p_buff_read_point].light_ave,
										(int)data_buff[p_buff_read_point].temp_ave);
#endif
			}
			else if(SEND_ALL == send_mode)
			{
#ifndef DISABLE_SENSOR_DEPENDENCE
				ASCII_data_count = (uint16_t)sprintf(ASCII_data, "/%010d,%04d,%04d,%04d,%04d,%04d,%04d",
						(int)data_buff[p_buff_read_point].time,
						(int)(data_buff[p_buff_read_point].light_ave+0.5),
						(int)(data_buff[p_buff_read_point].light_min+0.5),
						(int)(data_buff[p_buff_read_point].light_max+0.5),
						(int)(data_buff[p_buff_read_point].temp_ave*100+0.5),
						(int)(data_buff[p_buff_read_point].temp_min*100+0.5),
						(int)(data_buff[p_buff_read_point].temp_max*100+0.5));
#else
				ASCII_data_count = (uint16_t)sprintf(ASCII_data, "/%010d,%04d,%04d,%04d,%04d,%04d,%04d",
						(int)data_buff[p_buff_read_point].time,
						(int)data_buff[p_buff_read_point].light_ave,
						(int)data_buff[p_buff_read_point].light_min,
						(int)data_buff[p_buff_read_point].light_max,
						(int)data_buff[p_buff_read_point].temp_ave,
						(int)data_buff[p_buff_read_point].temp_min,
						(int)data_buff[p_buff_read_point].temp_max);
#endif
			}
			memcpy(send_buff + send_buff_addr, ASCII_data, ASCII_data_count);
			send_buff_addr = (uint16_t)(send_buff_addr + ASCII_data_count);
			packet_data_count = (uint16_t)(packet_data_count + ASCII_data_count);

			p_buff_read_point++;

			if(p_buff_read_point == PROCESSED_DATA_BUFF_SIZE)
			{
				p_buff_read_point = 0;
			}
			processed_buff_stored_data_size--;
		}

		// Data length
		ASCII_data_count = (uint16_t)sprintf(ASCII_data, "%04d", packet_data_count);
		memcpy(send_buff+2, ASCII_data, ASCII_data_count);

		break;


	case SEND_CURRENT_ANA:

		s_RTC_current_time = rtc_current_time_get();

#ifndef DISABLE_SENSOR_DEPENDENCE
		ASCII_data_count = (uint16_t)sprintf(ASCII_data, "%010d,%04d,%04d",
				(int)s_RTC_current_time,
				(int)(convert_unit_light(*analog_value_ch00)+0.5),
				(int)(convert_unit_temp(*analog_value_ch01)*100+0.5));
#else
		ASCII_data_count = (uint16_t)sprintf(ASCII_data, "%010d,%04d,%04d",
				(int)s_RTC_current_time,
				(int)(*analog_value_ch00),
				(int)(*analog_value_ch01));
#endif

		memcpy(send_buff + send_buff_addr, ASCII_data, ASCII_data_count);
		send_buff_addr = (uint16_t)(send_buff_addr + ASCII_data_count);
		packet_data_count = (uint16_t)(packet_data_count + ASCII_data_count);

		// Data length
		ASCII_data_count = (uint16_t)sprintf(ASCII_data, "%04d", packet_data_count);
		memcpy(send_buff+2, ASCII_data, ASCII_data_count);

		break;

	case SEND_WAKEUP_COMPLETE:
	case SEND_COMMAND_ERR:
		send_buff[send_buff_addr] = SEND_DATA_SPACE;
		send_buff_addr = (uint16_t)(send_buff_addr + SEND_DATA_SIZE_SPACE);
		packet_data_count = (uint16_t)(packet_data_count + SEND_DATA_SIZE_SPACE);

		// Data length
		ASCII_data_count = (uint16_t)sprintf(ASCII_data, "%04d", packet_data_count);
		memcpy(send_buff+2, ASCII_data, ASCII_data_count);

		break;
	default:
		break;
	}

	send_buff[send_buff_addr] = END_CODE;
	send_buff_addr = (uint16_t)(send_buff_addr + PACKET_FORMAT_SIZE_END_CODE);

	return send_buff_addr;
}

bool wakeup_pin_state_check(void)
{
	fsp_err_t err = FSP_SUCCESS;

	/* Local variables */
	bsp_io_level_t wakeup_pin_read_data;

	err = R_IOPORT_PinRead(&g_ioport_ctrl, WAKE_UP_PIN, &wakeup_pin_read_data);
	if(err != FSP_SUCCESS) __BKPT(0);

	if(wakeup_pin_read_data == BSP_IO_LEVEL_HIGH)
	{
		return PIN_HIGH__LED_ON;
	}
	else
	{
		return PIN_LOW__LED_OFF;
	}
}
