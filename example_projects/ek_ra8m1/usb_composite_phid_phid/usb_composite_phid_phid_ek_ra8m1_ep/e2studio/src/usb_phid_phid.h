/***********************************************************************************************************************
 * File Name    : usb_phid_phid.h
 * Description  : Contains macros from threads
 ***********************************************************************************************************************/
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
#ifndef USB_PHID_PHID_H_
#define USB_PHID_PHID_H_

/* Macro definitions */
#define PHID_RD_LENGTH                  (76U)              // keyboard report descriptor length
#define PHID2_RD_LENGTH                 (50U)              // mouse report descriptor length
#define HID_D_LENGTH                    (9U)               // hid descriptor length
#define PHID_D_INDEX                    (18U)              // keyboard HID descriptor index
#define PHID2_D_INDEX                   (43U)              // mouse HID descriptor index

#define KEY_DATA_LEN                    (8U)               // keyboard report buffer length
#define KEY_DATA_INDEX                  (2U)               // key first press index for key report
#define USAGE_ID_A                      (0x04)             // keyboard usage id of A
#define USAGE_ID_0                      (0x27)             // keyboard usage id of 0
#define KEY_SIZE_NUM                    (2U)

#define MOUSE_DATA_LEN                  (3U)                // mouse report buffer length
#define MOUSE_X_INDEX                   (1U)                // X value index for mouse report
#define MOUSE_Y_INDEX                   (2U)                // Y value index for mouse report
#define MOUSE_SPEED                     (10U)               // mouse move speed
#define MOUSE_STAND                     (0x00)              // mouse stand value
#define MOUSE_X_RIGHT(x)                 ((x))              // the sign for X speed value
#define MOUSE_X_LEFT(x)                  (-(x))               // the sign for X speed value
#define MOUSE_Y_DOWN(x)                  ((x))              // the sign for Y speed value
#define MOUSE_Y_UP(x)                    (-(x))               // the sign for Y speed value

#define MOVE_RIGHT_TIMES                (30U)               //
#define MOVE_DOWN_TIMES                 (60U)
#define MOVE_LEFT_TIMES                 (90U)
#define MOVE_UP_TIMES                   (120U)

#define TASK_DELAY                       (1U)               // delay for task
#define IDLE_VAL_INDEX                   (1U)               // index value
#define BUFF_SIZE                        (16U)              // size of send data buffer
#define ALIGN                            (4U)               // alignment
#define DELAY_1_SEC                      (1000U)            // 1 seconds delay
#define DELAY_2_SEC                      (2000U)            // 2 seconds delay
#define DELAY_3_SEC                      (3000U)            // 3 seconds delay

#define EP_INFO "\r\nThis project demonstrates the basic functionality of USB Composite driver on Renesas RA MCUs\r\n" \
                "based on Renesas FSP using FreeRTOS. The project configures RA board as a Keyboard as well as Mouse.\r\n"\
                "On pressing k key from RTT Viewer, MCU sends the host PC characters from a - z & numbers 0 - 9.\r\n"\
                "On pressing m key from RTT Viewer, MCU moves the mouse pointer on the host PC screen on a square.\r\n"

#define EP_HINT "\r\nEnter 'k' character to run Keyboard emulator or 'm' character to run Mouse emulator. \r\n"



#endif /* USB_PHID_PHID_H_ */
