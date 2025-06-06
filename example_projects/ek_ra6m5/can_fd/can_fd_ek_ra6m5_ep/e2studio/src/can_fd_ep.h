/***********************************************************************************************************************
 * File Name    : can_fd_ep.h
 * Description  : Contains declarations of data structures and functions used in can_fd_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef CAN_FD_EP_H_
#define CAN_FD_EP_H_

#include "common_utils.h"

/* Macros for CAN frame  */
#define CAN_MAILBOX_NUMBER_0            (0U)               /* Mail box number */
#define CAN_CLASSIC_FRAME_DATA_BYTES    (8U)               /* Data Length code for classic frame */
#define CAN_FD_DATA_LENGTH_CODE         (64)               /* Data Length code for FD frame */
#define CAN_ID                          (0x1100)           /* ID to be updated in transmit frame */

/* Acceptance filter array parameters */
#define CANFD_FILTER_ID                 (0x00001000)
#define MASK_ID                         (0x1FFFF000)
#define MASK_ID_MODE                    (1)

#define ZERO                            (0U)               /* Array Index value */
#define NULL_CHAR                       ('\0')
#define WAIT_TIME                       (10000U)           /* Wait time value */

#define EP_VERSION                      ("1.1")
#define MODULE_NAME                     "CAN FD"
#define BANNER_INFO                     "\r\n************************************************************************"\
                                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                      *"\
                                        "\r\n*   Example Project Version %s                                        *"\
                                        "\r\n*   Flex Software Pack Version  %d.%d.%d                                  *"\
                                        "\r\n************************************************************************"\
                                        "\r\nRefer to readme.txt file for more details on Example Project and" \
                                        "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO                         "\r\nThis example project showcases CAN FD communication using two Renesas RA"\
                                        "\r\nboards. Users can configure the nominal baud rate, data baud rate, and"\
                                        "\r\nsample point at runtime, and initiate data transmission with the selected"\
                                        "\r\nsettings. To stop the transmission and return to the baud rate"\
                                        "\r\nconfiguration menu, users simply press the Enter key without entering any"\
                                        "\r\ninput."\
                                        "\r\nDuring transmission, data is sent from Board1 to Board2. Upon receiving"\
                                        "\r\nthe data, Board2 displays it on the terminal and sends an updated message"\
                                        "\r\nback to Board1. When Board1 receives this data, it converts the CAN frame"\
                                        "\r\nto a CAN FD frame and transmits it back to Board2. Board2, upon receiving"\
                                        "\r\nthis frame, further updates the data and sends it to Board1 using the CAN"\
                                        "\r\nFD format."\
                                        "\r\nFinally, once the transmission cycle completes, Board1 prints the received"\
                                        "\r\ndata on the terminal.\r\n\r\n"


#define MENU_OPTION                     "\r\n1. Select the CANFD nominal rate."\
                                        "\r\n2. Select the CANFD FD data rate."\
                                        "\r\n3. Select the CANFD sample point."\
                                        "\r\n4. Change CANFD baud rate and start CANFD transmission.\r\n"

#define CANFD_BAUDRATE_INFO             "\r\nCANFD nominal rate: %d bits/s"\
		                                "\r\nCANFD FD data rate: %d bits/s"\
		                                "\r\nCANFD sample point: %d%% \r\n"

#define NOMINAL_RATE_OPTION             "\r\nNominal rate option:"\
                                        "\r\n  1. 250 kbits/s"\
                                        "\r\n  2. 500 kbits/s"\
                                        "\r\n  3. 1 Mbits/s"\
                                        "\r\n  4. Custom (bits/s)\r\n"

#if (defined BOARD_RA8M1_EK) || (defined BOARD_RA8M1_EK) || (defined BOARD_RA8T1_MCK) || (defined BOARD_RA8E1_FPB)
#define DATA_RATE_OPTION                "\r\nFD data rate option:"\
                                        "\r\n  1. 1 Mbits/s"\
                                        "\r\n  2. 2 Mbits/s"\
                                        "\r\n  3. 8 Mbits/s"\
                                        "\r\n  4. Custom (bits/s)\r\n"
#else
#define DATA_RATE_OPTION                "\r\nFD data rate option:"\
                                        "\r\n  1. 1 Mbits/s"\
                                        "\r\n  2. 2 Mbits/s"\
                                        "\r\n  3. 5 Mbits/s"\
                                        "\r\n  4. Custom (bits/s)\r\n"
#endif
#define PERFORM_TRANSMISSION            "\r\n\r\nTo start CAN transmission: Send any data (except NULL)"\
		                                "\r\n\r\nTo back to the configuration baud rate menu: Press the Enter key.\r\n"

#define KBPS_250_VAL                    (250000U)
#define KBPS_500_VAL                    (500000U)
#define MBPS_1_VAL                      (1000000U)
#define MBPS_2_VAL                      (2000000U)
#define MBPS_5_VAL                      (5000000U)
#if (defined BOARD_RA8M1_EK) || (defined BOARD_RA8M1_EK) || (defined BOARD_RA8T1_MCK) || (defined BOARD_RA8E1_FPB)
#define MBPS_8_VAL                      (8000000U)
#endif

/* Macros for special character definitions */
#define CHAR_LF                         ('\n')
#define CHAR_CR                         ('\r')

#define BUFF_SIZE                       (100U)

/* Macros for log of CAN error events */
#define LOG_ERR_WARNING                  "Error Warning event"
#define LOG_ERR_PASSIVE                  "Error Passive event"
#define LOG_ERR_BUS_OFF                  "Bus Off event"
#define LOG_BUS_RECOVERY                 "Bus Off Recovery event"
#define LOG_MAILBOX_MESSAGE_LOST         "Mailbox has been overrun"
#define LOG_ERR_BUS_LOCK                 "Bus lock detected"
#define LOG_ERR_CHANNEL                  "Channel error has occurred"
#define LOG_TX_ABORTED                   "Transmit abort event"
#define LOG_ERR_GLOBAL                   "Global error has occurred"
#define LOG_TX_FIFO_EMPTY                "Transmit FIFO is empty"
#define LOG_FIFO_MESSAGE_LOST            "Receive FIFO overrun"

typedef struct {
    can_event_t err_event;
    const char *err_log;
} can_error_event_t;

/* Enumeration of LEDs on board */
typedef enum e_led_state
{
   red = 1,
   green = 2,
   blue = 3,
} led_state_t;

/* Enumeration of main menu */
typedef enum e_menu
{
   NOMINAL_BAUDRATE_SET = 1,
   DATA_BAUDRATE_SET,
   SAMPLE_POINT_SET,
   START_TRANSMISSION,
} menu_t;

/* Enumeration of nominal baudrate option */
typedef enum e_nominal_rate_opt
{
   KBPS_250 = 1,
   KBPS_500,
   KBPS_1000,
   NOMINAL_CUSTOM,
} nominal_rate_opt_t;

/* Enumeration of data baudrate option */
typedef enum e_data_rate_opt
{
   MBPS_1 = 1,
   MBPS_2,
   MBPS_5,
   DATA_CUSTOM,
} data_rate_opt_t;

/* Structure representing a CAN timing */
typedef enum e_baudrate_set_status
{
    NOMINAL_SET_SUCCESS                 = 1U << 0,
    DATA_SET_SUCCESS                    = 1U << 1,
    SAMPLE_POINT_SET_SUCCESS            = 1U << 2,
} baudrate_set_status_t;

/* Structure representing a CAN timing */
typedef struct st_can_timing {
    uint16_t sjw;
    uint16_t phase_seg1;
    uint16_t phase_seg2;
    uint16_t prescaler;
    uint32_t baudrate;
} can_timing_t;

/* User defined functions */
void canfd_entry(void);
void led_update(led_state_t led_state);
uint32_t get_user_input(void);

#endif /* CAN_FD_EP_H_ */
