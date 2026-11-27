/***********************************************************************************************************************
 * File Name    : ssi_example_app.h
 * Description  : Public interface for the SSI example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef SSI_EXAMPLE_APP_H_
#define SSI_EXAMPLE_APP_H_

#define EP_VERSION      "1.2"

#define MODULE_NAME     "r_ssi"

#define BANNER_INFO     "\r\n******************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                 *"\
                        "\r\n*   Example Project Version %s                                  *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                             *"\
                        "\r\n******************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" driver\r\n"

#define EP_INFO         "\r\nThis example project demonstrates audio streaming using the SSI module"\
                        "\r\nwith a double-buffer mechanism. A sine wave is generated and continuously"\
                        "\r\nstreamed via a loopback connection between the SSITX and SSIRX pins."\
                        "\r\nThe sine wave data is stored in two buffers, alternating between them"\
                        "\r\nto ensure continuous and uninterrupted streaming. The transmitted and"\
                        "\r\nreceived audio data are compared to verify the successful completion of"\
                        "\r\nthe SSI transfer. The results will be displayed on the terminal."\
						"\r\nThe sample data can be observed using waveform rendering in the memory"\
                        "\r\nviewer of e2studio.\r\n"

/* Single entry-point called in hal_entry.c */
void ssi_example_app_run(void);

#endif /* SSI_EXAMPLE_APP_H_ */
