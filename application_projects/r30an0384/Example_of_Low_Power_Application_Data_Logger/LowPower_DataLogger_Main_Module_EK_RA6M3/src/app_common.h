/*
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/

#include "hal_data.h"

/* SEGGER RTT and error related headers */
#include "SEGGER_RTT/SEGGER_RTT.h"

//#define DISABLE_SENSOR_DEPENDENCE

#define SEGGER_INDEX            (0)
#define APP_PRINT(fn_, ...)      SEGGER_RTT_printf (SEGGER_INDEX,(fn_), ##__VA_ARGS__);

#define SET_FLAG                (0x1)
#define CLEAR_FLAG              (0x0)

#define PIN_HIGH__LED_ON        (BSP_IO_LEVEL_HIGH)
#define PIN_LOW__LED_OFF        (BSP_IO_LEVEL_LOW)

#define DATAFULL_PIN            (BSP_IO_PORT_08_PIN_05)
#define ANALOG_LEVEL_CH00_PIN   (BSP_IO_PORT_08_PIN_04)
#define ANALOG_LEVEL_CH01_PIN   (BSP_IO_PORT_08_PIN_03)
#define WAKE_UP_PIN             (BSP_IO_PORT_07_PIN_08)

#define USER_BUTTON_S1          (BSP_IO_PORT_00_PIN_09)
#define USER_BUTTON_S2          (BSP_IO_PORT_00_PIN_08)

#define LED1_BLUE               (BSP_IO_PORT_04_PIN_03)
#define LED2_GREEN              (BSP_IO_PORT_04_PIN_00)
#define LED3_RED                (BSP_IO_PORT_01_PIN_00)

#define PACKET_FORMAT_SIZE_DATA_TIME            (10u)
#define PACKET_FORMAT_SIZE_DATA_LIGHT           (4u)
#define PACKET_FORMAT_SIZE_DATA_TEMP            (4u)
#define PACKET_FORMAT_SIZE_DATA_TEMP_A_U        (2u)
#define PACKET_FORMAT_SIZE_DELIMITER_CODE       (1u)
#define PACKET_FORMAT_SIZE_START_CODE           (1u)
#define PACKET_FORMAT_SIZE_ATTRIBUTE_CODE       (1u)
#define PACKET_FORMAT_SIZE_DATA_LENGTH_CODE     (4u)
#define PACKET_FORMAT_SIZE_END_CODE             (1u)

extern volatile bool g_button_s1_pushed;
extern volatile bool g_button_s2_pushed;

extern volatile bool g_sci0_uart_transmit_complete_flag;
extern volatile bool g_sci0_uart_recv_flag;

extern void app_main(void);
