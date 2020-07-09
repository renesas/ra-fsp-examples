/***********************************************************************************************************************
 * File Name    : slcdc_ep.h
 * Description  : Contains declarations of data structures used in hal_entry.c.
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

#ifndef SLCDC_EP_H_
#define SLCDC_EP_H_

#define EP_INFO  "\r\nThis example project demonstrates the typical use of the SLCDC HAL module APIs.\r\n"\
                 "The project displays digits from 990 to 9999 on Segment LCD with increment of 21.\r\n"\
                 "If an error occurs in the operation, the project displays 'Err' on the LCD screen and\r\n"\
                 "RTT will print the error.  Refer Readme.txt file to know details about the required\r\n"\
                 "external connections\r\n"

#define ZERO_INDEX                      (0U)
#define FIRST_INDEX                     (1U)

#define INITIAL_COUNT                   (990U)
#define FINAL_COUNT                     (9999U)
#define COUNT_INCREMENT_STEP            (21U)
#define COUNT_DELAY                     (20U)
#define LCD_STABILIZATION_DELAY         (5U)

#define TEN_THOUSAND                    (10000U)
#define THOUSANDS                       (1000U)
#define HUNDREDS                        (100U)
#define TENS                            (10U)
#define UNITS                           (1U)
#define HUNDREDS_DIGIT_MAX_VAL          (999U)
#define TENS_DIGIT_MAX_VAL              (99U)
#define UNIT_DIGIT_MAX_VAL              (9U)
#define NUMBER_OF_SEGMENT_TO_WRITE      (1U)

#define TOTAL_NUMBER_OF_DIGITS          (10U)
#define NUMBER_OF_DIGITS_ON_DISPLAY     (4U)
#define NUMBER_OF_SEG_FOR_EACH_DIGIT    (2U)
#define NUMBER_OF_DIGITS_REQ_FOR_ERR    (2U)

/* enum for the digit place */
typedef enum e_number_units {
    MN_UNITS,
    MN_TENS,
    MN_HUNDREDS,
    MN_THOUSANDS,
} number_units_t;

/* Structure to hold LCD segment addresses. There are total 4 digits on LCD and printing a digit requires 2 segment.
 * This structure holds the address of each segment corresponding to each digits place */
const uint8_t digit_start_addresses[NUMBER_OF_DIGITS_ON_DISPLAY][NUMBER_OF_SEG_FOR_EACH_DIGIT]=
{
 { 7,8 }, // units     digit is controlled by SLCD Data register 7 & 8
 { 5,6 }, // tens      digit is controlled by SLCD Data register 5 & 6
 { 3,4 }, // hundreds  digit is controlled by SLCD Data register 3 & 4
 { 1,2 }, // thousands digit is controlled by SLCD Data register 1 & 2
};

/* Structure holding Data corresponds to each digit */
const uint8_t digit_data[TOTAL_NUMBER_OF_DIGITS][NUMBER_OF_SEG_FOR_EACH_DIGIT] =
{
 { 0xb, 0xe },    // 0 digit is display by writing 0xb and 0xe to the appropriate digit start address
 { 0x0, 0x6 },    // 1 digit is display by writing 0x0 and 0x6 to the appropriate digit start address
 { 0x7, 0xc },    // 2 digit is display by writing 0x7 and 0xc to the appropriate digit start address
 { 0x5, 0xe },    // 3 digit is display by writing 0x5 and 0xe to the appropriate digit start address
 { 0xc, 0x6 },    // 4 digit is display by writing 0xc and 0x6 to the appropriate digit start address
 { 0xd, 0xa },    // 5 digit is display by writing 0xd and 0xa to the appropriate digit start address
 { 0xf, 0xa },    // 6 digit is display by writing 0xf and 0xa to the appropriate digit start address
 { 0x0, 0xe },    // 7 digit is display by writing 0x0 and 0xe to the appropriate digit start address
 { 0xf, 0xe },    // 8 digit is display by writing 0xf and 0xe to the appropriate digit start address
 { 0xd, 0xe },    // 9 digit is display by writing 0xd and 0xe to the appropriate digit start address
};

/* Structure holding data for "E" & "r" */
const uint8_t err_data[NUMBER_OF_DIGITS_REQ_FOR_ERR][NUMBER_OF_SEG_FOR_EACH_DIGIT] =
{
 { 0xf, 0x8 },    // Alphabet 'E' data
 { 0xa, 0x8 },    // Alphabet 'r' data
};

#endif /* SLCDC_EP_H_ */
