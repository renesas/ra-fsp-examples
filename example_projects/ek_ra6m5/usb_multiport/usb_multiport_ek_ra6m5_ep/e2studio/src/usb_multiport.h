/***********************************************************************************************************************
 * File Name    : usb_multiport.h
 * Description  : Contains macros, data structures and function declaration used in EP
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
#ifndef USB_MULTIPORT_H_
#define USB_MULTIPORT_H_

#define MEDIA_INSERTED          ( 1U << 1 )       // USB media inserted state
#define MEDIA_REMOVED           ( 1U << 0 )       // USB media removed state
#define SUCCESS                 (0)               // Success case
#define USB_READ                (1u)              // RTT Input selection for read operation
#define USB_WRITE               (2u)              // RTT Input selection for Re-initialization of USB
#define WRITE_MODE              "w"               // File Write mode
#define READ_MODE               "r"               // File Read mode
#define FILE_NAME               "test.txt"        // File name for write/read
#define WRITE_ITEM_SIZE         (511u)            // The number of elements to read
#define READ_WRITE_FAILURE      (0)               // Fail case of read write
#define WAIT_TIME               (4000)            // Wait time for USB detection
#define CARRIAGE_RETURN         ('\r')            // Carriage return
#define SIZE                    (1U)              // Size to read byte by byte


#define MENU                    "\r\n USB HMSC Menu options\r\n"\
                                "\r\n 1. Read the content from file\r\n"\
                                "\r\n 2. Write the content to  file\r\n"\

#define EP_INFO                 "\r\nThis project demonstrates the basic functionality of USB Multiport on Renesas RA MCUs\r\n" \
                                "based on Renesas FSP using FreeRTOS. USB PCDC is configured for full speed with priority\r\n"\
                                "lower than USB HMSC configured as high speed. Jlink RTTViewer is used for the input and\r\n" \
                                "status display. Tera term is used to display the output and also to capture the data from\r\n"\
                                "it based on user input on Jlink RTT viewer. The project checks for the predefined file and\r\n"\
                                "reads the content and sends the data to usb_pcdc which is displayed on the Tera term. Data\r\n"\
                                "captured from Tera term(max 512 bytes) will be written to file. This EP requires external hardware\r\n"\
                                "connections and jumper setting modification prior to operation. Refer to the accompanying readme.txt\r\n"\
                                "file for more details.\r\n"

#define LINE_CODING_LENGTH       (0x07U)                // Line coding length
#define NUM_STRING_DESCRIPTOR    (7U)                   // String descriptor
#define READ_BUF_SIZE            (512)                  // Read buffer size

/* function declaration */
fsp_err_t file_system_and_usb_init(void);               /* Initialize USB HMSC device */
void clean_up(void);                                    /* Clean up freertos plus fat */
bool wait_till_usb_detected(void);                      /* Check and wait for usb detection */
void usb_read_operation(void);                          /* Performs USB read operation */
void usb_write_operation(void);                         /* Performs USB write operation */
bool check_usb_connected(void);                         /* Check USB connection status */
void process_usb_operation(uint8_t input_buffer);       /* USB read/write operation process */
fsp_err_t process_usb_pcdc_events(void);                /* Process pcdc events */
void fillDataBufferandWritetoFile(void);                    /* buffer update and write to file */


#endif /* USB_MULTIPORT_H_ */
