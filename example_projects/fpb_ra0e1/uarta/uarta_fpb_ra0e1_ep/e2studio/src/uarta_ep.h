/***********************************************************************************************************************
 * File Name    : uarta_ep.h
 * Description  : Contains data structures and functions used in uarta_ep.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef UARTA_EP_H_
#define UARTA_EP_H_

#define EP_INFO     "\r\nThe Example Project demonstrates the basic functionality of the UARTA driver on"\
		            "\r\nRenesas RA MCUs based on FSP, the Virtual COM is used as example usage of the UARTA."\
					"\r\nThe user can select UARTA baud rate from the menu on J-Link RTT Viewer (e.g., 9600 bps,"\
					"\r\n14400 bps, 19200 bps,...). After the UARTA is initialized successfully, a message prompts"\
					"\r\nthe user to open and configure parameters for Tera Term on PC. Then the user can enter and"\
					"\r\nsend an ASCII string (up to 256 characters) from Tera Term to the RA board, which will also"\
					"\r\nbe displayed on J-Link RTT Viewer and echoed back to Tera Term by the RA board. Additionally,"\
                    "\r\nthe EP information and any error messages will be displayed on J-Link RTT Viewer.\r\n"

#define EP_MESSAGE  "\r\nPlease open the Tera Term application on the PC and set up the serial connection to:"\
                    "\r\nThe COM port is facilitated by the J-Link OB VCOM, operating at a baud rate of %d bps,"\
                    "\r\nwith a data length of 8 bits, no parity check, one-bit stop, and without any flow control."\
                    "\r\n\r\n"

#define EP_MENU     "\r\n UARTA Baud Rate Selection Menu:"\
		            "\r\n 1. 9600 bps"\
					"\r\n 2. 14400 bps"\
					"\r\n 3. 19200 bps"\
					"\r\n 4. 38400 bps"\
					"\r\n 5. 57600 bps"\
					"\r\n 6. 115200 bps"\
					"\r\n Please select the desired baud rate by entering the corresponding number: "

#define ECHO_MESSAGE    "Received data from RA board: "

/* Macro to define UARTA operation selection */
#define UARTA_9600                  (1U)
#define UARTA_14400                 (2U)
#define UARTA_19200                 (3U)
#define UARTA_38400                 (4U)
#define UARTA_57600                 (5U)
#define UARTA_115200                (6U)

/* Macro for RTT Viewer */
#define RTT_DELAY_VALUE             (200U)

/* Macro for UARTA receive */
#define NULL_CHAR                   ('\0')
#define CR_ASCII                    (0x0D)
#define LF_ASCII                    (0x0A)
#define BUFFER_LEN                  (256U)

/* Macro for UARTA transmit */
#define TIME_OUT_FOR_ONE_BYTE       (1000)
#define TIME_OUT_VALUE              (1U)
#define TIME_OUT_UNIT               (BSP_DELAY_UNITS_MICROSECONDS)

/* Scaling factor for baud rate error values, 10ppm or .001% */
#define UARTA_BAUD_ERROR_SCALING_1  (95)
#define UARTA_BAUD_ERROR_SCALING_2  (170)

/* Mask for valid mapping of uart_clock_div_t to UTA0CK.CK */
#define UARTA_CLOCK_DIV_MASK        (0x07U)

/* Wait time needs to be rounded up to enable TX/RX. */
#define UARTA_CONVERT_TO_MICRO_SECOND   (999999U)

/* Public function declarations */
/***********************************************************************************************************************
 *  Function Name: uarta_ep_entry
 *  Description  : This function is used to initialize the related module and start the UARTA example operation.
 *  Arguments    : None
 *  Return Value : None
 **********************************************************************************************************************/
void uarta_ep_entry (void);
/***********************************************************************************************************************
* End of function uarta_ep_entry
***********************************************************************************************************************/

#endif /* UARTA_EP_H_ */
