/***********************************************************************************************************************
 * File Name    : mipi_csi.h
 * Description  : Contains data structures and functions used in mipi_csi.c.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2025 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef MIPI_CSI_H_
#define MIPI_CSI_H_

#include "common_utils.h"
#include "bsp_pin_cfg.h"
#include "camera_sensor.h"
#include "glcdc_display.h"
#include "switch_init.h"

#define EP_VERSION      ("1.0")
#define MODULE_NAME     "r_mipi_csi"
#define BANNER_INFO     "\r\n********************************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                          *"\
                        "\r\n*   Example Project Version %s                                                *"\
                        "\r\n*   Flex Software Pack Version  %d.%d.%d                                          *"\
                        "\r\n********************************************************************************"\
                        "\r\nRefer to the accompanying .md file for Example Project details and" \
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO         "\r\nThis example project demonstrates the basic functionality of MIPI CSI on Renesas\r\n"\
                        "RA MCUs using Renesas FSP. The I2C module configures an external camera sensor to\r\n"\
                        "operate with the MIPI interface. Captured image data is received by the MIPI CSI\r\n"\
                        "module, then processed by the VIN module: clipped per ITU-R BT.601, converted from\r\n"\
                        "YCbCr-422 to RGB, scaled to the user-selected resolution, and stored in SDRAM.\r\n"\
                        "The resolution change is handled by the VIN module, allowing dynamic output\r\n"\
                        "resizing without reconfiguring the camera sensor.\r\n\r\n"\
                        "Before choosing the camera mode, the user is prompted via terminal to select the\r\n"\
                        "desired image resolution. After that, the user can choose to capture a live image\r\n"\
                        "or a test pattern by typing '1', '2', or '3' to return to the main menu.\r\n"\
                        "Captured images are viewable as raw data in e2studio using the Memory view.\r\n"\
                        "If a Parallel Graphics LCD is connected, the image is also displayed on-screen.\r\n\r\n"


#define SELECT_LIVE_CAMERA       ('1')
#define SELECT_TEST_PATTERN      ('2')
#define BACK_TO_MAIN_MENU        ('0')
#define MAIN_MENU                "\r\nSelect the resolution:"\
                                 "\r\n1. Full parallel display (1024x600)"\
                                 "\r\n2. VGA  (640 x 480)"\
                                 "\r\n3. QVGA (320 x 240)"\
                                 "\r\nSelect:"

#define CAMERA_MODE_SELECTION    "\r\nSelect Camera Mode:"\
                                 "\r\n1. Live camera"\
								 "\r\n2. Test pattern"\
								 "\r\n0. Back to main menu"\
								 "\r\nSelect:\r\n"

#define NULL_CHAR                ('\0')
#define INDEX_CHECK              (0U)

/* Switch on-board declaration */
#define MIPI_SEL_PIN             (6U)

/* Public functions declarations */
void handle_error (fsp_err_t err,  char *err_str);
void mipi_csi_ep_entry(void);

#endif /* MIPI_CSI_H_ */
