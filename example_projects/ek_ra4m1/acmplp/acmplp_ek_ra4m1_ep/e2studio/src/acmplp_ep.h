/***********************************************************************************************************************
 * File Name    : acmplp_ep.h
 * Description  : Contains MACROS and function declarations .
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
 * Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
#ifndef ACMPLP_EP_H_
#define ACMPLP_EP_H_

/* Macros definitions */
#define B_SELECT_INPUT_PIN  (1u)      /* to enable input Pin to comparator of acmplp.*/
#define B_SELECT_VREF_PIN   (1u)      /* to enable reference voltage pin of acmplp.*/
#define BUF_SIZE            (16u)     /* buffer size for RTT input data */
#define NORMAL_MODE         (1u)      /* for acmplp normal mode operation */
#define EXIT                (2u)
#define INITIAL_VALUE       '\0'
#define BUF_INDEX           (0u)
#define DAC_MAX_VAL         (4095u)   /* Max count value for 12 bit DAC */

#define EP_INFO             "\r\nThe project initializes ACMPLP module in Normal mode." \
    "\r\nIn Normal mode, user can enter DAC value(input to ACMPLP) within permitted range(0-4095)." \
    "\r\nWhen DAC input value is greater than set reference voltage(another input to ACMPLP)," \
    "\r\nComparator output status is HIGH and on-board LED is turned ON." \
    "\r\nIf the output status (when DAC input less than ref voltage) is LOW," \
    "then the LED is turned OFF.\r\n"


/* Function declaration */
fsp_err_t acmplp_operation(void);
fsp_err_t init_acmplp(void);
void deinit_acmplp(void);


#endif /* ACMPLP_EP_H_ */
