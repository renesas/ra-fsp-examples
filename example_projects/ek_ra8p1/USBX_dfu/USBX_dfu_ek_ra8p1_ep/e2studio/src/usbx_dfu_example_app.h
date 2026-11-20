/***********************************************************************************************************************
 * File Name    : usbx_dfu_example_app.h
 * Description  : Public interface for the USBX DFU example application layer.
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * Copyright (c) 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 **********************************************************************************************************************/

#ifndef USBX_DFU_EXAMPLE_APP_H_
#define USBX_DFU_EXAMPLE_APP_H_

#include "usbx_dfu_ep.h"

#define EP_VERSION      "1.0"

#define MODULE_NAME     "USBX_dfu"

#define BANNER_INFO     "\r\n********************************************************************************"\
                        "\r\n*   Renesas FSP Example Project for "MODULE_NAME" Module                            *"\
                        "\r\n*   Example Project Version %s                                                *"\
                        "\r\n*   Flex Software Pack Version %d.%d.%d                                           *"\
                        "\r\n********************************************************************************"\
                        "\r\nRefer to readme.txt file for more details on Example Project and"\
                        "\r\nFSP User's Manual for more information about "MODULE_NAME" module\r\n"

#define EP_INFO         "\r\nThe example project demonstrates the basic functionalities of a USBX Device Firmware"\
                        "\r\nUpdate (DFU) driver on Renesas RA MCUs based on Renesas FSP using Azure RTOS. The RA"\
                        "\r\nboard is connected to the host PC, where it acts as a peripheral device. Prepare a"\
                        "\r\nfile (.txt, .srec, .xml, .hex, etc.) to download to the RA MCU which supports both CDC"\
                        "\r\nand DFU interfaces. In a real-world application, new device firmware can be downloaded."\
                        "\r\nIn this example project, a set of data is used to demonstrate the download functionality."\
                        "\r\nThe DFU example application works in four steps:"\
                        "\r\n  a. First, the system is initialized with support for the Interface Association"\
                        "\r\n     Descriptor. The CDC and DFU classes are then initialized with separate interfaces."\
                        "\r\n  b. Second, the device operates in Run-Time mode with the CDC and DFU interfaces. An"\
                        "\r\n     application on the host (Windows PC) sends a DFU request. Upon reception of this"\
                        "\r\n     command, the system un-initializes the Run-Time configuration and initializes the"\
                        "\r\n     device in DFU mode."\
                        "\r\n  c. Third, the device disappears from the bus for a short time (enough for the host and"\
                        "\r\n     the device to detect a reset sequence). Upon restarting, the device is exclusively"\
                        "\r\n     in DFU mode and downloads the data sent from the host."\
                        "\r\n  d. Finally, when the new data has been downloaded, the host application resets the"\
                        "\r\n     device, and upon re-enumeration, the device reverts to its normal operation with the"\
                        "\r\n     new firmware. The RA MCU displays the USB enumeration status on the terminal.\r\n"

/* External variables - Defined in usbx_dfu_example_app.c */
extern volatile bool g_download_done;
extern volatile bool g_dfu_manifestation_done;
extern UX_SLAVE_CLASS_DFU_PARAMETER g_ux_dfu_peri_class_parameter;
extern UCHAR g_dfu_data[BUFFER_SIZE];
extern ULONG g_data_cnt;
extern ULONG g_block_no_cnt;

/* Single entry-point called in usbx_peri_dfu_thread_entry.c */
void usbx_peri_dfu_example_app_run(void);

/* Function declaration */
void usbx_dfu_cleanup(void);

#endif /* USBX_DFU_EXAMPLE_APP_H_ */
