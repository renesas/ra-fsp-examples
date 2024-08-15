/***********************************************************************************************************************
 * File Name    : usb_phid_ep.h
 * Description  : Contains macros from threads
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#ifndef USB_PHID_EP_H_
#define USB_PHID_EP_H_

/* Macro definitions */
#define USB_RECEIVE_REPORT_DESCRIPTOR    (76U)              // report descriptor
#define USB_RECEIVE_HID_DESCRIPTOR       (9U)               // hid descriptor
#define DATA_LEN                         (8U)               // data length
#define SIZE_NUM                         (2U)               // numlock data size
#define USAGE_ID_A                       (0x04)             // keyboard usage id of A
#define USAGE_ID_0                       (0x27)             // keyboard usage id of 0
#define DELAY                            (100U)             // delay for write
#define INDEX_THIRD                      (2U)               // third index for usb buffer
#define TASK_DELAY                       (1U)               // delay for task
#define CD_LENGTH                        (18U)              // config descriptor length
#define IDLE_VAL_INDEX                   (1U)               // index value
#define BUFF_SIZE                        (16U)              // size of send data buffer
#define ALIGN                            (4U)               // alignment
#define DELAY_3_SEC                      (3000U)            // 3 seconds delay

#define EP_INFO "\r\nThis project demonstrates the basic functionalities of USB PHID driver on \r\n"\
                "Renesas RA MCUs based on Renesas FSP. The project configures RA board as a Keyboard.\r\n"\
                "On pressing any key from RTTViewer, MCU sends the host PC characters from a - z & numbers 0 - 9.\r\n"



#endif /* USB_PHID_EP_H_ */
