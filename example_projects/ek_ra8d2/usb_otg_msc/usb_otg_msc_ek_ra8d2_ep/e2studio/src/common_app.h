/***********************************************************************************************************************
* File Name    : common_app.h
* Description  : Contains macros, data structures, and functions commonly used in the EP.
***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef COMMON_APP_H_
#define COMMON_APP_H_

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "common_utils.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
/* Macros for the EP information */
#define EP_VERSION              ("1.0")

#define MODULE_NAME             "USB OTG MSC"

#define BANNER_INFO	            "\r\n******************************************************************"\
								"\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module           *"\
								"\r\n*   Example Project Version %s                                  *"\
								"\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
								"\r\n******************************************************************"\
								"\r\nRefer to readme.txt file for more details on Example Project and"\
        						"\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO                 "\r\nThis example project demonstrates USB OTG operations based on the USB basic"\
                                "\r\ndriver of the RA MCU. Typically, the RA board functions as a USB MSC class"\
                                "\r\ndevice, using the SD card as block media. When connected to a PC via a USB"\
                                "\r\nmicro cable, the file system on the SD card is displayed on the PC. When a USB"\
                                "\r\nOTG cable is attached to the RA board's USB port, the board switches to the USB"\
                                "\r\nMSC host class. In host mode, the SD card acts as block media, and USB storage"\
                                "\r\ndevices connected to the RA board's USB port also act as block media. FAT file"\
                                "\r\nsystem operations are then performed to manage files on the storage devices.\r\n"

#define MENU                    "\r\n\r\nSelect a file system operation:"\
                                "\r\n   1. USB disk: List USB disk directory"\
                                "\r\n   2. USB disk: Format USB disk"\
                                "\r\n   3. USB disk: Create and write data to a file on USB disk"\
                                "\r\n   4. USB disk: Copy a file from USB disk to SD card"\
                                "\r\n   5. SD card: List SD card directory"\
                                "\r\n   6. SD card: Format SD card"\
                                "\r\n   7. SD card: Create and write data to a file on SD card"\
                                "\r\n   8. SD card: Copy a file from SD card to USB disk"\
                                "\r\n   User input: \r\n"

#define NOTIFY_NO_CLEAR                 (0U)
#define NOTIFY_ALL_CLEAR                (UINT32_MAX)
#define TICK_NO_DELAY                   (0U)

#define USB_OTG_MSC_THREAD_SLEEP_TICK   (1U)
#define FAT_THREAD_SLEEP_TICK           (1U)
#define TERM_THREAD_SLEEP_TICK          (1U)

#if (BSP_CFG_RTOS != 0U)
/* Macros for printing info, handling errors, and trapping errors */
#define PRINT_EP_BANNER                 (term_send_output_queue(TERM_OUTPUT_EP_BANNER, NULL, RESET_VALUE))
#define PRINT_EP_INFO                   (term_send_output_queue(TERM_OUTPUT_EP_INFO, NULL, RESET_VALUE))
#define PRINT_EP_MENU                   (term_send_output_queue(TERM_OUTPUT_EP_MENU, NULL, RESET_VALUE))
#define PRINT_INFO_STR(str)             (term_send_output_queue(TERM_OUTPUT_INFO_STR, (str), strlen(str)))
#define PRINT_ERR_STR(str)              (term_send_output_queue(TERM_OUTPUT_ERR_STR, (str), strlen(str)))
#define TERM_ERR_RET(cond, err, str)    ({\
                                            if (cond)\
                                            {\
                                                term_send_output_queue(TERM_OUTPUT_ERR_STR, (str), strlen(str));\
                                                return (err);\
                                            }\
                                        })
#define TERM_ERR_TRAP(cond, err, str)   ({\
                                            if (cond)\
                                            {\
                                                term_send_output_queue(TERM_OUTPUT_ERR_STR, (str), strlen(str));\
                                                term_send_output_queue(TERM_OUTPUT_ERR_TRAP, &(err), sizeof(uint32_t));\
                                                vTaskSuspend(xTaskGetCurrentTaskHandle());\
                                            }\
                                        })
#endif

/***********************************************************************************************************************
 * Typedef definitions
 **********************************************************************************************************************/
/* Enumeration for terminal event IDs */
typedef enum term_event_id
{
    /* Enumeration IDs for general use */
    TERM_INPUT_MESSAGE,
    TERM_OUTPUT_EP_BANNER,
    TERM_OUTPUT_EP_INFO,
    TERM_OUTPUT_EP_MENU,
    TERM_OUTPUT_INFO_STR,
    TERM_OUTPUT_ERR_STR,
    TERM_OUTPUT_ERR_TRAP,
} term_event_id_t;

#endif /* COMMON_APP_H_ */
