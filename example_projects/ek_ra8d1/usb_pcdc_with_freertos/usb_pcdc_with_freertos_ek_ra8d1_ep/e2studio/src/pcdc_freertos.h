/***********************************************************************************************************************
 * File Name    pcdc_freertos.h
 * Description  : Contains macros, data structures and functions used in the pcdc_freertos source code
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2022 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef PCDC_FREERTOS_H_
#define PCDC_FREERTOS_H_

#define LINE_CODING_LENGTH             (0x07U)                // Line coding length
#define DATA_LEN                       (512U)                 // Data Length
#define BUFFER_INDEX                   (511U)
#define ARRAY_INDEX                    (0U)
#define INVALID_WAIT_TIME              (0U)
#define INVALID_SIZE                   (0U)
#define NUM_STRING_DESCRIPTOR          (7U)
#define MAX_DELAY                      (2000U)
#define BAUD_ERROR_RATE                (5000U)
#define MSG_WAIT_TIME                  (10000U)
#define INCREMENT_BYTES                (2U)
#define SCI_UART_INVALID_16BIT_PARAM   (0xFFFFU)
/* User defined function */
void deinit_uart(void);
void deinit_usb(void);

#define EP_INFO                 "\r\nThis project demonstrates the basic functionality of USB PCDC driver on Renesas RA MCUs\r\n" \
                                "based on Renesas FSP using FreeRTOS. In this example, User will be able\r\n"\
                                "to use the RA board as a  CDC to UART converter by connecting through a COM port from the HOST PC.\r\n"\
                                "The converter performs the data reception and data transmission asynchronously,\r\n"\
                                "user can connect any UART device to receive the data typed on the serial terminal(Tera term) and \r\n"\
                                "send any data to the converter which will show up on the serial terminal(Tera term). \r\n"\
                                "User can also change the baud rate, stop bit and parity bit configuration at run time through serial terminal (Term term)\r\n"\
                                "Status of application will be displayed on Jlink RTTViewer\r\n"
#endif /* PCDC_FREERTOS_H_ */
