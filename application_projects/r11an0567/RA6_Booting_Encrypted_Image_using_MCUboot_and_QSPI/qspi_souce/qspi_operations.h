/***********************************************************************************************************************
* Copyright (c) 2022 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef QSPI_OPERATIONS_H_
#define QSPI_OPERATIONS_H_

void init_qspi(void);
void deinit_qspi(const spi_flash_protocol_t spi_protocol_mode);
fsp_err_t get_flash_status(void);

#endif /* QSPI_OPERATIONS_H_ */
