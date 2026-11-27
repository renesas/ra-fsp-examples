/***********************************************************************************************************************
 * File Name    : ceu_example_app.h
 * Description  : Public interface for the CEU example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef CEU_EXAMPLE_APP_H_
#define CEU_EXAMPLE_APP_H_

#include "ceu_ep.h"

#define EP_VERSION      "1.0"

#define MODULE_NAME     "r_ceu"

#define BANNER_INFO     "\r\n********************************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                               *"\
                        "\r\n*   Example Project Version %s                                                *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                                           *"\
                        "\r\n********************************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#if defined (BOARD_RA8E1_FPB)
#define EP_INFO         "\r\nThe example project demonstrates the basic functionality of CEU driver on Renesas"\
                        "\r\nRA MCUs based on FSP. Upon successful initialization of CEU and camera setup,"\
                        "\r\nthe user can press any key to capture and store a QVGA (320 x 240) format image"\
                        "\r\nin SRAM. The captured image can be viewed in e2studio using the 'memory viewer'.\r\n"

#define QVGA_INFO       "\r\n\r\nSelected SRAM to store the image buffer."\
                        "\r\nImage resolution is fixed at QVGA 320x240."
#else
#define EP_INFO         "\r\nThe example project demonstrates the basic functionality of CEU driver on Renesas"\
                        "\r\nRA MCUs based on FSP. Upon successful initialization of CEU and camera setup,"\
                        "\r\nthe user is requested to select memory (1. SDRAM and 2. SRAM) to save image."\
                        "\r\nPress '1' to select 'SDRAM' to choose SXGA (1280 x 960) format and store the"\
                        "\r\nimage in SDRAM. Press '2' to select 'SRAM' to choose VGA (640 x 480) format and"\
                        "\r\nstore the image in SRAM. The captured image can be viewed in e2studio using the"\
                        "\r\n'memory viewer'.\r\n"

#define SXGA_INFO       "\r\n\r\nSelected SDRAM to store the image buffer."\
                        "\r\nImage resolution is fixed at SXGA 1280x960."

#define VGA_INFO        "\r\n\r\nSelected SRAM to store the image buffer."\
                        "\r\nImage resolution is fixed at VGA 640x480."
#endif /* BOARD_RA8E1_FPB */

#if defined (BOARD_RA8E1_FPB)
#define MAIN_MENU       "\r\nEnter any key to capture and store the image buffer."\
                        "\r\nUser input: "
#else
#define SELECT_SDRAM    ('1')
#define SELECT_SRAM     ('2')
#define MAIN_MENU       "\r\nSelect memory to store the image buffer:"\
                        "\r\n1. SDRAM"\
                        "\r\n2. SRAM"\
                        "\r\nUser input: "
#endif /* BOARD_RA8E1_FPB */

/* Single entry-point called in hal_entry.c */
void ceu_example_app_run(void);

#endif /* CEU_EXAMPLE_APP_H_ */
