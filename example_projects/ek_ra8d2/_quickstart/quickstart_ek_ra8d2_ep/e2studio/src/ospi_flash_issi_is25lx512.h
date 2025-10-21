/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/**********************************************************************************************************************
 * File Name    : ospi_flash_issi_is25lx512.c
 * Version      : 1.0
 * Description  : OSPI flash ISSI_IS25LX512 device interface file
 *********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 20.10.2025 1.00     First Release
 *********************************************************************************************************************/

#ifndef OSPI_FLASH_ISSI_IS25LX512_H_
#define OSPI_FLASH_ISSI_IS25LX512_H_

fsp_err_t ospi_flash_open(void);
void ospi_performance_test(uint32_t data_size, uint32_t *ospi_performance_write_result, uint32_t *ospi_performance_read_result);

#endif /* OSPI_FLASH_ISSI_IS25LX512_H_ */
