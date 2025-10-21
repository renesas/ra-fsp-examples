/*
* Copyright (c) 2020 - 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/
/***********************************************************************************************************************
 * File Name    : menu_ext.h
 * Version      : 1.0
 * Description  : Definitions for the external memory read write test.
 **********************************************************************************************************************/
/**********************************************************************************************************************
 * History : DD.MM.YYYY Version  Description
 *         : 20.10.2025 1.00     First Release
 *********************************************************************************************************************/

//#include "r_typedefs.h"
#include "common_utils.h"

#ifndef OSPI_FUNCTIONS_FLASH_TEST_H_
#define OSPI_FUNCTIONS_FLASH_TEST_H_

test_fn ext_display_menu(void);

extern st_rgb565_text_block_page_t g_ext_page;

#endif /* OSPI_TEST_H_ */
