/***********************************************************************************************************************
 * File Name    : acmplp_ep.h
 * Description  : Contains MACROS and function declarations .
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#ifndef ACMPLP_EP_H_
#define ACMPLP_EP_H_

/* Macros definitions */
#define B_SELECT_INPUT_PIN  (1u)      /* to enable input Pin to comparator of acmplp.*/
#define B_SELECT_VREF_PIN   (1u)      /* to enable reference voltage pin of acmplp.*/
#define BUF_SIZE            (16u)     /* buffer size for RTT input data */
#define NORMAL_MODE         (1u)      /* for acmplp normal mode operation */
#define EXIT                (2u)
#define INITIAL_VALUE       ('\0')
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
