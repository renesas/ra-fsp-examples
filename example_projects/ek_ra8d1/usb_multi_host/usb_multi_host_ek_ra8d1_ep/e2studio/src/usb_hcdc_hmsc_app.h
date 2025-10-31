/***********************************************************************************************************************
 * File Name    : usb_hcdc_hmsc_app.h
 * Version      : .
 * Description  : Contains data structures and functions used in usb_hcdc_thread_entry.c and usb_hmsc_thread_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef USB_HCDC_HMSC_APP_H_
#define USB_HCDC_HMSC_APP_H_

#include "common_utils.h"
#include "r_usb_typedef.h"
/** Macros definitions **/
#define EP_INFO                 "\r\nThis project demonstrates the basic functionalities of USB Host"\
                                "\r\nComposite (CDC and MSC) on Renesas RA MCUs based on Renesas FSP."\
                                "\r\nIn this example, the application will configure the MCU as a Host "\
                                "\r\nCommunications Device Class (HCDC) and as a Host Mass Storage"\
                                "\r\nClass (HMSC). The EP will present a menu list for user to communicate"\
                                "\r\nwith both the PMSC and PCDC devices. The usb_composite EP can be"\
                                "\r\nused as the PCDC device and the PMSC device to demonstrate this "\
                                "\r\nUSB Multi Host example. For HCDC operation, the host prompts the"\
                                "\r\nuser to input text for transfer to the PCDC device and retrieves"\
                                "\r\nechoed data from the PCDC device. For HMSC operation, the EP"\
                                "\r\nperforms File Operations such as Creating, Writing, Reading and"\
                                "\r\nDeleting of a file and USB Specific operations such as formatting,"\
                                "\r\nejecting and reinitializing a USB drive.\r\n"\
                                "\r\nIn case only the CDC device is connected or only the PMSC device"\
                                "\r\nis connected, The EP will provide the corresponding operation:"\
                                "\r\nHCDC operation or HMSC operation.\r\n"\

/* USB Drive HCDC Menu options */
#define USB_HCDC_MAIN_MENU      "\r\n!! The CDC device is connected !!"\
                                "\r\nUSB Multi Host menu options (CDC Operation):"\
                                "\r\n 1. Write and read data from the CDC device\r\n"


/* USB Drive HMSC Menu options */
#define USB_HMSC_MAIN_MENU      "\r\n!! The MSC device is connected !!"\
                                "\r\nUSB Multi Host menu options (MSC Operation):" \
                                "\r\n 2. Format USB drive" \
                                "\r\n 3. Write 10 KB to "FILE_NAME" file" \
                                "\r\n 4. Delete the created file" \
                                "\r\n 5. Safely eject USB drive\r\n"

/** Menu selection keys **/
#define SET_LINE_CODING             (USB_CDC_SET_LINE_CODING | USB_HOST_TO_DEV | USB_CLASS | USB_INTERFACE)
#define GET_LINE_CODING             (USB_CDC_GET_LINE_CODING | USB_DEV_TO_HOST | USB_CLASS | USB_INTERFACE)
#define SET_CONTROL_LINE_STATE      (USB_CDC_SET_CONTROL_LINE_STATE | USB_HOST_TO_DEV | USB_CLASS | USB_INTERFACE)
#define LINE_CODING_LENGTH          (0x07U)
#define VALUE_ZERO                  (0x0000U)
#define NO_WAIT_TIME                (0)
#define CDC_READ_DATA_LEN           (512)
#define CDC_WRITE_DATA_LEN          (512)
#define BYTES_RECEIVED_ZERO         (0U)
#define SUCCESS                     (0)
#define DELAY_200_TICK              (200)
#define RESET_VALUE                 (0x00)
#define ASCII_CHAR_A                (65)
#define MENU_EXIT_CRTL              (0x20)

#define FILE_NAME                   "ra_usb.txt"
#define WRITE_MODE                  "w"
#define READ_MODE                   "r"
#define WRITE_ITEM_SIZE             (10*1024u)

#define READ_WRITE_FAILURE          (0)
#define ASCII_CHAR_A                (65)
#define INDEX_ONE                   (1U)
#define INDEX_TWO                   (2U)
#define INDEX_FOUR                  (4U)
#define NUM_OF_APP_CALLBACK         (2)
#define INDEX_ZERO                  (0U)

#define IS_CDC_DEVICE_CONNECTED     (1 << 1)
#define IS_MSC_DEVICE_CONNECTED     (1 << 2)
#define USB_MSC_CMD_COMPLETE        (1 << 3)
#define USB_COMPOSITE_DEVICE_ENABLE (1 << 4)
#define ALL_BIT                     (0xFFFF)

#define USB_CDC_WRITE               (1)
#define USB_MSC_IGNORE              (1)
#define USB_MSC_FORMAT              (2)
#define USB_MSC_WRITE               (3)
#define USB_MSC_DELETE              (4)
#define USB_MSC_SAFELY_EJECT        (5)
#define MSC_MENU_INDEX_MAX          (USB_MSC_SAFELY_EJECT)
#define MSC_MENU_INDEX_MIN          (USB_MSC_FORMAT)
#define MENU_INDEX_MAX              (USB_MSC_SAFELY_EJECT)
#define MENU_INDEX_MIN              (USB_CDC_WRITE)
#define MIN_CLUSTERS_FAT16          (4085)
#define INPUT_STATUS_NO_INPUT       (0)
#define INPUT_STATUS_HAVE_INPUT     (1)

#define NEWLINE_CHAR                ('\n')
#define CARRIAGE_CHAR               ('\r')
#define SPACE_CHAR                  (' ')
#define KEY_SIZE_IN_BYTES           (64)
#define FALSE                       (0)
#define TRUE                        (1)

/* EP contents */
#define EP_VERSION                  ("1.0")
#define MODULE_NAME                 "USB Multiple Host"
#define BANNER_INFO                 "\r\n******************************************************************"\
                                    "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module     *"\
                                    "\r\n*   Example Project Version %s                                  *"\
                                    "\r\n*   Flex Software Pack Version  %d.%d.%d                            *"\
                                    "\r\n******************************************************************"\
                                    "\r\nRefer to readme.txt file for more details on Example Project and" \
                                    "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

/* Define color macros */
#define GREEN   "\033[1;32m"
#define RESET   "\033[0m"

/* Define user input macro */
bool handle_use_input();
void trim_line_endings(char *str);
#define APP_GET_USER_INPUT()        ({\
                                    bool input_status = false;\
                                    if (xSemaphoreTake(g_user_input_semaphore, portMAX_DELAY)) {\
                                    input_status = handle_use_input(); \
                                    xSemaphoreGive(g_user_input_semaphore);\
                                    vTaskDelay (10);\
                                    }\
                                    input_status;\
                                    })

/* Overwrite macro */
#undef APP_PRINT
#define APP_PRINT(fn_, ...)         ({\
                                    if (xSemaphoreTake(g_app_print_semaphore, portMAX_DELAY))\
                                    {\
		                            TERM_PRINTF((fn_), ##__VA_ARGS__);\
		                            xSemaphoreGive(g_app_print_semaphore);\
                                    }\
                                    })



/** Function declarations **/
void handle_error(fsp_err_t err, char *err_str);
void usb_app_hcdc_callback(usb_event_info_t *p_event_info, usb_hdl_t cur_task, usb_onoff_t usb_state);
void init_freertos_plus_fat(void);
bool check_usb_connection(bool check_for_cdc_connect, bool check_for_msc_connect);
extern TaskHandle_t usb_hcdc_thread;
extern TaskHandle_t usb_hmsc_thread;

#endif /* USB_HCDC_HMSC_APP_H_ */
