/***********************************************************************************************************************
 * File Name    : RA_ZMOD4XXX_Common.h
 * Description  : Contains common data structures and function prototypes for ZMOD4XX sensor
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
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#include "zmod_thread.h"
#include "common_init.h"

#ifndef RA_ZMOD4XXX_COMMON_H_
#define RA_ZMOD4XXX_COMMON_H_

#define DEMO_IRQ_ENABLE (1)

/* ZMOD sensor read data sequence enum */
typedef enum e_demo_sequence
{
    DEMO_SEQUENCE_1 = (1),
    DEMO_SEQUENCE_2,
    DEMO_SEQUENCE_3,
    DEMO_SEQUENCE_4,
    DEMO_SEQUENCE_5,
    DEMO_SEQUENCE_6,
    DEMO_SEQUENCE_7,
    DEMO_SEQUENCE_8,
    DEMO_SEQUENCE_9,
} demo_sequence_t;

/* ZMOD sensor call back status enum */
typedef enum e_demo_callback_status
{
    DEMO_CALLBACK_STATUS_WAIT = (0), DEMO_CALLBACK_STATUS_SUCCESS, DEMO_CALLBACK_STATUS_REPEAT,
} demo_callback_status_t;

/* Function declarations */
void g_zmod4xxx_sensor0_quick_setup(void);
void g_zmod4xxx_sensor1_quick_setup(void);
void start_iaq_1st_gen(void);
void start_oaq_1st_gen(void);

extern volatile demo_callback_status_t g_i2c_callback_status;
extern volatile demo_callback_status_t g_irq_callback_status;
extern volatile demo_callback_status_t g_i2c_callback_status1;
extern volatile demo_callback_status_t g_irq_callback_status1;

#endif /* RA_ZMOD4XXX_COMMON_H_ */
