/*
 * qspi_operations.h
 *
 *  Created on: Aug 20, 2022
 *      Author: a5099044
 */

#ifndef QSPI_OPERATIONS_H_
#define QSPI_OPERATIONS_H_

void      init_qspi(void);
void      deinit_qspi(const spi_flash_protocol_t spi_protocol_mode);
fsp_err_t get_flash_status(void);

#endif                                 /* QSPI_OPERATIONS_H_ */
