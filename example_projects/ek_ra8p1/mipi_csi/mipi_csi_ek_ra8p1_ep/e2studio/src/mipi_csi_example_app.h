/***********************************************************************************************************************
 * File Name    : mipi_csi_example_app.h
 * Description  : Public interface for the MIPI CSI example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef MIPI_CSI_EXAMPLE_APP_H_
#define MIPI_CSI_EXAMPLE_APP_H_

#define EP_VERSION      "1.0"

#define MODULE_NAME     "r_mipi_csi"

#define BANNER_INFO     "\r\n********************************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                          *"\
                        "\r\n*   Example Project Version %s                                                *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                                           *"\
                        "\r\n********************************************************************************"\
                        "\r\nRefer to the accompanying .md file for Example Project details and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO         "\r\nThis example project demonstrates the basic functionality of MIPI CSI on Renesas"\
                        "\r\nRA MCUs using Renesas FSP. The I2C module configures an external camera sensor to"\
                        "\r\noperate with the MIPI interface. Captured image data is received by the MIPI CSI"\
                        "\r\nmodule, then processed by the VIN module: clipped per ITU-R BT.601, converted from"\
                        "\r\nYCbCr-422 to RGB, scaled to the user-selected resolution, and stored in SDRAM."\
                        "\r\nThe resolution change is handled by the VIN module, allowing dynamic output"\
                        "\r\nresizing without reconfiguring the camera sensor.\r\n"\
                        "\r\nBefore choosing the camera mode, the user is prompted via terminal to select the"\
                        "\r\ndesired image resolution. After that, the user can choose to capture a live image"\
                        "\r\nor a test pattern by typing '1', '2', or '3' to return to the resolution menu."\
                        "\r\nCaptured images are viewable as raw data in e2studio using the Memory view."\
                        "\r\nIf a Parallel Graphics LCD is connected, the image is also displayed on-screen."

#define EP_MENU         "\r\n\r\nResolution menu:"\
                        "\r\n1. Full parallel display (1024x600)"\
                        "\r\n2. VGA (640 x 480)"\
                        "\r\n3. QVGA (320 x 240)"\
                        "\r\nUser input: "

#define SUB_MENU        "\r\n\r\nCamera mode menu:"\
                        "\r\n1. Live camera"\
                        "\r\n2. Test pattern"\
                        "\r\n3. Back to resolution menu"\
                        "\r\nUser input: "

/* Single entry-point called in hal_entry.c */
void mipi_csi_example_app_run(void);

#endif /* MIPI_CSI_EXAMPLE_APP_H_ */
