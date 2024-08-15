/***********************************************************************************************************************
 * File Name    : ov3640.h
 * Description  : Contains data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2023 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef CEU_H_
#define CEU_H_

#include "common_utils.h"

/* MACRO for RTT Viewer */
#define NULL_CHAR               ('\0')
#define SELECT_SDRAM_CHAR       ('1')
#define SELECT_SRAM_CHAR        ('2')
#define SELECT_END_EP_CHAR      ('3')
#define INDEX_CHECK             (0U)
#define MAIN_MENU               "\r\nSelect memory to store the image buffer:"\
                                "\r\n1. SDRAM"\
								"\r\n2. SRAM"\
								"\r\nSelect: \r\n"

/* MACRO for image resolution */
#define VGA_WIDTH               (640U)
#define VGA_HEIGHT              (480U)
#define SXGA_WIDTH              (1280U)
#define SXGA_HEIGHT             (960U)
#define BYTE_PER_PIXEL          (2U)

/* Functions declarations */
fsp_err_t ceu_operation (capture_instance_t const * p_instance, uint8_t * p_buffer, uint32_t width, uint32_t height);
void selection_menu (void);
void handle_error (fsp_err_t err,  const char *err_str);

#endif /* CEU_H_ */
