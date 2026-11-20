/***********************************************************************************************************************
 * File Name    : board_cfg.h
 * Description  : Board specific/configuration data.
 **********************************************************************************************************************/
/***********************************************************************************************************************
* Copyright (c) 2022 - 2026 Renesas Electronics Corporation and/or its affiliates
*
* SPDX-License-Identifier: BSD-3-Clause
***********************************************************************************************************************/

#ifndef BOARD_CONFIG_H_
#define BOARD_CONFIG_H_

#include "hal_data.h"

/* Macro for the kit name, based on the board macro */
#if defined (BOARD_RA6M3_EK) || defined (BOARD_RA6M3G_EK)
#define KIT_NAME                "EK-RA6M3"
#elif defined (BOARD_RA8M1_EK)
#define KIT_NAME                "EK-RA8M1"
#elif defined (BOARD_RA8D1_EK)
#define KIT_NAME                "EK-RA8D1"
#elif defined (BOARD_RA6M5_EK)
#define KIT_NAME                "EK-RA6M5"
#elif defined (BOARD_RA8T1_MCK)
#define KIT_NAME                "MCK-RA8T1"
#elif defined (BOARD_RA8P1_EK)
#define KIT_NAME                "EK-RA8P1"
#elif defined (BOARD_RA8P1_EK_V2)
#define KIT_NAME                "EK-RA8P1 V2"
#elif defined (BOARD_RA8D2_EK)
#define KIT_NAME                "EK-RA8D2"
#elif defined (BOARD_RA8M2_EK)
#define KIT_NAME                "EK-RA8M2"
#elif defined (BOARD_RA8T2_EK)
#define KIT_NAME                "EK-RA8T2"
#endif /* Specific board */

/* Macro for the reset value */
#define RESET_VALUE             (0x00)

/* Macro to convert the ADC value of the internal MCU die temperature sensor to degrees Celsius */
#define ADCTEMP_AS_C(a)         ((((float)(a)) * 0.298147f) - 287.388f)

#endif /* BOARD_CONFIG_H_ */
