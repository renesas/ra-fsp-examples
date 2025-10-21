/***********************************************************************************************************************
* File Name    : i3c_slave_ep.h
* Description  : Contains declarations of data structures and functions used in i3c_slave_ep.c.
***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef I3C_SLAVE_EP_H_
#define I3C_SLAVE_EP_H_

#include "hal_data.h"

/* External global variable */
extern const i3c_extended_cfg_t g_i3c0_cfg_extend;

/* Macros for RA boards that support I3C HDR-DDR mode */
#if defined(BOARD_RA8D1_EK) || defined(BOARD_RA8P1_EK) || defined(BOARD_RA8M2_EK)
#define I3C_HDR_DDR_SUPPORT
#define I3C_HDR_COMMAND_CODE_WRITE                      (0x55)
#define I3C_HDR_COMMAND_CODE_READ                       (0xD5)
#endif

#define I3C_SLAVE_DEVICE_STATIC_ADDRESS                 (0x41)
#define MAX_WRITE_DATA_LEN                              (32)
#define MAX_READ_DATA_LEN                               (32)
#define MAX_IBI_PAYLOAD_SIZE                            (32)
#define WAIT_TIME                                       (100)
#define MAX_WAIT_TIME_BUS_INIT_10S                      (100UL)

/* Macros for external IRQ */
#if defined(BOARD_RA2E2_EK)
#define USER_SW_IRQ_NUMBER                              (0x01)
#define USER_SW_PIN                                     (BSP_IO_PORT_02_PIN_05)
#elif defined(BOARD_RA4E2_EK) || defined(BOARD_RA6E2_EK) || defined(BOARD_RA4T1_MCK) || defined(BOARD_RA6T3_MCK)
#define USER_SW_IRQ_NUMBER                              (0x09)
#define USER_SW_PIN                                     (BSP_IO_PORT_03_PIN_04)
#elif defined(BOARD_RA8D1_EK) || defined(BOARD_RA8M1_EK) || defined(BOARD_RA8P1_EK) || defined(BOARD_RA8M2_EK)
#define USER_SW_IRQ_NUMBER                              (0x0C)
#define USER_SW_PIN                                     (BSP_IO_PORT_00_PIN_08)
#elif defined(BOARD_RA4L1_EK)
#define USER_SW_IRQ_NUMBER                              (0x07)
#define USER_SW_PIN                                     (BSP_IO_PORT_00_PIN_01)
#elif defined(BOARD_RA2L2_EK)
#define USER_SW_IRQ_NUMBER                              (0x07)
#define USER_SW_PIN                                     (BSP_IO_PORT_00_PIN_15)
#endif

#define WORD_ALIGN                                      (4)
#define ONE                                             (1)

/* BCR structure defined by MIPI I3C */
#define BCR_BIT6_7_FOR_I3C_SLAVE                        (0x0)
#define BCR_BIT6_7_FOR_I3C_MASTER                       (0x1)
#define BCR0_POS                                        (0)
#define BCR0_MSK                                        (0x01)
#define BCR1_POS                                        (1)
#define BCR1_MSK                                        (0x02)
#define BCR2_POS                                        (2)
#define BCR2_MSK                                        (0x04)
#define BCR3_POS                                        (3)
#define BCR3_MSK                                        (0x08)
#define BCR4_POS                                        (4)
#define BCR4_MSK                                        (0x10)
#define BCR5_POS                                        (5)
#define BCR5_MSK                                        (0x20)
#define BCR76_POS                                       (6)
#define BCR76_MSK                                       (0xc0)

/* The user defined I3C slave BCR */
#define I3C_SLAVE_DEVICE_SPEED_LIMITATION               (0)
#define I3C_SLAVE_DEVICE_IBI_REQUEST_CAPABLE            (1)
#define I3C_SLAVE_DEVICE_IBI_WITH_PAYLOAD               (1)
#define I3C_SLAVE_DEVICE_OFFLINE_CAPABLE                (0)
#define I3C_SLAVE_DEVICE_IS_BRIDGE                      (0)
#define I3C_SLAVE_DEVICE_SUPPORT_GETCAPS                (g_i3c0_cfg_extend.slave_command_response_info.hdr_ddr_support)
#define I3C_SLAVE_DEVICE_ROLE                           (BCR_BIT6_7_FOR_I3C_SLAVE)

#define BCR_SETTING (((I3C_SLAVE_DEVICE_SPEED_LIMITATION << BCR0_POS) & BCR0_MSK) |\
		             ((I3C_SLAVE_DEVICE_IBI_REQUEST_CAPABLE << BCR1_POS) & BCR1_MSK) |\
                     ((I3C_SLAVE_DEVICE_IBI_WITH_PAYLOAD << BCR2_POS) & BCR2_MSK) |\
		             ((I3C_SLAVE_DEVICE_OFFLINE_CAPABLE << BCR3_POS) & BCR3_MSK) |\
                     ((I3C_SLAVE_DEVICE_IS_BRIDGE << BCR4_POS) & BCR4_MSK) | \
                     ((I3C_SLAVE_DEVICE_SUPPORT_GETCAPS << BCR5_POS) & BCR5_MSK) |\
                     ((I3C_SLAVE_DEVICE_ROLE << BCR76_POS) & BCR76_MSK))

/* DCR setting */
#define DCR_SETTING                                     (0x00)

/* PID setting */
#define PID_TYPE_VENDOR_FIXED                           (0x0)
#define PID_TYPE_RANDOM                                 (0x1)
/* MIPI Manufacturer ID */
#define PID_RENESAS_MANUFACTURER_ID                     (0x04CCU)
/* Higher 16 bits in PID structure */
#define PID_MANUFACTURER_POS                            (1)
#define PID_MANUFACTURER_MSK                            (0xfffe)
#define PID_TYPE_POS                                    (0)
#define PID_TYPE_MSK                                    (0x0001)
/* Lower 32 bits in PID structure */
#define PID_VENDOR_DEF_POS                              (0)
#define PID_VENDOR_DEF_MSK                              (0x00000fff)
#define PID_INSTANCE_ID_POS                             (12)
#define PID_INSTANCE_ID_MSK                             (0x0000f000)
#define PID_PART_ID_POS                                 (16)
#define PID_PART_ID_MSK                                 (0xffff0000)

/* The user configuration */
#define PID_MANUFACTURER                                (PID_RENESAS_MANUFACTURER_ID)
#define PID_TYPE_SELECTION                              (PID_TYPE_VENDOR_FIXED)
#define PID_VENDOR_PART_ID                              (0x0005U)
#define PID_VENDOR_INSTANCE_ID                          (0x00U)
#define PID_VENDOR_DEF                                  (0x0000U)

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

#define EP_INFO            "This example project demonstrates the typical use of the I3C Driver as a Slave\r\n"\
                           "device on Renesas RA MCUs based on Renesas FSP. Once initialization is successful,\r\n"\
                           "the EP waits for DAA, or if DAA is not completed it will initiates hot join request.\r\n"\
                           "If on-board switch is pressed, it will initiate IBI transfer request. Error and info\r\n"\
                           "messages will be printed on J-Link RTT Viewer.\r\n"

/* Function declaration */
void i3c_slave_entry(void);

#endif /* I3C_SLAVE_EP_H_ */
