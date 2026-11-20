/***********************************************************************************************************************
 * File Name    : lin_slave_ep.h
 * Description  : Contains macros, data structures, and functions used in lin_slave_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2025 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef LIN_SLAVE_EP_H_
#define LIN_SLAVE_EP_H_

#include <stdbool.h>
#include "hal_data.h"
#include "console/uart_console.h"

/* Common macros */
#define RESET_VALUE                           (0x00)
#define NULL_CHAR                             ('\0')

/* User command macros */
#define SLAVE_SET_BAUDRATE                    ('1')

#if BSP_PERIPHERAL_SAU_PRESENT
#define SLAVE_ENTER_LPM                       ('2')
#endif /* BSP_PERIPHERAL_SAU_PRESENT */

/* Baud rate macros */
#define MIN_BAUDRATE_INDEX                    (1U)
#define MAX_BAUDRATE_INDEX                    (6U)
#define LIN_BAUDRATE_COUNT                    (6U)
#define LIN_DEFAULT_BAUDRATE                  (19200U)

/* LIN frame ID macros */
#define READ_FRAME_ID_10H                     (0x10U)
#define READ_FRAME_ID_11H                     (0x11U)
#define READ_FRAME_ID_12H                     (0x12U)
#define READ_FRAME_ID_13H                     (0x13U)
#define READ_FRAME_ID_20H                     (0x20U)
#define READ_FRAME_ID_21H                     (0x21U)
#define READ_FRAME_ID_22H                     (0x22U)
#define READ_FRAME_ID_23H                     (0x23U)

/* LIN frame data length macros */
#define FRAME_ID_10H_DATA_LENGTH              (2U)
#define FRAME_ID_11H_DATA_LENGTH              (2U)
#define FRAME_ID_12H_DATA_LENGTH              (2U)
#define FRAME_ID_13H_DATA_LENGTH              (2U)
#define FRAME_ID_20H_DATA_LENGTH              (3U)
#define FRAME_ID_21H_DATA_LENGTH              (3U)
#define FRAME_ID_22H_DATA_LENGTH              (3U)
#define FRAME_ID_23H_DATA_LENGTH              (3U)

/* LIN transfer index macros */
#define FRAME_ID_10H_INDEX                    (0U)
#define FRAME_ID_11H_INDEX                    (1U)
#define FRAME_ID_12H_INDEX                    (2U)
#define FRAME_ID_13H_INDEX                    (3U)
#define FRAME_ID_20H_INDEX                    (4U)
#define FRAME_ID_21H_INDEX                    (5U)
#define FRAME_ID_22H_INDEX                    (6U)
#define FRAME_ID_23H_INDEX                    (7U)

#define MIN_INDEX_OF_FRAME_ID_READ            (0U)
#define MAX_INDEX_OF_FRAME_ID_READ            (3U)
#define MIN_INDEX_OF_FRAME_ID_WRITE           (4U)
#define MAX_INDEX_OF_FRAME_ID_WRITE           (7U)

/* LIN transfer macros */
#define MAX_NUMBER_OF_FRAME_ID                (8U)
#define MAX_TRANSFER_LENGTH                   (8U)
#define TIMEOUT_LIMIT                         (1000000U)
#define LIN_PID_MASK_ID                       (0x3FU)

/* LIN API mapping */
#if BSP_PERIPHERAL_SAU_PRESENT

#define LIN_OPEN                              (R_SAU_LIN_Open)
#define LIN_WRITE                             (R_SAU_LIN_Write)
#define LIN_READ                              (R_SAU_LIN_Read)
#define LIN_COMMUNICATION_ABORT               (R_SAU_LIN_CommunicationAbort)
#define LIN_BAUD_CALCULATE                    (R_SAU_UART_BaudCalculate)
#define LIN_CLOSE                             (R_SAU_LIN_Close)

#elif BSP_PERIPHERAL_SCI_B_PRESENT || BSP_PERIPHERAL_SCI_PRESENT

#if BSP_PERIPHERAL_SCI_PRESENT

#define LIN_OPEN                              (R_SCI_LIN_Open)
#define LIN_WRITE                             (R_SCI_LIN_Write)
#define LIN_READ                              (R_SCI_LIN_Read)
#define LIN_COMMUNICATION_ABORT               (R_SCI_LIN_CommunicationAbort)
#define LIN_BAUD_CALCULATE                    (R_SCI_LIN_BaudCalculate)
#define LIN_CLOSE                             (R_SCI_LIN_Close)

#elif BSP_PERIPHERAL_SCI_B_PRESENT

#define LIN_OPEN                              (R_SCI_B_LIN_Open)
#define LIN_WRITE                             (R_SCI_B_LIN_Write)
#define LIN_READ                              (R_SCI_B_LIN_Read)
#define LIN_COMMUNICATION_ABORT               (R_SCI_B_LIN_CommunicationAbort)
#define LIN_BAUD_CALCULATE                    (R_SCI_B_LIN_BaudCalculate)
#define LIN_CLOSE                             (R_SCI_B_LIN_Close)

#endif /* BSP_PERIPHERAL_SCI_PRESENT */

#endif /* BSP_PERIPHERAL_SAU_PRESENT || BSP_PERIPHERAL_SCI_B_PRESENT || BSP_PERIPHERAL_SCI_PRESENT */

/* LIN frame expectation structure */
typedef struct
{
    uint8_t         id;
    const uint8_t * p_expected;
    size_t          length;
} lin_frame_expectation_t;

/* External variables defined in lin_slave_ep.c */
extern uint32_t g_slave_baudrate;
extern uint8_t g_slave_rx_buf[MAX_TRANSFER_LENGTH];
extern volatile uint8_t g_received_id;
extern volatile uint32_t g_lin_event_flags;

extern lin_transfer_params_t g_lin_slave_transfer_info[MAX_NUMBER_OF_FRAME_ID];

#if BSP_PERIPHERAL_SAU_PRESENT
extern sau_uart_baudrate_setting_t g_uart0_baud_setting;
extern sau_uart_baudrate_setting_t g_slave_break_field_baud_setting;
#endif /* BSP_PERIPHERAL_SAU_PRESENT */

/* Public function declarations */
fsp_err_t lin_slave_open(void);
void lin_slave_close(void);

fsp_err_t lin_slave_receive(uint8_t frame_id_index);
fsp_err_t lin_slave_transmit(uint8_t frame_id_index);
fsp_err_t lin_slave_baudset(uint32_t baud_rate);

int32_t lin_slave_find_transfer_index(uint8_t frame_id);

#if BSP_PERIPHERAL_SAU_PRESENT
fsp_err_t lin_slave_lpm_open(void);
void lin_slave_lpm_close(void);
fsp_err_t lin_slave_sleep_enter(void);
fsp_err_t lin_slave_sleep_exit(void);
#endif /* BSP_PERIPHERAL_SAU_PRESENT */

#endif /* LIN_SLAVE_EP_H_ */
