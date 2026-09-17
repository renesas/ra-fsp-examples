/***********************************************************************************************************************
 * File Name    : frame_preemption_example.h
 * Description  : Contains function declarations for the Frame Preemption sender example application.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef FRAME_PREEMPTION_EXAMPLE_H_
#define FRAME_PREEMPTION_EXAMPLE_H_

#define EP_VERSION      ("1.0")
#define MODULE_NAME     "layer3_switch"
#define BANNER_INFO     "\r\n*********************************************************************"\
                        "\r\n*   Renesas FSP Example Project for " MODULE_NAME " Module            *"\
                        "\r\n*   Example Project Version %s                                     *"\
                        "\r\n*   Flex Software Pack Version  %d.%d.%d                               *"\
                        "\r\n*********************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and" \
                        "\r\nFSP User's Manual for more information about " MODULE_NAME " module\r\n"

#define EP_INFO         "\r\nThis example demonstrates IEEE 802.3br / 802.1Qbu Frame Preemption on the" \
                        "\r\nRA8T2 MCU using the internal Layer 3 Switch (L3 Switch). The example" \
                        "\r\nconsists of two separate projects that run on two RA8T2-MCK boards" \
                        "\r\nconnected back-to-back over Ethernet.\r\n\r\n"

void frame_preemption_example_entry(void);

#endif /* FRAME_PREEMPTION_EXAMPLE_H_ */
