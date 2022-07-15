/***********************************************************************************************************************
 * File Name    : oximeter.h
 * Description  : Containd user defined data types used in oximeter
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
 * Copyright (C) 2020 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

#ifndef OXIMETER_H_
#define OXIMETER_H_

#include <stdint.h>
#include "ob1203_bio/SPO2/SPO2.h"
#include "oximstruct.h"
#include "r_typedefs.h"
#include "common_init.h"

#define LOW_AC_PERSIST (10)
#define BUSY 1
#define NOT_BUSY 0
#define DEMO_MODE (0)
#define CALIBRATION_MODE (1)
#define BUSY 1
#define NOT_BUSY 0
#define POWER_ON_PIN BSP_IO_PORT_01_PIN_07
#define PROX_DELAY 100
#define SAMPLES_TO_SKIP_FOR_UART (0)
#define MEAS_PS (1)

extern uint8_t mode;
extern uint8_t no_disp_spo2;
extern struct oxim ox;

void defaultConfig(struct oxim *_ox, struct ob1203 *_ob); //populate the default settings here
void ob1203_spo2_main(struct oxim * _ox, struct spo2 * _sp, struct ob1203 * _ob);
void main_init(struct oxim * _ox, struct spo2 * _sp, struct ob1203 * _ob);
void proxEvent(void);
void dataEvent(void);
void IRQ_Disable(void);
void IRQ_Enable(void);
void switch_mode(struct oxim *_ox, struct ob1203 *_ob, struct spo2 *_sp, uint8_t prox_bio_mode);
void reset_oximeter_params(struct oxim *_ox);
void get_sensor_data(struct oxim *_ox, struct ob1203 *_ob, struct spo2 *_sp);
void setup_calibration_mode(void);
void delayMicroseconds(uint32_t us);
bool_t R_OB1203_get_sensing_status(st_sensorsOB_t *p_data);
void check_for_alg_reset(struct oxim *_ox, struct ob1203 *_ob,struct spo2 *_sp);
void print_ob_data(struct oxim * _ox,struct spo2 * _sp);
#endif /* OXIMETER_H_ */
