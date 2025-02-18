/***********************************************************************************************************************
 * File Name    : dac_operation.h
 * Description  : Contains MACROS and function declarations .
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/
#ifndef DAC_OPERATION_H_
#define DAC_OPERATION_H_

/* function declarations */
fsp_err_t dac_write(uint16_t * p_val);
fsp_err_t init_dac(void);
void deinit_dac(void);

#endif /* DAC_OPERATION_H_ */
