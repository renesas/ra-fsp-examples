/**********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO
 * THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
 *********************************************************************************************************************//**********************************************************************************************************************
 * File Name    : currency_table.h
 * Version      : .
 * Description  : .
 *********************************************************************************************************************/

/*
 * currency_table.h
 *
 *  Created on: 19 Jun 2023
 *      Author: b3800117
 */

#include <math.h>

#ifndef CURRENCY_TABLE_H
#define CURRENCY_TABLE_H

typedef struct st_currency_rates {
    double austrailian_dollar;      // AUS
    double british_pound_sterlin;   // GBP
    double canadian_dollar;         // CAN
    double chinese_yen;             // CNY
    double european_union_euro;     // EUR
    double hong_kong_dollar;        // HKD
    double indian_rupee;            // INR
    double jananese_yen;            // JPY
    double singaphorean_dollar;     // SGD
    double unitedstates_dollar;        // USD
} st_currency_rates_t;


extern st_currency_rates_t aus; // Austrailian Dollar

#endif /* CURRENCY_TABLE_H */
