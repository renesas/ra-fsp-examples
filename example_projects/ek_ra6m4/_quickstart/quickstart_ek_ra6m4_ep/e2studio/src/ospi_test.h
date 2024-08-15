/***********************************************************************************************************************
 * File Name    : ospi_test.h
 * Description  : .
 **********************************************************************************************************************/

/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef OSPI_TEST_H_
#define OSPI_TEST_H_

/**********************************************************************************************************************
 * Function Name: ospi_performance_test
 * Description  : .
 * Arguments    : data_size
 *              : ospi_performance_write_result
 *              : ospi_performance_read_result
 * Return Value : .
 *********************************************************************************************************************/
void ospi_performance_test (uint32_t data_size,
                            uint32_t * ospi_performance_write_result,
                            uint32_t * ospi_performance_read_result);
#endif /* OSPI_TEST_H_ */
