/***********************************************************************************************************************
 * File Name    : mram_ep.h
 * Description  : Contains macros, data structures and functions used in mram_ep.c
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef MRAM_EP_H_
#define MRAM_EP_H_

/*******************************************************************************************************************//**
 * @ingroup MRAM_EP
 * @{
 **********************************************************************************************************************/

#include "common_utils.h"

/* Start address for MRAM access in example project (avoid overwriting application code) */
#define MRAM_START_ADDR     (0x020FF000U)
/* Number of bytes to read/write */
#define NUM_BYTES           (128)
/* Erase block size is 32 bytes, so the number of blocks must be aligned accordingly */
#define BLOCK_NUM           ((NUM_BYTES + (BSP_FEATURE_MRAM_PROGRAMMING_SIZE_BYTES - 1)) / \
                             BSP_FEATURE_MRAM_PROGRAMMING_SIZE_BYTES)

/* Switch commands */
#define MRAM_WRITE          (1U)
#define MRAM_READ           (2U)
#define MRAM_ERASE          (3U)
#define EXIT                (4U)

#define BANNER_INFO         "\r\nThis example project demonstrates the basic functionality of the MRAM driver on"\
                            "\r\nRenesas RA MCUs using the Renesas Flexible Software Package (FSP). The user can"\
                            "\r\ninteract with the application via RTT input to execute write, read, and erase"\
                            "\r\noperations, starting at a fixed MRAM address. The operation status and data"\
                            "\r\ncontent are displayed in the RTT Viewer.\r\n"

#define MENU                "\r\nChoose an option from the menu below:\r\n"\
                            "Press 1 to WRITE to MRAM\r\n"\
                            "Press 2 to READ from MRAM\r\n"\
                            "Press 3 to ERASE MRAM\r\n"\
                            "Press 4 to EXIT"

/* MRAM operating function */
void mram_entry(void);

/*******************************************************************************************************************//**
 * @} (end defgroup MRAM_EP)
 **********************************************************************************************************************/

#endif /* MRAM_EP_H_ */
