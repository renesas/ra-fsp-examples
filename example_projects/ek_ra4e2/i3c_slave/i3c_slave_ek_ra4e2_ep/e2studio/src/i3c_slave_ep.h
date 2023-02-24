/***********************************************************************************************************************
 * File Name    : i3c_slave_ep.h
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


#ifndef I3C_SLAVE_EP_H_
#define I3C_SLAVE_EP_H_

#define I3C_SLAVE_DEVICE_STATIC_ADDRESS          (0x41)
#define MAX_WRITE_DATA_LEN                       (64)
#define MAX_READ_DATA_LEN                        (64)
#define MAX_IBI_PAYLOAD_SIZE                     (32)
#define WAIT_TIME                                (100)
#define MAX_WAIT_TIME_BUS_INIT_10S               (100UL)
/* External IRQ channel number */
#define USER_SW_IRQ_NUMBER                       (0x07)

#define WORD_ALIGN   (4)
#define ONE          (1)

/* BCR structure defined by MIPI I3C */
#define BCR_BIT6_7_FOR_I3C_SLAVE                    (0x0)
#define BCR_BIT6_7_FOR_I3C_MASTER                   (0x1)
#define BCR0_Pos                                    (0)
#define BCR0_Msk                                    (0x01)
#define BCR1_Pos                                    (1)
#define BCR1_Msk                                    (0x02)
#define BCR2_Pos                                    (2)
#define BCR2_Msk                                    (0x04)
#define BCR3_Pos                                    (3)
#define BCR3_Msk                                    (0x08)
#define BCR4_Pos                                    (4)
#define BCR4_Msk                                    (0x10)
#define BCR5_Pos                                    (5)
#define BCR5_Msk                                    (0x20)
#define BCR76_Pos                                   (6)
#define BCR76_Msk                                   (0xc0)

/* user defined I3C slave BCR */
#define I3C_SLAVE_DEVICE_SPEED_LIMITATION           (0)
#define I3C_SLAVE_DEVICE_IBI_REQUEST_CAPABLE        (1)
#define I3C_SLAVE_DEVICE_IBI_WITH_PAYLOAD           (1)
#define I3C_SLAVE_DEVICE_OFFLINE_CAPABLE            (0)
#define I3C_SLAVE_DEVICE_IS_BRIDGE                  (0)
#define I3C_SLAVE_DEVICE_SUPPORT_GETCAPS            (0)
#define I3C_SLAVE_DEVICE_ROLE                       (BCR_BIT6_7_FOR_I3C_SLAVE)

#define BCR_SETTING (((I3C_SLAVE_DEVICE_SPEED_LIMITATION << BCR0_Pos) & BCR0_Msk) | \
		             ((I3C_SLAVE_DEVICE_IBI_REQUEST_CAPABLE << BCR1_Pos) & BCR1_Msk) | \
                     ((I3C_SLAVE_DEVICE_IBI_WITH_PAYLOAD << BCR2_Pos) & BCR2_Msk) | \
		             ((I3C_SLAVE_DEVICE_OFFLINE_CAPABLE << BCR3_Pos) & BCR3_Msk) | \
                     ((I3C_SLAVE_DEVICE_IS_BRIDGE << BCR4_Pos) & BCR4_Msk) | \
                     ((I3C_SLAVE_DEVICE_SUPPORT_GETCAPS << BCR5_Pos) & BCR5_Msk) | \
                     ((I3C_SLAVE_DEVICE_ROLE << BCR76_Pos) & BCR76_Msk))

/*  DCR setting */
#define DCR_SETTING              (0x00)

/* PID setting */
#define PID_TYPE_VENDOR_FIXED       (0x0)
#define PID_TYPE_RANDOM             (0x1)
/* MIPI Manufacturer ID */
#define PID_RENESAS_MANUFACTURER_ID (0x04CCU)
/* Higher 16Bits in PID structure */
#define PID_MANUFACTURER_Pos        (1)
#define PID_MANUFACTURER_Msk        (0xfffe)
#define PID_TYPE_Pos                (0)
#define PID_TYPE_Msk                (0x0001)
/* Lower 32Bits in PID structure */
#define PID_VENDOR_DEF_Pos          (0)
#define PID_VENDOR_DEF_Msk          (0x00000fff)
#define PID_INSTANCE_ID_Pos         (12)
#define PID_INSTANCE_ID_Msk         (0x0000f000)
#define PID_PART_ID_Pos             (16)
#define PID_PART_ID_Msk             (0xffff0000)

/* user config */
#define PID_MANUFACTURER            (PID_RENESAS_MANUFACTURER_ID)
#define PID_TYPE_SELECTION          (PID_TYPE_VENDOR_FIXED)
#define PID_VENDOR_PART_ID          (0x0005U)
#define PID_VENDOR_INSTANCE_ID      (0x00U)
#define PID_VENDOR_DEF              (0x0000U)

/* I3C Event Flag  */
#define I3C_EVENT_FLAG_ENTDAA_ADDRESS_PHASE             0x00000001
#define I3C_EVENT_FLAG_IBI_READ_COMPLETE                0x00000002
#define I3C_EVENT_FLAG_IBI_READ_BUFFER_FULL             0x00000004
#define I3C_EVENT_FLAG_READ_BUFFER_FULL                 0x00000008
#define I3C_EVENT_FLAG_IBI_WRITE_COMPLETE               0x00000010
#define I3C_EVENT_FLAG_HDR_EXIT_PATTERN_DETECTED        0x00000020
#define I3C_EVENT_FLAG_ADDRESS_ASSIGNMENT_COMPLETE      0x00000040
#define I3C_EVENT_FLAG_COMMAND_COMPLETE                 0x00000080
#define I3C_EVENT_FLAG_WRITE_COMPLETE                   0x00000100
#define I3C_EVENT_FLAG_READ_COMPLETE                    0x00000200
#define I3C_EVENT_FLAG_TIMEOUT_DETECTED                 0x00000400
#define I3C_EVENT_FLAG_INTERNAL_ERROR                   0x00000800

#define EP_INFO                "This example project demonstrates the typical use of the I3C Driver as slave\r\n"\
                               "device on Renesas RA MCUs based on Renesas FSP. once initialization is successful,\r\n"\
                               "The EP waits for DAA, or if DAA is not completed it will initiates hot join request.\r\n"\
                               "If on-board switch is pressed, it will initiate IBI transfer request.\r\n"\
                               "Error and info messages will be printed on JlinkRTTViewer.\r\n"\
                               "Refer to readme.txt file for more details on Example Project and\r\n"\
                               "FSP User's Manual for more information about "MODULE_NAME" driver\r\n"

/* function declarations */
fsp_err_t i3c_slave_init(void);
fsp_err_t i3c_slave_ops(void);
fsp_err_t icu_init(void);
bool read_onboard_sw_status(void);
void i3c_deinit(void);
void icu_deinit(void);
void agt_deinit(void);

#endif /* I3C_SLAVE_EP_H_ */
