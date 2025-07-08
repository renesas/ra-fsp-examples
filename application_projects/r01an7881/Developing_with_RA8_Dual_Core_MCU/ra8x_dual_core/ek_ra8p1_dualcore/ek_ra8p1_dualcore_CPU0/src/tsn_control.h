/***********************************************************************************************************************
 * File Name    : tsn_control.h
 * Description  : This file contains the declarations for ADC-TSN initialization, start scan temperature
 *                stop scan temperature, read temperature data.
 * This header file is intended for use with the corresponding implementation
 * in tsn_control.c.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef TSN_CONTROL_H_
#define TSN_CONTROL_H_

#include "apn_cpu0.h"

/***********************************************************************************************************************
 * User-defined APIs
 **********************************************************************************************************************/
void init_adc_tsn(void);
void tsn_scan_start(void);
void tsn_scan_stop(void);
void adc_tsn_deinit(void);
void tsn_read_data(uint16_t *tsn_buf);

#endif /* TSN_CONTROL_H_ */
