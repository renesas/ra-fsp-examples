/***********************************************************************************************************************
 * File Name    : temp_sensor.h
 * Description  : Contains data structures and functions used in temp_sensor.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef TEMP_SENSOR_H_
#define TEMP_SENSOR_H_

/* Macro to provide delay in read ADC data */
#define DELAY_ONE_SECOND                (1U)

/* User defined functions */
fsp_err_t adc_read_data(uint16_t * adc_data);
void adc_deinit(void);
fsp_err_t adc_init(void);

#endif /* TEMP_SENSOR_H_ */
