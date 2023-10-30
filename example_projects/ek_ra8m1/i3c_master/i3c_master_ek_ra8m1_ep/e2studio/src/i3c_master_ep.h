/***********************************************************************************************************************
 * File Name    : i3c_master_ep.h
 * Description  : Contains declarations of data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#ifndef I3C_MASTER_EP_H_
#define I3C_MASTER_EP_H_

#define MAX_IBI_PAYLOAD_SIZE                            (32)
#define MAX_WRITE_DATA_LEN                              (32)
#define MAX_READ_DATA_LEN                               (32)
#define I3C_MASTER_DEVICE_STATIC_ADDRESS                (0x70)
#define I3C_MASTER_DEVICE_DYNAMIC_ADDRESS               (0x70)
#define I3C_SLAVE_DEVICE_DYNAMIC_ADDRESS_START          (0x71)
#define WAIT_TIME                                       (3000)
/* MACRO for null character */
#define NULL_CHAR   ('\0')
#define WORD_ALIGN   (4)
#define ONE          (1)

/* I3C Event Flags */
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
    MASTER_WRITE_READ = 2
}rtt_menu_options_t;


/** Structure for configuring a slave address when the driver is in slave mode (See @ref i3c_api_t::deviceCfgSet). */
typedef struct s_i3c_slave_device_information
{
    uint8_t             static_address;
    uint8_t             dynamic_address;
    i3c_slave_info_t    slave_info;                ///< PID, BCR, and DCR registers for the device (Slave mode only).
    uint16_t            max_read_length;
    uint16_t            max_write_length;
    uint8_t             pending_interrupt;         ///< Contains the interrupt number of any pending interrupt, or 0 if no interrupts are pending.
    uint8_t             protocol_error;
    uint8_t             vendor_status;             ///< Reserved for vendor-specific meaning.
    i3c_activity_state_t activity_mode;
} i3c_device_information_t;

#define EP_INFO                "This example project demonstrates the typical use of the I3C Driver as master\r\n"\
                               "on Renesas RA MCUs based on Renesas FSP. The I3C master demonstrates operation\r\n"\
                               "with I3C slave connected on another RA board. once initialization is\r\n"\
                               "successful, it will start assigning dynamic addresses to slave devices present on the bus.\r\n"\
                               "The EP performs write/read operation and displays device information based on user input.\r\n"\
                               "Error and info messages will be printed on JlinkRTTViewer.\r\n"\
                               "Refer to readme.txt file for more details on Example Project and\r\n"\
                               "FSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_FUNCTION_MENU       " \r\nI3C Master operations : \r\n"\
                               "1. Display I3C slave device Information if exists on I3C bus\r\n"\
                               "2. I3C Write Read operation\r\n"


#endif /* I3C_MASTER_EP_H_ */
