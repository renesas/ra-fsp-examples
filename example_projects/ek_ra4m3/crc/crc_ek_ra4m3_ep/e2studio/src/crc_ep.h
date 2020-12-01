/***********************************************************************************************************************
 * File Name    : crc_ep.h
 * Description  : Contains macros and function declarations.
 **********************************************************************************************************************/
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

#ifndef CRC_EP_H_
#define CRC_EP_H_

/* for on board LED */
#if defined (BOARD_RA4W1_EK) || defined (BOARD_RA6T1_RSSK)
#define LED_ON             BSP_IO_LEVEL_LOW
#define LED_OFF            BSP_IO_LEVEL_HIGH
#else
#define LED_ON             BSP_IO_LEVEL_HIGH
#define LED_OFF            BSP_IO_LEVEL_LOW
#endif

/*Length of input buffer to calculate CRC in normal mode*/
#define NUM_BYTES          (4U)

/* size of input buffer */
#define BUF_LEN            (8U)

/* Led toggle delay */
#define TOGGLE_DELAY       (0x15E)

/* 8 and 16 bit seed value and data length */
#define SEED_VALUE             (0x00000000)
#define EIGHT_BIT_DATA_LEN     (5U)
#define SIXTEEN_BIT_DATA_LEN   (6U)


#define EP_INFO  "\r\nThis Example Project demonstrates CRC operation for transmission\r\n"\
				  "in normal mode and reception in snoop mode. On any key press through\r\n"\
				  "RTT Viewer CRC value in normal mode is calculated for 4 bytes of data\r\n"\
				  "The calculated CRC value along with the data is trans-received on\r\n"\
				  "sci_uart through loop-back.\r\n"\
				  "\r\nIf the CRC value for snoop mode is zero and transmit and receive\r\n"\
				  "buffer are equal then On-board LED blinks as sign of successful\r\n"\
				  "CRC operation. On a data mismatch, LED stays ON. Failure messages\r\n"\
				  "and status is displayed on RTTViewer.\r\n"

/* Check IO-port API return and trap error(if any error occurs) cleans up and display failure details on RTT viewer */
#define VALIDATE_IO_PORT_API(API)	if (FSP_SUCCESS != API)\
									{	APP_PRINT("%s API failed at Line number %d", \
													#API, __LINE__);\
										cleanup();\
										APP_ERR_TRAP(true);\
									}

/* Function declaration */
void toggle_led(void);
void cleanup(void);
void deinit_crc(void);
void deinit_uart(void);


#endif /* CRC_EP_H_ */
