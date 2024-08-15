/***********************************************************************************************************************
 * File Name    : crc_ep.h
 * Description  : Contains macros and function declarations.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef CRC_EP_H_
#define CRC_EP_H_

/* for on board LED */
#if defined (BOARD_RA4W1_EK) || defined (BOARD_RA6T1_RSSK) || defined (BOARD_RA6T2_MCK)
#define LED_ON             (BSP_IO_LEVEL_LOW)
#define LED_OFF            (BSP_IO_LEVEL_HIGH)
#else
#define LED_ON             (BSP_IO_LEVEL_HIGH)
#define LED_OFF            (BSP_IO_LEVEL_LOW)
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


#define EP_INFO     "\r\nThe example project demonstrates the typical use of the CRC HAL module APIs.\r\n"\
                    "The project demonstrates CRC operation for data transmission in normal mode and\r\n"\
                    "reception in snoop mode through sci interface. If board not support snoop mode (EK-RA4E2,\r\n"\
                    "EK-RA6E2, MCK-RA4T1, MCK-RA6T3) reception in normal mode through sci interface.\r\n"\
                    "If board not support snoop mode (FPB-RA0E1) reception in normal mode through sau interface.\r\n"\
                    "On Pressing any key through RTT Viewer CRC value in normal mode is calculated for 4 bytes of data.\r\n"\
                    "The calculated CRC value along with input data is transmitted and received on sci_uart\r\n"\
                    "(except for FPB-RA0E1 support sau_uart) through loop-back.\r\n"\
                    "\r\nOnce the transfer is complete, and if CRC value for snoop mode is zero and transmit\r\n"\
                    "and receive buffer are equal then On-board LED blinks as sign of successful CRC operation.\r\n"\
                    "On data mismatch, LED stays ON. Failure and status messages are displayed on RTT Viewer.\r\n"\


/* Check IO-port API return and trap error(if any error occurs) cleans up and display failure details on RTT viewer */
#define VALIDATE_IO_PORT_API(API)   ({\
                                    if (FSP_SUCCESS != (API))\
                                    {   APP_PRINT("%s API failed at Line number %d", \
                                                    #API, __LINE__);\
                                        cleanup();\
                                        APP_ERR_TRAP(true);\
                                    }\
                                    })

/* Function declaration */
void toggle_led(void);
void cleanup(void);
void deinit_crc(void);
void deinit_uart(void);


#endif /* CRC_EP_H_ */
