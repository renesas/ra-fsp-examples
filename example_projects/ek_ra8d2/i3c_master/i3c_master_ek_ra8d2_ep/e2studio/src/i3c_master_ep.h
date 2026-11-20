/***********************************************************************************************************************
 * File Name    : i3c_master_ep.h
 * Description  : Contains macros, variables, data structures and functions used in i3c_master_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef I3C_MASTER_EP_H_
#define I3C_MASTER_EP_H_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "hal_data.h"

/* Macro for RA boards that support I3C HDR-DDR mode */
#if defined (BOARD_RA8D1_EK) || defined (BOARD_RA8P1_EK) || defined (BOARD_RA8D2_EK) || defined (BOARD_RA8M2_EK) ||\
    defined (BOARD_RA8T2_EK) || defined (BOARD_RA8P1_EK_V2)
#define I3C_HDR_DDR_SUPPORT
#endif /* I3C HDR-DDR mode support */

/* Macro for the reset value */
#define RESET_VALUE                                     (0x00)

/* Macros for IBI payload and data transfer buffer sizes */
#define MAX_IBI_PAYLOAD_SIZE                            (32U)
#define MAX_WRITE_DATA_LEN                              (32U)
#define MAX_READ_DATA_LEN                               (32U)

/* Macros for I3C Master and Slave device addresses */
#define I3C_MASTER_DEVICE_STATIC_ADDRESS                (0x70)
#define I3C_MASTER_DEVICE_DYNAMIC_ADDRESS               (0x70)
#define I3C_SLAVE_DEVICE_DYNAMIC_ADDRESS_START          (0x71)

/* Macro for the event wait timeout in milliseconds */
#define WAIT_TIME                                       (3000U)

/* Macro for the delay before reading, to allow the slave to prepare its buffer */
#define DELAY_FOR_SLAVE_PREPARE                         (50U)

/* Macro for null character */
#define NULL_CHAR                                       ('\0')

/* Macro for 4-byte word alignment */
#define WORD_ALIGN                                      (4U)

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

/* Structure for configuring a Slave address when the driver is in Slave mode (See @ref i3c_api_t::deviceCfgSet) */
typedef struct s_i3c_slave_device_information
{
    uint8_t static_address;
    uint8_t dynamic_address;
    i3c_slave_info_t slave_info;            /* PID, BCR, and DCR registers for the device (Slave mode only) */
    uint16_t max_read_length;
    uint16_t max_write_length;
    uint8_t pending_interrupt;              /* Contains the interrupt number of any pending interrupt,
                                             * or 0 if no interrupts are pending */
    uint8_t protocol_error;
    uint8_t vendor_status;                  /* Reserved for vendor-specific meaning */
    i3c_activity_state_t activity_mode;
} i3c_device_information_t;

/* Extern variables - defined in i3c_master_ep.c*/
extern uint32_t g_num_device_on_bus;
extern i3c_device_information_t g_device_slave_info;
extern volatile bool g_b_bus_initialized;
extern volatile bool g_b_ibi_transfer_received;
extern volatile bool g_b_hot_join_done;
extern uint8_t g_ibi_target_address;
extern uint32_t g_data_transfer_size;
extern uint8_t g_write_data[MAX_WRITE_DATA_LEN];
extern uint8_t *g_p_next;
extern uint8_t *g_p_last;
extern i3c_command_descriptor_t command_descriptor;
extern uint32_t g_write_read_routine_count;

/* Function declarations */
fsp_err_t i3c_master_init(void);
fsp_err_t agt_init(void);
fsp_err_t i3c_master_process_event(uint32_t *p_event_flag);
fsp_err_t master_write_read_verify(void);
fsp_err_t i3c_ccc_reset_address_send(void);
uint32_t i3c_app_event_notify(uint32_t set_event_flag_value, uint32_t timeout);
void set_next_read_buffer(void);
void i3c_deinit(void);
void agt_deinit(void);

#endif /* I3C_MASTER_EP_H_ */
