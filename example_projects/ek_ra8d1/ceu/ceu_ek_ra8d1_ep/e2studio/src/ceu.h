/***********************************************************************************************************************
 * File Name    : ov3640.h
 * Description  : Contains data structures and functions used in hal_entry.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
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
