/***********************************************************************************************************************
 * File Name    : uarta_ep.h
 * Description  : Contains macros, data structures, and functions used in uarta_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2024 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef UARTA_EP_H_
#define UARTA_EP_H_

#include <stdbool.h>
#include "hal_data.h"

/* Macro for the reset value */
#define RESET_VALUE                     (0x00)

/* Macros for UARTA receive */
#define NULL_CHAR                       ('\0')
#define CR_ASCII                        (0x0D)
#define LF_ASCII                        (0x0A)
#define BUFFER_LEN                      (256U)

/* Macros for UARTA transmit */
#define TIME_OUT_FOR_ONE_BYTE           (1000U)
#define TIME_OUT_VALUE                  (1U)
#define TIME_OUT_UNIT                   (BSP_DELAY_UNITS_MICROSECONDS)

/* Scaling factor for baud rate error values, 10ppm or .001% */
#if defined(BOARD_RA4C1_EK)
#define UARTA_BAUD_ERROR_SCALING_1      (180U)
#else
#define UARTA_BAUD_ERROR_SCALING_1      (95U)
#endif
#define UARTA_BAUD_ERROR_SCALING_2      (170U)

/* Mask for valid mapping of uart_clock_div_t to UTA0CK.CK */
#define UARTA_CLOCK_DIV_MASK            (0x07U)

/* Wait time needs to be rounded up to enable TX/RX */
#define UARTA_CONVERT_TO_MICRO_SECOND   (999999U)

/* Macro for EP baud rate menu */
#define EP_MENU                         "\r\nUARTA baud rate selection menu:"\
                                        "\r\n 1. 9600 bps"\
                                        "\r\n 2. 14400 bps"\
                                        "\r\n 3. 19200 bps"\
                                        "\r\n 4. 38400 bps"\
                                        "\r\n 5. 57600 bps"\
                                        "\r\n 6. 115200 bps"\
                                        "\r\nPlease select the desired baud rate by entering the corresponding number: "

#define EP_INVALID_INPUT                "\r\nInvalid input.\r\n"

/* Macro for the baud rate change notice (short) */
#define EP_GUIDE1                       "\r\n\r\nSelected baud rate: %d bps."\
                                        "\r\nPlease change the terminal setting to selected baud rate,"\
                                        "\r\nThen type any key and press enter to start UARTA operation.\r\n"

/* Macro for EP guide */
#define EP_GUIDE2                       "\r\nInput messages to send: "

/* Macros for echo message */
#define ECHO_MESSAGE                    "\r\nReceived data from RA board: "
#define ECHO_MESSAGE_LEN                (strlen(ECHO_MESSAGE))

/* Macros to define UARTA baud rate menu selection */
#define UARTA_9600                      (1U)
#define UARTA_14400                     (2U)
#define UARTA_19200                     (3U)
#define UARTA_38400                     (4U)
#define UARTA_57600                     (5U)
#define UARTA_115200                    (6U)

/* External variables - Defined in uarta_ep.c */
extern uint8_t g_rx_buffer[BUFFER_LEN];
extern uint8_t g_tx_buffer[BUFFER_LEN];
extern volatile uint16_t g_rx_len;
extern volatile bool g_rx_flag;

/* Set by uarta_callback() on a UART receive error event; polled and cleared by the application layer */
extern volatile fsp_err_t g_uarta_rx_error;

/* Set by uarta_get_baud_rate() to the user's selected baud rate; read by uarta_calculate_baud_rate() */
extern uint32_t g_baud_rate;

/* Function declarations */
fsp_err_t uarta_init(void);
fsp_err_t uarta_calculate_baud_rate(void);
fsp_err_t uarta_send(uint8_t * p_data);
void uarta_deinit(void);
fsp_err_t uarta_get_baud_rate(void);

#endif /* UARTA_EP_H_ */
