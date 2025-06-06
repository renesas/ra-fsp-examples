/***********************************************************************************************************************
 * File Name    : user_config.h
 * Description  : As required for the file such as Contains macros, data structures and functions used common to the EP
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2020 - 2024 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef USER_CONFIG_H_
#define USER_CONFIG_H_

#define CARD_DETECTION_PIN_IS_USED             (1)

#if defined(BOARD_RA4L1_EK)
#define SMCI_VCC                               (BSP_IO_PORT_07_PIN_00)
#define SMCI_RST                               (BSP_IO_PORT_02_PIN_05)
#define SMCI_DETECT                            (BSP_IO_PORT_01_PIN_05)
#elif defined(BOARD_RA2L2_EK)
#define SMCI_VCC                               (BSP_IO_PORT_01_PIN_11)
#define SMCI_RST                               (BSP_IO_PORT_01_PIN_04)
#define SMCI_DETECT                            (BSP_IO_PORT_02_PIN_06)
#else
#define SMCI_VCC                               (BSP_IO_PORT_08_PIN_04)
#define SMCI_RST                               (BSP_IO_PORT_08_PIN_03)
#define SMCI_DETECT                            (BSP_IO_PORT_07_PIN_08)
#endif

#endif /* USER_CONFIG_H_ */
