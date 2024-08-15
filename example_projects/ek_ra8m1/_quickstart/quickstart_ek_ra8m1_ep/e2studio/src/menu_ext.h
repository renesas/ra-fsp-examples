/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

/***********************************************************************************************************************
 * File Name    : ospi_test.h
 * Description  : .
 **********************************************************************************************************************/


#ifndef OSPI_TEST_H_
#define OSPI_TEST_H_

extern test_fn ext_display_menu (void);

extern void ospi_performance_test (uint32_t data_size,
                                    uint32_t * ospi_performance_write_result,
                                    uint32_t * ospi_performance_read_result);

#endif /* OSPI_TEST_H_ */

