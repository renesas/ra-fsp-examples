/***********************************************************************************************************************
 * File Name    : RA_ZMOD4XXX_Common.h
 * Description  : Contains common data structures and function prototypes for ZMOD4XX sensor
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
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
void start_oaq_no2_o3(void);

extern volatile demo_callback_status_t g_i2c_callback_status;
extern volatile demo_callback_status_t g_irq_callback_status;
extern volatile demo_callback_status_t g_i2c_callback_status1;
extern volatile demo_callback_status_t g_irq_callback_status1;

#endif /* RA_ZMOD4XXX_COMMON_H_ */
