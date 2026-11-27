/***********************************************************************************************************************
 * File Name    : can_fd_ep.h
 * Description  : Contains macros, variables, data structures, and functions used in can_fd_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef CAN_FD_EP_H_
#define CAN_FD_EP_H_

#include <stdbool.h>
#include "hal_data.h"

/* Macro for the reset value */
#define RESET_VALUE                     (0x00)

/* Macro for the null character */
#define NULL_CHAR                       ('\0')

/* Macros for CAN frame */
#define CAN_MAILBOX_NUMBER_0            (0U)               /* Mail box number */
#define CAN_CLASSIC_FRAME_DATA_BYTES    (8U)               /* Data length code for classic frame */
#define CAN_FD_DATA_LENGTH_CODE         (64)               /* Data length code for FD frame */
#define CAN_ID                          (0x1100)           /* ID to be updated in transmit frame */

/* Acceptance filter array parameters */
#define CANFD_FILTER_ID                 (0x00001000)
#define MASK_ID                         (0x1FFFF000)
#define MASK_ID_MODE                    (1)

/* Macro for the wait time value, in microseconds */
#define WAIT_TIME                       (10000U)

#if defined (BOARD_RA8M1_EK) || defined (BOARD_RA8D1_EK) || defined (BOARD_RA8T1_MCK) || defined (BOARD_RA8E1_FPB) ||\
    defined (BOARD_RA8M2_EK) || defined (BOARD_RA8D2_EK) || defined (BOARD_RA8E2_EK) || defined (BOARD_RA8P1_EK) ||\
    defined (BOARD_RA8T2_EK) || defined (BOARD_RA8T2_MCK)
#define SUPPORT_MAX_DATA_RATE_8MBPS
#else
#define SUPPORT_MAX_DATA_RATE_5MBPS
#endif /* Specific board */

/* Macro for the size of the command input buffer */
#define BUFFER_SIZE                     (16U)

/* Macros for log of CAN error events */
#define LOG_ERR_WARNING                 "Error Warning event"
#define LOG_ERR_PASSIVE                 "Error Passive event"
#define LOG_ERR_BUS_OFF                 "Bus Off event"
#define LOG_BUS_RECOVERY                "Bus Off Recovery event"
#define LOG_MAILBOX_MESSAGE_LOST        "Mailbox has been overrun"
#define LOG_ERR_BUS_LOCK                "Bus lock detected"
#define LOG_ERR_CHANNEL                 "Channel error has occurred"
#define LOG_TX_ABORTED                  "Transmit abort event"
#define LOG_ERR_GLOBAL                  "Global error has occurred"
#define LOG_TX_FIFO_EMPTY               "Transmit FIFO is empty"
#define LOG_FIFO_MESSAGE_LOST           "Receive FIFO overrun"

/* Enumeration of the on-board LED */
typedef enum e_led_state
{
    LED_CASE_1 = 1,
    LED_CASE_2 = 2,
    LED_CASE_3 = 3
} led_state_t;

/* Macros for the on-board LED */
#if defined (BOARD_RA8T1_MCK) || defined (BOARD_RA4T1_MCK) || defined (BOARD_RA6T3_MCK) || defined (BOARD_RA8T2_MCK)
#define LED_ON                          (BSP_IO_LEVEL_LOW)
#define LED_OFF                         (BSP_IO_LEVEL_HIGH)
#else
#define LED_ON                          (BSP_IO_LEVEL_HIGH)
#define LED_OFF                         (BSP_IO_LEVEL_LOW)
#endif /* Specific board */

/* Structure mapping a CAN error event to its log message */
typedef struct
{
    can_event_t err_event;
    const char * err_log;
} can_error_event_t;

/* Enumeration of main menu */
typedef enum e_menu
{
    NOMINAL_BAUDRATE_SET = 1,
    DATA_BAUDRATE_SET,
    SAMPLE_POINT_SET,
    START_TRANSMISSION
} menu_t;

/* Enumeration of the transmission menu */
typedef enum e_transmission_menu
{
    TRANSMIT_DATA = 1,
    RETURN_TO_BAUDRATE_MENU
} transmission_menu_t;

/* Enumeration of nominal baudrate option */
typedef enum e_nominal_rate_opt
{
    KBPS_250 = 1,
    KBPS_500,
    KBPS_1000,
    NOMINAL_CUSTOM
} nominal_rate_opt_t;

/* Enumeration of data baudrate option */
typedef enum e_data_rate_opt
{
    MBPS_1 = 1,
    MBPS_2,
#if defined (SUPPORT_MAX_DATA_RATE_5MBPS)
    MBPS_5,
#elif defined (SUPPORT_MAX_DATA_RATE_8MBPS)
    MBPS_8,
#endif /* SUPPORT_MAX_DATA_RATE */
    DATA_CUSTOM
} data_rate_opt_t;

/* Enumeration of baudrate set status */
typedef enum e_baudrate_set_status
{
    NOMINAL_SET_SUCCESS      = 1U << 0,
    DATA_SET_SUCCESS         = 1U << 1,
    SAMPLE_POINT_SET_SUCCESS = 1U << 2
} baudrate_set_status_t;

/* Extern variable - Used for on-board LED */
extern bsp_leds_t g_bsp_leds;

/* Extern variables - Defined in can_fd_ep.c */
extern volatile uint32_t g_canfd_event_flag;
extern can_frame_t g_canfd_tx_frame;
extern can_frame_t g_canfd_rx_frame;
extern uint8_t g_tx_data[CAN_CLASSIC_FRAME_DATA_BYTES];
extern uint8_t g_rx_data[CAN_CLASSIC_FRAME_DATA_BYTES];
extern uint8_t g_tx_fd_data[CAN_FD_DATA_LENGTH_CODE];
extern uint8_t g_rx_fd_data[CAN_FD_DATA_LENGTH_CODE];

/* Function declarations */
fsp_err_t can_fd_init(void);
void can_fd_deinit(void);
fsp_err_t timer_init(void);
void timer_deinit(void);
fsp_err_t can_fd_read_frame(can_frame_t * p_rx_frame, bool * p_data_received);
fsp_err_t can_fd_write_frame(can_frame_t can_transmit_frame);
void can_fd_data_update(void);
void led_update(led_state_t led_state);
void led_pin_initialization(void);
fsp_err_t timer_start_measure(void);
fsp_err_t timer_get_measure(uint32_t * p_time);

#endif /* CAN_FD_EP_H_ */
