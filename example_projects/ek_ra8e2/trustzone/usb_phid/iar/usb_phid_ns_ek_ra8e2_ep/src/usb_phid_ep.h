/***********************************************************************************************************************
 * File Name    : usb_phid_ep.h
 * Description  : Contains macros from threads
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef USB_PHID_EP_H_
#define USB_PHID_EP_H_

/* Macro definitions */
#define USB_RECEIVE_REPORT_DESCRIPTOR    (76U)              /* Report descriptor */
#define USB_RECEIVE_HID_DESCRIPTOR       (9U)               /* HID descriptor */
#define DATA_LEN                         (8U)               /* Data length */
#define SIZE_NUM                         (2U)               /* Numlock data size */
#define USAGE_ID_A                       (0x04)             /* Keyboard usage id of A */
#define USAGE_ID_0                       (0x27)             /* Keyboard usage id of 0 */
#define DELAY                            (100U)             /* Delay for write */
#define INDEX_THIRD                      (2U)               /* Third index for usb buffer */
#define TASK_DELAY                       (1U)               /* Delay for task */
#define CD_LENGTH                        (18U)              /* Configure descriptor length */
#define IDLE_VAL_INDEX                   (1U)               /* Index value */
#define BUFF_SIZE                        (16U)              /* Size of send data buffer */
#define ALIGN                            (4U)               /* Alignment */
#define DELAY_3_SEC                      (3000U)            /* 3 seconds delay */

#define EP_INFO "\r\nThis project demonstrates the basic functionalities of USB PHID driver on\r\n"\
                "Renesas RA MCUs based on Renesas FSP. The project configures RA board as a Keyboard.\r\n"\
                "On pressing any key from RTT Viewer, MCU sends characters from a - z & numbers 0 - 9\r\n"\
                "to the host PC.\r\n"

#endif /* USB_PHID_EP_H_ */
