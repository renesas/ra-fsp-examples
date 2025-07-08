/***********************************************************************************************************************
* File Name    : i3c_master_ep.h
* Description  : Contains declarations of data structures and functions used in hal_entry.c.
***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#include "common_utils.h"

#ifndef I3C_MASTER_EP_H_
#define I3C_MASTER_EP_H_

/* Macro for RA boards that support I3C HDR-DDR mode */
#if defined (BOARD_RA8D1_EK) || defined (BOARD_RA8P1_EK)
#define I3C_HDR_DDR_SUPPORT
#endif

#define MAX_IBI_PAYLOAD_SIZE                            (32)
#define MAX_WRITE_DATA_LEN                              (32)
#define MAX_READ_DATA_LEN                               (32)
#define I3C_MASTER_DEVICE_STATIC_ADDRESS                (0x70)
#define I3C_MASTER_DEVICE_DYNAMIC_ADDRESS               (0x70)
#define I3C_SLAVE_DEVICE_DYNAMIC_ADDRESS_START          (0x71)
#define WAIT_TIME                                       (3000)

/* Macro for null character */
#define NULL_CHAR                                       ('\0')

#define WORD_ALIGN                                      (4)
#define ONE                                             (1)

/* I3C event flags */
#define I3C_EVENT_FLAG_ENTDAA_ADDRESS_PHASE             (0x00000001)
#define I3C_EVENT_FLAG_IBI_READ_COMPLETE                (0x00000002)
#define I3C_EVENT_FLAG_IBI_READ_BUFFER_FULL             (0x00000004)
#define I3C_EVENT_FLAG_READ_BUFFER_FULL                 (0x00000008)
#define I3C_EVENT_FLAG_IBI_WRITE_COMPLETE               (0x00000010)
#define I3C_EVENT_FLAG_HDR_EXIT_PATTERN_DETECTED        (0x00000020)
#define I3C_EVENT_FLAG_ADDRESS_ASSIGNMENT_COMPLETE      (0x00000040)
#define I3C_EVENT_FLAG_COMMAND_COMPLETE                 (0x00000080)
#define I3C_EVENT_FLAG_WRITE_COMPLETE                   (0x00000100)
#define I3C_EVENT_FLAG_READ_COMPLETE                    (0x00000200)
#define I3C_EVENT_FLAG_TIMEOUT_DETECTED                 (0x00000400)
#define I3C_EVENT_FLAG_INTERNAL_ERROR                   (0x00000800)

typedef enum rtt_menu_options
{
    DISPLAY_I3C_SLAVE_INFO = 1,
    MASTER_WRITE_READ,
#ifdef I3C_HDR_DDR_SUPPORT
    MASTER_WRITE_READ_HDR,
#endif /* I3C_HDR_DDR_SUPPORT */
}rtt_menu_options_t;

/* Structure for configuring a slave address when the driver is in slave mode (See @ref i3c_api_t::deviceCfgSet) */
typedef struct s_i3c_slave_device_information
{
    uint8_t             static_address;
    uint8_t             dynamic_address;
    i3c_slave_info_t    slave_info;                /* PID, BCR, and DCR registers for the device (Slave mode only) */
    uint16_t            max_read_length;
    uint16_t            max_write_length;
    uint8_t             pending_interrupt;         /* Contains the interrupt number of any pending interrupt,
                                                    * or 0 if no interrupts are pending */
    uint8_t             protocol_error;
    uint8_t             vendor_status;             /* Reserved for vendor-specific meaning */
    i3c_activity_state_t activity_mode;
} i3c_device_information_t;

#define EP_INFO            "This example project demonstrates the typical use of the I3C Driver as master\r\n"\
                           "on Renesas RA MCUs based on Renesas FSP. The I3C master demonstrates operation\r\n"\
                           "with an I3C slave connected on another RA board. Once initialization is successful,\r\n"\
                           "it will start assigning dynamic addresses to slave devices present on the bus.\r\n"\
                           "The EP performs write/read operations and displays device information based on\r\n"\
                           "user input. Error and info messages will be printed on the J-Link RTT Viewer.\r\n"

#ifdef I3C_HDR_DDR_SUPPORT
#define EP_FUNCTION_MENU   "\r\nI3C Master operations: \r\n"\
                           "1. Display I3C slave device Information if exists on I3C bus\r\n"\
                           "2. I3C Write Read operation in SDR\r\n"\
                           "3. I3C Write Read operation in HDR-DDR\r\n"
#else
#define EP_FUNCTION_MENU   "\r\nI3C Master operations: \r\n"\
                           "1. Display I3C slave device Information if exists on I3C bus\r\n"\
                           "2. I3C Write Read operation\r\n"
#endif /* I3C_HDR_DDR_SUPPORT */

/* Global function declarations */
uint32_t i3c_app_event_notify(uint32_t set_event_flag_value, uint32_t timout);
void set_next_read_buffer(void);
void i3c_master_entry(void);

#endif /* I3C_MASTER_EP_H_ */
