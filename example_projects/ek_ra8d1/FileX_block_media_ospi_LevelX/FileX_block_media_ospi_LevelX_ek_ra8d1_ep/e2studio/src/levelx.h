/**********************************************************************************************************************
 * File Name    : levelx.h
 * Description  : Contains data structures and functions used in levelx.c
 *********************************************************************************************************************/
/**********************************************************************************************************************
* Copyright (c) 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
**********************************************************************************************************************/

#ifndef LEVELX_H_
#define LEVELX_H_

/***********************************************************************************************************************
 * Includes
 **********************************************************************************************************************/
#include "common_utils.h"
#include "terminal.h"
#include "board_ospi.h"

/***********************************************************************************************************************
 * Macro definitions
 **********************************************************************************************************************/
#define MSG_SIZE                		(1024U)
#define UNUSE_ARGS                      (0)

#define NOR_SPI_FLASH_CLEARED           (0xFF)
#define BYTES_PER_WORD                  (4U)

#define SECTOR_WIRTE_SIZE               (128U)
#define SECTOR_READ_SIZE                (128U)
#define SECTOR_WIRTE_TEXT               "This operation demonstrates the low-level access with the specific\r\n"\
                                        "sector of the Flash\r\n"

/**********************************************************************************************************************
 * Public function prototypes
 *********************************************************************************************************************/
fsp_err_t sector_write(void);
fsp_err_t sector_read(void);
fsp_err_t sector_erase(void);
UINT sector_info_get(void);
UINT defragment_flash(void);

#endif /* LEVELX_H_ */
