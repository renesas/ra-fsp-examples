/***********************************************************************************************************************
 * File Name    : lin_master_ep.h
 * Description  : Contains macros data structures and functions used in lin_master_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2025 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef LIN_MASTER_EP_H_
#define LIN_MASTER_EP_H_

#include <stdbool.h>
#include <string.h>
#include "hal_data.h"

/* Macro for the reset value */
#define RESET_VALUE                         (0x00)

/* Macros for user command */
#define MASTER_SET_BAUDRATE                 ('1')
#define MASTER_WRITE                        ('2')
#define MASTER_READ                         ('3')
#if BSP_PERIPHERAL_SAU_PRESENT
#define WAKE_UP_SLAVE                       ('4')
#endif
#define MIN_BAUDRATE_INDEX                  (1U)
#define MAX_BAUDRATE_INDEX                  (6U)
#define MIN_NUMBER_OF_RX_FRAME_ID           (0U)
#define MAX_NUMBER_OF_RX_FRAME_ID           (4U)
#define EXIT_INDEX                          (0U)
#define MAX_NUMBER_OF_TX_FRAME_ID           (4U)

/* Macros for frame ID */
#define ENABLE_MASTER_TIMEOUT_MANAGEMENT    (1U)
#define WRITE_FRAME_ID_20H                  (0x20)
#define WRITE_FRAME_ID_21H                  (0x21)
#define WRITE_FRAME_ID_22H                  (0x22)
#define WRITE_FRAME_ID_23H                  (0x23)

#define READ_FRAME_ID_10H                   (0x10)
#define READ_FRAME_ID_11H                   (0x11)
#define READ_FRAME_ID_12H                   (0x12)
#define READ_FRAME_ID_13H                   (0x13)

#define FRAME_ID_20H_DATA_LENGTH            (3)
#define FRAME_ID_21H_DATA_LENGTH            (3)
#define FRAME_ID_22H_DATA_LENGTH            (3)
#define FRAME_ID_23H_DATA_LENGTH            (3)

#define FRAME_ID_10H_DATA_LENGTH            (2)
#define FRAME_ID_11H_DATA_LENGTH            (2)
#define FRAME_ID_12H_DATA_LENGTH            (2)
#define FRAME_ID_13H_DATA_LENGTH            (2)

#define FRAME_ID_20H_INDEX                  (0)
#define FRAME_ID_21H_INDEX                  (1)
#define FRAME_ID_22H_INDEX                  (2)
#define FRAME_ID_23H_INDEX                  (3)

#define MAX_TRANSFER_LENGTH                 (8)
#define LIN_BAUDRATE_COUNT                  (8)
#define LIN_TIMEOUT_FACTOR                  (14U)
#define LIN_TIMEOUT_DIVISOR                 (10U)
#define LIN_DEFAULT_BAUDRATE                (19200)
#define IS_WRITE                            (1U)
#define IS_READ                             (0U)

#define TIMEOUT_LIMIT                       (1000000U)
#define LIN_PID_MASK_ID                     (0x3FU)    /* Mask to extract 6-bit LIN ID from PID */

#if BSP_PERIPHERAL_SAU_PRESENT

/* LIN API mapping */
#define LIN_OPEN                            (R_SAU_LIN_Open)
#define LIN_WRITE                           (R_SAU_LIN_Write)
#define LIN_COMMUNICATION_ABORT             (R_SAU_LIN_CommunicationAbort)
#define LIN_READ                            (R_SAU_LIN_Read)
#define LIN_BAUD_CALCULATE                  (R_SAU_UART_BaudCalculate)
#define LIN_CLOSE                           (R_SAU_LIN_Close)

/* Timer API mapping */         
#define TIMER_OPEN                          (R_TAU_Open)
#define TIMER_RESET                         (R_TAU_Reset)
#define TIMER_PERIOD_SET                    (R_TAU_PeriodSet)
#define TIMER_INFO_GET                      (R_TAU_InfoGet)
#define TIMER_START                         (R_TAU_Start)
#define TIMER_STOP                          (R_TAU_Stop)
#define TIMER_CLOSE                         (R_TAU_Close)

#elif BSP_PERIPHERAL_SCI_B_PRESENT || BSP_PERIPHERAL_SCI_PRESENT

/* Timer API mapping */
#define TIMER_OPEN                          (R_GPT_Open)
#define TIMER_RESET                         (R_GPT_Reset)
#define TIMER_PERIOD_SET                    (R_GPT_PeriodSet)
#define TIMER_INFO_GET                      (R_GPT_InfoGet)
#define TIMER_START                         (R_GPT_Start)
#define TIMER_STOP                          (R_GPT_Stop)
#define TIMER_CLOSE                         (R_GPT_Close)

/* LIN API mapping */
#if BSP_PERIPHERAL_SCI_PRESENT
#define LIN_OPEN                            (R_SCI_LIN_Open)
#define LIN_WRITE                           (R_SCI_LIN_Write)
#define LIN_COMMUNICATION_ABORT             (R_SCI_LIN_CommunicationAbort)
#define LIN_BAUD_CALCULATE                  (R_SCI_LIN_BaudCalculate)
#define LIN_READ                            (R_SCI_LIN_Read)
#define LIN_CLOSE                           (R_SCI_LIN_Close)

#elif BSP_PERIPHERAL_SCI_B_PRESENT
#define LIN_OPEN                            (R_SCI_B_LIN_Open)
#define LIN_WRITE                           (R_SCI_B_LIN_Write)
#define LIN_COMMUNICATION_ABORT             (R_SCI_B_LIN_CommunicationAbort)
#define LIN_BAUD_CALCULATE                  (R_SCI_B_LIN_BaudCalculate)
#define LIN_READ                            (R_SCI_B_LIN_Read)
#define LIN_CLOSE                           (R_SCI_B_LIN_Close)
#endif          
#endif /* BSP_PERIPHERAL_SAU_PRESENT || BSP_PERIPHERAL_SCI_PRESENT || BSP_PERIPHERAL_SCI_B_PRESENT */

typedef struct
{
    uint32_t header_timeout_us;   /* Header timeout in microseconds */
    uint32_t response_timeout_us; /* Response timeout in microseconds */
} lin_timing_t;

typedef struct
{
    uint8_t id;
    const uint8_t * p_expected;
    size_t length;
} lin_frame_expectation_t;

/* External variables - Defined in lin_master_ep.c */
extern uint32_t g_baudrate;
extern uint8_t g_master_rx_buf[MAX_TRANSFER_LENGTH];
extern volatile uint8_t g_received_id;
extern lin_transfer_params_t g_lin_master_tx_transfer_info[MAX_NUMBER_OF_TX_FRAME_ID];
extern lin_transfer_params_t g_lin_master_tx_header_transfer[MAX_NUMBER_OF_RX_FRAME_ID];
extern lin_transfer_params_t g_lin_master_rx_transfer_info[MAX_NUMBER_OF_RX_FRAME_ID];

#if BSP_PERIPHERAL_SAU_PRESENT
extern sau_uart_baudrate_setting_t g_uart0_baud_setting;
extern sau_uart_baudrate_setting_t g_master_break_field_baud_setting;
#endif /* BSP_PERIPHERAL_SAU_PRESENT */

/* Function declarations */
fsp_err_t lin_master_open(void);
void lin_master_close(void);
fsp_err_t lin_master_transmit(uint8_t frame_id_index);
fsp_err_t lin_master_receive(uint8_t frame_id_index);
fsp_err_t lin_master_baudset(uint32_t baud_rate);

#if ENABLE_MASTER_TIMEOUT_MANAGEMENT
fsp_err_t timer_open(void);
void timer_close(void);
#endif /* ENABLE_MASTER_TIMEOUT_MANAGEMENT */

#if BSP_PERIPHERAL_SAU_PRESENT
fsp_err_t lin_master_send_wakeup(void);
#endif /* BSP_PERIPHERAL_SAU_PRESENT */

#endif /* LIN_MASTER_EP_H_ */
