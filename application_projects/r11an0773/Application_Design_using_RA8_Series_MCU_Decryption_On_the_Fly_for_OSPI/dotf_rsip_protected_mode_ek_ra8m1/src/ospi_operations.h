/*
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
*/


#ifndef OSPI_OPERATIONS_H_
#define OSPI_OPERATIONS_H_

/***************************************************************************************************************************
 * Includes   <System Includes> , "Project Includes"
 ***************************************************************************************************************************/
#include "hal_data.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/

#define OSPI_DATA_LENGTH                        (0x120)

/**********************************************************************************************************************
 * Exported global variables
 **********************************************************************************************************************/
extern spi_flash_direct_transfer_t direct_command_preset[5];

extern volatile const uint32_t ospi_data[OSPI_DATA_LENGTH];
extern volatile uint32_t ospi_read_data[OSPI_DATA_LENGTH];

/**********************************************************************************************************************
 * Global Function Prototypes
 **********************************************************************************************************************/
extern void ospi_init(void);
extern void execute_encrypted_func_ospi(void);
extern void execute_plaintext_fun_ospi(void);

#endif /* OSPI_OPERATIONS_H_ */
