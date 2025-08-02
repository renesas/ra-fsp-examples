/***********************************************************************************************************************
 * File Name    : app_data_output.c
 * Description  : Contains data structures and functions used in app_data_output.c.
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2022 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
************************************************************************************************************************/

#include "hal_data.h"
#include "stdio.h"
#include "app_common.h"
#include "app_data_output.h"

static uint16_t packet_size;
static uint8_t uart_send_buff[SEND_BUFF_SIZE] = {0};

static uint8_t past_transfer_addr = 0x0;

void data_output(uint8_t send_mode);
void data_send(uint8_t send_mode);
uint16_t  packet_create(uint8_t send_mode, uint16_t* raw_light_data_buff, uint16_t* raw_temp_data_buff, uint16_t* analog_value_ch00, uint16_t* analog_value_ch01, uint32_t* current_time, uint8_t* send_buff);
uint8_t current_dtc_transfer_addr_get(void);

/*******************************************************************************************************************//**
 * @brief data output
 * @param (send_mode) Mode to be sent
 * @retval None
 **********************************************************************************************************************/
void data_output(uint8_t send_mode)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Open the SCI module */
    err = R_SCI_UART_Open(&g_sci_uart0_ctrl, &g_sci_uart0_cfg);
    if (err != FSP_SUCCESS)
    {
        pin_state_change(ERROR_LED, PIN_HIGH__LED_ON);
        __BKPT(0);
    }

    /* Send data */
    data_send(send_mode);

    /* Close the SCI module */
    err = R_SCI_UART_Close(&g_sci_uart0_ctrl);
    if (err != FSP_SUCCESS)
    {
        pin_state_change(ERROR_LED, PIN_HIGH__LED_ON);
        __BKPT(0);
    }

}

/*******************************************************************************************************************//**
 * @brief data send
 * @param (send_mode) Mode to be sent
 * @retval None
 **********************************************************************************************************************/
void data_send(uint8_t send_mode)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Local variables */
    uint32_t rtc_current_time;
    uint16_t current_analog_value_ch00;
    uint16_t current_analog_value_ch01;

    /* Get current time */
    rtc_current_time = rtc_current_time_get();

    /* Get current ADC read value when SEND_CURRENT_ANA mode */
    if (send_mode == SEND_LEVEL_JUDGEMENT_RESULT)
    {
        current_analog_value_ch00 = ADC0_CH00_DATA_REGISTER;
        current_analog_value_ch01 = ADC0_CH01_DATA_REGISTER;
    }

    /* Create a packet */
    packet_size = packet_create(send_mode, &g_raw_light_data_buff[0], &g_raw_temp_data_buff[0],
                                &current_analog_value_ch00, &current_analog_value_ch01, &rtc_current_time, &uart_send_buff[0]);

    /* Send the packet */
    err = R_SCI_UART_Write(&g_sci_uart0_ctrl, &uart_send_buff[0], packet_size);
    if (err != FSP_SUCCESS)
    {
        pin_state_change(ERROR_LED, PIN_HIGH__LED_ON);
        __BKPT(0);
    }

    /* Wait for transmit complete */
    while (g_uart_transmit_complete_flag != SET_FLAG);
    g_uart_transmit_complete_flag = CLEAR_FLAG;
}

/*******************************************************************************************************************//**
 * @brief Create packet with various parameters
 * @param (send_mode)           Mode to be sent
 * @param (raw_light_data_buff) Address of raw light data buffer
 * @param (raw_temp_data_buff)  Address of raw temperature data buffer
 * @param (analog_value_ch00)   Address of ch0 analog value variable
 * @param (analog_value_ch01)   Address of ch1 analog value variable
 * @param (current_time)        Address of gotten current RTC time variable
 * @param (send_buff)           Address of send buffer
 * @retval (send_buff_addr)     Size of created packet data
 **********************************************************************************************************************/
uint16_t  packet_create(uint8_t send_mode, uint16_t* raw_light_data_buff, uint16_t* raw_temp_data_buff, uint16_t* analog_value_ch00, uint16_t* analog_value_ch01, uint32_t* current_time, uint8_t* send_buff)
{
    /* Local variables */
    uint16_t buff_data_count;
    uint8_t current_transfer_addr;
    uint16_t send_buff_addr;
    uint16_t packet_data_count;
    uint8_t buff_read_point;

    char ASCII_data[ASCII_DATA_SIZE];
    uint16_t ASCII_data_count;

    /* Initialize variables */
    send_buff_addr = 0;
    packet_data_count = 0;
    buff_data_count = 0;

    // Start code
    send_buff[send_buff_addr] = START_CODE;
    send_buff_addr = (uint16_t)(send_buff_addr + PACKET_FORMAT_SIZE_START_CODE);

    // Attribute code
    switch (send_mode)
    {
        case SEND_AT_24H_PERIOD:
            send_buff[send_buff_addr] = ATTRIBUTE_24H_PERIOD;
            break;
        case SEND_AT_MANUAL_TIMING:
            send_buff[send_buff_addr] = ATTRIBUTE_MANUAL_TIMING;
            break;
        case SEND_LEVEL_JUDGEMENT_RESULT:
            send_buff[send_buff_addr] = ATTRIBUTE_LEVEL_JUDGEMENT_RESULT;
            break;
        default:
            break;
    }
    send_buff_addr = (uint16_t)(send_buff_addr + PACKET_FORMAT_SIZE_ATTRIBUTE_CODE);

    // DELIMITER_CODE_C ','
    send_buff[send_buff_addr] = DELIMITER_CODE_C;
    send_buff_addr = (uint16_t)(send_buff_addr + PACKET_FORMAT_SIZE_DELIMITER_CODE);

    // Length
    send_buff_addr = (uint16_t)(send_buff_addr + PACKET_FORMAT_SIZE_DATA_LENGTH_CODE);

    // DELIMITER_CODE_C ','
    send_buff[send_buff_addr] = DELIMITER_CODE_C;
    send_buff_addr = (uint16_t)(send_buff_addr + PACKET_FORMAT_SIZE_DELIMITER_CODE);

    // Data
    switch(send_mode)
    {
        case SEND_AT_24H_PERIOD:
        case SEND_AT_MANUAL_TIMING:

            // Time
            ASCII_data_count = (uint16_t)sprintf(ASCII_data, "%010d",
                                                 (int)(*current_time));
            memcpy(send_buff + send_buff_addr, ASCII_data, ASCII_data_count);
            send_buff_addr = (uint16_t)(send_buff_addr + ASCII_data_count);
            packet_data_count = (uint16_t)(packet_data_count + ASCII_data_count);

            // DELIMITER_CODE_C ','
            send_buff[send_buff_addr] = DELIMITER_CODE_C;
            send_buff_addr = (uint16_t)(send_buff_addr + PACKET_FORMAT_SIZE_DELIMITER_CODE);
            packet_data_count = (uint16_t)(packet_data_count + PACKET_FORMAT_SIZE_DELIMITER_CODE);

            // Number of data
            send_buff_addr = (uint16_t)(send_buff_addr + PACKET_FORMAT_SIZE_DATA_NUMBER_CODE);
            packet_data_count = (uint16_t)(packet_data_count + PACKET_FORMAT_SIZE_DATA_NUMBER_CODE);

            /* Get current DTC transfer address */
            current_transfer_addr = current_dtc_transfer_addr_get();

            buff_read_point = past_transfer_addr;

            // Data
            while (buff_read_point != current_transfer_addr)
            {
                ASCII_data_count = (uint16_t)sprintf(ASCII_data, ",%04d/%04d",
                                                     (int)raw_light_data_buff[buff_read_point],
                                                     (int)raw_temp_data_buff[buff_read_point]);

                memcpy(send_buff + send_buff_addr, ASCII_data, ASCII_data_count);
                send_buff_addr = (uint16_t)(send_buff_addr + ASCII_data_count);
                packet_data_count = (uint16_t)(packet_data_count + ASCII_data_count);

                buff_data_count++;

                buff_read_point++;
                if (buff_read_point == RAW_DATA_BUFF_SIZE)
                {
                    buff_read_point = 0;
                }
            }

            if(send_mode == SEND_AT_24H_PERIOD){
                // Update past transfer address
                past_transfer_addr = current_transfer_addr;
            }

            // Number of data
            ASCII_data_count = (uint16_t)sprintf(ASCII_data, "%02d",
                                                 buff_data_count);
            memcpy(send_buff + PACKET_POSITION_DATA_NUMBER, ASCII_data, ASCII_data_count);

            // Data length
            ASCII_data_count = (uint16_t)sprintf(ASCII_data, "%04d", packet_data_count);
            memcpy(send_buff + PACKET_POSITION_DATA_LENGTH, ASCII_data, ASCII_data_count);

            break;

        case SEND_LEVEL_JUDGEMENT_RESULT:

            // Time + Data
            ASCII_data_count = (uint16_t)sprintf(ASCII_data, "%010d,%04d/%04d",
                                                 (int)(*current_time),
                                                 (int)(*analog_value_ch00),
                                                 (int)(*analog_value_ch01));

            memcpy(send_buff + send_buff_addr, ASCII_data, ASCII_data_count);
            send_buff_addr = (uint16_t)(send_buff_addr + ASCII_data_count);
            packet_data_count = (uint16_t)(packet_data_count + ASCII_data_count);

            // Data length
            ASCII_data_count = (uint16_t)sprintf(ASCII_data, "%04d", packet_data_count);
            memcpy(send_buff + PACKET_POSITION_DATA_LENGTH, ASCII_data, ASCII_data_count);

            break;

        default:
            break;
    }

    // End code
    send_buff[send_buff_addr] = END_CODE;
    send_buff_addr = (uint16_t)(send_buff_addr + PACKET_FORMAT_SIZE_END_CODE);

    // LF code
    send_buff[send_buff_addr] = LF_CODE;
    send_buff_addr = (uint16_t)(send_buff_addr + PACKET_FORMAT_SIZE_END_CODE);

    return send_buff_addr;
}

/*******************************************************************************************************************//**
 * @brief Get current DTC transfer address
 * @param None
 * @retval current DTC transfer address
 **********************************************************************************************************************/
uint8_t current_dtc_transfer_addr_get(void)
{
    fsp_err_t err = FSP_SUCCESS;

    /* Local variables */
    transfer_properties_t dtc_current_transfer_info;

    err = R_DTC_InfoGet(&g_dtc_data_transfer_ctrl, &dtc_current_transfer_info);
    if (err != FSP_SUCCESS)
    {
        pin_state_change(ERROR_LED, PIN_HIGH__LED_ON);
        __BKPT(0);
    }

    return (uint8_t)(RAW_DATA_BUFF_SIZE - dtc_current_transfer_info.transfer_length_remaining);
}
