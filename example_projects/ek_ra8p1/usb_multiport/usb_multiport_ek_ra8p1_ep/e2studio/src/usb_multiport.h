/***********************************************************************************************************************
 * File Name    : usb_multiport.h
 * Description  : Contains macros, data structures and function declaration used in the EP
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef USB_MULTIPORT_H_
#define USB_MULTIPORT_H_

#define MEDIA_INSERTED         ( 1U << 1 )       /* USB media inserted state */
#define MEDIA_REMOVED          ( 1U << 0 )       /* USB media removed state */
#define SUCCESS                (0)               /* Success case */
#define USB_READ               (1u)              /* RTT input selection for read operation */
#define USB_WRITE              (2u)              /* RTT input selection for re-initialization of USB */
#define WRITE_MODE             "w"               /* File Write mode */
#define READ_MODE              "r"               /* File Read mode */
#define FILE_NAME              "test.txt"        /* File name for write/read */
#define WRITE_ITEM_SIZE        (511u)            /* The number of elements to read */
#define READ_WRITE_FAILURE     (0)               /* Fail case of read write */
#define WAIT_TIME              (4000)            /* Wait time for USB detection */
#define CARRIAGE_RETURN        ('\r')            /* Carriage return */
#define SIZE                   (1U)              /* Size to read byte by byte */


#define MENU                   "\r\n USB HMSC Menu options\r\n"\
                               "\r\n 1. Read the content from file\r\n"\
                               "\r\n 2. Write the content to file\r\n"\

#define EP_INFO                "\r\nThis project demonstrates the basic functionality of USB Multiport on Renesas\r\n"\
                               "RA MCUs based on Renesas FSP using FreeRTOS. USB PCDC is configured for full speed\r\n"\
                               "with priority lower than USB HMSC configured as high speed. J-Link RTT Viewer is\r\n"\
                               "used for the input and status display. Tera Term is used to display the output and\r\n"\
                               "also to capture the data from it based on user input on J-Link RTT Viewer.\r\n"\
                               "The project checks for the predefined file and reads the content and sends the\r\n"\
                               "data to USB PCDC which is displayed on the Tera Term. Data captured from\r\n"\
                               "Tera Term (max 512 bytes) will be written to file. This EP requires external\r\n"\
                               "hardware connections and jumper setting modification prior to operation. Refer\r\n"\
                               "to the accompanying readme.txt file for more details.\r\n"

#define LINE_CODING_LENGTH      (0x07U)                /* Line coding length */
#define NUM_STRING_DESCRIPTOR   (7U)                   /* String descriptor */
#define READ_BUF_SIZE           (512)                  /* Read buffer size */

/* Function declarations */
fsp_err_t file_system_and_usb_init(void);              /* Initialize USB HMSC device */
void clean_up(void);                                   /* Clean up FREERTOS plus fat */
bool wait_till_usb_detected(void);                     /* Check and wait for USB detection */
void usb_read_operation(void);                         /* Performs USB read operation */
void usb_write_operation(void);                        /* Performs USB write operation */
bool check_usb_connected(void);                        /* Check USB connection status */
void process_usb_operation(uint8_t input_buffer);      /* USB read/write operation process */
fsp_err_t process_usb_pcdc_events(void);               /* Process PCDC events */
void fillDataBufferandWritetoFile(void);               /* Buffer update and write to file */

#endif /* USB_MULTIPORT_H_ */
