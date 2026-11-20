/***********************************************************************************************************************
 * File Name    : levelx_ep.h
 * Description  : Contains macros, data structures, and functions used in levelx_ep.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 ***********************************************************************************************************************/

#ifndef LEVELX_EP_H_
#define LEVELX_EP_H_

#include "board_ospi.h"

/* Macro for the message buffer size */
#define MSG_SIZE              (1024U)

/* Macro for unused API arguments */
#define UNUSE_ARGS            (0)

/* Macro for the cleared NOR SPI flash byte value */
#define NOR_SPI_FLASH_CLEARED (0xFF)

/* Macro for the number of bytes per word */
#define BYTES_PER_WORD        (4U)

/* Macros for the sector write and read buffer sizes */
#define SECTOR_WRITE_SIZE     (128U)
#define SECTOR_READ_SIZE      (128U)

/* Function declaration */
UINT levelx_init(void);
UINT defragment_flash(void);
UINT sector_info_get(ULONG logical_sector, ULONG **physical_sector_map_entry, ULONG **physical_sector_address);
fsp_err_t sector_write(ULONG * const p_flash_addr, ULONG * const p_src);
fsp_err_t sector_read(ULONG * const p_flash_addr, ULONG * const p_dest);
fsp_err_t sector_erase(ULONG * p_sector_address);

#endif /* LEVELX_EP_H_ */
