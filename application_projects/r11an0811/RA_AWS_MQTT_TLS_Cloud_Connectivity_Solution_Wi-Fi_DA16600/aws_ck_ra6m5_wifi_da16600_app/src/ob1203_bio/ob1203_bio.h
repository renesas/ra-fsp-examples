/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name    : ob1203_bio.h
 * Description  : Contains macros, data structures and functions declarations for OB1203
 ***********************************************************************************************************************/

#ifndef OB1203_BIO_OB1203_BIO_H_
#define OB1203_BIO_OB1203_BIO_H_

#include <OB_driver/rm_ob1203.h>
#include "SPO2/SPO2.h"

#define OB1203_BIO_IRQ_ENABLE (1)
#define OB1203_BIO_LED_MAX_NUM (2)

typedef enum e_ob1203_bio_led_type
{
    OB1203_BIO_LED_TYPE_IR = 0,
    OB1203_BIO_LED_TYPE_RED = 1,
} ob1203_bio_led_type_t;

typedef struct st_ob1203_bio_ppg_data_check
{
    uint16_t threshold;
    uint16_t warm_up;
} ob1203_bio_ppg_data_check_t;

typedef struct st_ob1203_led_target_counts
{
    uint32_t target_counts;
    uint8_t in_range_persist;
    bool in_range;
} ob1203_led_target_counts_t;

typedef struct st_ob1203_bio_data
{
    uint16_t heart_rate;
    uint16_t spo2;
    uint16_t respiration_rate;
    float perfusion_index;
} ob1203_bio_data_t;

typedef struct st_ob1203_bio_gain_currents
{
    rm_ob1203_led_current_t currents;
    rm_ob1203_gain_t gain;
} ob1203_bio_gain_currents_t;

typedef struct st_ob1203_bio
{
    rm_ob1203_instance_t *p_prox_instance;
    rm_ob1203_instance_t *p_ppg_instance;
    rm_ob1203_instance_t *p_instance;
    rm_ob1203_operation_mode_t mode;
    bool mode_change;
    uint32_t low_signal_counts;
    spo2_t * p_spo2;
    ob1203_bio_ppg_data_check_t data_check;
    ob1203_led_target_counts_t led_target_counts[OB1203_BIO_LED_MAX_NUM];
    uint8_t overflow;
    uint8_t number_of_samples;
    rm_ob1203_device_interrupt_cfg_t interrupt_cfg;
    uint32_t ppg_data_for_pi;
    bool prev_in_range;
    bool target_counts_change;
    bool algorithm_ready;
} ob1203_bio_t;

bool ob1203_bio_open(ob1203_bio_t *const p_bio,
                     rm_ob1203_instance_t *const p_prox_instance,
                     rm_ob1203_instance_t *const p_ppg_instance,
                     spo2_t *const p_spo2);
bool ob1203_bio_measurement_start(ob1203_bio_t *const p_bio);
bool ob1203_bio_operation_mode_init(ob1203_bio_t *const p_bio);
bool ob1203_bio_measurement_stop(ob1203_bio_t *const p_bio);
bool ob1203_bio_measurement_period_wait(ob1203_bio_t *const p_bio);
bool ob1203_bio_ppg_raw_data_read(ob1203_bio_t *const p_bio, rm_ob1203_raw_data_t *const p_raw_data);
bool ob1203_bio_ppg_data_calculate(ob1203_bio_t *const p_bio,
                                   rm_ob1203_raw_data_t *const p_raw_data,
                                   rm_ob1203_ppg_data_t *const p_ppg_data,
                                   bool *const p_valid);
bool ob1203_bio_auto_gain_currents_control(ob1203_bio_t *const p_bio,
                                           rm_ob1203_ppg_data_t *const p_ppg_data,
                                           ob1203_bio_gain_currents_t *const p_gain_currents,
                                           bool *const p_update);
bool ob1203_bio_gain_currents_reconfigure(ob1203_bio_t *const p_bio, ob1203_bio_gain_currents_t *const p_gain_currents);
bool ob1203_bio_algorithm_preparation_check(ob1203_bio_t *const p_bio, bool *const p_ready);
bool ob1203_bio_mode_change_check(ob1203_bio_t *const p_bio, bool *const p_change);
bool ob1203_bio_samples_add(ob1203_bio_t *const p_bio, rm_ob1203_ppg_data_t *const p_ppg_data);
bool ob1203_bio_samples_clear(ob1203_bio_t *const p_bio);
bool ob1203_bio_algorithm_reset(ob1203_bio_t *const p_bio);
bool ob1203_bio_hr_spo2_calculate(ob1203_bio_t *const p_bio, ob1203_bio_data_t *const p_bio_data);
bool ob1203_bio_rr_calculate(ob1203_bio_t *const p_bio, ob1203_bio_data_t *const p_bio_data);
bool ob1203_bio_perfusion_index_check(ob1203_bio_t *const p_bio, bool * const p_valid);
#endif /* OB1203_BIO_OB1203_BIO_H_ */
