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
#endif

/* Macro definitions */
#define ADCTEMP_AS_C(a)         ((((float)a) * 0.298147f) - 287.388f)
#define ADCTEMP_AS_F(a)         ((((float)a) * 0.536652f) - 485.298f)

#define ETH_PREINIT     "\r\n \r\n--------------------------------------------------------------------------------"\
                        "\r\nEthernet adapter Configuration for Renesas "KIT_NAME": Pre IP Init       "\
                        "\r\n--------------------------------------------------------------------------------\r\n\r\n"

#define ETH_POSTINIT    "\r\n \r\n--------------------------------------------------------------------------------"\
                        "\r\nEthernet adapter Configuration for Renesas "KIT_NAME": Post IP Init       "\
                        "\r\n--------------------------------------------------------------------------------\r\n\r\n"

#endif /* BOARD_CONFIG_H_ */
