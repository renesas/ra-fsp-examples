/***********************************************************************************************************************
 * File Name    : usb_phid_phid.h
 * Description  : Contains macros from threads
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef USB_PHID_PHID_H_
#define USB_PHID_PHID_H_

/* Macros definitions */
#define PHID_RD_LENGTH                  (76U)   /* Keyboard report descriptor length */
#define PHID2_RD_LENGTH                 (50U)   /* Mouse report descriptor length */
#define HID_D_LENGTH                    (9U)    /* Hid descriptor length */
#define PHID_D_INDEX                    (18U)   /* Keyboard HID descriptor index */
#define PHID2_D_INDEX                   (43U)   /* Mouse HID descriptor index */

#define KEY_DATA_LEN                    (8U)    /* Keyboard report buffer length */
#define KEY_DATA_INDEX                  (2U)    /* Key first press index for key report */
#define USAGE_ID_A                      (0x04)  /* Keyboard usage id of A */
#define USAGE_ID_0                      (0x27)  /* Keyboard usage id of 0 */
#define KEY_SIZE_NUM                    (2U)

#define MOUSE_DATA_LEN                  (3U)    /* Mouse report buffer length */
#define MOUSE_X_INDEX                   (1U)    /* X value index for mouse report */
#define MOUSE_Y_INDEX                   (2U)    /* Y value index for mouse report */
#define MOUSE_SPEED                     (10U)   /* Mouse move speed */
#define MOUSE_STAND                     (0x00)  /* Mouse stand value */
#define MOUSE_X_RIGHT(x)                ((x))   /* The sign for X speed value */
#define MOUSE_X_LEFT(x)                 (-(x))  /* The sign for X speed value */
#define MOUSE_Y_DOWN(x)                 ((x))   /* The sign for Y speed value */
#define MOUSE_Y_UP(x)                   (-(x))  /* The sign for Y speed value */

#define MOVE_RIGHT_TIMES                (30U)
#define MOVE_DOWN_TIMES                 (60U)
#define MOVE_LEFT_TIMES                 (90U)
#define MOVE_UP_TIMES                   (120U)

#define TASK_DELAY                      (1U)     /* Delay for task */
#define IDLE_VAL_INDEX                  (1U)     /* Index value */
#define BUFF_SIZE                       (16U)    /* Size of send data buffer */
#define ALIGN                           (4U)     /* Alignment */
#define DELAY_1_SEC                     (1000U)  /* 1 seconds delay */
#define DELAY_2_SEC                     (2000U)  /* 2 seconds delay */
#define DELAY_3_SEC                     (3000U)  /* 3 seconds delay */

#define EP_INFO         "\r\nThis project demonstrates the basic functionality of USB Composite driver\r\n" \
                        "on Renesas RA MCUs using Renesas FSP and FreeRTOS. The RA board is configured\r\n"\
                        "to function as either a Keyboard or a Mouse, based on user input: \r\n"\
                        "On pressing the 'k' key from the RTT Viewer, the MCU sends characters a to z \r\n"\
                        "and numbers 0 to 9 to the host PC.\r\n"\
                        "On pressing the 'm' key from the RTT Viewer, the MCU moves the mouse pointer \r\n"\
                        "in a square pattern on the host PC screen.\r\n"

#define EP_HINT         "\r\nEnter 'k' character to run Keyboard emulator or 'm' character to run Mouse emulator. \r\n"

#endif /* USB_PHID_PHID_H_ */
