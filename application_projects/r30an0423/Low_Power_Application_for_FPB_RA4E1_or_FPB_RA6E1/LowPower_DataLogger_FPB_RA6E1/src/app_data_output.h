/***********************************************************************************************************************
 * File Name    : app_data_output.h
 * Description  : Contains data structures and functions used in app_data_output.h
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2022 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
************************************************************************************************************************/

#include "hal_data.h"

#define START_CODE							(0x5E) // '^'
#define END_CODE                            (0x24) // '$'
#define LF_CODE							    (0x0a) // '\n'

#define ATTRIBUTE_24H_PERIOD                (0x31) // '1'
#define ATTRIBUTE_MANUAL_TIMING		      	(0x32) // '2'
#define ATTRIBUTE_LEVEL_JUDGEMENT_RESULT    (0x33) // '3'

#define DELIMITER_CODE_C                    (0x2C) // ','
#define DELIMITER_CODE_S                    (0x2F) // '/'

#define PACKET_FORMAT_SIZE_START_CODE		(1u)
#define PACKET_FORMAT_SIZE_END_CODE         (1u)
#define PACKET_FORMAT_SIZE_DELIMITER_CODE   (1u)
#define PACKET_FORMAT_SIZE_ATTRIBUTE_CODE	(1u)
#define PACKET_FORMAT_SIZE_DATA_LENGTH_CODE	(4u)
#define PACKET_FORMAT_SIZE_DATA_NUMBER_CODE (2u)
#define PACKET_FORMAT_SIZE_TIME_DATA        (10u)

#define PACKET_POSITION_DATA_NUMBER         (PACKET_FORMAT_SIZE_START_CODE + PACKET_FORMAT_SIZE_ATTRIBUTE_CODE + PACKET_FORMAT_SIZE_DELIMITER_CODE + PACKET_FORMAT_SIZE_DATA_LENGTH_CODE + \
                                             PACKET_FORMAT_SIZE_DELIMITER_CODE + PACKET_FORMAT_SIZE_TIME_DATA + PACKET_FORMAT_SIZE_DELIMITER_CODE)
#define PACKET_POSITION_DATA_LENGTH         (PACKET_FORMAT_SIZE_START_CODE + PACKET_FORMAT_SIZE_ATTRIBUTE_CODE + PACKET_FORMAT_SIZE_DELIMITER_CODE)

#define SEND_BUFF_SIZE                      (984u) /* Set send buff size. It's same as maximum transfer size that can be expected.
                                                      Maximum transfer size = (a)Start code'^' 1byte + (b)Attribute code 1byte + (c)Delimiter code',' 1byte x2 + (d)Data length 4bytes +
                                                                              (e)Data 973bytes (Time 10bytes + Delimiter',' 1byte + Data num 2bytes + (Delimiter',' 1byte +
                                                                              Light data 4bytes + Delimiter'/' 1byte + Temp data 4bytes) * stored data num (2 times per hour * 24hours * double buffer)) +
                                                                              (f)End code'$' 1byte + (g)Linefeed code'\n' 1byte + Null
                                                                              = 984 bytes;
                                                   */

#define ASCII_DATA_SIZE                     (23u) /* Set ASCII data size. It's same as maximum size of data created by sprintf()*/

extern void data_output(uint8_t send_mode);
